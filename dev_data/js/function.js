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

function setContent(stage) {
 jsonResponse = '';
 var xmlHttp=createXmlHttpObject();
 var pages = window.location.search.substring(1).split("&");
 pages[0] = (pages[0]?pages[0]:'index');
 xmlHttp.open('GET', pages[0]+".json",true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  document.getElementById('download-json').href = pages[0]+".json";
  var jsonPage;
  if (xmlHttp.status==200){
   jsonPage=JSON.parse(xmlHttp.responseText);
   var jsonEdit=xmlHttp.responseText;
   if (jsonPage.configs){
    var fileNumber = 0;
    (function foo(){
     xmlHttp.open('GET', jsonPage.configs[fileNumber],true);
     xmlHttp.send(null);
     xmlHttp.onload = function() {
      if (xmlHttp.status==200){
       var jsonResponseNew = JSON.parse(xmlHttp.responseText);
       var jsonResponseOld = jsonResponse;
       jsonResponse = Object.assign(jsonResponseNew, jsonResponseOld);
      } else {
       document.getElementById('url-content').innerHTML += '<li class="alert alert-danger" style="margin:5px 0px;">File "'+jsonPage.configs[fileNumber]+'" not found.<\/li>';
      }
      fileNumber++;
      if(fileNumber == jsonPage.configs.length) {
       for (var y=0; y < pages.length; y++) {
        jsonResponse["urlArray"+[y]] = pages[y];
       }
       jsonResponse.urlArray=window.location.search.substring(1);
       var theCookies = document.cookie.split(';');
       for (var y = 1 ; y <= theCookies.length; y++) {
        jsonResponse[theCookies[y-1].split("=")[0].replace(/^ /,'')] = theCookies[y-1].split("=")[1];
       }
       document.title = renameBlock(jsonResponse, jsonPage.title);
       document.getElementById('title').innerHTML = renameBlock(jsonResponse, jsonPage.title);
       document.getElementById("contents").className = jsonPage.class;
       document.getElementById("contents").style = jsonPage.style;
       document.getElementById('contents').innerHTML = '';
       if (jsonPage.reload) {
        set_real_time = setTimeout("setContent('edit')", jsonPage.reload);
       }
       if (stage == 'first') {
        document.body.innerHTML += '<a href="/donate.htm" class="hidden-xs btn btn-link" target="_blank" style="position:fixed;bottom:0px;">'+(jsonResponse.LangDonate?jsonResponse.LangDonate:'Donate')+'<\/a>';
        val('edit-json', jsonEdit);
        toggle('content','hide');
       } else {
        jsonPage=JSON.parse(val('edit-json'));
       }
       val('edit-butt','view');
       if (jsonPage.content){
        viewTemplate(jsonPage,jsonResponse,'contents');
       } else {
        document.getElementById('url-content').innerHTML += '<li class="alert alert-danger" style="margin:5px 0px;">content array not found in "'+pages[0]+'.json"<\/li>';
        document.getElementById('contents').innerHTML += '<br><br><h1>File "'+pages[0]+'.json" cannot view.<\/h1><hr><h2>You can edit it right.<\/h2>';
        toggle('edit-content');
        toggle('url-content');
       }
      }
      if(fileNumber < jsonPage.configs.length) {
       jsonPage.configs[fileNumber] = renameBlock(jsonResponse, jsonPage.configs[fileNumber]);
       foo();
      }
     }
    })()
   } else {
    document.getElementById('contents').innerHTML = '<br><br><h1>File "'+pages[0]+'.json" cannot view.<\/h1><hr><h2>Please add configs array.<br>Example: "configs":["/config.live.json"]<br>You can edit this file on right side of this page.<\/h2>';
    val('edit-json', jsonEdit);
    toggle('content','hide');
    toggle('edit-content');
    toggle('url-content');
   }
  } else {
   document.getElementById('contents').innerHTML += '<br><br><h1>Files "'+pages[0]+'.json" not found.<\/h1><hr><h2>Maybe you want to open some file of these:<\/h2><h3 id="file-list">Loading...<\/h3>';
   toggle('content','hide');
   xmlHttp.open('GET', '/list?dir=/',true);
   xmlHttp.send(null);
   xmlHttp.onload = function() {
    if (xmlHttp.status==200){
     html('file-list',' ');
     var jsonFiles = JSON.parse(xmlHttp.responseText);
     for(var i = 0;i<jsonFiles.length;i++) {
      if (jsonFiles[i].name.substr(-4) == 'json'){
       document.getElementById('file-list').innerHTML += '<a href="/page.htm?'+jsonFiles[i].name.slice(0,-5)+'">'+jsonFiles[i].name+'<\/a><br>';
      }
     }
    }
   }
  }
 }
}

function searchModule(modules,find) {
 var findModules = '';
 for(var key in modules) {
  if (modules[key] == find) {
   findModules = 1;
  }
 }
 return findModules;
}

