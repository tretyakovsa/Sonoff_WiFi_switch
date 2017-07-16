var xmlHttp=createXmlHttpObject();
function createXmlHttpObject(){
 if(window.XMLHttpRequest){
  xmlHttp=new XMLHttpRequest();
 }else{
  xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');
 }
 return xmlHttp;
}

var set_real_time;

function load(stage){
 var xmlHttp=createXmlHttpObject();
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
  xmlHttp.open('PUT','/config.live.json',true);
  xmlHttp.send(null);
  xmlHttp.onload = function(e) {
   var jsonResponse1=JSON.parse(xmlHttp.responseText);
   xmlHttp.open('GET','/lang/lang.'+jsonResponse1.lang+'.json',true);
   xmlHttp.send(null);
   xmlHttp.onload = function(e) {
    var jsonResponse2=JSON.parse(xmlHttp.responseText);
    xmlHttp.open('GET','/modules.json',true);
    xmlHttp.send(null);
    xmlHttp.onload = function(e) {
     var modules=JSON.parse(xmlHttp.responseText);
     jsonResponse = Object.assign(jsonResponse1, jsonResponse2);
     jsonResponse.module = modules.module;
     var theCookies = document.cookie.split(';');
     for (var i = 1 ; i <= theCookies.length; i++) {
      jsonResponse[theCookies[i-1].split("=")[0].replace(/^ /,'')] = theCookies[i-1].split("=")[1];
     }
     if (stage == 'first') {
      if (jsonResponse.ip=='0.0.0.0') { toggle('btn-devices'); }
      toggle('content');
      loadBlock(jsonResponse);
     } else {
      handleServerResponse(stage,jsonResponse);
     }
    }
   }
  }
 }
}

function loadBlock(jsonResponse) {
 var data = document.getElementsByTagName('body')[0].innerHTML;
 var new_string;
 for (var key in jsonResponse) {
  data = data.replace(new RegExp('{{'+key+'}}', 'g'), jsonResponse[key]);
 }
 for (var key in jsonResponse.module) {
  data = data.replace(new RegExp('module-'+jsonResponse.module[key]+' hidden', 'g'), 'show');
 }
 document.getElementsByTagName('body')[0].innerHTML = data;
 handleServerResponse('',jsonResponse);
}

function searchModule(modules,find) {
 for(var key in modules) {
  if (modules[key] == find) {
   return "yes";
  }
 }
}

function val(id,val){
 if (val) {
  document.getElementById(id).value=(val==' '?'':val);
 } else {
  var v = document.getElementById(id).value;
  return v;
 }
}

function html(id,val){
 if (val) {
  document.getElementById(id).innerHTML=(val==' '?'':val);
 } else {
  var v = document.getElementById(id).innerHTML;
  return v;
 }
}

function send_request_edit(submit,server,filename){
 xmlHttp = new XMLHttpRequest();
 var formData = new FormData();
 formData.append("data", new Blob([server], { type: 'text/html' }), filename);
 xmlHttp.open("POST", "/edit");
 xmlHttp.send(formData);
}



function send_request_post(submit,server,state){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("POST", server, true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  if (state != null && state!='undefined'){
   var response=JSON.parse(xmlHttp.responseText);
   var block = state.split(',');
   for (var i = 0 ; i < block.length; i++) {
    var htmlblock = document.getElementById(block[i].slice(2,-2));
    if (response.class && response.class!='undefined') {htmlblock.className = response.class;}
    if (response.style && response.style!='undefined') {htmlblock.style = response.style;}
    if (response.title && response.title!='undefined') {
     if (htmlblock.tagName == 'INPUT') {htmlblock.value = renameBlock(jsonResponse, response.title);}
     if (htmlblock.tagName == 'DIV' ||htmlblock.tagName == 'A' || htmlblock.tagName == 'H1' || htmlblock.tagName == 'H2' || htmlblock.tagName == 'H3' || htmlblock.tagName == 'H4' || htmlblock.tagName == 'H5' || htmlblock.tagName == 'H6') {htmlblock.innerHTML = renameBlock(jsonResponse, response.title);}
    }
    if (typeof(element) != 'undefined' && element != null){
     element.innerHTML += '<li class="alert alert-info" style="margin:5px 0px;"><span class="label label-success">'+block[i]+'</span> '+xmlHttp.responseText.replace(/</g,'&lt;')+'</li>';
    }
   }
  }
 }
}


