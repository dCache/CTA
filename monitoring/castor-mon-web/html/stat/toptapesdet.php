<?php
//this script prints the "Top Tapes" Graph for all service classes
//user account 
include ("../../../conf/castor-mon-web");
//checks if included dynamic library needed for the Oracle db
if (!function_exists('ociplogon')) {
	if (strtoupper(substr(PHP_OS, 0, 3) == 'WIN')) {
		dl('php_oci8.dll');
	}
}
//connection
$conn = ocilogon($db_instances[$service]['username'],$db_instances[$service]['pass'],$db_instances[$service]['serv']);
if(!$conn) {
	$e = oci_error();
	print htmlentities($e['message']);
	exit;
}
$query1 = "select svcclass
	   from ".$db_instances[$service]['schema']."SvcclassMap_MV";

if (!($parsed1 = OCIParse($conn, $query1))) 
	{ echo "Error Parsing Query";exit();}
if (!OCIExecute($parsed1))
	{ echo "Error Executing Query";exit();}
$i = 0;
//fetch data into local tables
while (OCIFetch($parsed1)) {
	$pool[$i] = OCIResult($parsed1,1);
	$i++;		
}
echo "<table><tr>";
  if ($period != NULL)
	foreach($pool as $p) {	
		echo "<tr><td align = 'center'><img src='tapes.php?service=$service&p=$p&period=$period'></td></tr>";
   }
  else
	  foreach($pool as $p) {	
		echo "<tr><td align = 'center'><img src='tapes.php?service=$service&p=$p&from=$date1 $date1hour&to=$date2 $date2hour'></td></tr>";
   }
 echo"</tr></table>";
 ?>