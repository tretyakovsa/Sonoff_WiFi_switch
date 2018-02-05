/*
Web Developer: Renats Kevrels (ex. Zozula)
Site: http://www.onclick.lv
Contact: info [at] onclick.lv
Skype: renat2985
Twitter: @Ramzies
Facebook: http://www.facebook.com/renat2985
GitHub: https://github.com/renat2985
From: Latvia, Valmiera
*/

var ajax = {};
ajax.x = function () {
 var xhr;
 if (window.XMLHttpRequest) {
  xhr = new XMLHttpRequest();
 } else {
  xhr = new ActiveXObject("Microsoft.XMLHTTP");
 }
 return xhr;
};

ajax.send = function (url, callback, method, data, async) {
 submit_disabled(true);
 if (async === undefined) {
  async = true;
 }
 var x = ajax.x();
 x.open(method, url, async);
 x.onreadystatechange = function () {
  if (x.readyState == 4) {
   submit_disabled(false);
   callback(x.responseText)
  }
 };
 if (method == 'POST') {
  x.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
 }
 x.send(data)
};

ajax.get = function (url, data, callback, async) {
 var query = [];
 for (var key in data) {
  query.push(encodeURIComponent(key) + '=' + encodeURIComponent(data[key]));
 }
 ajax.send(url + (query.length ? '?' + query.join('&') : ''), callback, 'GET', null, async)
};

ajax.post = function (url, data, callback, async) {
 var query = [];
 for (var key in data) {
  query.push(encodeURIComponent(key) + '=' + encodeURIComponent(data[key]));
 }
 ajax.send(url, callback, 'POST', query.join('&'), async)
};

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

document.onkeydown = function(e){
 var evtobj = window.event? event : e
 var element=document.getElementById('edit-content');
 let charCode = String.fromCharCode(evtobj.which).toLowerCase();
 if (charCode === 'm' && evtobj.ctrlKey && element) {toggle('edit-content');toggle('url-content');}
 if (charCode === 's' && evtobj.ctrlKey && element) {
  evtobj.preventDefault();send_request_edit(this, val('edit-json'), window.location.search.substring(1).split("&")[0]+'.json');toggle('edit-content');toggle('url-content');
 }
}

function setContent(stage) {
 jsonResponse = '';
 var pages = window.location.search.substring(1).split("&");
 pages[0] = (pages[0]?pages[0]:'index');
 ajax.get(pages[0]+'.json',{},function(response) {
  document.getElementById('download-json').href = pages[0]+".json";
  var jsonPage;
  if (response!='FileNotFound'){
   jsonPage=JSON.parse(response);
   var jsonEdit=response;
   if (jsonPage.configs){
    var fileNumber = 0;
    (function foo(){
     ajax.get(jsonPage.configs[fileNumber],{},function(response) {
      if (response!='FileNotFound'){
       var jsonResponseNew = JSON.parse(response);
       var jsonResponseOld = jsonResponse;
       jsonResponse = Object.assign(jsonResponseNew, jsonResponseOld);
       document.getElementById('url-content').innerHTML += '<li><span class="label label-warning">GET</span> <a href="'+jsonPage.configs[fileNumber]+'" class="btn btn-link" style="text-transform:none;text-align:left;white-space:normal;display:inline">'+jsonPage.configs[fileNumber]+'</a> <span class="label label-default">200 OK</span></li>';
      } else {
       document.getElementById('url-content').innerHTML += '<li><span class="label label-warning">GET</span> <a href="'+jsonPage.configs[fileNumber]+'" class="btn btn-link" style="text-transform:none;text-align:left;white-space:normal;display:inline">'+jsonPage.configs[fileNumber]+'</a> <span class="label label-danger">File Not Found</span></li>';
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
       if (jsonPage.title) {
        document.title = renameBlock(jsonResponse, jsonPage.title);
        document.getElementById('title').innerHTML = renameBlock(jsonResponse, jsonPage.title);
       }
       if (jsonPage.class) {document.getElementById('content').className = jsonPage.class;}
       if (jsonPage.style) {document.getElementById('content').style = jsonPage.style;}
       if (jsonPage.reload) {set_real_time = setTimeout("setContent('edit')", jsonPage.reload);}
       if (stage == 'first') {
        document.body.innerHTML += '<a href="/donate.htm" class="hidden-xs btn btn-link" target="_blank" style="position:fixed;bottom:0;"><i class="fav-img"></i> '+(jsonResponse.LangDonate?jsonResponse.LangDonate:'Donate')+'<\/a>';
        val('edit-json', jsonEdit);
        toggle('container_column','hide');
       } else {
        document.getElementById('content').innerHTML = '';
        jsonPage=JSON.parse(val('edit-json'));
       }
       val('edit-butt','view');
       if (jsonPage.content){
        viewTemplate(jsonPage,jsonResponse);
       } else {
        document.getElementById('url-content').innerHTML += '<li class="alert alert-danger" style="margin:5px 0;">content array not found in "'+pages[0]+'.json"<\/li>';
        document.getElementById('content').innerHTML += '<br><br><h1>File "'+pages[0]+'.json" cannot view.<\/h1><hr><h2>You can edit it right.<\/h2>';
        toggle('edit-content');
        toggle('url-content');
       }
      }
      if(fileNumber < jsonPage.configs.length) {
       jsonPage.configs[fileNumber] = renameBlock(jsonResponse, jsonPage.configs[fileNumber]);
       foo();
      }
     },true);
    })()
   } else {
    document.getElementById('content').innerHTML = '<br><br><h1>File "'+pages[0]+'.json" cannot view.<\/h1><hr><h2>Please add configs array.<br>Example: "configs":["/config.live.json"]<br>You can edit this file on right side of this page.<\/h2>';
    val('edit-json', jsonEdit);
    toggle('container_column','hide');
    toggle('edit-content');
    toggle('url-content');
   }
  } else {
   document.getElementById('content').innerHTML += '<br><br><h1>Files "'+pages[0]+'.json" not found.<\/h1><hr><h2>Maybe you want to open some file of these:<\/h2><h3 id="file-list">Loading...<\/h3>';
   toggle('container_column','hide');
   ajax.get('/list?dir=/',{},function(response) {
    html('file-list',' ');
    var jsonFiles = JSON.parse(response);
    for(var i = 0;i<jsonFiles.length;i++) {
     if (jsonFiles[i].name.substr(-4) == 'json'){
      document.getElementById('file-list').innerHTML += '<a href="/page.htm?'+jsonFiles[i].name.slice(0,-5)+'">'+jsonFiles[i].name+'<\/a><br>';
     }
    }
   },true);
  }
 },true);
}