function viewTemplate(jsonPage,jsonResponse,idName) {
 var element = document.getElementById(idName);
 for(var i = 0;i<jsonPage.content.length;i++) {
  if (!jsonPage.content[i].module || searchModule(jsonResponse.module,jsonPage.content[i].module)) {
   var action_val = renameGet(jsonPage.content[i].action);
   var name_val = (jsonPage.content[i].name?jsonPage.content[i].name:'');
   //    var title_val = renameBlock(jsonResponse, jsonPage.content[i].title);
   var class_val = (jsonPage.content[i].class?jsonPage.content[i].class:'');
   var style_val = (jsonPage.content[i].style?'style="'+jsonPage.content[i].style+'"':'');
   var pattern_val = (jsonPage.content[i].pattern?jsonPage.content[i].pattern:'');
   var state_val = renameBlock(jsonResponse, jsonPage.content[i].state);
   var response_val = renameBlock(jsonResponse, jsonPage.content[i].response);
   var module_val = jsonPage.content[i].module;
   var type_val=jsonPage.content[i].type;
   if (type_val == 'hr') {
    element.innerHTML += '<hr id="'+name_val+'" class="'+class_val+'" '+style_val+'>';
   }
   if (type_val == 'h1' || type_val == 'h2' || type_val == 'h3' || type_val == 'h4' || type_val == 'h5' || type_val == 'h6') {
    element.innerHTML += '<'+type_val+' id="'+name_val+'" class="'+class_val+'" '+style_val+'>'+renameBlock(jsonResponse, jsonPage.content[i].title)+'<\/'+type_val+'>';
   }
   if (type_val == 'input') {
    if (action_val) action_val = 'onfocusout="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+jsonPage.content[i].action+'\'),\''+response_val+'\')"';
    element.innerHTML += '<input '+action_val+' id="'+name_val+'" class="form-control '+class_val+'" '+style_val+' '+(pattern_val?'pattern="'+pattern_val+'"':'')+' placeholder="'+renameBlock(jsonResponse, jsonPage.content[i].title)+'" value="'+state_val+'">';
   }
   if (type_val == 'password') {
    if (action_val) action_val = 'onfocusout="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+jsonPage.content[i].action+'\'),\''+response_val+'\')"';
    element.innerHTML += '<input '+action_val+' id="'+name_val+'" class="form-control '+class_val+'" '+style_val+' '+(pattern_val?'pattern="'+pattern_val+'"':'')+' placeholder="'+renameBlock(jsonResponse, jsonPage.content[i].title)+'" value="'+state_val+'" onfocus="this.type=\'text\'" type="password">';
   }
   if (type_val == 'button') {
    if (action_val) action_val = 'onclick="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+jsonPage.content[i].action+'\'),\''+response_val+'\')"';
    element.innerHTML += '<input id="'+name_val+'" '+action_val+' class="'+class_val+'" '+style_val+' value="'+renameBlock(jsonResponse, jsonPage.content[i].title)+'" type="button">';
   }
   if (type_val == 'checkbox') {
    var checked = '';
    if (state_val == 1){ checked = 'checked'; }
    if (action_val) { action_val = 'onchange="val(this.id,(this.checked?\'1\':\'0\'));send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+jsonPage.content[i].action+'\'),\''+response_val+'\')"'; } else { action_val = 'onchange="val(this.id,(this.checked?\'1\':\'0\'));"'; }
    element.innerHTML += '<label '+style_val+'><input id="'+name_val+'" value="'+state_val+'" '+action_val+' type="checkbox" class="'+class_val+'" '+checked+'> '+renameBlock(jsonResponse, jsonPage.content[i].title)+'<\/label>';
   }
   if (type_val == 'range') {
    if (action_val) action_val = 'onchange="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+jsonPage.content[i].action+'\'),\''+response_val+'\')"';
    element.innerHTML += '<label '+style_val+' style="display:block;"><h4>'+renameBlock(jsonResponse, jsonPage.content[i].title)+'<\/h4> <input id="'+name_val+'" class="form-control '+class_val+'" '+action_val+' '+pattern_val+' value="'+state_val+'" type="range"><\/label>';
   }
   if (type_val == 'table') {
    var thead = '';
    var jsonTable = jsonPage.content[i].title;
    element.innerHTML += '<table class="'+class_val+'" '+style_val+' id="'+name_val+'"><thead id="thead-'+state_val.replace(/[^a-z0-9]/gi,'-')+'"><tr><td><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center></td></tr><\/thead><tbody id="tbody-'+state_val.replace(/[^a-z0-9]/gi,'-')+'"><\/tbody><\/table>';
    loadTable(state_val, jsonTable);
   }
   if (type_val == 'select') {
    if (action_val) action_val = 'onchange="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+jsonPage.content[i].action+'\'),\''+response_val+'\')"';
    var option = '';
    jsonSelect = jsonPage.content[i].title;
    //jsonSelect = JSON.parse(renameBlock(jsonResponse, jsonPage.content[i].title));
    //jsonSelect = renameBlock(jsonResponse, jsonPage.content[i].title).slice(1, -1);
    for(var key in jsonSelect) {
     option += '<option value="'+renameBlock(jsonResponse, key)+'"'+(state_val==key?' selected':'')+'>'+renameBlock(jsonResponse, jsonSelect[key])+'<\/option>';
    }
    element.innerHTML += '<select class="form-control '+class_val+'" '+style_val+' '+action_val+' id="'+name_val+'">'+option+'<\/select>';
   }
   if (type_val == 'configs') {
    var htmlopt = '';
    htmlopt += '<div id="'+name_val+'"><div id="'+state_val.replace(/[^a-z0-9]/gi,'-')+'" class="'+renameBlock(jsonResponse, '{{'+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden}}')+'" '+style_val+'><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center><\/div><\/div>';
    htmlopt += '<div class="btn-group btn-block"><input  style="width:85%" onclick="changeTextarea(\''+state_val.replace(/[^a-z0-9]/gi,'-')+'\');send_request_edit(this, val(\''+state_val.replace(/[^a-z0-9]/gi,'-')+'-edit\'),\'configs/'+state_val+'\');alert(\''+jsonResponse.LangReset2+'\')" class="btn btn-block btn-success" value="'+jsonResponse.LangSave+'" type="button">';
    htmlopt += '<a href="#" style="width:15%" class="btn btn-info dropdown-toggle" onclick="toggle(\'cloud\');return false"><i class="cloud-img"></i> <span class="caret"></span></a>';
    htmlopt += '<ul class="dropdown-menu hidden" style="right:0;left:auto" id="cloud"><li><a onclick="toggle(\'cloud\');cloudUpload(\''+jsonResponse.mac+'\',\''+jsonResponse.configs+'\');alert(\''+jsonResponse.LangReset2+'\');return false" href="#"><i class="cloud-img"></i> '+jsonResponse.LangCloudUpload+'</a></li><li><a onclick="toggle(\'cloud\');cloudDownload(\''+jsonResponse.mac+'\',\''+jsonResponse.configs+'.txt\');alert(\''+jsonResponse.LangReset2+'\');return false" href="#"><i class="cloud-img"></i> '+jsonResponse.LangCloudDownload+'</a></li></ul>';
    htmlopt += '</div>';
    element.innerHTML += htmlopt;
    setTimeout("loadConfigs('"+state_val+"')", 500);
   }
   if (type_val == 'link') {
    element.innerHTML += '<a id="'+name_val+'" class="'+class_val+'" '+style_val+' href="'+renameGet(jsonPage.content[i].action)+'">'+renameBlock(jsonResponse, jsonPage.content[i].title)+'<\/a>';
   }
   if (type_val == 'text') {
    element.innerHTML += '<div id="'+name_val+'" class="'+class_val+'" '+style_val+'>'+renameBlock(jsonResponse, jsonPage.content[i].title)+'<\/div>';
   }
   if (type_val == 'iframe') {
    element.innerHTML += renameBlock(jsonResponse, jsonPage.content[i].title)+'<iframe src="'+state_val+'" id="'+name_val+'" class="'+class_val+'" '+style_val+'><\/iframe>';
   }
   if (type_val == 'chart') {
    element.innerHTML += '<div id="'+name_val+'" class="'+renameBlock(jsonResponse, '{{'+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden}}')+'"><button class="close" onclick="hide(\''+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden\',this);" type="button">×<\/button><a href="'+renameGet(jsonPage.content[i].action)+'" target="_blank" class="close">'+(typeof action_val!='undefined'&&action_val?'<i class="popup-img"><\/i>':'')+'<\/a><h2><span id="'+state_val.replace(/[^a-z0-9]/gi,'')+'-title">'+renameBlock(jsonResponse, jsonPage.content[i].title)+'</span> <span id="'+state_val.replace(/[^a-z0-9]/gi,'')+'-data"></span><\/h2><div id="'+state_val.replace(/[^a-z0-9]/gi,'')+'" class="'+class_val+'" '+style_val+'><\/div><hr><\/div>';
    if (renameBlock(jsonResponse, '{{'+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden}}') != 'hidden') {
     setTimeout("loadChart('"+state_val.replace(/[^a-z0-9]/gi,'')+"','"+state_val+"', {"+jsonPage.content[i].options+"},"+jsonPage.content[i].refresh+","+jsonPage.content[i].points+")", 500);
    }
   }
   if (type_val == 'loadJson') {
    element.innerHTML += '<div id="json-'+state_val.replace(/[^a-z0-9]/gi,'-')+'" class="'+class_val+'" '+style_val+'><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center><\/div>';
    loadJson(state_val, jsonResponse, 'json-'+state_val.replace(/[^a-z0-9]/gi,'-'));
   }
   if (type_val == 'scenary-list') {
    element.innerHTML += '<table class="'+class_val+'" '+style_val+' id="'+name_val+'"><tbody id="scenary-list"><\/tbody><\/table>';
    loadScenary(jsonResponse,'loadList');
   }
   if (type_val == 'scenary') {
    var option = '';
    option += '<select class="form-control" id="ssdp-list0" style="width:50%;display:inline" onchange="loadScenaryList(0,\'loadInTextarea\',this.options[this.selectedIndex].value);loadLive(this.value,\'config.live.json\',\'ssdp-module\')"><\/select>';
    option += '<select class="form-control" id="ssdp-module" style="width:50%;display:inline" onchange="pattern(this.querySelector(\':checked\').getAttribute(\'title\'));"><\/select>';
    option += '<select class="form-control" id="ssdp-condition" style="width:50%;display:inline"><option value="=">'+jsonResponse.LangEqual+' (=)<\/option><option value="<">'+jsonResponse.LangLess+' (<)<\/option><option value=">">'+jsonResponse.LangMore+' (>)<\/option><option value="<=">'+jsonResponse.LangLess+' '+jsonResponse.LangOr+' '+jsonResponse.LangEqual+' (<=)<\/option><option value=">=">'+jsonResponse.LangMore+' '+jsonResponse.LangOr+' '+jsonResponse.LangEqual+' (>=)<\/option><option value="!=">'+jsonResponse.LangNotEqual+' (!=)<\/option><\/select>';
    option += '<input class="form-control" id="ssdp-command" pattern="" style="width:40%;display:inline" value=""><a href="#" class="btn btn-default" style="width:10%;" onclick="loadLive2();return false"><i class="find-replace-img"></i></a>';
    option += '<br><h3>'+jsonResponse.LangThen+'</h3> ';
    option += '<select class="form-control" id="ssdp-list1" style="width:50%;display:inline" onchange="loadLive(this.value,\'command.json\',\'scenary-then\')"><\/select>';
    option += '<select class="form-control" style="width:50%;display:inline" id="scenary-then"><\/select>';
    option += '<input class="form-control" placeholder="Optional" id="scenary-othe" type="text" /><textarea id="scenary-list-edit" style="display:none" class="form-control"></textarea>';
    option += "<input onclick=\"loadInTextarea();send_request_edit(this, val('scenary-list-edit'),'scenary.save.txt','send_request(this,\\'http://\\'+document.getElementById(\\'ssdp-list0\\').options[document.getElementById(\\'ssdp-list0\\').selectedIndex].value+\\'/setscenary\\');loadScenary(jsonResponse,\\'loadList\\');val(\\'ssdp-list0\\', \\' \\');val(\\'ssdp-command\\', \\' \\');val(\\'ssdp-list1\\', \\' \\');document.getElementById(\\'ssdp-module\\').options.length=0;document.getElementById(\\'scenary-then\\').options.length=0;',document.getElementById('ssdp-list0').options[document.getElementById('ssdp-list0').selectedIndex].value);\" class=\"btn btn-block btn-success\" value=\""+jsonResponse.LangSave+"\" type=\"button\">";
    element.innerHTML += '<h3>'+jsonResponse.LangIf+'</h3> '+option;
    loadScenary(jsonResponse);
   }
   if (type_val == 'wifi') {
    element.innerHTML += '<div class="btn-group btn-block" id="ssid-group"><a href="#" class="btn btn-default btn-block dropdown-toggle" onclick="toggle(\'ssid-select\');loadWifi(\'ssid-select\',\''+name_val+'\');return false"><span id="ssid-name">'+state_val+'<\/span> <span class="caret"><\/span><\/a><ul class="dropdown-menu hidden" id="ssid-select"><li><a href="#">'+jsonResponse.LangLoading+'<\/a><\/li><\/ul><\/div>';
    element.innerHTML += '<input id="'+name_val+'" value="'+state_val+'" class="form-control hidden '+class_val+'" '+style_val+' '+pattern_val+' placeholder="'+renameBlock(jsonResponse, jsonPage.content[i].title)+'">';
   }
   if (type_val == 'time' && typeof jsonResponse.time !== "undefined") {
    element.innerHTML += '<h2 id="'+name_val+'" '+style_val+'>'+renameBlock(jsonResponse, jsonPage.content[i].title)+' <strong id="time" class="'+class_val+'">'+state_val+'<\/strong><\/h2>';
    clearTimeout(set_real_time);
    var res = jsonResponse.time.split(":");
    real_time(hours=res[0],min=res[1],sec=res[2]);
   }
   if (type_val == 'rgb') {
    element.innerHTML += '<div class="'+name_val+'-thumb '+class_val+'"><div class="'+name_val+'-preview"><\/div><img alt="" '+style_val+' src="'+renameBlock(jsonResponse, jsonPage.content[i].title)+'"><\/div><canvas id="'+name_val+'-cs" style="display:none"><\/canvas>';
    element.innerHTML += '<input id="'+name_val+'" value="'+state_val+'" class="form-control hidden">';
    setTimeout("createRGB('"+name_val+"', '"+jsonPage.content[i].action+"','"+module_val+"','"+response_val+"')", 500);
   }
   if (type_val == 'dev') {
    var dev_html = '<div id="'+name_val+'" class="'+class_val+'" '+style_val+'><a href="/help.htm" target="_blank" class="close"><i class="help-img"><\/i><\/a>'+renameBlock(jsonResponse, jsonPage.content[i].title)+'<span id="dev-update" class="hidden"><a href="/edit" class="btn btn-default" target="_blank">File manager<\/a> <a href="/page.htm?starting" class="btn btn-default">Starting log<\/a> ';
    if (searchModule(jsonResponse.module,"upgrade")){
     dev_html += ' <div class="btn-group"><a href="#" class="btn btn-danger dropdown-toggle" onclick="toggle(\'repos-all\');loadBuild(\'sonoff\',\'all\');return false">Upgrade Build & Spiffs <span class="caret"><\/span><\/a><ul class="dropdown-menu hidden" id="repos-all" style="min-width:350px"><li><a href="https://github.com/tretyakovsa/Sonoff_WiFi_switch/commits/master" style="text-align:right" target="_blank"><i class="help-img"><\/i> Github code history<\/a><ul id="sonoff-all" style="margin-right:20px"><li><a href="#">'+jsonResponse.LangLoading+'<\/a><\/li><\/ul><\/li><\/ul><\/div>';
    }
    dev_html += '<br><b><a href="#" onclick="toggle(\'repos-bin\');return false">'+jsonResponse.LangOtheSetting+'<\/a><\/b><span id="repos-bin" class="hidden">';
    dev_html += '<form method="POST" action="/update" enctype="multipart/form-data"><div class="btn-group"><input type="file" class="btn btn-primary btn-xs" name="update" style="height:33px" accept=".bin"><input type="submit" class="btn btn-default btn-sm" value="Update build" onclick="this.value=\''+jsonResponse.LangLoading+'\';" style="height:33px"><\/div><\/form><hr>';
    dev_html += jsonResponse.LangType+': <div class="btn-group"><select class="btn btn-default btn-sx" onchange="send_request(this, \'/configs?set=\'+this.value,\'[[configs-edit-button]]\')"><option value="'+jsonResponse.configs+'">'+jsonResponse.configs+'<\/option><option value="sonoff-rf">Sonoff-rf / Sonoff / Wi-Fi Smart socket<\/option><option value="rgb">RGB (WS2811-12/NeoPixel)<\/option><option value="jalousie">Jalousie<\/option><option value="leakag">Leakag<\/option><option value="smart-room">Smart-Room<\/option><option value="5v-wifi-relay">5v WiFi Relay<\/option><option value="manually">Manually</option></select> <a href="/page.htm?configs&'+jsonResponse.configs.toLowerCase()+'" id="configs-edit-button" class="btn btn-primary">Edit<\/a><\/div>';
    dev_html += '<\/span><\/span><\/div>';
    element.innerHTML += dev_html;
   }
  }
 }
}

