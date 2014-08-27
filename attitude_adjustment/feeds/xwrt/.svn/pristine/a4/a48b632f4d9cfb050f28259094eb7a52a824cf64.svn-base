#!/usr/bin/webif-page
<?
#
#credit goes to arantius and GasFed
#
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/graphs-subcategories.sh

header "Graphs" "graphs_cpu_subcategory#CPU" "@TR<<graphs_cpu_CPU_Usage#CPU Usage>>" "" ""
# This construction supports all recent browsers, degrades correctly, 
# see http://joliclic.free.fr/html/object-tag/en/object-svg.html
?>
<center>
	<object type="image/svg+xml" data="/cgi-bin/webif/graph_cpu_svg.sh"
		width="500" height="250">
		<param name="src" value="/cgi-bin/webif/graph_cpu_svg.sh" />
		<a href="http://www.adobe.com/svg/viewer/install/main.html">@TR<<graphs_svg_download#If the graph is not fuctioning download the viewer here.>></a>
	</object>
</center>
<? footer ?>
<!--
##WEBIF:name:Graphs:1:graphs_cpu_subcategory#CPU
-->