function searchModule(modules,find) {
 var findModules = '';
 for(var key in modules) {
  if (modules[key].indexOf(find)!= -1) {
   findModules = 1;
  }
 }
 return findModules;
}

function viewTemplate(jsonPage,jsonResponse) {
 // var element = document.getElementById(idName);
 //for(var i = 0;i<jsonPage.content.length;i++) {
 var i = 0;
 for(var key in jsonPage) {
  var element=document.getElementById(key);
  if(Array.isArray(jsonPage[key]) && element){

   var arr = jsonPage[key];
   for (var j = 0; j<arr.length; j++) {
    var obj = arr[j];

    if (!obj.module || searchModule(jsonResponse.module,obj.module)) {
     var action_val = renameGet(obj.action);
     var name_val = (obj.name?obj.name:'');
     //    var title_val = renameBlock(jsonResponse, obj.title);
     var class_val = (obj.class?renameBlock(jsonResponse, obj.class):'');
     var style_val = (obj.style?'style="'+obj.style+'"':'');
     var pattern_val = (obj.pattern?obj.pattern:'');
     var state_val = renameBlock(jsonResponse, obj.state);
     var response_val = renameBlock(jsonResponse, obj.response);
     var module_val = obj.module;
     var type_val=obj.type;
     if (type_val == 'hr') {
      element.innerHTML += '<hr id="'+name_val+'" class="'+class_val+'" '+style_val+'>';
     }
     if (type_val == 'h1' || type_val == 'h2' || type_val == 'h3' || type_val == 'h4' || type_val == 'h5' || type_val == 'h6') {
      element.innerHTML += '<'+type_val+' id="'+name_val+'" class="'+class_val+'" '+style_val+'>'+renameBlock(jsonResponse, obj.title)+'<\/'+type_val+'>';
     }
     if (type_val == 'input') {
      if (action_val) action_val = 'onfocusout="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      element.innerHTML += '<input '+action_val+' id="'+name_val+'" class="form-control '+class_val+'" '+style_val+' '+(pattern_val?'pattern="'+pattern_val+'"':'')+' placeholder="'+renameBlock(jsonResponse, obj.title)+'" value="'+state_val+'">';
     }
     if (type_val == 'password') {
      if (action_val) action_val = 'onfocusout="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      element.innerHTML += '<input '+action_val+' id="'+name_val+'" class="form-control '+class_val+'" '+style_val+' '+(pattern_val?'pattern="'+pattern_val+'"':'')+' placeholder="'+renameBlock(jsonResponse, obj.title)+'" value="'+state_val+'" onfocus="this.type=\'text\'" type="password">';
     }
     if (type_val == 'button') {
      if (action_val) action_val = 'onclick="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      element.innerHTML += '<input id="'+name_val+'" '+action_val+' class="'+class_val+'" '+style_val+' value="'+renameBlock(jsonResponse, obj.title)+'" type="button">';
     }
     if (type_val == 'checkbox') {
      var checked = '';
      if (state_val == 1){ checked = 'checked'; }
      if (action_val) { action_val = 'onchange="val(this.id,(this.checked?\'1\':\'0\'));send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"'; } else { action_val = 'onchange="val(this.id,(this.checked?\'1\':\'0\'));"'; }
      element.innerHTML += '<label '+style_val+'><input id="'+name_val+'" value="'+state_val+'" '+action_val+' type="checkbox" class="'+class_val+'" '+checked+'> '+renameBlock(jsonResponse, obj.title)+'<\/label>';
     }
     if (type_val == 'range') {
      if (action_val) action_val = 'onchange="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      element.innerHTML += '<label '+style_val+' style="display:block;"><h4>'+renameBlock(jsonResponse, obj.title)+'<\/h4> <input id="'+name_val+'" class="form-control '+class_val+'" '+action_val+' '+pattern_val+' value="'+state_val+'" type="range"><\/label>';
     }
     if (type_val == 'table') {
      var thead = '';
      var jsonTable = obj.title;
      element.innerHTML += '<table class="'+class_val+'" '+style_val+' id="'+name_val+'"><thead id="thead-'+state_val.replace(/[^a-z0-9]/gi,'-')+'"><tr><td><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center></td></tr><\/thead><tbody id="tbody-'+state_val.replace(/[^a-z0-9]/gi,'-')+'"><\/tbody><\/table>';
      loadTable(state_val, jsonTable);
     }
     if (type_val == 'select') {
      if (action_val) action_val = 'onchange="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      var option = '';
      jsonSelect = obj.title;
      //jsonSelect = JSON.parse(renameBlock(jsonResponse, obj.title));
      //jsonSelect = renameBlock(jsonResponse, obj.title).slice(1, -1);
      for(var key in jsonSelect) {
       option += '<option value="'+renameBlock(jsonResponse, key)+'"'+(state_val==key?' selected':'')+'>'+renameBlock(jsonResponse, jsonSelect[key])+'<\/option>';
      }
      element.innerHTML += '<select class="form-control '+class_val+'" '+style_val+' '+action_val+' id="'+name_val+'">'+option+'<\/select>';
     }
     if (type_val == 'configs') {
      var htmlopt = '';
      htmlopt += '<div id="'+name_val+'"><div id="'+state_val.replace(/[^a-z0-9]/gi,'-')+'" class="'+class_val+'" '+style_val+'><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center><\/div><\/div>';
      htmlopt += '<div class="btn-group btn-block"><input  style="width:85%" onclick="changeTextarea(\''+state_val.replace(/[^a-z0-9]/gi,'-')+'\');send_request_edit(this, val(\''+state_val.replace(/[^a-z0-9]/gi,'-')+'-edit\'),\'configs/'+state_val+'\');alert(\''+jsonResponse.LangReset2+'\')" class="btn btn-block btn-success" value="'+jsonResponse.LangSave+'" type="button">';
      htmlopt += '<a href="#" style="width:15%" class="btn btn-info dropdown-toggle" onclick="toggle(\'cloud\');return false"><i class="cloud-img"></i> <span class="caret"></span></a>';
      htmlopt += '<ul class="dropdown-menu hidden" style="right:0;left:auto" id="cloud"><li><a onclick="toggle(\'cloud\');cloudUpload(\''+jsonResponse.mac+'\',\''+jsonResponse.configs+'\');alert(\''+jsonResponse.LangReset2+'\');return false" href="#"><i class="cloud-img"></i> '+jsonResponse.LangCloudUpload+'</a></li><li><a onclick="toggle(\'cloud\');cloudDownload(\''+jsonResponse.mac+'\',\''+jsonResponse.configs+'.txt\');alert(\''+jsonResponse.LangReset2+'\');return false" href="#"><i class="cloud-img"></i> '+jsonResponse.LangCloudDownload+'</a></li></ul>';
      htmlopt += '</div>';
      element.innerHTML += htmlopt;
      setTimeout("loadConfigs('"+state_val+"')", 500);
     }
     if (type_val == 'link') {
      element.innerHTML += '<a id="'+name_val+'" class="'+class_val+'" '+style_val+' href="'+renameGet(obj.action)+'">'+renameBlock(jsonResponse, obj.title)+'<\/a>';
     }
     if (type_val == 'img') {
      element.innerHTML += '<img id="'+name_val+'" class="'+class_val+'" '+style_val+' src="'+renameGet(obj.state)+'" onclick="'+renameGet(obj.action)+'" title="'+renameBlock(jsonResponse, obj.title)+'"\/>';
     }
     if (type_val == 'text') {
      element.innerHTML += '<div id="'+name_val+'" class="'+class_val+'" '+style_val+'>'+renameBlock(jsonResponse, obj.title)+'<\/div>';
     }
     if (type_val == 'iframe') {
      element.innerHTML += renameBlock(jsonResponse, obj.title)+'<iframe src="'+state_val+'" id="'+name_val+'" class="'+class_val+'" '+style_val+'><\/iframe>';
     }
     if (type_val == 'chart') {
      element.innerHTML += '<div id="'+name_val+'" class="'+renameBlock(jsonResponse, '{{'+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden}}')+'"><button class="close" onclick="hide(\''+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden\',this);" type="button">×<\/button><a href="'+renameGet(obj.action)+'" target="_blank" class="close">'+(typeof action_val!='undefined'&&action_val?'<i class="popup-img"><\/i>':'')+'<\/a><h2><span id="'+state_val.replace(/[^a-z0-9]/gi,'')+'-title">'+renameBlock(jsonResponse, obj.title)+'</span> <span id="'+state_val.replace(/[^a-z0-9]/gi,'')+'-data"></span><\/h2><div id="'+state_val.replace(/[^a-z0-9]/gi,'')+'" class="'+class_val+'" '+style_val+'><\/div><hr><\/div>';
      if (renameBlock(jsonResponse, '{{'+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden}}') != 'hidden') {
       setTimeout("loadChart('"+state_val.replace(/[^a-z0-9]/gi,'')+"','"+state_val+"', {"+obj.options+"},"+obj.refresh+","+obj.points+")", 500);
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
      element.innerHTML += '<input id="'+name_val+'" value="'+state_val+'" class="form-control hidden '+class_val+'" '+style_val+' '+pattern_val+' placeholder="'+renameBlock(jsonResponse, obj.title)+'">';
     }
     if (type_val == 'time' && typeof jsonResponse.time !== "undefined") {
      element.innerHTML += '<h2 id="'+name_val+'" '+style_val+'>'+renameBlock(jsonResponse, obj.title)+' <strong id="time" class="'+class_val+'">'+state_val+'<\/strong><\/h2>';
      clearTimeout(set_real_time);
      var res = jsonResponse.time.split(":");
      real_time(hours=res[0],min=res[1],sec=res[2]);
     }
     if (type_val == 'rgb') {
      element.innerHTML += '<div class="'+name_val+'-thumb '+class_val+'"><div class="'+name_val+'-preview"><\/div><img alt="" '+style_val+' src="'+renameBlock(jsonResponse, obj.title)+'"><\/div><canvas id="'+name_val+'-cs" style="display:none"><\/canvas>';
      element.innerHTML += '<input id="'+name_val+'" value="'+state_val+'" class="form-control hidden">';
      setTimeout("createRGB('"+name_val+"', '"+obj.action+"','"+module_val+"','"+response_val+"')", 500);
     }
     if (type_val == 'dev') {
      var dev_html = '<div id="'+name_val+'" class="'+class_val+'" '+style_val+'><a href="/help.htm" target="_blank" class="close"><i class="help-img"><\/i><\/a>'+renameBlock(jsonResponse, obj.title)+'<span id="dev-update" class="hidden"><a href="/edit" class="btn btn-default" target="_blank">File manager<\/a> <a href="/page.htm?starting" class="btn btn-default">Starting log<\/a> ';
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
  i++;
 }
}

function loadJson(state_val, jsonResponse, idName) {
 ajax.get(state_val+'?'+Math.floor(Math.random()*10000),{},function(response) {
  html(idName, ' ');
  jsonPage=JSON.parse(response);
  viewTemplate(jsonPage,jsonResponse);
 },true);
}

function pattern(s) {
 document.getElementById("ssdp-command").setAttribute("pattern","["+(s=='number'?'0-9':'0-9a-zA-Z:')+"]{1,15}");
}

function loadScenaryList(jsonResponse,selectDevice,urlList) {
 ajax.get((urlList?'http://'+urlList:'')+"/scenary.save.txt?"+Math.floor(Math.random()*10000),{},function(response) {
  if (selectDevice == 'loadInTextarea') {
   //html("scenary-list-edit",response);
   document.getElementById("scenary-list-edit").innerHTML = response;
  } else if (Number.isInteger(selectDevice) == true) {
   var reg = new RegExp("([\\s\\S]+?)(id\\s+\\d+)", "mig");
   send_request_edit(this, response.replace(reg,function(a,b,c){return new RegExp("^id+\\s+"+selectDevice+"$").test(c)?"":a}),'scenary.save.txt','html(\'scenary-list\', \' \');send_request(this,\'http://'+urlList+'/setscenary\');loadScenary(jsonResponse,\'loadList\');',urlList);
  } else {
     //  alert(selectDevice+' '+urlList);
   var createText = '';
   var block = response.split(/\n|\r| /);
   //var block = response.split(/\n\r|\r\n|\n|\r| /g);
   for (var i = 0 ; i < block.length; i++) {
    createText += ' '+(renameBlock(jsonResponse, '{{Lang'+block[i]+'}}')===undefined?block[i]:renameBlock(jsonResponse, '{{Lang'+block[i]+'}}'));
   }
   document.getElementById("scenary-list").innerHTML += '<tr><td colspan="2"><h4><a href="http://'+urlList+'">'+selectDevice+'</a></h4></td></tr>'+createText.replace(/if /gi,'<tr><td><b>'+jsonResponse.LangIf+'</b> ').replace(/and /gi,'<b>and</b> ').replace(/then /gi,'<b>'+jsonResponse.LangThen+'</b> ').replace(/(id)\s+(\d+)/mg, '<\/td><td><a class="btn btn-sm btn-danger" style="float:right;" href="#" onclick="if(confirm(\''+jsonResponse.LangDel+'?\')){loadScenaryList(jsonResponse,$2,\''+urlList+'\');}return false"><i class="del-img"></i> <span class="hidden-xs">'+jsonResponse.LangDel+'</span></a><\/td><\/tr>');
  }
 },true);
}

function loadScenary(jsonResponse,loadList) {
 html('scenary-list', '<tr><td colspan="2"><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center></td></tr>');
 ajax.get('/ssdp.list.json?'+Math.floor(Math.random()*10000),{},function(response) {
  html('scenary-list', ' ');
  var option = '';
  var ipDevice=JSON.parse(response);
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
 },true);
}

function loadLive(ip,file,where) {
 html(where,'<option value="">Loading...</option>');
 ajax.get('http://'+ip+'/'+file,{},function(response) {
  var option = '';
  var jsonLive=JSON.parse(response);
  for(var key in jsonLive) {
   option += '<option value="'+key+'" title="'+typeof jsonLive[key]+'">'+(renameBlock(jsonResponse, '{{Lang'+key+'}}')===undefined?key:renameBlock(jsonResponse, '{{Lang'+key+'}}'))+'<\/option>';
  }
  html(where,'<option value="">'+jsonResponse.LangSelect+'<\/option>'+option);
 },true);
}

function loadLive2() {
 val("ssdp-command","Loading...");
 var ip = document.getElementById("ssdp-list0").options[document.getElementById("ssdp-list0").selectedIndex].value;
 var who = document.getElementById("ssdp-module").options[document.getElementById("ssdp-module").selectedIndex].value;
 ajax.get('http://'+ip+'/config.live.json',{},function(response) {
  var jsonLive=JSON.parse(response);
  for(var key in jsonLive) {
   if (key == who) {
    document.getElementById("ssdp-command").value= jsonLive[key];
   }
  }
 },true);
}

function loadInTextarea() {
 var element = document.getElementById("scenary-list-edit");
 element.innerHTML += '\r\n\r\nif '+document.getElementById("ssdp-module").options[document.getElementById("ssdp-module").selectedIndex].value+' '+document.getElementById("ssdp-condition").options[document.getElementById("ssdp-condition").selectedIndex].value+' '+document.getElementById("ssdp-command").value;
 element.innerHTML += '\r\nthen '+document.getElementById("ssdp-list1").options[document.getElementById("ssdp-list1").selectedIndex].text+' '+document.getElementById("scenary-then").options[document.getElementById("scenary-then").selectedIndex].value+' '+document.getElementById("scenary-othe").value;
 element.innerHTML += '\r\nid '+Math.floor(Math.random()*10000);
}

function val(id,val){
 if (document.getElementById(id)){
  if (val) {
   document.getElementById(id).value=(val==' '?'':val);
  } else {
   var v = document.getElementById(id).value;
   return v;
  }
 }
}

function html(id,val){
 if (document.getElementById(id)){
  if (val) {
   document.getElementById(id).innerHTML=(val==' '?'':val);
  } else {
   var v = document.getElementById(id).innerHTML;
   return v;
  }
 }
}

function send_request_edit(submit,server,filename,geturl,gethost){
 var xmlHttp = new XMLHttpRequest();
 var old_submit = submit.value;
 submit.value = jsonResponse.LangLoading;
 submit_disabled(true);
 var formData = new FormData();
 formData.append("data", new Blob([server], { type: 'text/json' }), "/"+filename);
 // ajax.post((gethost?'http://'+gethost:'')+'/edit',{data:formData},function(response) {
 xmlHttp.open("POST", (gethost?'http://'+gethost:'')+"/edit");
 xmlHttp.onload = function() {
  submit.value=old_submit;
  submit_disabled(false);
  if (geturl){
   //window.location = geturl;
   eval(geturl);
  }
 }
 // },true);
 xmlHttp.send(formData);
}

function send_request_post(submit,server,state){
 ajax.post(server,{},function(response) {
  if (state != null && state!='undefined'){
   var response=JSON.parse(response);
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
     element.innerHTML += '<li class="alert alert-info" style="margin:5px 0;"><span class="label label-success">'+block[i]+'</span> '+response.replace(/</g,'&lt;')+'</li>';
    }
   }
  }
 },true);
}

function send_request(submit,server,state){
 var old_submit = submit.value;
 submit.value = jsonResponse.LangLoading;
 ajax.get(server,{},function(responses) {
  submit.value=old_submit;
  var element =  document.getElementById('url-content');
  if (typeof(element) != 'undefined' && element != null){
   element.innerHTML += '<li><span class="label label-warning">GET</span> <a href="'+server+'" class="btn btn-link" style="text-transform:none;text-align:left;white-space:normal;display:inline">'+server+'</a> <span class="label label-'+(responses=='FileNotFound'?'danger':'default')+'">'+(responses=='FileNotFound'?'File Not Found':'200 OK')+'</span></li>';
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
     var response=JSON.parse(responses);
     var htmlblock = document.getElementById(block[i].slice(2,-2));
     if (response.class && response.class!='undefined') {htmlblock.className = response.class;}
     if (response.style && response.style!='undefined') {htmlblock.style = response.style;}
     if (response.title && response.title!='undefined') {
      if (htmlblock.tagName == 'INPUT') {
       htmlblock.value = renameBlock(jsonResponse, response.title);
      }
      if (htmlblock.tagName == 'SELECT') {
       var option = '';
       jsonSelect = response.title;
       for(var key in jsonSelect) {
        option += '<option value="'+renameBlock(jsonResponse, key)+'">'+renameBlock(jsonResponse, jsonSelect[key])+'<\/option>';
       }
       htmlblock.innerHTML = option;
      }
      if (htmlblock.tagName == 'DIV' ||htmlblock.tagName == 'A' || htmlblock.tagName == 'H1' || htmlblock.tagName == 'H2' || htmlblock.tagName == 'H3' || htmlblock.tagName == 'H4' || htmlblock.tagName == 'H5' || htmlblock.tagName == 'H6') {htmlblock.innerHTML = renameBlock(jsonResponse, response.title);}
     }
     if (htmlblock.tagName == 'TABLE' && response.state) {
      loadTable(response.state,response.title);
     }
     if (htmlblock.tagName == 'A' && response.action) {
      htmlblock.href = response.action;
     }
     if (typeof(element) != 'undefined' && element != null){
      element.innerHTML += '<li class="alert alert-info" style="margin:5px 0;"><a href="#'+block[i].slice(2,-2)+'" class="label label-success">'+block[i]+'</a> '+responses.replace(/</g,'&lt;')+'</li>';
     }
    }
   }
  } else {
   if (typeof(element) != 'undefined' && element != null){
    element.innerHTML += '<li class="alert alert-info" style="margin:5px 0;">'+responses.replace(/</g,'&lt;')+'</li>';
   }
  }
  // load('next');
 },true);
}

