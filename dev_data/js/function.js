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
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
  xmlHttp.open('PUT','/configs.json',true);
  xmlHttp.send(null);
  xmlHttp.onload = function(e) {
   jsonResponse1=JSON.parse(xmlHttp.responseText);
   xmlHttp.open('GET','/lang/lang.'+jsonResponse1.lang+'.json',true);
   xmlHttp.send(null);
   xmlHttp.onload = function(e) {
    jsonResponse2=JSON.parse(xmlHttp.responseText);
    jsonResponse = Object.assign(jsonResponse1, jsonResponse2);
    if (first == 'first') {
     toggle('content');
     loadBlock(jsonResponse,first);
    } else {
     handleServerResponse();
    }
   }
  }
 }
}

function loadBlock(jsonResponse,first) {
 var data = document.getElementsByTagName('body')[0].innerHTML;
 var new_string;
 for (var key in jsonResponse) {
  new_string = data.replace(new RegExp('{{'+key+'}}', 'g'), jsonResponse[key]);
  data = new_string;
 }
 document.getElementsByTagName('body')[0].innerHTML = new_string;
 handleServerResponse();
}

function val(id){
 var v = document.getElementById(id).value;
 return v;
}
function send_request(submit,server){
 old_submit = submit.value;
 submit.value = 'Loading...';
 submit_disabled(true);
 request = new XMLHttpRequest();
 request.open("GET", server, true);
 request.send(null);
 request.onload = function(e) {
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

function language(set,submit){
 server = "/lang?set="+set;
 send_request(submit,server);
 setTimeout(function(){
  location.reload();
 }, 1000);
}

function LoadWifi(ssids){
 document.getElementById("ssid-select").innerHTML  += '<option value="">Loading...</option>';
 xmlHttp.open('GET','/wifiscan.json',true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  jsonWifi=JSON.parse(xmlHttp.responseText);
  html = "<option value="+ssids+">"+ssids+"</option>";
  for(var key in jsonWifi) {
   html += "<option value="+jsonWifi[key].ssid+">" +jsonWifi[key].ssid + jsonWifi[key].pass + " (" +jsonWifi[key].dbm + " dBm)</option>"
  }
  document.getElementById("ssid-select").innerHTML = html+'<option value="" disabled>———</option><option value="" onclick="toggle(\'ssid-select\');toggle(\'ssid\');">'+jsonResponse.LangHiddenWifi+'</option>';
 }
}