function send_request(submit,server,state){
 var old_submit = submit.value;
 submit.value = jsonResponse.LangLoading;
 submit_disabled(true);
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("GET", server, true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {

  submit.value=old_submit;
  submit_disabled(false);

  var element =  document.getElementById('url-content');
  if (typeof(element) != 'undefined' && element != null){
   element.innerHTML += '<li><span class="label label-warning">GET</span> <a href="'+server+'" class="btn btn-link" style="text-transform:none;text-align:left;white-space:normal;display:inline">'+server+'</a> <span class="label label-'+(xmlHttp.status==200?'default':'danger')+'">'+xmlHttp.status+' '+xmlHttp.statusText+'</span></li>';
  }

  var ddnsUrl1 =  document.getElementById('ddns-url1');
  if (typeof(ddnsUrl1) != 'undefined' && ddnsUrl1 != null){
   ddnsUrl1.innerHTML = '<a href="http://'+jsonResponse.ip+':'+jsonResponse.ddnsPort+'/'+server+'">http://'+jsonResponse.ip+':'+jsonResponse.ddnsPort+'/'+server+'</a>';
  }
  var ddnsUrl2 =  document.getElementById('ddns-url2');
  if (typeof(ddnsUrl2) != 'undefined' && ddnsUrl2 != null && jsonResponse.ddnsName){
   ddnsUrl2.innerHTML = '<a href="http://'+jsonResponse.ddnsName+':'+jsonResponse.ddnsPort+'/'+server+'">http://'+jsonResponse.ddnsName+':'+jsonResponse.ddnsPort+'/'+server+'</a>';
  }

  if (state != '' && state != null && state!='undefined'){
   var block = state.split(',');
   for (var i = 0 ; i < block.length; i++) {
    if (block[i].slice(0, 2) != '[[') {
     window.location = block[i];
    } else {
     var response=JSON.parse(xmlHttp.responseText);
     var htmlblock = document.getElementById(block[i].slice(2,-2));
     if (response.class && response.class!='undefined') {htmlblock.className = response.class;}
     if (response.style && response.style!='undefined') {htmlblock.style = response.style;}
     if (response.title && response.title!='undefined') {
      if (htmlblock.tagName == 'INPUT') {htmlblock.value = renameBlock(jsonResponse, response.title);}
      if (htmlblock.tagName == 'DIV' ||htmlblock.tagName == 'A' || htmlblock.tagName == 'H1' || htmlblock.tagName == 'H2' || htmlblock.tagName == 'H3' || htmlblock.tagName == 'H4' || htmlblock.tagName == 'H5' || htmlblock.tagName == 'H6') {htmlblock.innerHTML = renameBlock(jsonResponse, response.title);}
     }
     if (typeof(element) != 'undefined' && element != null){
      element.innerHTML += '<li class="alert alert-info" style="margin:5px 0px;"><a href="#'+block[i].slice(2,-2)+'" class="label label-success">'+block[i]+'</a> '+xmlHttp.responseText.replace(/</g,'&lt;')+'</li>';
     }
    }

   }
  } else {
   if (typeof(element) != 'undefined' && element != null){
    element.innerHTML += '<li class="alert alert-info" style="margin:5px 0px;">'+xmlHttp.responseText.replace(/</g,'&lt;')+'</li>';
   }
  }


  // load('next');
 }
}

function submit_disabled(request){
 var inputs = document.getElementsByTagName("input");
 for (var i = 0; i < inputs.length; i++) {
  if (inputs[i].type === 'button') {inputs[i].disabled = request;}
 }
}

function toggle(target,status) {
 var curVal = document.getElementById(target).classList;
 if (curVal.contains('hidden')) {
  if (status != 'show') {
   curVal.remove('hidden');
   curVal.add('show');
  }
 } else {
  if (status != 'hidden') {
   curVal.remove('show');
   curVal.add('hidden');
  }
 }
}

//function setLang(submit){
// var xmlHttp=createXmlHttpObject();
// xmlHttp.open('GET',"/lang?set="+submit,true);
// xmlHttp.send(null);
// xmlHttp.onload = function(e) {
//  location.reload();
// }
//}

function loadWifi(ssids,hiddenIds){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET','/wifi.scan.json',true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  var jsonWifi=JSON.parse(xmlHttp.responseText);
  jsonWifi.networks.sort(function(a,b){return (a.dbm < b.dbm) ? 1 : ((b.dbm < a.dbm) ? -1 : 0);});
  var html = '';
  for(i = 0;i<jsonWifi.networks.length;i++) {
   var wifiSignal = '';
   if (jsonWifi.networks[i].dbm <= -0) { wifiSignal = '<i class="wifi wifi-0-60"></i>';}
   if (jsonWifi.networks[i].dbm <= -60) { wifiSignal = '<i class="wifi wifi-60-70"></i>';}
   if (jsonWifi.networks[i].dbm <= -70) { wifiSignal = '<i class="wifi wifi-70-80"></i>';}
   if (jsonWifi.networks[i].dbm <= -80) { wifiSignal = '<i class="wifi wifi-80-90"></i>';}
   if (jsonWifi.networks[i].dbm <= -90) { wifiSignal = '<i class="wifi wifi-90-100"></i>';}
   html += '<li><a href="#" onclick="val(\''+hiddenIds+'\',\''+jsonWifi.networks[i].ssid+'\');toggle(\'ssid-select\');html(\'ssid-name\',\''+jsonWifi.networks[i].ssid+'\');return false"><div style="float:right">'+(jsonWifi.networks[i].pass?'<i class="wifi wifi-key"></i>':'')+' '+wifiSignal+' <span class="label label-default">'+jsonWifi.networks[i].dbm+' dBm</span></div><b>'+jsonWifi.networks[i].ssid+'</b></a></li>';
  }
  document.getElementById(ssids).innerHTML = (html?html:'<li>No WiFi</li>')+'<li><a href="#" onclick="toggle(\'ssid-group\');toggle(\'ssid\');return false"><b>'+jsonResponse.LangHiddenWifi+'</b></a></li>';
 }
}

