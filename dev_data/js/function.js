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
   var block = document.getElementById(state.slice(2,-2));
   if (response.class && response.class!='undefined') {block.className = response.class;}
   if (response.style && response.style!='undefined') {block.style = response.style;}
   if (response.title && response.title!='undefined') {
    if (block.tagName == 'INPUT') {block.value = renameBlock(jsonResponse, response.title);}
    if (block.tagName == 'DIV' ||block.tagName == 'A' || block.tagName == 'H1' || block.tagName == 'H2' || block.tagName == 'H3' || block.tagName == 'H4' || block.tagName == 'H5' || block.tagName == 'H6') {block.innerHTML = renameBlock(jsonResponse, response.title);}
   }
   if (typeof(element) != 'undefined' && element != null){
    element.innerHTML += '<li class="alert alert-info" style="margin:5px 0px;">'+xmlHttp.responseText.replace(/</g,'&lt;')+'</li>';
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

  if (state != null && state!='undefined'){
   var response=JSON.parse(xmlHttp.responseText);
   var block = document.getElementById(state.slice(2,-2));
   if (response.class && response.class!='undefined') {block.className = response.class;}
   if (response.style && response.style!='undefined') {block.style = response.style;}
   if (response.title && response.title!='undefined') {
    if (block.tagName == 'INPUT') {block.value = renameBlock(jsonResponse, response.title);}
    if (block.tagName == 'DIV' ||block.tagName == 'A' || block.tagName == 'H1' || block.tagName == 'H2' || block.tagName == 'H3' || block.tagName == 'H4' || block.tagName == 'H5' || block.tagName == 'H6') {block.innerHTML = renameBlock(jsonResponse, response.title);}
   }
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

function setLang(submit){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET',"/lang?set="+submit,true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  location.reload();
 }
}

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

function loadLang(langids){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET','/lang.list.json',true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  var jsonLang=JSON.parse(xmlHttp.responseText);
  var html = '';
  for(var key in jsonLang) {
   var view_lang = jsonLang[key].name.substr(10,2);
   html += '<li><a href="#" onclick="setLang(\''+view_lang+'\')" title="'+jsonLang[key].name+'">'+view_lang+'</a></li>';
  }
  document.getElementById(langids).innerHTML = (html?html:'<li>No langs in folder: /lang/lang.*.json.gz</li>');
 }
}

function loadTimer(timerids,module){
 var xhttp=createXmlHttpObject();
 xhttp.open("GET", "/timer.save.json", true);
 xhttp.send(null);
 xhttp.onload = function(e) {
  var timers=JSON.parse(xhttp.responseText);
  timers.timer.sort(function(a,b){return (a.time > b.time) ? 1 : ((b.time > a.time) ? -1 : 0);});
  var html = '';
  for (var i = 0; i < timers.timer.length; i++) {
   if(timers.timer[i].module == module) {
    if (timers.timer[i].trigger == "on") {timers.timer[i].trigger = '<span class="label label-success">'+jsonResponse["LangOn."]+'</span>';}
    if (timers.timer[i].trigger == "off") {timers.timer[i].trigger = '<span class="label label-danger">'+jsonResponse["LangOff."]+'</span>';}
    if (timers.timer[i].trigger == "not") {timers.timer[i].trigger = '<span class="label label-info">'+jsonResponse["LangSwitch."]+'<\/span>';}
    timers.timer[i].day = jsonResponse["Lang"+timers.timer[i].day];
    html += '<li>'+timers.timer[i].trigger+' <b>'+timers.timer[i].day+'<\/b> '+timers.timer[i].time+'<\/li>';
   }
  }
  document.getElementById(timerids).innerHTML = (html?html:'<li>'+jsonResponse.LangNoTimers+'</li>');
 }
}

function loadSpace(spaceids){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET','/devices.list.json',true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  var jsonSpace=JSON.parse(xmlHttp.responseText);
  var html = '';
  for(var key in jsonSpace) {
   html += '<option value="'+jsonSpace[key].space+'">';
  }
  document.getElementById(spaceids).innerHTML = html;
 }
}

function real_time(hours,min,sec) {
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
