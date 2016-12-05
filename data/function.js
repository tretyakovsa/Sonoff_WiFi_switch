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
   xmlHttp.open('PUT','/lang.'+jsonResponse1.lang+'.json',true);
   xmlHttp.send(null);
   xmlHttp.onload = function(e) {
    jsonResponse2=JSON.parse(xmlHttp.responseText);
    jsonResponse = Object.assign(jsonResponse1, jsonResponse2);
    loadBlock(jsonResponse,first);
   }
  }
 }
}

function loadBlock(jsonResponse,first) {
 data = document.getElementsByTagName('body')[0].innerHTML;
 var new_string;
 for (var key in jsonResponse) {
  new_string = data.replace(new RegExp('{{'+key+'}}', 'g'), jsonResponse[key]);
  data = new_string;
 }
 document.getElementsByTagName('body')[0].innerHTML = new_string;
 handleServerResponse();
 if (first == 'first') {
  toggle('content');
 }
}

function val(id){
 var v = document.getElementById(id).value;
 return v;
}
function send_request(submit,server){
 request = new XMLHttpRequest();
 request.open("GET", server, true);
 request.send();
 save_status(submit,request);
}
function save_status(submit,request){
 old_submit = submit.value;
 request.onreadystatechange = function() {
  if (request.readyState != 4) return;
  submit.value = request.responseText;
  setTimeout(function(){
   submit.value=old_submit;
   submit_disabled(false);
  }, 1000);
 }
 submit.value = 'Loading...';
 submit_disabled(true);
}
function submit_disabled(request){
 var inputs = document.getElementsByTagName("input");
 for (var i = 0; i < inputs.length; i++) {
  if (inputs[i].type === 'submit') {inputs[i].disabled = request;}
 }
}
function toggle(target) {
 var curVal = document.getElementById(target).classList;
 if (curVal.contains('hidden')) {
  curVal.remove('hidden');
  curVal.add('show');
 } else {
  curVal.remove('show');
  curVal.add('hidden');
 }
}

function language(set,submit){
 server = "/lang?set="+set;
 send_request(submit,server);
 setTimeout(function(){
  location.reload();
 }, 1000);
}
