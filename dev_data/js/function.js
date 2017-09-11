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
 var xmlHttp=createXmlHttpObject();
 pages[0] = (pages[0]?pages[0]:'index');
 xmlHttp.open('GET', pages[0]+".json",true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
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
     xmlHttp.onload = function(e) {
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
        for(i = 0;i<jsonPage.content.length;i++) {
         if (!jsonPage.content[i].module || searchModule(jsonResponse.module,jsonPage.content[i].module)=='yes') {
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
           document.getElementById('contents').innerHTML += '<hr id="'+name_val+'" class="'+class_val+'" '+style_val+'>';
          }
          if (type_val == 'h1' || type_val == 'h2' || type_val == 'h3' || type_val == 'h4' || type_val == 'h5' || type_val == 'h6') {
           document.getElementById('contents').innerHTML += '<'+type_val+' id="'+name_val+'" class="'+class_val+'" '+style_val+'>'+renameBlock(jsonResponse, jsonPage.content[i].title)+'<\/'+type_val+'>';
          }
          if (type_val == 'input') {
           if (action_val) action_val = 'onfocusout="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+jsonPage.content[i].action+'\'),\''+response_val+'\')"';
           document.getElementById('contents').innerHTML += '<input '+action_val+' id="'+name_val+'" class="form-control '+class_val+'" '+style_val+' '+(pattern_val?'pattern="'+pattern_val+'"':'')+' placeholder="'+renameBlock(jsonResponse, jsonPage.content[i].title)+'" value="'+state_val+'">';
          }
          if (type_val == 'password') {
           if (action_val) action_val = 'onfocusout="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+jsonPage.content[i].action+'\'),\''+response_val+'\')"';
           document.getElementById('contents').innerHTML += '<input '+action_val+' id="'+name_val+'" class="form-control '+class_val+'" '+style_val+' '+(pattern_val?'pattern="'+pattern_val+'"':'')+' placeholder="'+renameBlock(jsonResponse, jsonPage.content[i].title)+'" value="'+state_val+'" onfocus="this.type=\'text\'" type="password">';
          }
          if (type_val == 'button') {
           if (action_val) action_val = 'onclick="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+jsonPage.content[i].action+'\'),\''+response_val+'\')"';
           document.getElementById('contents').innerHTML += '<input id="'+name_val+'" '+action_val+' class="'+class_val+'" '+style_val+' value="'+renameBlock(jsonResponse, jsonPage.content[i].title)+'" type="button">';
          }
          if (type_val == 'checkbox') {
           var checked = '';
           if (state_val == 1){ checked = 'checked'; }
           if (action_val) { action_val = 'onchange="val(this.id,(this.checked?\'1\':\'0\'));send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+jsonPage.content[i].action+'\'),\''+response_val+'\')"'; } else { action_val = 'onchange="val(this.id,(this.checked?\'1\':\'0\'));"'; }
           document.getElementById('contents').innerHTML += '<label '+style_val+'><input id="'+name_val+'" value="'+state_val+'" '+action_val+' type="checkbox" class="'+class_val+'" '+checked+'> '+renameBlock(jsonResponse, jsonPage.content[i].title)+'<\/label>';
          }
          if (type_val == 'range') {
           if (action_val) action_val = 'onchange="send_request(this, \''+(typeof module_val!='undefined'&&module_val?'cmd?command=':'')+'\'+renameGet(\''+jsonPage.content[i].action+'\'),\''+response_val+'\')"';
           document.getElementById('contents').innerHTML += '<label '+style_val+' style="display:block;"><h4>'+renameBlock(jsonResponse, jsonPage.content[i].title)+'<\/h4> <input id="'+name_val+'" class="form-control '+class_val+'" '+action_val+' '+pattern_val+' value="'+state_val+'" type="range"><\/label>';
          }
          if (type_val == 'table') {
           var thead = '';
           jsonTable = jsonPage.content[i].title;
           // for(var key in jsonTable) {
           //  thead += '<th>'+renameBlock(jsonResponse, jsonTable[key])+'<\/th>';
           // }
           document.getElementById('contents').innerHTML += '<table class="'+class_val+'" '+style_val+' id="'+name_val+'"><thead id="thead-'+state_val.replace(/[^a-z0-9]/gi,'-')+'"><\/thead><tbody id="tbody-'+state_val.replace(/[^a-z0-9]/gi,'-')+'"><\/tbody><\/table>';
           createTable(state_val, jsonTable);
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
           document.getElementById('contents').innerHTML += '<select class="form-control '+class_val+'" '+style_val+' '+action_val+' id="'+name_val+'">'+option+'<\/select>';
          }
          if (type_val == 'configs') {
           document.getElementById('contents').innerHTML += '<div id="'+name_val+'"><div id="'+state_val.replace(/[^a-z0-9]/gi,'-')+'" class="'+renameBlock(jsonResponse, '{{'+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden}}')+'" '+style_val+'><center>'+jsonResponse.LangLoading+'</center><\/div><\/div>';
           document.getElementById('contents').innerHTML += '<input onclick="changeTextarea(\''+state_val.replace(/[^a-z0-9]/gi,'-')+'\');send_request_edit(this, val(\''+state_val.replace(/[^a-z0-9]/gi,'-')+'-edit\'),\'configs/'+state_val+'\');alert(\''+jsonResponse.LangReset2+'\')" class="btn btn-block btn-success" value="'+jsonResponse.LangSave+'" type="button">';
           setTimeout("loadConfigs('"+state_val+"')", 500);
          }
          if (type_val == 'link') {
           document.getElementById('contents').innerHTML += '<a id="'+name_val+'" class="'+class_val+'" '+style_val+' href="'+renameGet(jsonPage.content[i].action)+'">'+renameBlock(jsonResponse, jsonPage.content[i].title)+'<\/a>';
          }
          if (type_val == 'text') {
           document.getElementById('contents').innerHTML += '<div id="'+name_val+'" class="'+class_val+'" '+style_val+'>'+renameBlock(jsonResponse, jsonPage.content[i].title)+'<\/div>';
          }
          if (type_val == 'iframe') {
           document.getElementById('contents').innerHTML += renameBlock(jsonResponse, jsonPage.content[i].title)+'<iframe src="'+state_val+'" id="'+name_val+'" class="'+class_val+'" '+style_val+'><\/iframe>';
          }
          if (type_val == 'chart') {
           document.getElementById('contents').innerHTML += '<div id="'+name_val+'" class="'+renameBlock(jsonResponse, '{{'+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden}}')+'"><button class="close" onclick="hide(\''+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden\',this);" type="button">×<\/button><a href="'+renameGet(jsonPage.content[i].action)+'" target="_blank" class="close">'+(typeof action_val!='undefined'&&action_val?'<i class="popup-img"><\/i>':'')+'<\/a><h2>'+renameBlock(jsonResponse, jsonPage.content[i].title)+'<\/h2><div id="'+state_val.replace(/[^a-z0-9]/gi,'')+'" class="'+class_val+'" '+style_val+'><\/div><hr><\/div>';
           if (renameBlock(jsonResponse, '{{'+state_val.replace(/[^a-z0-9]/gi,'')+'-hidden}}') != 'hidden') {
            //var charTime;
            //clearTimeout(charTime);
            //charTime = setTimeout("loadChart('"+name_val+"','"+state_val+"')", 500);
            setTimeout("loadChart('"+state_val.replace(/[^a-z0-9]/gi,'')+"','"+state_val+"', {"+jsonPage.content[i].options+"},"+jsonPage.content[i].refresh+","+jsonPage.content[i].points+")", 500);
           }
          }
          if (type_val == 'wifi') {
           document.getElementById('contents').innerHTML += '<div class="btn-group btn-block" id="ssid-group"><a href="#" class="btn btn-default btn-block dropdown-toggle" onclick="toggle(\'ssid-select\');loadWifi(\'ssid-select\',\''+name_val+'\');return false"><span id="ssid-name">'+state_val+'<\/span> <span class="caret"><\/span><\/a><ul class="dropdown-menu hidden" id="ssid-select"><li><a href="#">'+jsonResponse.LangLoading+'<\/a><\/li><\/ul><\/div>';
           document.getElementById('contents').innerHTML += '<input id="'+name_val+'" value="'+state_val+'" class="form-control hidden '+class_val+'" '+style_val+' '+pattern_val+' placeholder="'+renameBlock(jsonResponse, jsonPage.content[i].title)+'">';
          }
          if (type_val == 'time') {
           document.getElementById('contents').innerHTML += '<h2 id="'+name_val+'" '+style_val+'>'+renameBlock(jsonResponse, jsonPage.content[i].title)+' <strong id="time" class="'+class_val+'">'+state_val+'<\/strong><\/h2>';
           clearTimeout(set_real_time);
           var res = jsonResponse.time.split(":");
           real_time(hours=res[0],min=res[1],sec=res[2]);
          }
          if (type_val == 'rgb') {
           document.getElementById('contents').innerHTML += '<div class="'+name_val+'-thumb '+class_val+'"><div class="'+name_val+'-preview"><\/div><img alt="" '+style_val+' src="'+renameBlock(jsonResponse, jsonPage.content[i].title)+'"><\/div><canvas id="'+name_val+'-cs" style="display:none"><\/canvas>';
           document.getElementById('contents').innerHTML += '<input id="'+name_val+'" value="'+state_val+'" class="form-control hidden">';
           setTimeout("createRGB('"+name_val+"', '"+jsonPage.content[i].action+"','"+module_val+"','"+response_val+"')", 500);
          }
          if (type_val == 'dev') {
           var dev_html = '<div id="'+name_val+'" class="'+class_val+'" '+style_val+'><a href="/help.htm" target="_blank" class="close"><i class="help-img"><\/i><\/a>'+renameBlock(jsonResponse, jsonPage.content[i].title)+'<span id="dev-update" class="hidden"><a href="/edit" class="btn btn-default" target="_blank">File manager<\/a> <a href="/page.htm?starting" class="btn btn-default">Starting log<\/a> ';
           if (searchModule(jsonResponse.module,"upgrade") == 'yes'){
            dev_html += ' <div class="btn-group"><a href="#" class="btn btn-danger dropdown-toggle" onclick="toggle(\'repos-all\');loadBuild(\'sonoff\',\'all\');return false">Upgrade Build & Spiffs <span class="caret"><\/span><\/a><ul class="dropdown-menu hidden" id="repos-all" style="min-width:350px"><li><a href="https://github.com/tretyakovsa/Sonoff_WiFi_switch/commits/master" style="text-align:right" target="_blank"><i class="help-img"><\/i> Github code history<\/a><ul id="sonoff-all" style="margin-right:20px"><li><a href="#">'+jsonResponse.LangLoading+'<\/a><\/li><\/ul><\/li><\/ul><\/div>';
           }
           dev_html += '<br><b><a href="#" onclick="toggle(\'repos-bin\');return false">'+jsonResponse.LangSetting6+'<\/a><\/b><span id="repos-bin" class="hidden">';
           if (searchModule(jsonResponse.module,"upgrade") == 'yes'){
            dev_html += ' <div class="btn-group"><a href="#" class="btn btn-default btn-sm dropdown-toggle" onclick="toggle(\'repos-build\');return false">Build <span class="caret"><\/span><\/a><ul class="dropdown-menu hidden" id="repos-build" style="min-width:350px"><li><a href="#" onclick="toggle(\'sonoff-build\');loadBuild(\'sonoff\',\'build\');return false"><b>Sonoff<\/b> (Relay) <span class="caret"><\/span><\/a><a href="https://github.com/tretyakovsa/Sonoff_WiFi_switch/commits/master" style="float:right;margin-top:-27px" target="_blank"><i class="help-img"><\/i> History<\/a><ul class="hidden" id="sonoff-build" style="margin-right:20px"><li><a href="#">'+jsonResponse.LangLoading+'<\/a><\/li><\/ul><\/li><li><a href="#" onclick="toggle(\'rgb-build\');loadBuild(\'rgb\',\'build\');return false"><b>RGB<\/b> (WS2811-12/NeoPixel) <span class="caret"><\/span><\/a><a href="https://github.com/renat2985/rgb/commits/master" style="float:right;margin-top:-27px" target="_blank"><i class="help-img"><\/i> History<\/a><ul class="hidden" id="rgb-build" style="margin-right:20px"><li><a href="#">'+jsonResponse.LangLoading+'<\/a><\/li><\/ul><\/li><\/ul><\/div>';
            dev_html += ' <div class="btn-group"><a href="#" class="btn btn-default btn-sm dropdown-toggle" onclick="toggle(\'repos-spiffs\');return false">Spiffs <span class="caret"><\/span><\/a><ul class="dropdown-menu hidden" id="repos-spiffs" style="min-width:350px"><li><a href="#" onclick="toggle(\'sonoff-spiffs\');loadBuild(\'sonoff\',\'spiffs\');return false"><b>Sonoff<\/b> (Relay) <span class="caret"><\/span><\/a><a href="https://github.com/tretyakovsa/Sonoff_WiFi_switch/commits/master" style="float:right;margin-top:-27px" target="_blank"><i class="help-img"><\/i> History<\/a><ul class="hidden" id="sonoff-spiffs" style="margin-right:20px"><li><a href="#">'+jsonResponse.LangLoading+'<\/a><\/li><\/ul><\/li><li><a href="#" onclick="toggle(\'rgb-spiffs\');loadBuild(\'rgb\',\'spiffs\');return false""><b>RGB<\/b> (WS2811-12/NeoPixel) <span class="caret"><\/span><\/a><a href="https://github.com/renat2985/rgb/commits/master" style="float:right;margin-top:-27px" target="_blank"><i class="help-img"><\/i> History<\/a><ul class="hidden" id="rgb-spiffs" style="margin-right:20px"><li><a href="#">'+jsonResponse.LangLoading+'<\/a><\/li><\/ul><\/li><\/ul><\/div>';
           }
           dev_html += '<form method="POST" style="float:right" action="/update" enctype="multipart/form-data"><div class="btn-group"><input type="file" class="btn btn-primary btn-xs" name="update" style="height:33px" accept=".bin"><input type="submit" class="btn btn-default btn-sm" value="Download" onclick="this.value=\''+jsonResponse.LangLoading+'\';" style="height:33px"><\/div><\/form><hr>';
           dev_html += jsonResponse.LangType+': <div class="btn-group"><select class="btn btn-default btn-sx" onchange="send_request(this, \'/configs?set=\'+this.value,\'[[configs-edit-button]]\')"><option value="'+jsonResponse.configs+'">'+jsonResponse.configs+'<\/option><option value="sonoff-rf">Sonoff-rf / Sonoff / Wi-Fi Smart socket<\/option><option value="rgb">RGB (WS2811-12/NeoPixel)<\/option><option value="jalousie">Jalousie<\/option><option value="leakag">Leakag<\/option><option value="smart-room">Smart-Room<\/option><option value="manually">Manually<\/option><\/select> <a href="/page.htm?configs&'+jsonResponse.configs.toLowerCase()+'" id="configs-edit-button" class="btn btn-primary">Edit<\/a><\/div>';
           dev_html += '<\/span><\/span><\/div>';
           document.getElementById('contents').innerHTML += dev_html;
          }
         }
        }
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
   xmlHttp.onload = function(e) {
    if (xmlHttp.status==200){
     html('file-list',' ');
     var jsonFiles = JSON.parse(xmlHttp.responseText);
     for(i = 0;i<jsonFiles.length;i++) {
      if (jsonFiles[i].name.substr(-4) == 'json'){
       document.getElementById('file-list').innerHTML += '<a href="/page.htm?'+jsonFiles[i].name.slice(0,-5)+'">'+jsonFiles[i].name+'<\/a><br>';
      }
     }
    }
   }
  }
 }
};

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
 var xmlHttp = new XMLHttpRequest();
 var old_submit = submit.value;
 submit.value = jsonResponse.LangLoading;
 submit_disabled(true);
 var formData = new FormData();
 formData.append("data", new Blob([server], { type: 'text/html' }), filename);
 xmlHttp.open("POST", "/edit");
 xmlHttp.onload = function(e) {
  submit.value=old_submit;
  submit_disabled(false);
 }
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
      createTable(response.state,response.title);
     }
     if (htmlblock.tagName == 'A' && response.action) {
      htmlblock.href = response.action;
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



function loadConfigs(state_val) {
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("GET", "configs/"+state_val, true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
  document.getElementById(state_val.replace(/[^a-z0-9]/gi,'-')).innerHTML = '';
  var configsLinePin;
  var configsLine = xmlHttp.responseText.match(/^.*((\r\n|\n|\r)|$)/gm);
  for(var key in configsLine) {

   if (configsLine[key].substr(0,2) == '//') {
    document.getElementById(state_val.replace(/[^a-z0-9]/gi,'-')).innerHTML += '<label><input checked="" type="checkbox" style="display:none"><span class="label label-default">'+configsLine[key]+'</span><\/label></br>';
   } else {
    configsLinePin = configsLine[key].replace(/# /,'').split(' ');
    document.getElementById(state_val.replace(/[^a-z0-9]/gi,'-')).innerHTML += '<label><input type="checkbox" '+(configsLine[key].substring(0,2)!='# '?"checked":"")+'> '+configsLinePin[0]+'<\/label> '+(configsLinePin[1]?'<input class="form-control" style="display:inline;width:100px;" pattern="[a-zA-Z0-9\s]+" value="'+configsLinePin[1]+'">':'')+' '+(configsLinePin[2]?'<input class="form-control" style="display:inline;width:100px;" pattern="[a-zA-Z0-9\s]+" value="'+configsLinePin[2]+'">':'')+' '+(configsLinePin[3]?'<input class="form-control" style="display:inline;width:100px;" pattern="[a-zA-Z0-9\s]+" value="'+configsLinePin[3]+'">':'')+'</br>';
   }
  }
  document.getElementById(state_val.replace(/[^a-z0-9]/gi,'-')).innerHTML += '<textarea id="'+state_val.replace(/[^a-z0-9]/gi,'-')+'-edit" style="display:none" class="form-control"></textarea>';
  //changeTextarea(state_val.replace(/[^a-z0-9]/gi,'-'));
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

function createTable(state_val, jsonTable) {
 var xmlHttp=createXmlHttpObject();
 xmlHttp.open("GET", state_val+"?"+Math.floor(Math.random()*10000), true);
 xmlHttp.send(null);
 xmlHttp.onload = function(e) {
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
   };
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
    if (document.getElementById(id)) {
     var txt='';
     if (document.getElementById(id).tagName=='SELECT'){
      txt=document.getElementById(id).options[document.getElementById(id).selectedIndex].value;
     }
     else if (document.getElementById(id).tagName=='INPUT'){
      txt=encodeURIComponent(document.getElementById(id).value);
     } else {
      txt=document.getElementById(id).innerHTML;
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