function loadBuild(buildids,typeFile){
 htmls = '';
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET','http://backup.privet.lv/esp/build/'+buildids,true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  var jsonBuild=JSON.parse(xmlHttp.responseText);
  jsonBuild.sort(function(a,b){return (a.name< b.name) ? 1 : ((b.name < a.name) ? -1 : 0);});
  var html = '';
  for(i = 0;i<jsonBuild.length;i++) {
   if (typeFile == 'all' && jsonBuild[i].name.substring(0,5) == 'spiff') {
    html += '<li><a href="/upgrade?spiffs=http://backup.privet.lv/esp/'+buildids+'/spiffs.0xBB000_flash_size_1Mb.256Kb_'+jsonBuild[i].name.slice(36, -4)+'.bin&build=http://backup.privet.lv/esp/'+buildids+'/build.0x00000_flash_size_1Mb.256Kb_'+jsonBuild[i].name.slice(36, -4)+'.bin" '+(jsonResponse.spiffsData==jsonBuild[i].name?'style="font-weight:bold;"':'')+' onclick="return confirm(\''+jsonResponse.LangRefresh+' '+typeFile+' (Build & Spiffs (flash 1Mb 256Kb) '+jsonBuild[i].name.slice(36, -4)+')?\')">Build & Spiffs (flash 1Mb 256Kb) '+jsonBuild[i].name.slice(36, -4)+'<\/a><\/li>';
   }
   if (jsonBuild[i].name.substring(0,5) == typeFile.substring(0,5)) {
    html += '<li><a href="/upgrade?'+typeFile+'=http://backup.privet.lv/esp/'+buildids+'/'+jsonBuild[i].name+'" '+(jsonResponse[typeFile+"Data"]==jsonBuild[i].name?'style="font-weight:bold;"':'')+' onclick="return confirm(\''+jsonResponse.LangRefresh+' '+typeFile+' ('+jsonBuild[i].name+')?\')">'+jsonBuild[i].name+'<\/a><\/li>';
   }
  }
  document.getElementById(buildids+'-'+typeFile).innerHTML = (html?html:'<li><a href="#">No build in folder<\/a><\/li>');
 }
}

