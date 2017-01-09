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
     handleServerResponse();
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
  load();
 }
}

function submit_disabled(request){
 var inputs = document.getElementsByTagName("input");
 for (var i = 0; i < inputs.length; i++) {
  if (inputs[i].type === 'submit') {inputs[i].disabled = request;}
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
 xmlHttp.open('GET',"/lang?set="+submit.value,true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  location.reload();
 }
}

function LoadWifi(ssids){
 document.getElementById("ssid-select").innerHTML += '<option value="">'+jsonResponse.LangLoading+'</option>';
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET','/wifi.scan.json',true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  var jsonWifi=JSON.parse(xmlHttp.responseText);
  var html = "<option value="+ssids+">"+ssids+"</option>";
  for(var key in jsonWifi) {
   html += "<option value="+jsonWifi[key].ssid+">" +jsonWifi[key].ssid + jsonWifi[key].pass + " (" +jsonWifi[key].dbm + " dBm)</option>"
  }
  document.getElementById("ssid-select").innerHTML = html+'<option value="">'+jsonResponse.LangHiddenWifi+'</option>';
 }
}

function LoadLang(language){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET','/lang.list.json',true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  var jsonLang=JSON.parse(xmlHttp.responseText);
  var html = "<option value="+language+">"+language+"</option>";
  for(var key in jsonLang) {
   var view_lang = jsonLang[key].name.substr(10,2);
   html += "<option value="+view_lang+">"+view_lang+"</option>";
  }
  document.getElementById("language").innerHTML = html;
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
