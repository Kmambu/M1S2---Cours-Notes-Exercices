#!/usr/bin/env python

html="""
<head>
	<title>Peri Web Server</title>
	<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
	<script type="text/javascript">
		google.charts.load('current', {packages: ['corechart']});
		google.charts.setOnLoadCallback(drawChart);

		function drawChart() {
			$.get("./log_in", function(csvString) {
				var arrayData = $.csv.toArrays(csvString, {onParseValue: $.csv.hooks.castToScalar});

				var data = new google.visualization.arrayToDataTable(arrayData);

				var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
				chart.draw(data);
			});
		}
	</script>
</head>
<body>
LEDS:<br/>
<form method="POST" action="led.py">
  <input name="val" cols="20" placeholder ="choix Led"></input>
  <input name="ledVal" cols="20" placeholder="val Led"></input>
  <input type="submit" value="Entrer">
</form>
BP:<br/>
<form method="POST" action="bp.py">
  <input name="bpval" cols="20" placeholder="choix du BP"></input>
  <input type="submit" value="Entrer">
</form>
<div id="chart_div" style="width:400; height:300"></div>
</body>
"""

print html