function submit_disabled(request){
 var element = document.getElementsByTagName("input");
 for (var i = 0; i < element.length; i++) {
  if (element[i].type === 'button') {element[i].disabled = request;}
 }
}

function toggle(target,status) {
 if (document.getElementById(target)){
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
}

function loadWifi(ssids,hiddenIds){
 html(ssids, '<li><a href="#">Loading...</a></li>');
 ajax.get('/wifi.scan.json',{},function(response) {
  var jsonWifi=JSON.parse(response);
  jsonWifi.networks.sort(function(a,b){return (a.dbm < b.dbm) ? 1 : ((b.dbm < a.dbm) ? -1 : 0);});
  var html = '';
  for(var i = 0;i<jsonWifi.networks.length;i++) {
   var wifiSignal = '';
   if (jsonWifi.networks[i].dbm <= -0) { wifiSignal = '0-60';}
   if (jsonWifi.networks[i].dbm <= -60) { wifiSignal = '60-70';}
   if (jsonWifi.networks[i].dbm <= -70) { wifiSignal = '70-80';}
   if (jsonWifi.networks[i].dbm <= -80) { wifiSignal = '80-90';}
   if (jsonWifi.networks[i].dbm <= -90) { wifiSignal = '90-100';}
   html += '<li><a href="#" onclick="val(\''+hiddenIds+'\',\''+jsonWifi.networks[i].ssid+'\');toggle(\'ssid-select\');html(\'ssid-name\',\''+jsonWifi.networks[i].ssid+'\');return false"><div style="float:right">'+(jsonWifi.networks[i].pass?'<i class="wifi wifi-key"></i>':'')+' <i class="wifi wifi-'+wifiSignal+'"></i> <span class="label label-default">'+jsonWifi.networks[i].dbm+' dBm</span></div><b>'+jsonWifi.networks[i].ssid+'</b></a></li>';
  }
  document.getElementById(ssids).innerHTML = (html?html:'<li><a href="#">Not found WiFi</a></li>')+'<li><a href="#" onclick="toggle(\'ssid-group\');toggle(\'ssid\');return false"><b>'+jsonResponse.LangHiddenWifi+'</b></a></li>';
 },true);
}

function loadBuild(buildids,typeFile){
 ajax.get('http://backup.privet.lv/esp/build/'+buildids,{},function(response) {
  var jsonBuild=JSON.parse(response);
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
 },true);
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
 var element = document.getElementById(state_val.replace(/[^a-z0-9]/gi,'-'));
 element.innerHTML = '';
 ajax.get("/configs/"+state_val,{},function(response) {
  var configsLinePin;
  var configsLine = response.match(/^.*((\r\n|\n|\r)|$)/gm);
  for(var key in configsLine) {
   if (configsLine[key].substr(0,2) == '//') {
    var configsLine2 = configsLine[key].split(" ");
    var configsLine3 = '';
    for (var i = 1; i < configsLine2.length; i++) {
     configsLine3 = configsLine3+' '+renameBlock(jsonResponse, configsLine2[i]);
    }
    element.innerHTML += '<label title="'+configsLine[key]+'"><input checked="" type="checkbox" style="display:none" disabled readonly><small>// '+configsLine3+'</small><\/label></br>';
   } else {
    configsLinePin = configsLine[key].replace(/# /,'').split(' ');
    element.innerHTML += '<label style="margin-bottom:25px;"><input type="checkbox" '+(configsLine[key].substring(0,2)!='# '?"checked":"")+'> '+configsLinePin[0]+'<\/label> '+(configsLinePin[1]?'<input class="form-control" style="display:inline;width:'+Number(configsLinePin[1].length+3)+'0px;" pattern="[a-zA-Z0-9.]{1,20}" value="'+configsLinePin[1]+'">':'')+' '+(configsLinePin[2]?'<input class="form-control" style="display:inline;width:'+Number(configsLinePin[2].length+3)+'0px;" pattern="[a-zA-Z0-9.]{1,20}" value="'+configsLinePin[2]+'">':'')+' '+(configsLinePin[3]?'<input class="form-control" style="display:inline;width:'+Number(configsLinePin[3].length+3)+'0px;" pattern="[a-zA-Z0-9.]{1,20}" value="'+configsLinePin[3]+'">':'')+' '+(configsLinePin[4]?'<input class="form-control" style="display:inline;width:'+Number(configsLinePin[4].length+3)+'0px;" pattern="[a-zA-Z0-9.]{1,20}" value="'+configsLinePin[4]+'">':'')+' '+(configsLinePin[5]?'<input class="form-control" style="display:inline;width:'+Number(configsLinePin[5].length+3)+'0px;" pattern="[a-zA-Z0-9.]{1,20}" value="'+configsLinePin[5]+'">':'')+' '+(configsLinePin[6]?'<input class="form-control" style="display:inline;width:'+Number(configsLinePin[6].length+3)+'0px;" pattern="[a-zA-Z0-9.]{1,20}" value="'+configsLinePin[6]+'">':'')+' '+(configsLinePin[7]?'<input class="form-control" style="display:inline;width:'+Number(configsLinePin[7].length+3)+'0px;" pattern="[a-zA-Z0-9.]{1,20}" value="'+configsLinePin[7]+'">':'')+''+(configsLinePin[8]?'<input class="form-control" style="display:inline;width:'+Number(configsLinePin[8].length+3)+'0px;" pattern="[a-zA-Z0-9.]{1,20}" value="'+configsLinePin[8]+'">':'')+'</br>';
   }
  }
  //changeTextarea(state_val.replace(/[^a-z0-9]/gi,'-'));
 },true);
 element.innerHTML += '<textarea id="'+state_val.replace(/[^a-z0-9]/gi,'-')+'-edit" style="display:none" class="form-control"></textarea>';
}

function changeTextarea(state_val) {
 val(state_val.replace(/[^a-z0-9]/gi,'-')+'-edit', ' ');
 var area = document.querySelector('textarea');
 [].forEach.call(document.querySelectorAll('#'+state_val+'>*'), function(el){
  if(el.matches('label')) {
   area.value += '\r\n'+(el.children[0].checked?'':'# ');
   if (el.textContent.substr(0,2) == '//') {
    area.value += el.title;
   } else {
    area.value += el.textContent.replace(/ /,'');
   }
  }
  if(el.matches('input')) area.value += ' '+el.value;
 });
 area.value = area.value.replace(/\n+/g,'\n').slice(1);
}

function cloudUpload(mac,file) {
 changeTextarea(file+'-txt');
 ajax.post("http://backup.privet.lv/configs/?file="+mac+"-"+file,{data:val(file+'-txt-edit')},function(response) {
  send_request_edit(this, val(file+'-txt-edit'),'configs/'+file+'.txt');
 },true);
}


function cloudDownload(mac,file) {
 ajax.get("http://backup.privet.lv/configs/"+mac+"-"+file+"?"+Math.floor(Math.random()*10000),{},function(response) {
  send_request_edit(this, response,'configs/'+file+'','loadConfigs("'+file+'");');
 },true);
}


function loadTable(state_val, jsonTable) {
 ajax.get(state_val+"?"+Math.floor(Math.random()*10000),{},function(response) {
  var timers=JSON.parse(response);
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
    if (timers[setTable][i][keys] == "on") {timers[setTable][i][keys] = '<span class="label label-success">'+jsonResponse["LangOnM"]+'<\/span>';}
    if (timers[setTable][i][keys] == "off") {timers[setTable][i][keys] = '<span class="label label-danger">'+jsonResponse["LangOffM"]+'<\/span>';}
    if (timers[setTable][i][keys] == "open") {timers[setTable][i][keys] = '<span class="label label-success">'+jsonResponse["LangOpen"]+'<\/span>';}
    if (timers[setTable][i][keys] == "close") {timers[setTable][i][keys] = '<span class="label label-danger">'+jsonResponse["LangClose"]+'<\/span>';}
    if (timers[setTable][i][keys] == "not") {timers[setTable][i][keys] = '<span class="label label-info">'+jsonResponse["LangSwitch"]+'<\/span>';}
    if (timers[setTable][i][keys] == "relay") {timers[setTable][i][keys] = '<span class="label label-danger">relay<\/span>';}
    if (timers[setTable][i][keys] == "rgb") {timers[setTable][i][keys] = '<span class="label label-info">rgb<\/span>';}
    if (timers[setTable][i][keys] == "jalousie") {timers[setTable][i][keys] = '<span class="label label-success">jalousie<\/span>';}
    if (jsonTable[keys] == '{{LangDay}}') {timers[setTable][i][keys] = jsonResponse["Lang"+timers[setTable][i][keys]];}
    if (jsonTable[keys] == '{{LangDel}}') {timers[setTable][i][keys] = '<input class="btn btn-sm btn-danger" value="Удалить" onclick="if(confirm(\''+jsonResponse["LangDel"]+'?\')){send_request(this, renameGet(\'/timersDel?id='+timers[setTable][i][keys]+'\'),\'[[timer-list]]\');}" type="button">'}
    tbody += '<td>'+timers[setTable][i][keys]+'<\/td>';
   }
   document.getElementById('tbody-'+state_val.replace(/[^a-z0-9]/gi,'-')).innerHTML += '<tr>'+tbody+'<\/tr>';
  }
 },true);
}