function loadJson(state_val, jsonResponse, idName) {
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("GET", state_val+"?"+Math.floor(Math.random()*10000), true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  html(idName, ' ');
  jsonPage=JSON.parse(xmlHttp.responseText);
  viewTemplate(jsonPage,jsonResponse,idName);
 }
}

function pattern(s) {
 document.getElementById("ssdp-command").setAttribute("pattern","["+(s=='number'?'0-9':'0-9a-zA-Z:')+"]{1,15}");
}

function loadScenaryList(jsonResponse,selectDevice,urlList) {
 var xhttp=createXmlHttpObject();
 xhttp.open("GET", (urlList?'http://'+urlList:'')+"/scenary.save.txt?"+Math.floor(Math.random()*10000), true);
 xhttp.send(null);
 xhttp.onload = function() {
  var ipDevice=xhttp.responseText;
  if (selectDevice == 'loadInTextarea') {
   //html("scenary-list-edit",ipDevice);
   document.getElementById("scenary-list-edit").innerHTML = ipDevice;
  } else if (Number.isInteger(selectDevice) == true) {
   var reg = new RegExp("([\\s\\S]+?)(id\\s+\\d+)", "mig");
   send_request_edit(this, ipDevice.replace(reg,function(a,b,c){return new RegExp("^id+\\s+"+selectDevice+"$").test(c)?"":a}),'scenary.save.txt','html(\'scenary-list\', \' \');send_request(this,\'http://'+urlList+'/setscenary\');loadScenary(jsonResponse,\'loadList\');',urlList);
  } else {
   var createText = '';
    var block = ipDevice.split(/\n| /);
    for (var i = 0 ; i < block.length; i++) {
     createText += ' '+(renameBlock(jsonResponse, '{{Lang'+block[i]+'}}')===undefined?block[i]:renameBlock(jsonResponse, '{{Lang'+block[i]+'}}'));
    }
   document.getElementById("scenary-list").innerHTML += '<tr><td colspan="2"><h4><a href="http://'+urlList+'">'+selectDevice+'</a></h4></td></tr>'+createText.replace(/if /gi,'<tr><td><b>'+jsonResponse.LangIf+'</b> ').replace(/and /gi,'<b>and</b> ').replace(/then /gi,'<b>'+jsonResponse.LangThen+'</b> ').replace(/(id)\s+(\d+)/mg, '<\/td><td><a class="btn btn-sm btn-danger" style="float:right;" href="#" onclick="if(confirm(\''+jsonResponse.LangDel+'?\')){loadScenaryList(jsonResponse,$2,\''+urlList+'\');}return false"><i class="del-img"></i> <span class="hidden-xs">'+jsonResponse.LangDel+'</span></a><\/td><\/tr>');
  }
 }
}

