function chartStart(chartId,chartUrl){
 var myVar;
 function viewTime() {
  var now = new Date();
  return now.getHours()+':'+now.getMinutes()+':'+now.getSeconds();
 }

 var ne = document.createElement('h2');
 ne.id = chartId+'-title';
 document.getElementById(chartId).parentNode.insertBefore(ne,document.getElementById(chartId));

 function adddata() {
  var xmlHttp=createXmlHttpObject();
  xmlHttp.open("GET", "/"+chartUrl+".json", true);
  xmlHttp.send(null);
  xmlHttp.onload = function(e) {
   var module=JSON.parse(xmlHttp.responseText);
   if (module.title != null) {
    document.getElementById(chartId+'-title').innerHTML = module.title;
   }
   for (var i = 0; i < module.data.length; i++) {
    if (module.points != null && module.points <= LineChart.data.series[0].length) {
     LineChart.data.labels.shift();
     LineChart.data.series[0].shift();
     if (module.data2 != null) {
      LineChart.data.series[1].shift();
     }
     if (module.data3 != null) {
      LineChart.data.series[2].shift();
     }
    }
    LineChart.data.series[0].push(module.data[i]);
    if (module.data2 != null) {
     LineChart.data.series[1].push(module.data2[i]);
    }
    if (module.data3 != null) {
     LineChart.data.series[2].push(module.data3[i]);
    }
    if (module.label != null) {
     LineChart.data.labels.push(module.label[i]);
    } else {
     LineChart.data.labels.push(viewTime());
    }
    LineChart.update();
   }
   var strUser = module.refresh;
   clearInterval(myVar);
   myVar=setInterval(function(){adddata();}, strUser*1000);
  }
 }
 var LineChart = Chartist.Line("#"+chartId, {
  labels: [],
  series: [[],[],[]]
 }, {fullWidth:true});
 adddata();
}
