var xmlHttp=createXmlHttpObject();
function createXmlHttpObject(){
 if(window.XMLHttpRequest){
  xmlHttp=new XMLHttpRequest();
 }else{
  xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');
 }
 return xmlHttp;
}
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
    jsonResponse = Object.assign(jsonResponse1, jsonResponse2);
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

function loadBlock(jsonResponse) {
 var data = document.getElementsByTagName('body')[0].innerHTML;
 var new_string;
 for (var key in jsonResponse) {
  new_string = data.replace(new RegExp('{{'+key+'}}', 'g'), jsonResponse[key]);
  data = new_string;
 }
 document.getElementsByTagName('body')[0].innerHTML = new_string;
 handleServerResponse('',jsonResponse);
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

function send_request(submit,server){
 var old_submit = submit.value;
 submit.value = jsonResponse.LangLoading;
 submit_disabled(true);
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("GET", server, true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  submit.value=old_submit;
  submit_disabled(false);
  load('next');
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

function LoadWifi(ssids){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET','/wifi.scan.json',true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  var jsonWifi=JSON.parse(xmlHttp.responseText);
  var html = '';
  for(i = 0;i<jsonWifi.networks.length;i++) {
   html += '<li><a href="#" onclick="val(\'ssid\',\''+jsonWifi.networks[i].ssid+'\');toggle(\'ssid-select\');document.getElementById(\'ssid-name\').innerHTML=\''+jsonWifi.networks[i].ssid+'\';return false"><div style="float:right">'+(jsonWifi.networks[i].pass?'&#x1f512;':'')+' <span class="label label-info">'+(jsonWifi.networks[i].dbm+100)+'%</span> <span class="label label-default">'+jsonWifi.networks[i].dbm+' dBm</span></div><b>'+jsonWifi.networks[i].ssid+'</b></a></li>';
  }
  document.getElementById(ssids).innerHTML = (html?html:'<li>No WiFi</li>')+'<li><a href="#" onclick="toggle(\'ssid-group\');toggle(\'ssid\');return false"><b>'+jsonResponse.LangHiddenWifi+'</b></a></li>';
 }
}

function LoadLang(langids){
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

function LoadTimer(timerids){
 var xhttp=createXmlHttpObject();
 xhttp.open("GET", "/timer.save.json", true);
 xhttp.send(null);
 xhttp.onload = function(e) {
  var timers=JSON.parse(xhttp.responseText);
  var html = '';
  for (var i = 0; i < timers.timer.length; i++) {
   if (timers.timer[i].trigger == "on") {timers.timer[i].trigger = '<span class="label label-success">'+jsonResponse["LangOn."]+'</span>';}
   if (timers.timer[i].trigger == "off") {timers.timer[i].trigger = '<span class="label label-danger">'+jsonResponse["LangOff."]+'</span>';}
   timers.timer[i].day = jsonResponse["Lang"+timers.timer[i].day];
   html += '<li>'+timers.timer[i].trigger+' <b>'+timers.timer[i].day+'<\/b> '+timers.timer[i].time+'<\/li>';
  }
  document.getElementById(timerids).innerHTML = (html?html:'<li>No timers</li>');
 }
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