function renameBlock(jsonResponse, str) {
 if (str) {
  var arr=str.match(/\{\{\S+?\}\}/gim);
  if (arr) {
   for (var i=0; i<arr.length; i++) {
    var id=arr[i].slice(2, -2);
    //if (jsonResponse[id]) {
    //str = str.replace(new RegExp('{{'+id+'}}','g'), jsonResponse[id]);
    str = str.replace(new RegExp('{{'+id+'}}','g'), eval("jsonResponse."+id));
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
 ajax.get('https://api.github.com/repos/'+repos+'/commits',{},function(response) {
  html('commits-list', ' ');
  var jsonCommits=JSON.parse(response);
  //  jsonCommits.sort(function(a,b){return (a.updated_at< b.updated_at) ? 1 : ((b.updated_at < a.updated_at) ? -1 : 0);});
  for(var key in jsonCommits) {
   if (key < viewCommits) {
    document.getElementById('commits-list').innerHTML += '<p><span class="label label-default">&#8987; '+jsonCommits[key].commit.author.date.substring(0,10)+'<\/span> <a href="'+jsonCommits[key].html_url+'" target="_blank">'+jsonCommits[key].commit.message+'<\/a><\/p>';
   }
  }
 },true);
}

function loadIssues(repos,viewIssues){
 ajax.get('https://api.github.com/repos/'+repos+'/issues',{},function(response) {
  html('issues-list', ' ');
  var jsonIssues=JSON.parse(response);
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
    document.getElementById('issues-list').innerHTML += '<p><span class="label label-default">&#8987; '+jsonIssues[key].updated_at.substring(0,10)+'<\/span> <a href="'+jsonIssues[key].html_url+'" target="_blank">'+jsonIssues[key].title+'<\/a> <i>('+jsonIssues[key].comments+')<\/i><\/p>';
   }
  }
 },true);
}

function loadUpdate(repos, spiffs, LangUpgrade){
 ajax.get('https://api.github.com/repos/'+repos+'/contents/build',{},function(response) {
  var jsonBuild=JSON.parse(response);
  jsonBuild.sort(function(a,b){return (a.name< b.name) ? 1 : ((b.name < a.name) ? -1 : 0);});
  if (jsonBuild[0].name != spiffs) {
   html('update','<a href="/upgrade?spiffs=http://backup.privet.lv/esp/sonoff/'+jsonBuild[0].name+'&build=http://backup.privet.lv/esp/sonoff/build.0x00000'+jsonBuild[0].name.substring(14)+'" onclick="return confirm(\''+LangUpgrade+' \\n - New build: '+jsonBuild[0].name.split('_')[4].slice(0,-4)+' \\n - You build: '+(spiffs?spiffs.split('_')[4].slice(0,-4):'Not found')+'\')" title="'+LangUpgrade+'"><i class="warning-img"><\/i><\/a>');
  }
 },true);
}

function isValidJson(str,idMess) {
 try {
  JSON.parse(str);
  document.getElementById(idMess).style.border = "";
  html(idMess,' ');
 } catch (e) {
  document.getElementById(idMess).style.border = "2px solid red";
  html(idMess,'<div class="alert alert-danger">'+e.message+'<\/div><br><br><br>');
  return false;
 }
 return true;
}