function loadScenary(jsonResponse,loadList) {
 html('scenary-list', '<tr><td colspan="2"><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center></td></tr>');
 var xhttp=createXmlHttpObject();
 xhttp.open("GET", "/ssdp.list.json?"+Math.floor(Math.random()*10000), true);
 xhttp.send(null);
 xhttp.onload = function() {
  html('scenary-list', ' ');
  var option = '';
  var ipDevice=JSON.parse(xhttp.responseText);
  //ipDevice = Object.keys(ipDevice).sort((a, b) => ipDevice[b] - ipDevice[a]);
  if (loadList) {
   for (var i in ipDevice) {
    loadScenaryList(jsonResponse,i,ipDevice[i]);
   }
  } else {
   for (var i in ipDevice) {
    option += '<option value="'+ipDevice[i]+'">'+i+'<\/option>';
   }
   html("ssdp-list0",'<option value="">'+jsonResponse.LangSelect+'<\/option>'+option);
   html("ssdp-list1",'<option value="">'+jsonResponse.LangSelect+'<\/option>'+option);
  }
 }
}

function loadLive(ip,file,where) {
 html(where,'<option value="">Loading...</option>');
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET', "http://"+ip+"/"+file,true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  var option = '';
  var jsonLive=JSON.parse(xmlHttp.responseText);
  for(var key in jsonLive) {
   option += '<option value="'+key+'" title="'+typeof jsonLive[key]+'">'+(renameBlock(jsonResponse, '{{Lang'+key+'}}')===undefined?key:renameBlock(jsonResponse, '{{Lang'+key+'}}'))+'<\/option>';
  }
  html(where,'<option value="">'+jsonResponse.LangSelect+'<\/option>'+option);
 }
}

