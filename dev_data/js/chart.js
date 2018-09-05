function loadChart(chartId,chartUrl,charOptions,charRefresh,charPoints,chartChartist){
 var time_refresh_chart;
 function viewTime() {
  var now = new Date();
  return now.getHours()+':'+now.getMinutes()+':'+now.getSeconds();
 }

 //var ne = document.createElement('h2');
 //ne.id = chartId+'-title';
 //document.getElementById(chartId).parentNode.insertBefore(ne,document.getElementById(chartId));

 function adddata() {
  var xmlHttp=createXmlHttpObject();
  xmlHttp.open("GET", chartUrl, true);
  xmlHttp.send(null);
  xmlHttp.onload = function() {
   var module=JSON.parse(xmlHttp.responseText);
   if (module.title != null) {
    document.getElementById(chartId+'-title').innerHTML = module.title;
   }

   if (typeof module.data != null && typeof module.data1 != null && typeof module.data2 != null) {
    for (var i = 0; i < module.data.length; i++) {
     var points = (charPoints?charPoints:module.points);
     if (points != null && points <= LineChart.data.series[0].length) {
      LineChart.data.labels.shift();
      LineChart.data.series[0].shift();
      if (module.data1 != null) {
       LineChart.data.series[1].shift();
      }
      if (module.data2 != null) {
       LineChart.data.series[2].shift();
      }
     }

     LineChart.data.series[0].push(module.data[i]);

     var element = document.getElementById(chartId+'-data');
     if (element){
      element.innerHTML = '';
      if (module.data != null) {element.innerHTML += module.data + ' ';}
      if (module.data1 != null) {element.innerHTML += module.data1 + ' ';}
      if (module.data2 != null) {element.innerHTML += module.data2;}
     }

     if (module.data1 != null) {
      LineChart.data.series[1].push(module.data1[i]);
     }
     if (module.data2 != null) {
      LineChart.data.series[2].push(module.data2[i]);
     }
     if (module.label != null) {
      LineChart.data.labels.push(module.label[i]);
     } else {
      LineChart.data.labels.push(viewTime());
     }
     LineChart.update();
    }
   }

   charOptions = (charOptions=!'{}'?charOptions:'{'+module.options+'}');
   //LineChart.update(,,charOptions);

   //var strUser = module.refresh;
   var strUser = (charRefresh?charRefresh:module.refresh);
   if (typeof(strUser) == "undefined" || strUser == "undefined") {
    strUser = 100000000000;
   }
   clearInterval(time_refresh_chart);
   time_refresh_chart=setInterval(function(){adddata();}, strUser);
  }
 }

 if (typeof(chartChartist) == "undefined" || chartChartist == "undefined") {chartChartist = 'Line';}
 var LineChart = Chartist[''+chartChartist+'']("#"+chartId, {
  labels: [],
  series: [[],[],[]]
  //}, {fullWidth:true});
 }, charOptions);
 adddata();
}