function set_time_zone(submit){
 var gmtHours = new Date().getTimezoneOffset()/60*-1;
 val('timeZone', gmtHours);
 send_request(submit,'/timeZone?timeZone='+gmtHours);
}

//function loadLang(langids){
// var xmlHttp=createXmlHttpObject();
// xmlHttp.open('GET','/lang.list.json',true);
// xmlHttp.send(null);
// xmlHttp.onload = function(e) {
//  var jsonLang=JSON.parse(xmlHttp.responseText);
//  var html = '';
//  for(var key in jsonLang) {
//   var view_lang = jsonLang[key].name.substr(10,2);
//   html += '<li><a href="#" onclick="setLang(\''+view_lang+'\')" title="'+jsonLang[key].name+'">'+view_lang+'</a></li>';
//  }
//  document.getElementById(langids).innerHTML = (html?html:'<li>No langs in folder: /lang/lang.*.json.gz</li>');
// }
//}

//function loadTimer(timerids,module){
// var xhttp=createXmlHttpObject();
// xhttp.open("GET", "/timer.save.json", true);
// xhttp.send(null);
// xhttp.onload = function(e) {
//  var timers=JSON.parse(xhttp.responseText);
//  timers.timer.sort(function(a,b){return (a.time > b.time) ? 1 : ((b.time > a.time) ? -1 : 0);});
//  var html = '';
//  for (var i = 0; i < timers.timer.length; i++) {
//   if(timers.timer[i].module == module) {
//    if (timers.timer[i].trigger == "on") {timers.timer[i].trigger = '<span class="label label-success">'+jsonResponse["LangOn."]+'</span>';}
//    if (timers.timer[i].trigger == "off") {timers.timer[i].trigger = '<span class="label label-danger">'+jsonResponse["LangOff."]+'</span>';}
//    if (timers.timer[i].trigger == "not") {timers.timer[i].trigger = '<span class="label label-info">'+jsonResponse["LangSwitch."]+'<\/span>';}
//    timers.timer[i].day = jsonResponse["Lang"+timers.timer[i].day];
//    html += '<li>'+timers.timer[i].trigger+' <b>'+timers.timer[i].day+'<\/b> '+timers.timer[i].time+'<\/li>';
//   }
//  }
//  document.getElementById(timerids).innerHTML = (html?html:'<li>'+jsonResponse.LangNoTimers+'</li>');
// }
//}

//function loadSpace(spaceids){
// var xmlHttp=createXmlHttpObject();
// xmlHttp.open('GET','/devices.list.json',true);
// xmlHttp.send(null);
// xmlHttp.onload = function(e) {
//  var jsonSpace=JSON.parse(xmlHttp.responseText);
//  var html = '';
//  for(var key in jsonSpace) {
//   html += '<option value="'+jsonSpace[key].space+'">';
//  }
//  document.getElementById(spaceids).innerHTML = html;
// }
//}

function real_time(hours,min,sec) {
 var res = html('time').split(":");
 hours=res[0];
 min=res[1];
 sec=res[2];
 sec=Number(sec)+1;
 if (sec>=60){min=Number(min)+1;sec=0;}
 if (min>=60){hours=Number(hours)+1;min=0;}
 if (hours>=24){hours=0};
 html('time',hours+":"+min+":"+sec);
 set_real_time = setTimeout("real_time("+hours+","+min+","+sec+");", 1000);
}

function setCookie(name, value, days, submit) {
 if (days) {
  var date = new Date();
  date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
  var expires = "; expires=" + date.toGMTString();
 }
 else expires = "";
 document.cookie = name + "=" + value + expires + "; path=/";
}

function hide(name, submit) {
 if (confirm(jsonResponse.LangHedden)) {
  submit.parentNode.classList.add('hidden');
  setCookie(name,'hidden',365,submit);
 }
}

function delAllCookies() {
 var cookies = document.cookie.split(";");
 for (var i = 0; i < cookies.length; i++) {
  var cookie = cookies[i];
  var eqPos = cookie.indexOf("=");
  var name = eqPos > -1 ? cookie.substr(0, eqPos) : cookie;
  document.cookie = name + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT";
 }
}