function loadLive2() {
 val("ssdp-command","Loading...");
 var ip = document.getElementById("ssdp-list0").options[document.getElementById("ssdp-list0").selectedIndex].value;
 var who = document.getElementById("ssdp-module").options[document.getElementById("ssdp-module").selectedIndex].value;
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET', "http://"+ip+"/config.live.json",true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  var jsonLive=JSON.parse(xmlHttp.responseText);
  for(var key in jsonLive) {
   if (key == who) {
    document.getElementById("ssdp-command").value= jsonLive[key];
   }
  }
 }
}

function loadInTextarea() {
 var element = document.getElementById("scenary-list-edit");
 element.innerHTML += '\r\n\r\nif '+document.getElementById("ssdp-module").options[document.getElementById("ssdp-module").selectedIndex].value+' '+document.getElementById("ssdp-condition").options[document.getElementById("ssdp-condition").selectedIndex].value+' '+document.getElementById("ssdp-command").value;
 element.innerHTML += '\r\nthen '+document.getElementById("ssdp-list1").options[document.getElementById("ssdp-list1").selectedIndex].text+' '+document.getElementById("scenary-then").options[document.getElementById("scenary-then").selectedIndex].value+' '+document.getElementById("scenary-othe").value;
 element.innerHTML += '\r\nid '+Math.floor(Math.random()*10000);
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

function send_request_edit(submit,server,filename,geturl,gethost){
 var xmlHttp = new XMLHttpRequest();
 var old_submit = submit.value;
 submit.value = jsonResponse.LangLoading;
 submit_disabled(true);
 var formData = new FormData();
 formData.append("data", new Blob([server], { type: 'text/json' }), "/"+filename);
 xmlHttp.open("POST", (gethost?'http://'+gethost:'')+"/edit");
 xmlHttp.onload = function() {
  submit.value=old_submit;
  submit_disabled(false);
  if (geturl){
   //window.location = geturl;
   eval(geturl);
  }
 }
 xmlHttp.send(formData);
}

function send_request_post(submit,server,state){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("POST", server, true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  if (state != null && state!='undefined'){
   var response=JSON.parse(xmlHttp.responseText);
   var block = state.split(',');
   for (var i = 0 ; i < block.length; i++) {
    var element = document.getElementById(block[i].slice(2,-2));
    if (response.class && response.class!='undefined') {element.className = response.class;}
    if (response.style && response.style!='undefined') {element.style = response.style;}
    if (response.title && response.title!='undefined') {
     if (element.tagName == 'INPUT') {element.value = renameBlock(jsonResponse, response.title);}
     if (element.tagName == 'DIV' ||element.tagName == 'A' || element.tagName == 'H1' || element.tagName == 'H2' || element.tagName == 'H3' || element.tagName == 'H4' || element.tagName == 'H5' || element.tagName == 'H6') {element.innerHTML = renameBlock(jsonResponse, response.title);}
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
 xmlHttp.onload = function() {
  submit.value=old_submit;
  submit_disabled(false);
  var element =  document.getElementById('url-content');
  if (typeof(element) != 'undefined' && element != null){
   element.innerHTML += '<li><span class="label label-warning">GET</span> <a href="'+server+'" class="btn btn-link" style="text-transform:none;text-align:left;white-space:normal;display:inline">'+server+'</a> <span class="label label-'+(xmlHttp.status==200?'default':'danger')+'">'+xmlHttp.status+' '+xmlHttp.statusText+'</span></li>';
  }
  var ddnsUrl1 =  document.getElementById('ddns-url1');
  if (typeof(ddnsUrl1) != 'undefined' && ddnsUrl1 != null){
   ddnsUrl1.innerHTML = '<a href="http://'+location.hostname+'/'+server+'">http://'+location.hostname+'/'+server+'</a>';
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
     var element = document.getElementById(block[i].slice(2,-2));
     if (response.class && response.class!='undefined') {element.className = response.class;}
     if (response.style && response.style!='undefined') {element.style = response.style;}
     if (response.title && response.title!='undefined') {
      if (element.tagName == 'INPUT') {
       element.value = renameBlock(jsonResponse, response.title);
      }
      if (element.tagName == 'SELECT') {
       var option = '';
       jsonSelect = response.title;
       for(var key in jsonSelect) {
        option += '<option value="'+renameBlock(jsonResponse, key)+'">'+renameBlock(jsonResponse, jsonSelect[key])+'<\/option>';
       }
       element.innerHTML = option;
      }
      if (element.tagName == 'DIV' ||element.tagName == 'A' || element.tagName == 'H1' || element.tagName == 'H2' || element.tagName == 'H3' || element.tagName == 'H4' || element.tagName == 'H5' || element.tagName == 'H6') {element.innerHTML = renameBlock(jsonResponse, response.title);}
     }
     if (element.tagName == 'TABLE' && response.state) {
      loadTable(response.state,response.title);
     }
     if (element.tagName == 'A' && response.action) {
      element.href = response.action;
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
 var element = document.getElementsByTagName("input");
 for (var i = 0; i < element.length; i++) {
  if (element[i].type === 'button') {element[i].disabled = request;}
 }
}

function toggle(target,status) {
 var element = document.getElementById(target).classList;
 if (element.contains('hidden')) {
  if (status != 'show') {
   element.remove('hidden');
   element.add('show');
  }
 } else {
  if (status != 'hidden') {
   element.remove('show');
   element.add('hidden');
  }
 }
}

function loadWifi(ssids,hiddenIds){
 var xmlHttp=createXmlHttpObject();
 html(ssids, '<li><a href="#">Loading...</a></li>');
 xmlHttp.open('GET','/wifi.scan.json',true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  var jsonWifi=JSON.parse(xmlHttp.responseText);
  jsonWifi.networks.sort(function(a,b){return (a.dbm < b.dbm) ? 1 : ((b.dbm < a.dbm) ? -1 : 0);});
  var html = '';
  for(var i = 0;i<jsonWifi.networks.length;i++) {
   var wifiSignal = '';
   if (jsonWifi.networks[i].dbm <= -0) { wifiSignal = '<i class="wifi wifi-0-60"></i>';}
   if (jsonWifi.networks[i].dbm <= -60) { wifiSignal = '<i class="wifi wifi-60-70"></i>';}
   if (jsonWifi.networks[i].dbm <= -70) { wifiSignal = '<i class="wifi wifi-70-80"></i>';}
   if (jsonWifi.networks[i].dbm <= -80) { wifiSignal = '<i class="wifi wifi-80-90"></i>';}
   if (jsonWifi.networks[i].dbm <= -90) { wifiSignal = '<i class="wifi wifi-90-100"></i>';}
   html += '<li><a href="#" onclick="val(\''+hiddenIds+'\',\''+jsonWifi.networks[i].ssid+'\');toggle(\'ssid-select\');html(\'ssid-name\',\''+jsonWifi.networks[i].ssid+'\');return false"><div style="float:right">'+(jsonWifi.networks[i].pass?'<i class="wifi wifi-key"></i>':'')+' '+wifiSignal+' <span class="label label-default">'+jsonWifi.networks[i].dbm+' dBm</span></div><b>'+jsonWifi.networks[i].ssid+'</b></a></li>';
  }
  document.getElementById(ssids).innerHTML = (html?html:'<li><a href="#">Not found WiFi</a></li>')+'<li><a href="#" onclick="toggle(\'ssid-group\');toggle(\'ssid\');return false"><b>'+jsonResponse.LangHiddenWifi+'</b></a></li>';
 }
}

function loadBuild(buildids,typeFile){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET','http://backup.privet.lv/esp/build/'+buildids,true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  var jsonBuild=JSON.parse(xmlHttp.responseText);
  jsonBuild.sort(function(a,b){return (a.name< b.name) ? 1 : ((b.name < a.name) ? -1 : 0);});
  var html = '';
  for(var i = 0;i<jsonBuild.length;i++) {
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

function real_time(hours,min,sec) {
 var res = html('time').split(":");
 hours=res[0];
 min=res[1];
 sec=res[2];
 sec=Number(sec)+1;
 if (sec>=60){min=Number(min)+1;sec=00;}
 if (min>=60){hours=Number(hours)+1;min=00;}
 if (hours>=24){hours=00;}
 html('time',(hours<10&&hours.toString().length==1?"0"+hours:hours)+":"+(min<10&&min.toString().length==1?"0"+min:min)+":"+(sec<10?"0"+sec:sec));
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

function readCookie(name) {
 var nameEQ = name + "=";
 var ca = document.cookie.split(';');
 for (var i = 0; i < ca.length; i++) {
  var c = ca[i];
  while (c.charAt(0) == ' ') c = c.substring(1, c.length);
  if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length, c.length);
 }
 return null;
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

function hide(name, submit) {
 if (confirm(jsonResponse.LangHedden)) {
  submit.parentNode.classList.add('hidden');
  setCookie(name,'hidden',365,submit);
 }
}

function loadConfigs(state_val) {
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("GET", "configs/"+state_val, true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  var element = document.getElementById(state_val.replace(/[^a-z0-9]/gi,'-'));
  element.innerHTML = '';
  var configsLinePin;
  var configsLine = xmlHttp.responseText.match(/^.*((\r\n|\n|\r)|$)/gm);
  for(var key in configsLine) {
   if (configsLine[key].substr(0,2) == '//') {
    element.innerHTML += '<label><input checked="" type="checkbox" style="display:none" disabled readonly><small>'+configsLine[key]+'</small><\/label></br>';
   } else {
    configsLinePin = configsLine[key].replace(/# /,'').split(' ');
    element.innerHTML += '<label style="margin-bottom:25px;"><input type="checkbox" '+(configsLine[key].substring(0,2)!='# '?"checked":"")+'> '+configsLinePin[0]+'<\/label> '+(configsLinePin[1]?'<input class="form-control" style="display:inline;width:100px;" pattern="[a-zA-Z0-9\s]+" value="'+configsLinePin[1]+'">':'')+' '+(configsLinePin[2]?'<input class="form-control" style="display:inline;width:100px;" pattern="[a-zA-Z0-9\s]+" value="'+configsLinePin[2]+'">':'')+' '+(configsLinePin[3]?'<input class="form-control" style="display:inline;width:100px;" pattern="[a-zA-Z0-9\s]+" value="'+configsLinePin[3]+'">':'')+'</br>';
   }
  }
  element.innerHTML += '<textarea id="'+state_val.replace(/[^a-z0-9]/gi,'-')+'-edit" style="display:none" class="form-control"></textarea>';
  //changeTextarea(state_val.replace(/[^a-z0-9]/gi,'-'));
 }
}

function cloudUpload(mac,file) {
 changeTextarea(file+'-txt');
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("POST","http://backup.privet.lv/configs/?file="+mac+"-"+file,true);
 xmlHttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
 xmlHttp.send("data="+val(file+'-txt-edit'));
 //xmlHttp.onload = function () {
 send_request_edit(this, val(file+'-txt-edit'),'configs/'+file+'.txt');
 //}
}
function cloudDownload(mac,file) {
 submit_disabled(true);
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("GET", "http://backup.privet.lv/configs/"+mac+"-"+file+"?"+Math.floor(Math.random()*10000), true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  if(xmlHttp.status == 200) {
   var data = xmlHttp.responseText;
   send_request_edit(this, data,'configs/'+file+'','loadConfigs("'+file+'");');
  } else {
   alert('File not found in cloud.');
  }
 }
}

function changeTextarea(state_val) {
 val(state_val.replace(/[^a-z0-9]/gi,'-')+'-edit', ' ');
 var area = document.querySelector('textarea');
 [].forEach.call(document.querySelectorAll('#'+state_val+'>*'), function(el){
  if(el.matches('label')) {
   area.value += '\r\n'+(el.children[0].checked?'':'# ');
   if (el.textContent.substr(0,2) == '//') {
    area.value += el.textContent;
   } else {
    area.value += el.textContent.replace(/ /,'');
   }
  }
  if(el.matches('input')) area.value += ' '+el.value;
 });
 area.value = area.value.replace(/\n+/g,'\n').slice(1);
}

function loadTable(state_val, jsonTable) {
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("GET", state_val+"?"+Math.floor(Math.random()*10000), true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  var timers=JSON.parse(xmlHttp.responseText);
  var setTable = Object.keys(timers);
  html('thead-'+state_val.replace(/[^a-z0-9]/gi,'-'), ' ');
  html('tbody-'+state_val.replace(/[^a-z0-9]/gi,'-'), ' ');
  var thead = '';
  for(var key in jsonTable) {
   thead += '<th>'+renameBlock(jsonResponse, jsonTable[key])+'<\/th>';
  }
  document.getElementById('thead-'+state_val.replace(/[^a-z0-9]/gi,'-')).innerHTML += thead;
  for (var i = 0; i < timers[setTable].length; i++) {
   var tbody = '';
   for(var keys in jsonTable) {
    if (timers[setTable][i][keys] == "on") {timers[setTable][i][keys] = '<span class="label label-success">'+jsonResponse["LangOn."]+'<\/span>';}
    if (timers[setTable][i][keys] == "off") {timers[setTable][i][keys] = '<span class="label label-danger">'+jsonResponse["LangOff."]+'<\/span>';}
    if (timers[setTable][i][keys] == "open") {timers[setTable][i][keys] = '<span class="label label-success">'+jsonResponse["LangOpen"]+'<\/span>';}
    if (timers[setTable][i][keys] == "close") {timers[setTable][i][keys] = '<span class="label label-danger">'+jsonResponse["LangClose"]+'<\/span>';}
    if (timers[setTable][i][keys] == "not") {timers[setTable][i][keys] = '<span class="label label-info">'+jsonResponse["LangSwitch."]+'<\/span>';}
    if (timers[setTable][i][keys] == "relay") {timers[setTable][i][keys] = '<span class="label label-danger">relay<\/span>';}
    if (timers[setTable][i][keys] == "rgb") {timers[setTable][i][keys] = '<span class="label label-info">rgb<\/span>';}
    if (timers[setTable][i][keys] == "jalousie") {timers[setTable][i][keys] = '<span class="label label-success">jalousie<\/span>';}
    if (jsonTable[keys] == '{{LangDay}}') {timers[setTable][i][keys] = jsonResponse["Lang"+timers[setTable][i][keys]];}
    if (jsonTable[keys] == '{{LangDel}}') {timers[setTable][i][keys] = '<input class="btn btn-sm btn-danger" value="Удалить" onclick="if(confirm(\''+jsonResponse["LangDel"]+'?\')){send_request(this, renameGet(\'/timersDel?id='+timers[setTable][i][keys]+'\'),\'[[timer-list]]\');}" type="button">'}
    tbody += '<td>'+timers[setTable][i][keys]+'<\/td>';
   }
   document.getElementById('tbody-'+state_val.replace(/[^a-z0-9]/gi,'-')).innerHTML += '<tr>'+tbody+'<\/tr>';
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
   }
  }
 }
 if (typeof(str)!='undefined'&&str!=null&&str) {
  try {
   return eval(str);
  } catch(e) {
   return str;
  }
 } else {
  return '';
 }
 //return (typeof(str)!='undefined'&&str!=null?str:'');
}

function renameGet(str) {
 if (str) {
  var arr=str.match(/\[\[\S+?\]\]/gim);
  if (arr) {
   for (var i=0; i<arr.length; i++) {
    var id=arr[i].slice(2, -2);
    var element = document.getElementById(id);
    if (element) {
     var txt='';
     if (element.tagName=='SELECT'){
      txt=element.options[element.selectedIndex].value;
     }
     else if (element.tagName=='INPUT'){
      txt=encodeURIComponent(element.value);
     } else {
      txt=element.innerHTML;
     }
     str = str.replace(new RegExp('\\[\\['+id+'\\]\\]','g'), txt);
    }
   }
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
}
function useCanvas(el,image,callback){
 el.width = image.width;
 el.height = image.height;
 el.getContext('2d').drawImage(image, 0, 0, image.width, image.height);
 return callback();
}
function _(el){
 return document.querySelector(el);
}
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

function loadCommits(repos,viewCommits){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET','https://api.github.com/repos/'+repos+'/commits',true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  html('commits', ' ');
  var jsonCommits=JSON.parse(xmlHttp.responseText);
  //  jsonCommits.sort(function(a,b){return (a.updated_at< b.updated_at) ? 1 : ((b.updated_at < a.updated_at) ? -1 : 0);});
  for(var key in jsonCommits) {
   if (key < viewCommits) {
    document.getElementById('commits').innerHTML += '<p><span class="label label-default">&#8987; '+jsonCommits[key].commit.author.date.substring(0,10)+'<\/span> <a href="'+jsonCommits[key].html_url+'" target="_blank">'+jsonCommits[key].commit.message+'<\/a><\/p>';
   }
  }
 }
}

function loadIssues(repos,viewIssues){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET','https://api.github.com/repos/'+repos+'/issues',true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  html('issues', ' ');
  var jsonIssues=JSON.parse(xmlHttp.responseText);
  jsonIssues.sort(function(a,b){return (a.updated_at< b.updated_at) ? 1 : ((b.updated_at < a.updated_at) ? -1 : 0);});
  for(var key in jsonIssues) {
   if (jsonIssues[key].user.login == 'renat2985' || jsonIssues[key].user.login == 'tretyakovsa') {
    for(var label in jsonIssues[key].labels) {
     if (jsonIssues[key].labels[label].name == 'news') {
      if (readCookie("news-"+jsonIssues[key].id) != "hidden") {
       document.getElementById('news').innerHTML = '<div class="alert alert-dismissible alert-info"><button class="close" onclick="hide(\'news-'+jsonIssues[key].id+'\',this);" type="button">×<\/button>'+jsonIssues[key].title.substr(0, 100)+' <a href="'+jsonIssues[key].html_url+'" target="_blank">подробнее...<\/a><\/div>';
      }
     }
    }
   }
   if (key < viewIssues) {
    document.getElementById('issues').innerHTML += '<p><span class="label label-default">&#8987; '+jsonIssues[key].updated_at.substring(0,10)+'<\/span> <a href="'+jsonIssues[key].html_url+'" target="_blank">'+jsonIssues[key].title+'<\/a> <i>('+jsonIssues[key].comments+')<\/i><\/p>';
   }
  }
 }
}

function loadUpdate(repos, spiffs, LangUpgrade){
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open('GET','https://api.github.com/repos/'+repos+'/contents/build',true);
 xmlHttp.send(null);
 xmlHttp.onload = function() {
  var jsonBuild=JSON.parse(xmlHttp.responseText);
  jsonBuild.sort(function(a,b){return (a.name< b.name) ? 1 : ((b.name < a.name) ? -1 : 0);});
  if (jsonBuild[0].name != spiffs) {
   document.getElementById('update').innerHTML = '<a href="/upgrade?spiffs=http://backup.privet.lv/esp/sonoff/'+jsonBuild[0].name+'&build=http://backup.privet.lv/esp/sonoff/build.0x00000'+jsonBuild[0].name.substring(14)+'" onclick="return confirm(\''+LangUpgrade+' \\n - New build: '+jsonBuild[0].name.split('_')[4].slice(0,-4)+' \\n - You build: '+spiffs.split('_')[4].slice(0,-4)+'\')" title="'+LangUpgrade+'"><i class="warning-img"><\/i><\/a>';
  }
 }
}
