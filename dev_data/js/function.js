var xmlHttp=createXmlHttpObject();
function createXmlHttpObject(){
 if(window.XMLHttpRequest){
  xmlHttp=new XMLHttpRequest();
 }else{
  xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');
 }
 return xmlHttp;
}
function load(first){
 var xmlHttp=createXmlHttpObject();
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
  xmlHttp.open('PUT','/configs.json',true);
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
    if (first == 'first') {
     toggle('content');
     loadBlock(jsonResponse);
    } else {
     handleServerResponse(first);
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
 handleServerResponse();
}

function val(id,val){
 if (val) {
  document.getElementById(id).value=(val==' '?'':val);
 } else {
  var v = document.getElementById(id).value;
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

function language(submit){
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
  var html = "";
  for(var key in jsonWifi) {
   html += '<li><a href="#" onclick="val(\'ssid\',\''+jsonWifi[key].ssid+'\');toggle(\'ssid-select\');document.getElementById(\'ssid-name\').innerHTML=\''+jsonWifi[key].ssid+'\';return false"><b>' +jsonWifi[key].ssid + jsonWifi[key].pass + '</b> (' +jsonWifi[key].dbm + ' dBm)</a></li>';
  }
  document.getElementById(ssids).innerHTML = html+'<li><a href="#" onclick="toggle(\'ssid-group\');toggle(\'ssid\');return false"><b>'+jsonResponse.LangHiddenWifi+'</b></a></li>';
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
   html += '<li><a href="#" onclick="language(\''+view_lang+'\')" title="'+jsonLang[key].name+'">'+view_lang+'</a></li>';
  }
  document.getElementById(langids).innerHTML = html;
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
 submit.parentNode.classList.add('hidden');
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