function createTable(state_val, jsonTable) {
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("GET", state_val, true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  var timers=JSON.parse(xmlHttp.responseText);
  for (var i = 0; i < timers.timer.length; i++) {
   var tbody = '';
   for(var keys in jsonTable) {
    tbody += '<td>'+timers.timer[i][keys]+'<\/td>';
   }
   document.getElementById(state_val.replace(/[^a-z0-9]/gi,'')).innerHTML += '<tr>'+tbody+'<\/tr>';
  }
 }
}


function renameBlock(jsonResponse, str) {
 if (str) {
  var arr=str.match(/\{\{\S+?\}\}/gim);
  if (arr) {
   for (var i=0; i<arr.length; i++) {
    var id=arr[i].slice(2, -2);
    //if (jsonResponse[id]) {
    str = str.replace(new RegExp('{{'+id+'}}','g'), jsonResponse[id]);
    // }
   };
  }
 }
 return (typeof(str)!='undefined'&&str!=null?str:'');
}


function renameGet(str) {
 if (str) {
  var arr=str.match(/\[\[\S+?\]\]/gim);
  if (arr) {
   for (var i=0; i<arr.length; i++) {
    var id=arr[i].slice(2, -2);
    if (document.getElementById(id)) {
     var txt='';
     if (document.getElementById(id).tagName=='select'){
      txt=document.getElementById(id).options[document.getElementById(id).selectedIndex].value;
     } else {
      txt=encodeURIComponent(document.getElementById(id).value);
     }
     str = str.replace(new RegExp('\\[\\['+id+'\\]\\]','g'), txt);
    }
   };
  }
 }
 return str;
}


function createRGB(valID,actionID,moduleID,responseID){
 var img = _('.'+valID+'-thumb img'),
     canvas = _('#'+valID+'-cs'),
     result = _('.'+valID+'-result'),
     preview = _('.'+valID+'-preview'),x = '',y = '';
 // click function
 img.addEventListener('click', function(e){
  // chrome
  if(e.offsetX) {x=e.offsetX;y=e.offsetY;}
  // firefox
  else if(e.layerX) {x=e.layerX;y=e.layerY;}
  useCanvas(canvas,img,function(){
   // get image data
   var p = canvas.getContext('2d').getImageData(x, y, 1, 1).data;
   // nex_color = rgbToHex(p[0],p[1],p[2]);
   //on_rgb(this,p[0],p[1],p[2]);
   // alert(p[0]+" "+p[1]+" "+p[2]);
   if (valID){val(valID, rgbToHex(p[0],p[1],p[2]));}
   if (actionID){send_request(valID, (moduleID!='undefined'?'cmd?command=':'')+renameGet(actionID), responseID);}
   document.body.style.background = "#"+rgbToHex(p[0],p[1],p[2]);
  });
 },false);
 // preview function mousemove
 img.addEventListener('mousemove', function(e){
  // chrome
  if(e.offsetX) {x=e.offsetX;y=e.offsetY;}
  // firefox
  else if(e.layerX) {x=e.layerX;y=e.layerY;}
  useCanvas(canvas,img,function(){
   var p = canvas.getContext('2d').getImageData(x, y, 1, 1).data;
   // document.body.style.background = "#"+rgbToHex(p[0],p[1],p[2]);
  });
 },false);
}
function useCanvas(el,image,callback){
 el.width = image.width;
 el.height = image.height;
 el.getContext('2d').drawImage(image, 0, 0, image.width, image.height);
 return callback();
}
function _(el){
 return document.querySelector(el);
};
function componentToHex(c) {
 var hex = c.toString(16);
 return hex.length == 1 ? "0" + hex : hex;
}
function rgbToHex(r, g, b) {
 return componentToHex(r) + componentToHex(g) + componentToHex(b);
}
function findPos(obj) {
 var curleft = 0, curtop = 0;
 if (obj.offsetParent) {
  do {
   curleft += obj.offsetLeft;
   curtop += obj.offsetTop;
  } while (obj = obj.offsetParent);
  return { x: curleft, y: curtop };
 }
 return undefined;
}
