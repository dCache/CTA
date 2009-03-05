<?php
//Prestaged and Not-Prestaged Files per username
include ("../lib/_oci_cache.php");
include ("../jpgraph-1.27/src/jpgraph.php");
include ("../jpgraph-1.27/src/jpgraph_bar.php");
include("../lib/no_data.php");
//user account
include ("../../../conf/castor-mon-web");
//get posted values 
$period = $_GET['period'];
$service = $_GET['service'];
$from = $_GET['from'];
$to = $_GET['to'];
if ($period != NULL) { 
	$qn = 1;
	$pattern = '/[0-9]+([\/][0-9]+)?/';
	preg_match($pattern,$period,$matches);
	$period = $matches[0];
	if($period == NULL) $period = 10/1440;
}	
else { 
	$qn = 2;
	$pattern1 = '/[0-3][0-9][\/][0-1][0-9][\/][0-9][0-9][0-9][0-9][\s][0-2][0-9][:][0-5][0-9]/';
	preg_match($pattern1,$from,$matches1);preg_match($pattern1,$to,$matches2);
	$from = $matches1[0]; $to = $matches2[0];
	if (($from ==NULL)or($to == NULL)) {
		echo "<h2>Wrong Arguments</h2>";
	}
}
//Create new graph, enable image cache by setting countdown period(in minutes) 
//depending on selected $period. If the cached image is valid the script immediately 
//returns the cached image and exits without logining in the DB
if ($period == 10/1440) 
	$graph = new Graph(700,350,"auto",1);
else if ($period == 1/24)
	$graph = new Graph(700,350,"auto",5);
else if ($period == 1)
	$graph = new Graph(700,350,"auto",30);
else if ($period == 7)
	$graph = new Graph(700,350,"auto",60);
else if ($period == 30)
	$graph = new Graph(700,350,"auto",360);
else if ($period == 10000)
	$graph = new Graph(700,350,"auto",360);
else 
	$graph = new Graph(700,350,"auto");
//connection - db login
$conn = ocilogon($db_instances[$service]['username'],$db_instances[$service]['pass'],$db_instances[$service]['serv']);
if(!$conn) {
	$e = oci_error();
	print htmlentities($e['message']);
	exit;
}
if ($qn ==1 )
	$query1 = "select a.username, a.reqs prestage , b.reqs stage
			   from (
		   select username , count(username) reqs
		   from ".$db_instances[$service]['schema']."requests 
		   where NN_Requests_timestamp > sysdate - $period
		   and type = 'StagePrepareToGetRequest'
		   and state = 'TapeRecall'
		   group by username
			   order by reqs desc ) a , 
		   (select username , count(username) reqs
		   from ".$db_instances[$service]['schema']."requests 
		   where NN_Requests_timestamp > sysdate - $period
		   and type = 'StageGetRequest'
		   and state = 'TapeRecall'
		   group by username
			   order by reqs desc) b 
			   where a.username = b.username
			   order by prestage desc";
else if ($qn ==2 )
	$query1 = "select a.username, a.reqs prestage , b.reqs stage
			   from (
		   select username , count(username) reqs
		   from ".$db_instances[$service]['schema']."requests 
		   where NN_Requests_timestamp >= to_date('$from','dd/mm/yyyy HH24:Mi')
			and NN_Requests_timestamp <= to_date('$to','dd/mm/yyyy HH24:Mi')
		   and type = 'StagePrepareToGetRequest'
		   and state = 'TapeRecall'
		   group by username
			   order by reqs desc ) a , 
		   (select username , count(username) reqs
		   from ".$db_instances[$service]['schema']."requests 
		   where NN_Requests_timestamp >= to_date('$from','dd/mm/yyyy HH24:Mi')
			and NN_Requests_timestamp <= to_date('$to','dd/mm/yyyy HH24:Mi')
		   and type = 'StageGetRequest'
		   and state = 'TapeRecall'
		   group by username
			   order by reqs desc) b 
			   where a.username = b.username
			   order by prestage desc";
if (!($parsed1 = OCIParse($conn, $query1))) 
	{ echo "Error Parsing Query";exit();}
if (!OCIExecute($parsed1))
	{ echo "Error Executing Query";exit();}
$i = 0;
//fetch data into local tables
while (OCIFetch($parsed1)) {
	$pre_names[$i] = OCIResult($parsed1,1);
	$pre_reqs[$i] = OCIResult($parsed1,2);
	$reqs[$i] = OCIResult($parsed1,3);
	$i++;		
}
//No data retrieved -> Print out "No Data available" Image
if(empty($pre_names)) {
	No_Data_Image();
	exit();
};
//Create new graph
$graph->SetShadow();
$graph->SetScale("textlin");
$graph->title->Set("Prestaged / Non - Prestaged Requests per USER");
$graph->title->SetFont(FF_FONT1,FS_BOLD);
$graph->img->SetMargin(60,40,40,120);
$graph->yaxis->title->SetFont(FF_FONT1,FS_BOLD);
$graph->yaxis->SetTitleMargin(40);
$graph->xaxis->SetTickLabels($pre_names);
$graph->xaxis->SetLabelAngle(90);
$graph->xaxis->SetFont(FF_FONT1,FS_BOLD);
$graph->xaxis->SetTitleMargin(80);
$graph->xaxis->title->SetFont(FF_FONT1,FS_BOLD);
$b1 = new BarPlot($pre_reqs);
$b1->SetWidth(0.5);
$b1->SetFillColor("orangered");
$b1->value->SetFont(FF_FONT1,FS_BOLD,10);
$b1->value->SetAngle(90);
$b1->value->SetColor("orangered");
$b1->value->Show();
$b1->value->SetFormat('%0.0f');
$b1 -> SetLegend("Prestaged Requests");
$b2 = new BarPlot($reqs);
$b2->SetWidth(0.5);
$b2->SetFillColor("darkgoldenrod1");
$b2->value->SetFont(FF_FONT1,FS_BOLD,10);
$b2->value->SetColor("darkgoldenrod1");
$b2->value->SetAngle(90);
$b2->value->Show();
$b2->value->SetFormat('%0.0f');
$b2 -> SetLegend("Non Prestaged Requests");
$gbplot = new GroupBarPlot(array($b2,$b1));
$graph->Add($gbplot);
$graph->Stroke();

?> 