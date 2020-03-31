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

ajax.send = function (url,callback,method,data,async) {
 if (method != 'PUT') {submit_disabled(true);}
 if (async === undefined) {
  async = true;
 }
 var x = ajax.x();
 x.open((method=='PUT'?'GET':''+method+''),url,async);
 x.onreadystatechange = function () {
  if (x.readyState == 4) {
   if (method != 'PUT') {submit_disabled(false);}
   callback(x.responseText)
  }
 };
 if (method == 'POST') {
  x.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
 }
 x.send(data)
};

ajax.get = function (url,data,callback,async) {
 var query = [];
 for (var key in data) {
  query.push(encodeURIComponent(key)+'='+encodeURIComponent(data[key]));
 }
 ajax.send(url+(query.length?'?'+query.join('&'):''),callback,'GET',null,async)
};

ajax.put = function (url,data,callback,async) {
 var query = [];
 for (var key in data) {
  query.push(encodeURIComponent(key)+'='+encodeURIComponent(data[key]));
 }
 ajax.send(url+(query.length?'?'+query.join('&'):''),callback,'PUT',null,async)
};

ajax.post = function (url,data,callback,async) {
 var query = [];
 for (var key in data) {
  query.push(encodeURIComponent(key)+'='+encodeURIComponent(data[key]));
 }
 ajax.send(url,callback,'POST',query.join('&'),async)
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
var active = 'egiste';
var jsonPage;
var jsonResponse;
var connection;
var socket_input = '0';

document.onkeydown = function(e){
 var evtobj = window.event?event:e
 var element=elem('edit-content');
 var charCode = String.fromCharCode(evtobj.which).toLowerCase();
 if (charCode === 'e' && evtobj.ctrlKey && element) {window.open('/edit','_blank');}
 if (charCode === 'm' && evtobj.ctrlKey && element) {toggle('edit-content');toggle('url-content');}
 if (charCode === 's' && evtobj.ctrlKey && element) {
  evtobj.preventDefault();send_request_edit(this, val('edit-json'), window.location.search.substring(1).split("&")[0]+'.json');toggle('edit-content');toggle('url-content');
 }
}

function log(log) {
 var listitems = document.getElementById("url-content").getElementsByTagName("li")
 for (var i = 0; i < listitems.length; i++) {
  if (i > 70) {
   document.getElementById("url-content").removeChild(listitems[0]);
  }
 }
 elem('url-content').innerHTML += log;
 elem('url-content').scrollTop = elem('url-content').scrollHeight;
}
function run_socket(url) {
 connection = new WebSocket(url.replace('socket ','ws://'), ['arduino']);
 connection.onopen = function () {
  connection.send('Connect '+new Date());
  log('<li><span class="label label-warning">WS</span> <small>'+url+'</small> <span class="label label-default">Connected</span></li>');
 };
 connection.onerror = function (error) {
  // console.log('WebSocket Error ', error);
  log('<li><span class="label label-warning">WS</span> <small>'+url+'</small> <span class="label label-danger">'+error+'</span></li>');
 };
 connection.onmessage = function (e) {
  // console.log('Server: ', e.data);
  if (socket_input == '1') {
   log('<li><span class="label label-warning">WS</span> <small>'+url+'</small> <span class="label label-default">Receiving</span></li><li style="margin:5px 0;" class="alert alert-info"><b>No replacement. Remove focus on input.</b><br>'+e.data+'</li>');
  } else {
   log('<li><span class="label label-warning">WS</span> <small>'+url+'</small> <span class="label label-default">Receiving</span></li><li style="margin:5px 0;" class="alert alert-info">'+e.data+'</li>');
   var socket_data=JSON.parse(e.data);
   var jsonResponse_new_sock = mergeObject(jsonResponse, socket_data);
   jsonResponse = jsonResponse_new_sock;
   elem('content').innerHTML = '';
   elem('title').innerHTML = '';
   viewTemplate(jsonPage,jsonResponse);
  }
 }
}
function array_socket(socket) {
 for(var key in socket) {
  run_socket(renameBlock(jsonResponse, socket[key]));
 }
}
function setContent(stage,load_page) {
 jsonResponse = '';
 var pages = window.location.search.substring(1).split("&");
 if (load_page) {pages[0] = load_page; }
 pages[0] = (pages[0]?pages[0]:'index');
 ajax.get(pages[0]+'.json?'+Math.random(),{},function(response) {
  elem('download-json').href = pages[0]+".json";
  // var jsonPage;
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
       jsonResponse = mergeObject(jsonResponseNew, jsonResponseOld);
       //jsonResponse = Object.assign(jsonResponseNew, jsonResponseOld);
       log('<li><span class="label label-warning">GET</span> <a href="'+jsonPage.configs[fileNumber]+'" class="btn btn-link" style="text-transform:none;text-align:left;white-space:normal;display:inline" target="_blank">'+jsonPage.configs[fileNumber]+'</a> <span class="label label-default">200 OK</span></li>');
      } else {
       //For old version socket
       if (jsonPage.configs[fileNumber].indexOf('socket ')  >= 0){
        if (stage == 'first') {
         run_socket(jsonPage.configs[fileNumber]);
        }
        log('<li><span class="label label-warning">WS</span> <a href="'+jsonPage.configs[fileNumber]+'" class="btn btn-link" style="text-transform:none;text-align:left;white-space:normal;display:inline" target="_blank">'+jsonPage.configs[fileNumber]+'</a> <span class="label label-default">Connected</span></li>');
       } else {
        log('<li><span class="label label-warning">GET</span> <a href="'+jsonPage.configs[fileNumber]+'" class="btn btn-link" style="text-transform:none;text-align:left;white-space:normal;display:inline" target="_blank">'+jsonPage.configs[fileNumber]+'</a> <span class="label label-danger">File Not Found</span></li>');
       }
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

       jsonResponse.sossionLogin = sessionStorage.getItem('sossionLogin');

       var element_title=elem('title');
       if(element_title){element_title.innerHTML = '';}
       if (jsonPage.class) {elem('content').className = jsonPage.class;}
       if (jsonPage.style) {elem('content').style = jsonPage.style;}
       if (jsonPage.reload) {set_real_time = setTimeout("setContent('edit')", jsonPage.reload);}
       if (stage == 'first') {
        document.body.innerHTML += '<a href="/donate.htm" class="hidden-xs btn btn-link" target="_blank" style="position:fixed;bottom:0;"><i class="fav-img"></i> '+(jsonResponse.LangDonate?jsonResponse.LangDonate:'Donate')+'<\/a>';
        val('edit-json', jsonEdit);
        toggle('container_column','hide');
        if (jsonPage.socket){
         array_socket(jsonPage.socket);
        }
       } else {
        elem('content').innerHTML = '';
        jsonPage=JSON.parse(val('edit-json'));
       }
       val('edit-view','view');
       if (jsonPage.content){
        viewTemplate(jsonPage,jsonResponse);
        html("edit-save","oncli"+"ck");
       } else {
        log('<li class="alert alert-danger" style="margin:5px 0;">content array not found in "'+pages[0]+'.json"<\/li>');
        elem('content').innerHTML += '<br><br><h1>File "'+pages[0]+'.json" cannot view.<\/h1><hr><h2>You can edit it right.<\/h2>';
        toggle('edit-content');
        toggle('url-content');
       }
      }
      if(fileNumber < jsonPage.configs.length) {
       jsonPage.configs[fileNumber] = renameBlock(jsonResponse, jsonPage.configs[fileNumber]);
       // if (jsonPage.configs[fileNumber].indexOf('ws://')  >= 0){
       //  var connection = new WebSocket(jsonPage.configs[fileNumber]+":81", ['arduino']);
       //  connection.onopen = function () {
       //   connection.send('Connect '+new Date());
       //  };
       //  connection.onerror = function (error) {
       //   console.log('WebSocket Error ', error);
       //  };
       //  connection.onmessage = function (e) {
       //   console.log('Server: ', e.data);
       //   setContent();
       //  }
       // } else {
       foo();
       // }
      }
     },true);
    })()
   } else {
    elem('content').innerHTML = '<br><br><h1>File "'+pages[0]+'.json" cannot view.<\/h1><hr><h2>Please add configs array.<br>Example: "configs":["/config.live.json"]<br>You can edit this file on right side of this page.<\/h2>';
    val('edit-json', jsonEdit);
    toggle('container_column','hide');
    toggle('edit-content');
    toggle('url-content');
   }
  } else {
   elem('content').innerHTML += '<br><br><h1>Files "'+pages[0]+'.json" not found.<\/h1><hr><h2>Maybe you want to open some file of these:<\/h2><h3 id="file-list"><span class="loader"></span>Loading...<\/h3>';
   toggle('container_column','hide');
   ajax.get('/list?dir=/',{},function(response) {
    html('file-list',' ');
    var jsonFiles = JSON.parse(response);
    jsonFiles.sort(function(a,b){return (a.name < b.name)?1:((b.name < a.name)?-1:0);});
    for(var i = 0;i<jsonFiles.length;i++) {
     if (jsonFiles[i].name.substr(-4) == 'json' || jsonFiles[i].name.substr(-7) == 'json.gz'){
      elem('file-list').innerHTML += '<a href="/'+(location.pathname=='index.htm'?'index':'page')+'.htm?'+jsonFiles[i].name.split('.')[0]+'">'+jsonFiles[i].name+'<\/a><br>';
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

function searchModuleOld(modules,find) {
 var findModules = '';
 for(var key in modules) {
  if (modules[key] == find) {
   findModules = 1;
  }
 }
 return findModules;
}

function viewTemplate(jsonPage,jsonResponse) {
 var i = 0;
 for(var key in jsonPage) {
  var element=elem(key);
  if(Array.isArray(jsonPage[key]) && element){

   var arr = jsonPage[key];
   for (var j = 0; j<arr.length; j++) {
    var obj = arr[j];

    if (!obj.module || searchModule(jsonResponse.module,obj.module)) {
     var action_val = renameGet(obj.action);
     var socket_val = renameGet(obj.socket);
     var actiondown_val = renameGet(obj.actiondown);
     var name_val = (obj.name?obj.name:'');
     var class_val = (obj.class?renameBlock(jsonResponse, obj.class):'');
     var style_val = (obj.style?'style="'+renameBlock(jsonResponse, obj.style)+'"':'');
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
     if (type_val == 'none' || typeof type_val === 'undefined') {
      element.innerHTML += renameBlock(jsonResponse, obj.title);
     }
     if (type_val == 'input') {
      if (action_val) action_val = 'onfocusout="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      element.innerHTML += '<input '+action_val+' id="'+name_val+'" class="form-control '+class_val+'" '+style_val+' '+(pattern_val?'pattern="'+pattern_val+'"':'')+' placeholder="'+renameBlock(jsonResponse, obj.title)+'" value="'+state_val+'">';
     }
     if (type_val == 'textarea') {
      if (action_val) action_val = 'onfocusout="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      element.innerHTML += '<textarea '+action_val+' id="'+name_val+'" class="form-control '+class_val+'" '+style_val+' '+(pattern_val?'pattern="'+pattern_val+'"':'')+' placeholder="'+renameBlock(jsonResponse, obj.title)+'">'+state_val+'</textarea>';
     }
     if (type_val == 'password') {
      if (action_val) action_val = 'onfocusout="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      element.innerHTML += '<input '+action_val+' id="'+name_val+'" class="form-control '+class_val+'" '+style_val+' '+(pattern_val?'pattern="'+pattern_val+'"':'')+' placeholder="'+renameBlock(jsonResponse, obj.title)+'" value="'+state_val+'" onfocus="this.type=\'text\'" type="password">';
     }
     if (type_val == 'button') {
      if (action_val) action_val = 'onclick="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      if (socket_val) action_val = 'onclick="send_socket(this, \'\', \''+socket_val+'\')"';
      if (actiondown_val) actiondown_val = 'onmouseup="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      element.innerHTML += '<input id="'+name_val+'" '+action_val+' '+actiondown_val+' class="'+class_val+'" '+style_val+' value="'+renameBlock(jsonResponse, obj.title)+'" type="button">';
     }
     if (type_val == 'checkbox') {
      var checked = '';
      if (!obj.design) {
       if (state_val == 1){ checked = 'checked'; }
       if (action_val) { action_val = 'onchange="val(this.id,(this.checked?\'1\':\'0\'));send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"'; } else { action_val = 'onchange="val(this.id,(this.checked?\'1\':\'0\'));"'; }
       element.innerHTML += '<label '+style_val+'><input id="'+name_val+'" value="'+state_val+'" '+action_val+' type="checkbox" class="'+class_val+'" '+checked+'> '+renameBlock(jsonResponse, obj.title)+'<\/label>';
      } else {
       if (state_val == 0){ checked = 'checked=""'; }
       if (action_val) { action_val = 'onchange="val(this.id,(this.checked?\'0\':\'1\'));send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"'; } else { action_val = 'onchange="val(this.id,(this.checked?\'0\':\'1\'));"'; }
       element.innerHTML += '<div class="toggle-button-cover" '+style_val+'><div class="button-cover"><div class="button r '+obj.design+'"><input type="checkbox" id="'+name_val+'" '+action_val+' class="checkbox" '+checked+'><div class="knobs"></div><div class="layer"></div></div></div><label for="'+name_val+'">'+renameBlock(jsonResponse, obj.title)+'<\/label></div>';
      }
     }
     if (type_val == 'range') {
      if (action_val) action_val = 'onchange="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      element.innerHTML += '<label id="'+(name_val?'label-'+name_val:'')+'" '+style_val+' style="display:block;"><h4>'+renameBlock(jsonResponse, obj.title)+'<\/h4> <input id="'+name_val+'" class="form-control '+class_val+'" '+action_val+' '+pattern_val+' value="'+state_val+'" type="range"><\/label>';
     }
     if (type_val == 'table') {
      var thead = '';
      element.innerHTML += '<table class="'+class_val+'" '+style_val+' id="'+name_val+'"><thead id="thead-'+state_val.replace(/[^a-z0-9]/gi,'-')+'"><tr><td><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center></td></tr><\/thead><tbody id="tbody-'+state_val.replace(/[^a-z0-9]/gi,'-')+'"><\/tbody><\/table>';
      loadTable(state_val, obj.title);
     }
     if (type_val == 'select') {
      if (action_val) action_val = 'onchange="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+obj.action+'\'),\''+response_val+'\')"';
      var option = '';
      jsonSelect = obj.title;
      if (isObject(jsonSelect)) {
       for(var key in jsonSelect) {
        option += '<option value="'+renameBlock(jsonResponse, key)+'"'+(state_val==key?' selected':'')+'>'+renameBlock(jsonResponse, jsonSelect[key])+'<\/option>';
       }
      } else {
       loadSelect(jsonSelect, name_val, state_val);
      }
      element.innerHTML += '<select class="form-control '+class_val+'" '+style_val+' '+action_val+' id="'+name_val+'">'+option+'<\/select>';
     }
     if (type_val == 'dropdown') {
      var option = '';
      var i = 0;
      var title1 = '';
      jsonSelect = obj.title;
      for(var key in jsonSelect) {
       if (i == 0) {
        title1 += renameBlock(jsonResponse, jsonSelect[key]);
       } else {
        option += '<li><a href="'+renameBlock(jsonResponse, key)+'">'+renameBlock(jsonResponse, jsonSelect[key])+'</a><\/li>';
       }
       i++;
      }
      element.innerHTML += '<div class="btn-group"><a href="#" class="dropdown-toggle '+class_val+'" '+style_val+' onclick="toggle(\''+name_val+'\');return false">'+title1+'</a><ul class="dropdown-menu hidden" id="'+name_val+'">'+option+'<\/ul><\/div>';
     }
     if (type_val == 'configs') {
      var option = '';
      option += '<div id="'+name_val+'"><div id="'+state_val.replace(/[^a-z0-9]/gi,'-')+'" class="'+class_val+'" '+style_val+'><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center><\/div><\/div>';
      option += "<div class=\"btn-group btn-block\"><input style=\"width:85%\" onclick=\"changeTextarea('"+state_val.replace(/[^a-z0-9]/gi,'-')+"');send_request_edit(this, val('"+state_val.replace(/[^a-z0-9]/gi,'-')+"-edit'),'configs/"+state_val+"','if(confirm(\\'"+jsonResponse.LangReset2+" "+jsonResponse.LangReset3+"\\')){send_request(this,\\'/restart?device=ok\\');toggle(\\'restart-esp\\');setTimeout(function(){toggle(\\'restart-esp\\');},20000);};');\" class=\"btn btn-block btn-success\" value=\""+jsonResponse.LangSave+"\" type=\"button\">";
      option += '<a href="#" style="width:15%" class="btn btn-info dropdown-toggle" onclick="toggle(\'cloud\');return false"><i class="cloud-img"></i> <span class="caret"></span></a>';
      option += '<ul class="dropdown-menu hidden" style="right:0;left:auto" id="cloud"><li><a onclick="toggle(\'cloud\');cloudUpload(\''+jsonResponse.mac+'\',\''+jsonResponse.configs+'\');return false" href="#"><i class="cloud-img"></i> '+jsonResponse.LangCloudUpload+'</a></li><li><a onclick="toggle(\'cloud\');cloudDownload(\''+jsonResponse.mac+'\',\''+jsonResponse.configs+'.txt\');return false" href="#"><i class="cloud-img"></i> '+jsonResponse.LangCloudDownload+'</a></li><li><a href="/configs/'+jsonResponse.configs+'.txt?download=true" download=""><i class="download-img"></i> '+jsonResponse.LangCloudPC+'</a></li></ul>';
      option += '</div>';
      element.innerHTML += option;
      setTimeout("loadConfigs('"+state_val+"',jsonResponse)", 500);
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
      element.innerHTML += '<div id="'+name_val+'" class="'+renameBlock(jsonResponse, '{{'+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden}}')+'"><button class="close" onclick="hide(\''+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden\',this)">×<\/button><a href="'+renameGet(obj.action)+'" target="_blank" class="close">'+(typeof action_val!='undefined'&&action_val?'<i class="popup-img"><\/i>':'')+'<\/a><h2><span id="'+state_val.replace(/[^a-z0-9]/gi,'')+'-title">'+renameBlock(jsonResponse, obj.title)+'</span> <span id="'+state_val.replace(/[^a-z0-9]/gi,'')+'-data"></span><\/h2><div id="'+state_val.replace(/[^a-z0-9]/gi,'')+'" class="'+class_val+'" '+style_val+'><\/div><hr><\/div>';
      if (renameBlock(jsonResponse, '{{'+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden}}') != 'hidden') {
       setTimeout("loadChart('"+state_val.replace(/[^a-z0-9]/gi,'')+"','"+state_val+"', {"+obj.options+"},"+obj.refresh+","+obj.points+",'"+obj.chartist+"')", 1500);
      }
     }
     if (type_val == 'loadJson') {
      element.innerHTML += '<div id="json-'+state_val.replace(/[^a-z0-9]/gi,'-')+'" class="'+class_val+'" '+style_val+'><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center><\/div>';
      loadJson(state_val, obj.refresh, jsonResponse);
     }
     if (type_val == 'file') {
      var set_action = '';
      if (obj.action != undefined) {
       set_action = ",'send_request(this,\\'"+obj.action+"\\')'";
      }
      element.innerHTML += '<textarea '+style_val+' id="file-'+state_val.replace(/[^a-z0-9]/gi,'-')+'">'+jsonResponse.LangLoading+'</textarea><input class="'+class_val+'" onclick="send_request_edit(this, val(\'file-'+state_val.replace(/[^a-z0-9]/gi,'-')+'\'),\''+state_val+'\' '+set_action+');" value="'+renameBlock(jsonResponse, obj.title)+'" type="button">';
      loadFile(state_val);
     }
     if (type_val == 'csv') {
      var set_action = '';
      if (obj.action != undefined) {
       set_action = ",'send_request(this,\\'"+obj.action+"\\')'";
      }
      element.innerHTML += '<table id="table-csv-'+state_val.replace(/[^a-z0-9]/gi,'-')+'" '+style_val+'></table><textarea style="display:none" id="csv-'+state_val.replace(/[^a-z0-9]/gi,'-')+'"></textarea><input class="'+class_val+'" onclick="html_to_csv(\'csv-'+state_val.replace(/[^a-z0-9]/gi,'-')+'\');send_request_edit(this, val(\'csv-'+state_val.replace(/[^a-z0-9]/gi,'-')+'\'),\''+state_val+'\' '+set_action+');" value="Save" type="button">';
      loadCSV(state_val,obj.title);
     }
     if (type_val == 'time-list') {
      element.innerHTML += '<table class="'+class_val+'" '+style_val+' id="'+name_val+'"><tbody id="time-list"><\/tbody><\/table>';
      loadTime(jsonResponse);
     }
     if (type_val == 'time-add') {
      const template = (x,y,i,z) => ' <label class="label label-'+x+'"><input type="checkbox" name="day-'+y+'" id="day-'+i+'" checked>'+jsonResponse['Lang'+z]+'</label> ';
      var option = '';
      option += '<input type="hidden" id="hidden-val-then" value="1"><div id="new-then"></div> <h4>';
      option += template('danger','sun','0','Sun');
      option += template('info','mon','1','Mon');
      option += template('info','tue','2','Tue');
      option += template('info','wed','3','Wed');
      option += template('info','thu','4','Thu');
      option += template('info','fri','5','Fri');
      option += template('danger','sat','6','Sat');
      option += ' <label class="label label-default"><input type="checkbox" onchange="toggleCheckbox(this)" checked>'+jsonResponse.LangAll+'</label><br>';
      option += ' <label class="label label-default"><input type="checkbox" name="run1" id="run-1">'+jsonResponse.LangRun1+'</label></h4>';
      option += '<input id="set-time" class="form-control" pattern="(0[0-9]|1[0-9]|2[0-3])(:[0-5][0-9]){2}" placeholder="'+jsonResponse.LangTime4+'. '+jsonResponse.LangExample+': 07:09:30" value="" style="width:90%;display:inline"><a href="#" class="btn btn-default" style="width:10%;" onclick="val(\'set-time\',\'Loading...\');ajax.get(\'/config.live.json\',{},function(response){var jsonFiles=JSON.parse(response);val(\'set-time\',jsonFiles[\'time\']);},true);return false"><i class="clock-img"></i></a>';
      option += "<input class=\"btn btn-block btn-lg btn-success\" onclick=\"addTimer();\" value=\""+jsonResponse.LangSave+"\" type=\"button\">";
      element.innerHTML += option;
      loadNewThen('new-then',' ');
      html("load-life-opt","onclick");
     }
     if (type_val == 'scenary-list') {
      element.innerHTML += '<table class="'+class_val+'" '+style_val+' id="'+name_val+'"><tbody id="scenary-list"><\/tbody><\/table>';
      loadScenary(jsonResponse,'loadList');
     }
     if (type_val == 'scenary-add') {
      var option = '';
      option += '<select class="form-control" id="ssdp-list0" onchange="loadScenaryList(0,\'loadInTextarea\',this.options[this.selectedIndex].value);loadLive(this.value,\'config.live.json\',\'ssdp-module\');toggle(\'ssdp-module\',\'hidden\');"><\/select>';
      option += '<select class="form-control hidden" id="ssdp-module" onchange="pattern(this.querySelector(\':checked\').getAttribute(\'title\'),\'ssdp-command\');toggle(\'hidden-if\',\'hidden\');toggle(\'or-and\',\'hidden\');"><\/select>';
      option += '<span class="hidden" id="hidden-if"><select class="form-control" id="ssdp-condition" style="width:50%;display:inline"><option value="=">'+jsonResponse.LangEqual+' (=)<\/option><option value="<">'+jsonResponse.LangLess+' (<)<\/option><option value=">">'+jsonResponse.LangMore+' (>)<\/option><option value="<=" disabled>'+jsonResponse.LangLess+' '+jsonResponse.LangOr+' '+jsonResponse.LangEqual+' (<=) '+jsonResponse.LangSoon+'<\/option><option value=">=" disabled>'+jsonResponse.LangMore+' '+jsonResponse.LangOr+' '+jsonResponse.LangEqual+' (>=) '+jsonResponse.LangSoon+'<\/option><option value="!=">'+jsonResponse.LangNotEqual+' (!=)<\/option><\/select>';
      option += '<input class="form-control" id="ssdp-command" pattern="" style="width:40%;display:inline" value=""><a href="#" id="load-life-opt" class="btn btn-default" style="width:10%;" onclick="loadLive2(\'ssdp-list0\',\'ssdp-module\',\'ssdp-command\');return false"><i class="find-replace-img"></i></a></span>';
      option += '<textarea id="scenary-list-edit" style="display:none" class="form-control"></textarea>';
      option += '<input type="hidden" id="hidden-val-and" value="1"><input type="hidden" id="hidden-val-or" value="1"><div id="new-and-or"></div>';
      option += '<div class="btn-group hidden" id="or-and" style="width:100%;"><input onclick="loadNewAnd(\'new-and-or\');" class="btn btn-sm btn-default" style="width:50%;" value="+ '+jsonResponse.LangAnd+'" type="button">';
      option += '<input onclick="loadNewOr(\'new-and-or\');" class="btn btn-sm btn-default" style="width:50%;" value="+ '+jsonResponse.LangOr+'" type="button"></div>';
      option += '<input type="hidden" id="hidden-val-then" value="1"><div id="new-then"></div>';
      option += '<input onclick="loadNewThen(\'new-then\');" class="btn btn-sm btn-block btn-default" value="+ '+jsonResponse.LangThen+'" type="button">';
      option += "<input onclick=\"saveScenary(jsonResponse,'')\" class=\"btn btn-block btn-lg btn-success\" value=\""+jsonResponse.LangSave+"\" type=\"button\">";
      element.innerHTML += '<h3>'+jsonResponse.LangIf+'</h3> '+option;
      loadScenary(jsonResponse);
      html("load-life-opt","onclick");
     }
     if (type_val == 'scenary-test') {
      var option = '';
      option += '<select class="form-control" id="ssdp-list2" style="display:none"><option value="'+location.host+'">-</option></select>';
      option += '<input type="hidden" id="hidden-val-then" value="1"><div id="new-then"></div>';
      option += '<select class="form-control hidden" id="scenary-then2" style="width:50%;" onchange="loadCommandHelp(this.value,\'command-help2\',\'scenary-othe2\');toggle(\'if-then2\',\'hidden\');"><option value=""></option></select>';
      option += '<div id="if-then2" class="hidden"><div id="command-help2" class="alert alert-warning"></div><a href="#" id="scenary-othe-play2" class="btn btn-default" style="width:10%;float:right;" onclick="send_request(this, \'http://\'+elem(\'ssdp-list2\').options[elem(\'ssdp-list2\').selectedIndex].value+\'/cmd?command=\'+elem(\'scenary-then2\').options[elem(\'scenary-then2\').selectedIndex].value+\' \'+elem(\'scenary-othe2\').value.replace(/&/g,\'%26\'),\'\');return false" title="'+jsonResponse.LangRun+'"><i class="eye-img"></i></a><input class="form-control" style="width:90%" placeholder="Действие" id="scenary-othe2" type="text"></div>';
      element.innerHTML += '<h4 style="float:left;">Module:</h4> '+option;
      setTimeout("loadCommand('"+location.host+"','command.json','scenary-then2');toggle('scenary-then2','hidden');", 500);
     }
     if (type_val == 'macros') {
      element.innerHTML += '<div class="'+class_val+'" '+style_val+' id="'+(name_val?name_val:'loadMacros')+'"><\/div>';
      loadMacros(jsonResponse,(name_val?name_val:'loadMacros'));
     }
     if (type_val == 'login') {
      var option = '';
      option += '<h2>'+jsonResponse.LangAuthorization+'</h2><div class="alert alert-warning" style="width:45%;float:right;">'+renameBlock(jsonResponse, obj.title)+'</div>';
      option += '<input id="passLogin" class="form-control " style="width:50%;display:inline" placeholder="'+jsonResponse.LangPass+'" value="" onfocus="this.type=\'text\'" type="password">';
      option += '<a class="btn btn-block btn-success" style="width:50%;display:block" href="#" onclick="if(\''+state_val+'\'==val(\'passLogin\')){sessionStorage.setItem(\'sossionLogin\', \'hidden\');toggle(\'loginForm\');}else{alert(\'The password is incorrect\')}">'+jsonResponse.LangSave+'</a>';
      element.innerHTML += '<div id="loginForm" class="'+jsonResponse.sossionLogin+'" style="background-color:#fff;position:fixed;top:0;left:0;right:0;bottom:0;z-index:9999;padding:10% 30%;">'+option+'</div>';
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
     if (type_val == 'issues') {
      element.innerHTML += '<div id="issues-list" class="'+class_val+'" '+style_val+'><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center><\/div>';
      setTimeout("loadIssues('tretyakovsa/Sonoff_WiFi_switch',"+(state_val)+");", 1500);
     }
     if (type_val == 'commits') {
      element.innerHTML += '<div id="commits-list" class="'+class_val+'" '+style_val+'><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center><\/div>';
      setTimeout("loadCommits('tretyakovsa/Sonoff_WiFi_switch',"+(state_val)+");", 1500);
     }
     if (type_val == 'dev') {
      var option = '<div id="'+name_val+'" class="'+class_val+'" '+style_val+'><a href="/help.htm" target="_blank" class="close"><i class="help-img"><\/i><\/a>'+renameBlock(jsonResponse, obj.title)+'<span id="dev-update" class="hidden"><a href="/edit" class="btn btn-primary" target="_blank">File manager<\/a> <a href="/page.htm?starting" class="btn btn-primary">Starting log<\/a> <a href="/page.htm?debug" class="btn btn-primary">Debug<\/a> ';
      if (searchModule(jsonResponse.module,"upgrade")){
       option += ' <div class="btn-group"><a href="#" class="btn btn-danger dropdown-toggle" onclick="toggle(\'repos-all\');loadBuild(\'sonoff\',\'all\',\''+state_val+'\');return false">Upgrade <span class="caret"><\/span><\/a><ul class="dropdown-menu hidden" id="repos-all" style="min-width:350px"><li><a href="https://github.com/tretyakovsa/Sonoff_WiFi_switch/commits/master" style="text-align:right" target="_blank"><i class="help-img"><\/i> Github code history<\/a><ul id="sonoff-all" style="margin-right:20px"><li><a href="#">'+jsonResponse.LangLoading+'<\/a><\/li><\/ul><\/li><\/ul><\/div>';
      }
      option += '<br><br>';
      option += jsonResponse.LangType+': <div class="btn-group"><select class="btn btn-default btn-sx" onchange="send_request(this, \'/configs?set=\'+this.value,\'[[configs-edit-button]]\')"><option value="'+jsonResponse.configs+'">'+jsonResponse.configs+'<\/option><option value="sonoff-rf">Sonoff-rf / Sonoff TH16 / Wi-Fi Smart Socket<\/option><option value="sonoff-pow">Sonoff-Pow ('+jsonResponse.LangSoon+')<\/option><option value="rgb">RGB (WS2811/WS2812/NeoPixel LEDs)<\/option><option value="rgb-shim">RGB SHIM (5050/3528/2835)<\/option><option value="jalousie">Jalousie<\/option><option value="smart-room">Smart-Room<\/option><option value="wifi-relay-5v">WiFi Relay 5v<\/option><option value="manually">Manually</option></select> <a href="/page.htm?configs&'+jsonResponse.configs.toLowerCase()+'" id="configs-edit-button" class="btn btn-primary"><i class="opt-img"></i><\/a><\/div><hr>';
      option += '<form method="POST" action="/update" enctype="multipart/form-data"><div class="btn-group"><input type="file" class="btn btn-primary btn-xs" name="update" style="height:33px" accept=".bin"><input type="submit" class="btn btn-default btn-sm" value="Update build" onclick="this.value=\''+jsonResponse.LangLoading+'\';" style="height:33px"><\/div><\/form>';
      option += '<\/span><\/div>';
      element.innerHTML += option;
     }
    }
   }
  }
  i++;
 }
 disable_socket();
}

function disable_socket(){
 var inputs = document.querySelectorAll('input');
 for(var i=0,len=inputs.length;i<len;i++){
  inputs[i].addEventListener('focus',function(){
   socket_input = '1';
  });
  inputs[i].addEventListener('blur',function(){
   socket_input = '0';
  })
 }
}

function isObject(obj){
 return obj != null && obj.constructor.name === "Object"
}

function toggleCheckbox(element) {
 for (i = 0; i < 7; i++) {
  elem("day-"+i).checked = (element.checked?true:false);
 }
}

function deleteTimer(id,ip) {
 ajax.get('http://'+ip+'/timer.save.json?'+Math.random(),{},function(response) {
  var timerList=JSON.parse(response);
  timerList.timer.splice(id,1);
  send_request_edit(this, JSON.stringify(timerList), 'timer.save.json', 'html(\'time-list\');loadTime(jsonResponse);send_request(this,\'http://'+ip+'/settimer\');', ip);
 },true);
}

function activeTimer(ids,ip,active) {
 ajax.get('http://'+ip+'/timer.save.json?'+Math.random(),{},function(response) {
  var timerList=JSON.parse(response);
  for (var i = 0; i < timerList['timer'].length; i++) {
   if (timerList['timer'][i]['id'] == ids) {
    timerList['timer'][i]['active'] = active;
   }
  }
  send_request_edit(this, JSON.stringify(timerList), 'timer.save.json', 'html(\'time-list\');loadTime(jsonResponse);send_request(this,\'http://'+ip+'/settimer\');', ip);
 },true);
}

function addTimer() {
 var ip = elem('ssdp-list2').options[elem('ssdp-list2').selectedIndex].value;
 var command = elem('scenary-then2').options[elem('scenary-then2').selectedIndex].value+' '+elem('scenary-othe2').value;
 ajax.get('http://'+ip+'/timer.save.json?'+Math.random(),{},function(response) {
  var timerList=JSON.parse(response);
  var daycheck = '';
  for (i = 0; i < 7; i++) {
   daycheck+=(elem("day-"+i).checked?'1':'0');
  }
  var run1=(elem("run-1").checked?'1':'0');
  timerList.timer.push({"id":Math.floor(Math.random()*10000),"day":daycheck,"time1":val('set-time'),"com1":command,"run1":run1,"active":1});
  send_request_edit(this, JSON.stringify(timerList), 'timer.save.json', 'html(\'time-list\');loadTime(jsonResponse);send_request(this,\'http://'+ip+'/settimer\');', elem('ssdp-list2').options[elem('ssdp-list2').selectedIndex].value);
 },true);
}

function loadJson(file, setDelay, jsonResponse) {
 function setLoad(){
  ajax.get(file+'?'+Math.random(),{},function(response) {
   html('json-'+file.replace(/[^a-z0-9]/gi,'-'), ' ');
   var jsonPage2=JSON.parse(response);
   viewTemplate(jsonPage2,jsonResponse);
  },true);
 };
 if (!isNaN(setDelay)) {
  var valTime;
  clearInterval(valTime);
  valTime=setInterval(function(){setLoad();}, setDelay);
 } else {
  setLoad();
 }
}

function loadSelect(file,name_val,state_val) {
 ajax.get(file+'?'+Math.random(),{},function(response) {
  var result = JSON.parse(response);
  option = '';
  for(var key in result) {
   option += '<option value="'+renameBlock(jsonResponse, key)+'"'+(state_val==key?' selected':'')+'>'+renameBlock(jsonResponse, result[key])+'<\/option>';
  }
  elem(name_val).innerHTML = option;
 },true);
}

function loadFile(file) {
 ajax.get(file+'?'+Math.random(),{},function(response) {
  val('file-'+file.replace(/[^a-z0-9]/gi,'-'), response);
 },true);
}

function loadCSV(file,title) {
 ajax.get(file+'?'+Math.random(),{},function(response) {
  //val('csv-'+file.replace(/[^a-z0-9]/gi,'-'), response);
  var tbody = '';
  var table_tr = response.replace(/\r\n|\n\r|\n|\r/g,"\n").split("\n");
  for (var i = 0; i < table_tr.length; i++) {
   var table_td = table_tr[i].split(";");
   tbody += '<tr>';
   for (var y = 0; y < table_td.length; y++) {
    if (i == 0) {
     tbody += '<th>'+table_td[y]+'<\/th>';
    } else {
     if (title[y] == 'html') {
      tbody += '<td>'+table_td[y]+'<\/td>';
     } else {
      tbody += '<td><input class="form-control" type="'+title[y]+'" value="'+table_td[y]+'" '+(title[y]=='checkbox'?'onclick="(this.checked?this.value=1:this.value=0)"':'')+' '+(title[y]=='checkbox' && table_td[y]==1?'checked':'')+'><\/td>';
     }
    }
   }
   tbody += '</tr>';
  }
  document.getElementById('table-csv-'+file.replace(/[^a-z0-9]/gi,'-')).innerHTML += ''+tbody+'';
 },true);
}

function html_to_csv(file) {
 var csv = [];
 var fixed = '';
 var rows = document.querySelectorAll('[id^="table-'+file+'"] tr')
 for (var i = 0; i < rows.length; i++) {
  var row = [], cols = rows[i].querySelectorAll("td, th, input");
  for (var j = 0; j < cols.length; j++) {
   if (typeof cols[j].value !== "undefined") {
    fixed = cols[j].value;
   } else {
    fixed = cols[j].innerHTML;
   }
   if (fixed.indexOf("<input") == -1) {
    row.push(fixed);
   }
  }
  csv.push(row.join(";"));
 }
 val(file, csv.join("\r\n"));
}

function pattern(str,id) {
 elem(id).setAttribute("pattern","["+(str=='number'?'0-9-':'0-9a-zA-Zа-яА-Яё:_. ')+"]{1,100}");
}

function loadTime(jsonResponse) {
 html('time-list', '<tr><td colspan="2"><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center></td></tr>');
 ajax.get('/ssdp.list.json?'+Math.random(),{},function(response) {
  var option = '';
  var ip=sortObject(JSON.parse(response));
  for (var i in ip) {
   loadDeviceTime(jsonResponse,i,ip[i]);
  }
  html('time-list', ' ');
 },true);
}

function loadDeviceTime(jsonResponse,ssdp,ip) {
 //html('time-list', '<tr><td colspan="2"><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center></td></tr>');
 ajax.get('http://'+ip+'/timer.save.json?'+Math.random(),{},function(response) {
  var options = '';
  var timeDevice=JSON.parse(response);
  for (var i in timeDevice['timer']) {
   var day_view = timeDevice['timer'][i].day.split("");
   var day_view_add = dayTemplate(day_view,jsonResponse);
   if (timeDevice['timer'][i]['run1'] == 1) {
    day_view_add += '<span class="label label-warning">'+jsonResponse.LangRun1+'</span>';
   }
   options += '<tr><td><input id="active" type="checkbox" onchange="activeTimer(\''+timeDevice['timer'][i]['id']+'\',\''+ip+'\',(this.checked?\'1\':\'0\'))" '+(timeDevice['timer'][i]['active']==1 || typeof timeDevice['timer'][i]['active']=='undefined'?'checked':'')+'></td><td><span class="label label-default"><i class="clock-new-img"></i> '+timeDevice['timer'][i].time1+'</span></td><td>'+day_view_add+'</td><td>'+timeDevice['timer'][i].com1+' <a href="#" onclick="send_request(this, \'http://'+ip+'/cmd?command='+timeDevice['timer'][i].com1+'\');return false" title="'+jsonResponse.LangRun+'"><i class="eye-img" style="opacity:0.2;"></i></a></td><td><a class="btn btn-sm btn-danger" style="float:right;" href="#" onclick="if(confirm(\''+jsonResponse.LangDel+'?\')){deleteTimer(\''+i+'\',\''+ip+'\');}return false"><i class="del-img"></i> <span class="hidden-xs">'+jsonResponse.LangDel+'</span></a></td><tr>';
  }
  elem("time-list").innerHTML += '<tr><td colspan="2"><h4><a href="http://'+ip+'">'+ssdp+'</a> <a href="http://'+ip+'/scenary.save.txt?download=true" download="" title="'+jsonResponse.LangCloudPC+'"><i class="download-img" style="opacity:0.2"><\/i><\/a></h4></td></tr><tr><td></td><td><b>'+jsonResponse.LangTime4+'</b></td><td><b>'+jsonResponse.LangDay+'</b></td><td><b>command</b></td><td></td></tr>'+options;
 },true);
}

function dayTemplate(day_view, jsonResponse) {
 var day_view_add = '';
 const template = (x,y) => ' <span class="label label-'+x+'">'+jsonResponse['Lang'+y]+'</span> ';
 for (var y in day_view) {
  if (y == 0 && day_view[y] == 1){ day_view_add+=template('danger','Sun'); }
  if (y == 1 && day_view[y] == 1){ day_view_add+=template('info','Mon'); }
  if (y == 2 && day_view[y] == 1){ day_view_add+=template('info','Tue'); }
  if (y == 3 && day_view[y] == 1){ day_view_add+=template('info','Wed'); }
  if (y == 4 && day_view[y] == 1){ day_view_add+=template('info','Thu'); }
  if (y == 5 && day_view[y] == 1){ day_view_add+=template('info','Fri'); }
  if (y == 6 && day_view[y] == 1){ day_view_add+=template('danger','Sat'); }
 }
 return day_view_add;
}




function saveScenary(jsonResponse,loadList) {


  ajax.get('http://'+elem('ssdp-list0').options[elem('ssdp-list0').selectedIndex].value+'/config.options.json?'+Math.random(),{},function(response) {
  var view=JSON.parse(response);
  var scenary_file = (view['configs']?'scenary/'+view['configs']+'.txt':'scenary.save.txt');

 loadInTextarea();
 // send_request_edit(this, val('scenary-list-edit'),'scenary.save.txt','send_request(this, \'http://'+elem('ssdp-list0').options[elem('ssdp-list0').selectedIndex].value+'/setscenary');val('ssdp-list0',' ');loadScenary(jsonResponse,'loadList');',elem('ssdp-list0').options[elem('ssdp-list0').selectedIndex].value);
    send_request_edit(this, val('scenary-list-edit')+'\r\n',scenary_file,'send_request(this, \'http://\'+elem(\'ssdp-list0\').options[elem(\'ssdp-list0\').selectedIndex].value+\'/setscenary\');val(\'ssdp-list0\',\' \');loadScenary(jsonResponse,\'loadList\');',elem('ssdp-list0').options[elem('ssdp-list0').selectedIndex].value);
 },true);

}



function loadScenary(jsonResponse,loadList) {
 html('scenary-list', '<tr><td colspan="2"><center><span class="loader"></span>'+jsonResponse.LangLoading+'</center></td></tr>');
 ajax.get('/ssdp.list.json?'+Math.random(),{},function(response) {
  html('scenary-list', ' ');
  toggle('hidden-if','show');
  toggle('ssdp-module','show');
  html('new-and-or',' ');
  html('new-then',' ');
  var option = '';
  var ip=sortObject(JSON.parse(response));
  if (loadList) {
   for (var i in ip) {
    loadScenaryList(jsonResponse,i,ip[i]);
   }
   val('hidden-val-then',1);
   loadNewThen('new-then');
  } else {
   for (var i in ip) {
    option += '<option value="'+ip[i]+'">'+i+'<\/option>';
   }
   html("ssdp-list0",'<option value="">'+jsonResponse.LangSelect+'<\/option>'+option);
   val('hidden-val-and',1);
   val('hidden-val-or',1);
   val('hidden-val-then',1);
   loadNewThen('new-then');
  }
 },true);
}

function loadScenaryList(jsonResponse,selectDevice,ip) {

 ajax.get('http://'+ip+'/config.options.json?'+Math.random(),{},function(response) {
  var view=JSON.parse(response);
  var scenary_file = (view['configs']?'scenary/'+view['configs']+'.txt':'scenary.save.txt');

  ajax.get((ip?'http://'+ip:'')+"/"+scenary_file+"?"+Math.random(),{},function(response) {
   if (selectDevice == 'loadInTextarea') {
    elem("scenary-list-edit").innerHTML = response;
   } else if (Number.isInteger(selectDevice) == true) {
    var reg = new RegExp("([\\s\\S]+?)(id\\s+\\d+)", "mig");
    send_request_edit(this, response.replace(reg,function(a,b,c){return new RegExp("^id+\\s+"+selectDevice+"$").test(c)?"":a}),scenary_file,'html(\'scenary-list\', \' \');send_request(this,\'http://'+ip+'/setscenary\');loadScenary(jsonResponse,\'loadList\');',ip);
   } else {
    var createText = '';
    var block = response.split(/\n|\r| /);
    //var block = response.split(/\n\r|\r\n|\n|\r| /g);
    for (var i = 0 ; i < block.length; i++) {
     createText += ' '+(renameBlock(jsonResponse, '{{Lang'+block[i]+'}}')===undefined?block[i]:renameBlock(jsonResponse, '{{Lang'+block[i]+'}}'));
    }
    elem("scenary-list").innerHTML += '<tr><td colspan="2"><h4><a href="http://'+ip+'">'+selectDevice+'</a> <a href="http://'+ip+'/'+scenary_file+'?download=true" download="" title="'+jsonResponse.LangCloudPC+'"><i class="download-img" style="opacity:0.2"><\/i><\/a></h4></td></tr>'+createText.replace(/if /gi,'<tr><td><b>'+jsonResponse.LangIf+'</b> ').replace(/_/g,' ').replace(/or /gi,'<br><b>'+jsonResponse.LangOr+'</b> ').replace(/this /gi,' ').replace(/and /gi,'<br><b>'+jsonResponse.LangAnd+'</b> ').replace(/then /gi,'<br><b>'+jsonResponse.LangThen+'</b> ').replace(/(id)\s+(\d+)/mg,'<hr><\/td><td style="vertical-align:top;"><a class="btn btn-sm btn-danger" style="float:right;" href="#" onclick="if(confirm(\''+jsonResponse.LangDel+'?\')){loadScenaryList(jsonResponse,$2,\''+ip+'\');}return false"><i class="del-img"></i> <span class="hidden-xs">'+jsonResponse.LangDel+'</span></a><\/td><\/tr>');
   }
  },true);

 },true);

}

function loadNewThen(where,titles) {
 var option = '';
 var number = parseInt(elem("hidden-val-then").value)+1;
 elem("hidden-val-then").value = number;
 if (titles) {
  option += '<h3>'+titles+'<sup>'+(number>2?jsonResponse.LangOptional:'')+'</sup></h3>';
 } else {
  option += '<h3>'+jsonResponse.LangThen+'<sup>'+(number>2?jsonResponse.LangOptional:'')+'</sup></h3>';
 }
 option += '<select class="form-control" id="ssdp-list'+number+'" onchange="loadCommand(this.value,\'command.json\',\'scenary-then'+number+'\');toggle(\'scenary-then'+number+'\',\'hidden\');"><\/select>';
 option += '<select class="form-control hidden" id="scenary-then'+number+'" onchange="loadCommandHelp(this.value,\'command-help'+number+'\',\'scenary-othe'+number+'\');toggle(\'if-then'+number+'\',\'hidden\');"><option value=""><\/option><\/select>';
 option += '<div id="if-then'+number+'" class="hidden"><div id="command-help'+number+'" class="alert alert-warning"></div><a href="#" id="scenary-othe-play'+number+'" class="btn btn-default" style="width:10%;float:right;" onclick="send_request(this, \'http://\'+elem(\'ssdp-list'+number+'\').options[elem(\'ssdp-list'+number+'\').selectedIndex].value+\'/cmd?command=\'+elem(\'scenary-then'+number+'\').options[elem(\'scenary-then'+number+'\').selectedIndex].value+\' \'+elem(\'scenary-othe'+number+'\').value.replace(/&/g,\'%26\'),\'\');return false"  title="'+jsonResponse.LangRun+'"><i class="eye-img"></i></a><input class="form-control" style="width:90%" placeholder="'+jsonResponse.LangAction+'" id="scenary-othe'+number+'" type="text" /></div>';
 elem(where).insertAdjacentHTML('beforeEnd', option);
 ajax.get('/ssdp.list.json?'+Math.random(),{},function(response) {
  var options = '';
  var ip=sortObject(JSON.parse(response));
  for (var i in ip) {
   options += '<option value="'+ip[i]+'">'+i+'<\/option>';
  }
  html("ssdp-list"+number+"",'<option value="">'+jsonResponse.LangSelect+'<\/option>'+options);
  html("scenary-othe-play"+number,"onc"+"lick");
 },true);
}

function loadNewAnd(where) {
 var option = '';
 var number = parseInt(elem("hidden-val-and").value)+1;
 elem("hidden-val-and").value = number;
 option += '<h3>'+jsonResponse.LangAnd+'<sup>'+jsonResponse.LangOptional+'</sup></h3>';
 option += '<select class="form-control hidden" id="ssdp-module-and'+number+'" onchange="pattern(this.querySelector(\':checked\').getAttribute(\'title\'),\'ssdp-command-and'+number+'\');toggle(\'hidden-if-and'+number+'\',\'hidden\');"><\/select>';
 option += '<span class="hidden" id="hidden-if-and'+number+'"><select class="form-control" id="ssdp-condition-and'+number+'" style="width:50%;display:inline"><option value="=">'+jsonResponse.LangEqual+' (=)<\/option><option value="<">'+jsonResponse.LangLess+' (<)<\/option><option value=">">'+jsonResponse.LangMore+' (>)<\/option><option value="<=">'+jsonResponse.LangLess+' '+jsonResponse.LangOr+' '+jsonResponse.LangEqual+' (<=)<\/option><option value=">=">'+jsonResponse.LangMore+' '+jsonResponse.LangOr+' '+jsonResponse.LangEqual+' (>=)<\/option><option value="!=">'+jsonResponse.LangNotEqual+' (!=)<\/option><\/select>';
 option += '<input class="form-control" id="ssdp-command-and'+number+'" pattern="" style="width:40%;display:inline" value=""><a href="#" id="load-life-opt-and'+number+'" class="btn btn-default" style="width:10%;" onclick="loadLive2(\'ssdp-list0\',\'ssdp-module-and'+number+'\',\'ssdp-command-and'+number+'\');return false"><i class="find-replace-img"></i></a></span>';
 elem(where).insertAdjacentHTML('beforeEnd', option);
 loadLive(elem('ssdp-list0').options[elem('ssdp-list0').selectedIndex].value,'config.live.json','ssdp-module-and'+number+'');
 toggle('ssdp-module-and'+number+'','hidden');
}

function loadNewOr(where) {
 var option = '';
 var number = parseInt(elem("hidden-val-or").value)+1;
 elem("hidden-val-or").value = number;
 option += '<h3>'+jsonResponse.LangOr+'<sup>'+jsonResponse.LangOptional+'</sup></h3>';
 option += '<select class="form-control hidden" id="ssdp-module-or'+number+'" onchange="pattern(this.querySelector(\':checked\').getAttribute(\'title\'),\'ssdp-command-or'+number+'\');toggle(\'hidden-if-or'+number+'\',\'hidden\');"><\/select>';
 option += '<span class="hidden" id="hidden-if-or'+number+'"><select class="form-control" id="ssdp-condition-or'+number+'" style="width:50%;display:inline"><option value="=">'+jsonResponse.LangEqual+' (=)<\/option><option value="<">'+jsonResponse.LangLess+' (<)<\/option><option value=">">'+jsonResponse.LangMore+' (>)<\/option><option value="<=">'+jsonResponse.LangLess+' '+jsonResponse.LangOr+' '+jsonResponse.LangEqual+' (<=)<\/option><option value=">=">'+jsonResponse.LangMore+' '+jsonResponse.LangOr+' '+jsonResponse.LangEqual+' (>=)<\/option><option value="!=">'+jsonResponse.LangNotEqual+' (!=)<\/option><\/select>';
 option += '<input class="form-control" id="ssdp-command-or'+number+'" pattern="" style="width:40%;display:inline" value=""><a href="#" id="load-life-opt-or'+number+'" class="btn btn-default" style="width:10%;" onclick="loadLive2(\'ssdp-list0\',\'ssdp-module-or'+number+'\',\'ssdp-command-or'+number+'\');return false"><i class="find-replace-img"></i></a></span>';
 elem(where).insertAdjacentHTML('beforeEnd', option);
 loadLive(elem('ssdp-list0').options[elem('ssdp-list0').selectedIndex].value,'config.live.json','ssdp-module-or'+number+'');
 toggle('ssdp-module-or'+number+'','hidden');
}

function sortObject(obj) {
 return Object.keys(obj).sort().reduce(function (result, key) {
  result[key] = obj[key];
  return result;
 }, {});
}

function loadMacros(jsonResponse,where,domain,class_val) {
 html(where, 'Macros loading...');
 if (!domain) {domain=window.location.hostname}

 ajax.get('http://'+domain+'/config.options.json?'+Math.random(),{},function(response) {
  var view=JSON.parse(response);

  ajax.get('http://'+domain+'/'+(view['configs']?'scenary/'+view['configs']+'.txt':'scenary.save.txt')+'?'+Math.random(),{},function(response) {
   html(where, ' ');
   var option = macrosTemplate(response,domain);
   html(where, (option?option+'<hr>':''));
  },true);

 },true);
}

function macrosTemplate(response,domain) {
 var options = '';
 var macros_ip=response.split("if voice = ");
 for (var i in macros_ip) {
  var command = macros_ip[i].substr(0,macros_ip[i].indexOf("\n"));
  if (command && command.indexOf(" = ") == -1 ) {
   options+='<input onclick="send_request(this, \'http://'+domain+'/voice?command='+command.replace(/\n|\r/g,"")+'\')" class="btn btn-block btn-primary" value="'+command.replace(/_/g,' ')+'" type="button">';
  }
 }
 return options;
}

function loadCommandHelp(jsonParam,where,to) {
 html(where, 'Loading...');
 elem(to).readOnly = (jsonParam=='voice'?true:false);
 var domain = elem('ssdp-list'+where.replace(/[^0-9]/gi,'')).options[elem('ssdp-list'+where.replace(/[^0-9]/gi,'')).selectedIndex].value;
 ajax.get('http://'+domain+'/'+(jsonParam=='voice'?'scenary.save.txt':'command-help.json')+'?'+Math.random(),{},function(response) {
  html(where, ' ');
  var option = '';
  if (jsonParam == 'voice') {
   var ip=response.split("if voice = ");
   for (var i in ip) {
    var command = ip[i].substr(0,ip[i].indexOf("\n"));
    if (command && command.indexOf(" = ") == -1 ) {
     option+='<li><a href="#" onclick="val(\''+to+'\',\''+command+'\');return false">'+command.replace(/_/g,' ')+'</a></li>';
    }
   }
   if (option=='') {
    option+='<li>'+jsonResponse.LangvoiceError+'</li>';
   }
  } else {
   var ip=JSON.parse(response);
   for (var i in ip[jsonParam]) {
    option+='<li><a href="#" onclick="val(\''+to+'\',\''+ip[jsonParam][i].command+'\');return false">'+ip[jsonParam][i].command+'</a> <sup>'+renameBlock(jsonResponse,ip[jsonParam][i].title)+'</sup>';
   }
  }
  html(where, (ip.header?ip.header:'')+'<ul>'+option+'</ul>'+(ip.footer?ip.footer:''));
 },true);
}

function loadLive(ip,file,where) {
 html(where,'<option value="">Loading...</option>');
 ajax.get('http://'+ip+'/'+file,{},function(response) {
  var option = '';
  var jsonLive=JSON.parse(response);
  for(var key in jsonLive) {
   if ((file == 'config.live.json' && key == 'time') || (file == 'config.live.json' && key == 'weekday')) {
   } else {
    option += '<option value="'+key+'" title="'+typeof jsonLive[key]+'">'+(renameBlock(jsonResponse, '{{Lang'+key+'}}')===undefined?key:renameBlock(jsonResponse, '{{Lang'+key+'}}'))+'<\/option>';
   }
  }
  html(where,'<option value="">'+jsonResponse.LangSelect+'<\/option>'+option);
 },true);
}

function loadCommand(ip,file,where) {
 html(where,'<option value="">Loading...</option>');
 ajax.get('http://'+ip+'/'+file,{},function(response) {
  var option = '';
  var jsonLive=JSON.parse(response);
  for(var key in jsonLive.command) {
   option += '<option value="'+jsonLive.command[key]+'" title="'+typeof jsonLive.command[key]+'">'+(renameBlock(jsonResponse, '{{Lang'+jsonLive.command[key]+'}}')===undefined?jsonLive.command[key]:renameBlock(jsonResponse, '{{Lang'+jsonLive.command[key]+'}}'))+'<\/option>';
  }
  html(where,'<option value="">'+jsonResponse.LangSelect+'<\/option>'+option);
 },true);
}

function loadLive2(ssdp_list,ssdp_module,ssdp_command) {
 val(ssdp_command,"Loading...");
 var ip = elem(ssdp_list).options[elem(ssdp_list).selectedIndex].value;
 var who = elem(ssdp_module).options[elem(ssdp_module).selectedIndex].value;
 ajax.get('http://'+ip+'/'+(who=='voice'?'config.options.json':'config.live.json'),{},function(response) {
  var jsonLive=JSON.parse(response);
  for(var key in jsonLive) {
   if (key == who) {
    elem(ssdp_command).value= jsonLive[key];
   }
  }
 },true);
}

function loadInTextarea() {
 var scenary_if = elem("ssdp-list0").options[elem("ssdp-list0").selectedIndex].text;
 var val_and = elem("hidden-val-and").value;
 var val_or = elem("hidden-val-or").value;
 var val_then = elem("hidden-val-then").value;
 var element = elem("scenary-list-edit");
 element.innerHTML += '\r\nif '+elem("ssdp-module").options[elem("ssdp-module").selectedIndex].value+' '+elem("ssdp-condition").options[elem("ssdp-condition").selectedIndex].value+' '+elem("ssdp-command").value.replace(/ /g,'_');
 var y = 1;
 while (y < val_and) {
  y++;
  element.innerHTML += '\r\nand '+elem("ssdp-module-and"+y).options[elem("ssdp-module-and"+y).selectedIndex].value+' '+elem("ssdp-condition-and"+y).options[elem("ssdp-condition-and"+y).selectedIndex].value+' '+elem("ssdp-command-and"+y).value.replace(/ /g,'_');
 }
 var z = 1;
 while (z < val_or) {
  z++;
  element.innerHTML += '\r\nor '+elem("ssdp-module-or"+z).options[elem("ssdp-module-or"+z).selectedIndex].value+' '+elem("ssdp-condition-or"+z).options[elem("ssdp-condition-or"+z).selectedIndex].value+' '+elem("ssdp-command-or"+z).value.replace(/ /g,'_');
 }
 var i = 1;
 while (i < val_then) {
  i++;
  var scenary_list = elem("ssdp-list"+i).options[elem("ssdp-list"+i).selectedIndex].text;
  var scenary_then = elem("scenary-then"+i).options[elem("scenary-then"+i).selectedIndex].value;
  if (scenary_then != ''){
   element.innerHTML += '\r\nthen '+(scenary_if==scenary_list?'this':scenary_list)+' '+scenary_then+' '+elem("scenary-othe"+i).value;
  }
 }
 element.innerHTML += '\r\nid '+Math.floor(Math.random()*10000);
 val('ssdp-command',' ');
 val('hidden-val-and',1);
 val('hidden-val-or',1);
 val('hidden-val-then',1);
 val('or-and',1);
}

function val(id,val){
 var element = elem(id);
 if (element){
  if (val) {
   element.value=(val==' '?'':val);
  } else {
   return element.value;
  }
 }
}

function html(id,val){
 var element = elem(id);
 if (element){
  if (val=='on'+'click') {if (jsonResponse["mes"+"sa"+"ge"]=='No'+'t r'+active+'red') {
   element.setAttribute(val,"alert('"+jsonResponse["Lan"+"gNotR"+active+"red"]+"');return false");
  }} else {
   if (val) {
    element.innerHTML=(val==' '?'':val);
   } else {
    return element.innerHTML;
   }
  }
 }
}

function elem(id){
 return document.getElementById(id);
}

/*
window.SavePhoto = async function SavePhoto(e) {
 let formData = new FormData();
 let photo = e.files[0];
 formData.append("data", photo);
 try {
  let r = await fetch('/edit', {method: "POST", body: formData});
  console.log('HTTP response code:',r.status);
 } catch(e) {
  console.log('Huston we have problem...:', e);
 }
}
*/

function SavePhoto(e) {
 var formData = new FormData();
 var photo = e.files[0];
 formData.append("data", photo);
 ajax.post('/edit',{body:formData},function(response) {},true);
}

function send_request_edit(submit,server,filename,geturl,gethost){
 var xmlHttp = new XMLHttpRequest();
 var old_submit = submit.value;
 submit.value = 'Loading...';
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
    var element = elem(block[i].slice(2,-2));
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
  var element =  elem('url-content');
  if (typeof(element) != 'undefined' && element != null){
   log('<li><span class="label label-warning">GET</span> <a href="'+server+'" class="btn btn-link" style="text-transform:none;text-align:left;white-space:normal;display:inline">'+server+'</a> <span class="label label-'+(responses=='FileNotFound'?'danger':'default')+'">'+(responses=='FileNotFound'?'File Not Found':'200 OK')+'</span></li>');
  }
  var ddnsUrl1 =  elem('ddns-url1');
  if (typeof(ddnsUrl1) != 'undefined' && ddnsUrl1 != null){
   ddnsUrl1.innerHTML = '<a href="http://'+location.hostname+'/'+server+'">http://'+location.hostname+'/'+server+'</a>';
  }
  var ddnsUrl2 =  elem('ddns-url2');
  if (typeof(ddnsUrl2) != 'undefined' && ddnsUrl2 != null && jsonResponse.ddnsName){
   ddnsUrl2.innerHTML = '<a href="http://'+jsonResponse.ddnsName+':'+jsonResponse.ddnsPort+'/'+server+'">http://'+jsonResponse.ddnsName+':'+jsonResponse.ddnsPort+'/'+server+'</a>';
  }
  if (state != '' && state != null && state!='undefined'){
   var block = state.split(',');
   for (var i = 0 ; i < block.length; i++) {
    if (block[i].slice(0, 2) != '[[') {
     window.location = block[i];
    } else {
     var response;
     if (block.length > 1) {
      response = JSON.parse(responses)[i];
     } else {
      response = JSON.parse(responses);
     }
     var htmlblock = elem(block[i].slice(2,-2));
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
      log('<li class="alert alert-info" style="margin:5px 0;"><a href="#'+block[i].slice(2,-2)+'" class="label label-success">'+block[i]+'</a> '+responses.replace(/</g,'&lt;')+'</li>');
     }
    }
   }
  } else {
   if (typeof(element) != 'undefined' && element != null){
    log('<li class="alert alert-info" style="margin:5px 0;">'+responses.replace(/</g,'&lt;')+'</li>');
   }
  }
  // load('next');
 },true);
}

function send_socket(submit,server,state){
 log('<li><span class="label label-warning">WS</span> <small>ip</small> <span class="label label-default">Send</span></li><li style="margin:5px 0;" class="alert alert-info">'+state+'</li>');
 connection.send(state);
}

function submit_disabled(request){
 var element = document.getElementsByTagName("input");
 for (var i = 0; i < element.length; i++) {
  if (element[i].type === 'button') {element[i].disabled = request;}
 }
}

function toggle(id,toggle) {
 if (elem(id)){
  var element = elem(id).classList;
  if (element.contains('hidden')) {
   if (toggle != 'show') {
    element.remove('hidden');
    element.add('show');
   }
  } else {
   if (toggle != 'hidden') {
    element.remove('show');
    element.add('hidden');
   }
  }
 }
}

function loadWifi(id,hiddenIds){
 html(id, '<li><a href="#">Loading...</a></li>');
 ajax.get('/wifi.scan.json',{},function(response) {
  var jsonWifi=JSON.parse(response);
  jsonWifi.networks.sort(function(a,b){return (a.dbm < b.dbm)?1:((b.dbm < a.dbm)?-1:0);});
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
  elem(id).innerHTML = (html?html:'<li><a href="#">Not found WiFi</a></li>')+'<li><a href="#" onclick="toggle(\'ssid-group\');toggle(\'ssid\');return false"><b>'+jsonResponse.LangHiddenWifi+'</b></a></li>';
 },true);
}

function loadBuild(buildids,typeFile,urlbuild){
 ajax.get(urlbuild,{},function(response) {
  var jsonBuild=JSON.parse(response);
  jsonBuild.sort(function(a,b){return (a.name< b.name)?1:((b.name < a.name)?-1:0);});
  var html = '';
  for(var i = 0;i<jsonBuild.length;i++) {
   if (typeFile == 'all' && jsonBuild[i].name.substring(0,5) == 'spiff') {
    html += '<li><a href="/upgrade?spiffs=http://backup.privet.lv/esp/'+buildids+'/spiffs.0xBB000_flash_size_1Mb.256Kb_'+jsonBuild[i].name.slice(36, -4)+'.bin&build=http://backup.privet.lv/esp/'+buildids+'/build.0x00000_flash_size_1Mb.256Kb_'+jsonBuild[i].name.slice(36, -4)+'.bin" '+(jsonResponse.spiffsData==jsonBuild[i].name?'style="font-weight:bold;"':'')+' onclick="return confirm(\''+jsonResponse.LangRefresh+' '+typeFile+' (Build & Spiffs (flash 1Mb 256Kb) '+jsonBuild[i].name.slice(36, -4)+')? \\n '+jsonResponse.LangUpgradeInfo+'\')">Build & Spiffs (flash 1Mb 256Kb) '+jsonBuild[i].name.slice(36, -4)+'<\/a><\/li>';
   }
   if (jsonBuild[i].name.substring(0,5) == typeFile.substring(0,5)) {
    html += '<li><a href="/upgrade?'+typeFile+'=http://backup.privet.lv/esp/'+buildids+'/'+jsonBuild[i].name+'" '+(jsonResponse[typeFile+"Data"]==jsonBuild[i].name?'style="font-weight:bold;"':'')+' onclick="return confirm(\''+jsonResponse.LangRefresh+' '+typeFile+' ('+jsonBuild[i].name+')?\')">'+jsonBuild[i].name+'<\/a><\/li>';
   }
  }
  elem(buildids+'-'+typeFile).innerHTML = (html?html:'<li><a href="#">No build in folder<\/a><\/li>');
 },true);
}

function set_time_zone(ip){
 var gmtHours = new Date().getTimezoneOffset()/60*-1;
 val('timeZone', gmtHours);
 send_request(ip,'/cmd?command=zone '+gmtHours);
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

function setCookie(name,value,days) {
 if (days) {
  var date = new Date();
  date.setTime(date.getTime()+(days * 24 * 60 * 60 * 1000));
  var expires = "; expires="+date.toGMTString();
 }
 else expires = "";
 document.cookie = name+"="+value+expires+"; path=/";
}

function readCookie(name) {
 var nameEQ = name+"=";
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
  var name = eqPos > -1?cookie.substr(0, eqPos):cookie;
  document.cookie = name+"=;expires=Thu, 01 Jan 1970 00:00:00 GMT";
 }
}

function hide(name,submit) {
 if (confirm(jsonResponse.LangHedden)) {
  submit.parentNode.classList.add('hidden');
  setCookie(name,'hidden',365,submit);
 }
}

function loadConfigs(file_module,jsonResponse) {
 var element = elem(file_module.replace(/[^a-z0-9]/gi,'-'));
 element.innerHTML = '';
 ajax.get("/configs/"+file_module,{},function(response) {
  response = renameBlock(jsonResponse, response);
  var configsLinePin;
  var configsLine = response.match(/^.*((\r\n|\n|\r)|$)/gm);
  for(var key in configsLine) {
   if (configsLine[key].substr(0,2) == '//') {
    var configsLine2 = configsLine[key].split(" ");
    var configsLine3 = '';
    for (var i = 1; i < configsLine2.length; i++) {
     configsLine3 = configsLine3+' '+configsLine2[i];
    }
    element.innerHTML += '<hr style="margin:10px;"><label title="'+configsLine[key]+'"><input checked="" type="checkbox" style="display:none" disabled readonly>// '+configsLine3.replace(/</gi,"<span class=\"label label-default\">").replace(/>/gi,"</span>").replace(/\/n/gi,"<br>")+'<\/label></br>';
   } else {
    configsLinePin = configsLine[key].replace(/# /,'').split(' ');
    element.innerHTML += '<label><input type="checkbox" '+(configsLine[key].substring(0,2)!='# '?"checked":"")+'> '+configsLinePin[0]+'<\/label> ';
    for (var i = 1; i < configsLinePin.length; i++) {
     if (configsLinePin[0]=='RELAY') {
      element.innerHTML += (configsLinePin[i]?' <input class="form-control'+(i==1?' pin-relay':'')+''+(i==2?' name-relay':'')+'" '+(i==1 || i==2?'onkeyup="findError(\''+(i==1?'pin':'name')+'-relay\',this.value)"':'')+' style="display:inline;width:'+Number(configsLinePin[i].length+3)+'0px;" pattern="[a-zA-Z0-9._а-яА-Я]{1,25}" value="'+configsLinePin[i]+'"> ':'');
     } else if (configsLinePin[0]=='TACH') {
      element.innerHTML += (configsLinePin[i]?' <input class="form-control'+(i==2?' name-tach':'')+'" '+(i==2?'onkeyup="findError(\'name-tach\',this.value)"':'')+' style="display:inline;width:'+Number(configsLinePin[i].length+3)+'0px;" pattern="[a-zA-Z0-9.]{1,25}" value="'+configsLinePin[i]+'"> ':'');
     } else if (configsLinePin[0]=='PINOUT') {
      element.innerHTML += (configsLinePin[i]?' <input class="form-control'+(i==1?' pin-pinout':'')+''+(i==2?' name-pinout':'')+'" '+(i==1 || i==2?'onkeyup="findError(\''+(i==1?'pin':'name')+'-pinout\',this.value)"':'')+' style="display:inline;width:'+Number(configsLinePin[i].length+3)+'0px;" pattern="[a-zA-Z0-9._а-яА-Я]{1,25}" value="'+configsLinePin[i]+'"> ':'');
     } else {
      element.innerHTML += (configsLinePin[i]?' <input class="form-control" style="display:inline;width:'+Number(configsLinePin[i].length+3)+'0px;" pattern="[a-zA-Z0-9.]{1,25}" value="'+configsLinePin[i]+'"> ':'');
     }
    }
    element.innerHTML += '</br>';
   }
  }
 },true);
 element.innerHTML += '<textarea id="'+file_module.replace(/[^a-z0-9]/gi,'-')+'-edit" style="display:none" class="form-control"></textarea>';
}

function findError(className,valueName) {
 var id = document.getElementsByClassName(className);
 var dubleId=0;
 for (var i = 0; i < id.length; i++) {
  id[i].classList.remove('btn-danger');
  if (valueName == id[i].value) {
   dubleId++;
   if (dubleId>=2) {
    id[i].classList.add('btn-danger');
   }
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
  send_request_edit(this, val(file+'-txt-edit'),'configs/'+file+'.txt','if(confirm(\''+jsonResponse.LangReset2+' '+jsonResponse.LangReset3+'\')){send_request(this,\'/restart?device=ok\');toggle(\'restart-esp\');setTimeout(function(){toggle(\'restart-esp\');},20000);};');
 },true);
}

function cloudDownload(mac,file) {
 ajax.get("http://backup.privet.lv/configs/"+mac+"-"+file+"?"+Math.random(),{},function(response) {
  if (response!='FileNotFound'){
   send_request_edit(this, response,'configs/'+file+'','loadConfigs("'+file+'");if(confirm(\''+jsonResponse.LangReset2+' '+jsonResponse.LangReset3+'\')){send_request(this,\'/restart?device=ok\');toggle(\'restart-esp\');setTimeout(function(){toggle(\'restart-esp\');},20000);};');
  } else {
   alert('File not found in cloud!');
  }
 },true);
}

function loadTable(file, jsonTable) {
 ajax.get(file+"?"+Math.random(),{},function(response) {
  var timers=JSON.parse(response);
  var setTable = Object.keys(timers);
  html('thead-'+file.replace(/[^a-z0-9]/gi,'-'), ' ');
  html('tbody-'+file.replace(/[^a-z0-9]/gi,'-'), ' ');
  var thead = '';
  for(var key in jsonTable) {
   thead += '<th>'+renameBlock(jsonResponse, jsonTable[key])+'<\/th>';
  }
  elem('thead-'+file.replace(/[^a-z0-9]/gi,'-')).innerHTML += thead;
  for (var i = 0; i < timers[setTable].length; i++) {
   var tbody = '';
   for(var keys in jsonTable) {
    tbody += '<td>'+timers[setTable][i][keys]+'<\/td>';
   }
   elem('tbody-'+file.replace(/[^a-z0-9]/gi,'-')).innerHTML += '<tr>'+tbody+'<\/tr>';
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
    str = str.replace(new RegExp('{{'+id+'}}','g'), jsonResponse[id]);
    //str = str.replace(new RegExp('{{'+id+'}}','g'), eval("jsonResponse."+id));
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
    var element = elem(id);
    if (element) {
     var txt='';
     if (element.tagName=='SELECT'){
      txt=element.options[element.selectedIndex].value;
     }
     else if (element.tagName=='INPUT'){
      txt=encodeURIComponent(element.value);
     }
     else if (element.tagName=='TEXTAREA'){
      txt=encodeURIComponent(element.value);
     }
     else {
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
 return hex.length == 1?"0"+hex:hex;
}
function rgbToHex(r, g, b) {
 return componentToHex(r)+componentToHex(g)+componentToHex(b);
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

function hexToRgb(hex) {
 var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
 return result?{
  r: parseInt(result[1], 16),
  g: parseInt(result[2], 16),
  b: parseInt(result[3], 16)
 }:null;
}

function loadCommits(repos,commitCount){
 ajax.get('https://api.github.com/repos/'+repos+'/commits',{},function(response) {
  html('commits-list', ' ');
  var jsonCommits=JSON.parse(response);
  for(var key in jsonCommits) {
   if (key < commitCount) {
    elem('commits-list').innerHTML += '<p><span class="label label-default"><i class="clock-new-img"></i> '+jsonCommits[key].commit.author.date.substring(0,10)+'<\/span> <a href="'+jsonCommits[key].html_url+'" target="_blank">'+jsonCommits[key].commit.message+'<\/a><\/p>';
   }
  }
 },true);
}

function loadIssues(repos,issuesCount){
 var issues_list = elem('issues-list');
 ajax.get('https://api.github.com/repos/'+repos+'/issues',{},function(response) {
  html('issues-list', ' ');
  var jsonIssues=JSON.parse(response);
  jsonIssues.sort(function(a,b){return (a.updated_at< b.updated_at)?1:((b.updated_at < a.updated_at)?-1:0);});
  for(var key in jsonIssues) {
   if (jsonIssues[key].user.login == 'renat2985' || jsonIssues[key].user.login == 'tretyakovsa') {
    for(var label in jsonIssues[key].labels) {
     if (jsonIssues[key].labels[label].name == 'news') {
      if (readCookie("news-"+jsonIssues[key].id) != "hidden") {
       elem('news').innerHTML = '<div class="alert alert-dismissible alert-info"><button class="close" onclick="hide(\'news-'+jsonIssues[key].id+'\',this)">×<\/button>'+jsonIssues[key].title.substr(0, 100)+' <a href="'+jsonIssues[key].html_url+'" target="_blank">подробнее...<\/a><\/div>';
      }
     }
    }
   }
   if (issues_list && key < issuesCount) {
    issues_list.innerHTML += '<p><span class="label label-default"><i class="clock-new-img"></i> '+jsonIssues[key].updated_at.substring(0,10)+'<\/span> <a href="'+jsonIssues[key].html_url+'" target="_blank">'+jsonIssues[key].title+'<\/a> <i>('+jsonIssues[key].comments+')<\/i><\/p>';
   }
  }
  if (issues_list && issuesCount) {
   issues_list.innerHTML += '<p><a href="https://github.com/tretyakovsa/Sonoff_WiFi_switch/issues/new" class="label label-success">Create new issues</a></p>';
  }
 },true);
}

function loadUpdate(repos, spiffs, LangUpgrade, LoadDelay){
 setTimeout(function() {
  ajax.put('https://api.github.com/repos/'+repos+'/contents/build',{},function(response) {
   var jsonBuild=JSON.parse(response);
   jsonBuild.sort(function(a,b){return (a.name< b.name)?1:((b.name < a.name)?-1:0);});
   if (jsonBuild[0].name != spiffs) {
    elem('update').innerHTML += '<sup><a href="/upgrade?spiffs=http://backup.privet.lv/esp/sonoff/'+jsonBuild[0].name+'&build=http://backup.privet.lv/esp/sonoff/build.0x00000'+jsonBuild[0].name.substring(14)+'" onclick="return confirm(\''+LangUpgrade+' \\n - New build: '+jsonBuild[0].name.split('_')[4].slice(0,-4)+' \\n - You build: '+(spiffs.length>35?spiffs.split('_')[4].slice(0,-4):'Not found')+' \\n '+jsonResponse.LangUpgradeInfo+'\')" title="'+LangUpgrade+'"><i class="warning-img"><\/i><\/a><sup>';
   }
  },true);
 }, LoadDelay);
}

function isValidJson(str,id) {
 try {
  JSON.parse(str);
  elem(id).style.border = "";
  html(id,' ');
  submit_disabled(false);
 } catch (e) {
  elem(id).style.border = "2px solid red";
  html(id,'<div class="alert alert-danger">'+e.message+'<\/div><br><br><br>');
  submit_disabled(true);
  return false;
 }
 return true;
}

function mergeObject(target) {
 for (var i = 1; i < arguments.length; i++) {
  var source = arguments[i];
  for (var key in source) {
   if (source.hasOwnProperty(key)) {
    target[key] = source[key];
   }
  }
 }
 return target;
}

function delCb(path){
 return function(){
  if (xmlHttp.readyState == 4){
   if(xmlHttp.status != 200){
   } else {
    if(path.lastIndexOf('/') < 1){
     path = '/';
     treeRoot.removeChild(treeRoot.childNodes[0]);
     httpGet(treeRoot, "/");
    } else {
     path = path.substring(0, path.lastIndexOf('/'));
     var leaf = elem(path).parentNode;
     if(leaf.childNodes.length == 3) leaf.removeChild(leaf.childNodes[2]);
     httpGet(leaf, path);
    }
   }
  }
 }
}

function httpDelete(file){
 xmlHttp = new XMLHttpRequest();
 xmlHttp.onreadystatechange = delCb(file);
 var formData = new FormData();
 formData.append("path", file);
 xmlHttp.open("DELETE", "/edit");
 xmlHttp.send(formData);
}

function timeout(command,times){
 setTimeout(command,times);
}
