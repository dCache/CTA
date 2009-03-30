<?php
//Different file ".$db_instances[$service]['schema']." requests percentages (Pie Graph) -> Castor dashboard
//Include necessary libraries 
include ("../lib/_oci_no_cache.php");
include ("../jpgraph-1.27/src/jpgraph.php");
include ("../jpgraph-1.27/src/jpgraph_pie.php");
include ("../jpgraph-1.27/src/jpgraph_pie3d.php"); 
include ("../lib/no_data.php");
//include user account
include ("../../../conf/castor-mon-web/user.php");
//get posted data
$service = $_GET['service'];
//connection -db login
$conn = ocilogon($db_instances[$service]['username'],$db_instances[$service]['pass'],$db_instances[$service]['serv']);
if(!$conn) {
	$e = oci_error();
	print htmlentities($e['message']);
	exit;
}
$query1 = "select count(case when state='DiskHit' then 1 else null end) DiskHits, count(case when state='DiskCopy' then 1 else null end) DiskCopies,
			count(case when state='TapeRecall' then 1 else null end) TapeRecalls
			from ".$db_instances[$service]['schema']."requests 
			where timestamp >= sysdate - 15/1440
			and timestamp < sysdate - 5/1440";

if (!($parsed1 = OCIParse($conn, $query1))) 
	{ echo "Error Parsing Query";exit();}
if (!OCIExecute($parsed1))
	{ echo "Error Executing Query";exit();}
//Fetch data into local variables
if (OCIFetch($parsed1)) {
	$DiskHits = OCIResult($parsed1,1);
	$DiskCopy = OCIResult($parsed1,2);
	$Taperecall = OCIResult($parsed1,3);
};
//If percentages sum equals zero then display "no available data" image
if(($DiskHits+$DiskCopy+$Taperecall) == 0) {
	No_Data_Image();
	exit();
};
//create new pie graph
$data = array($DiskHits,$DiskCopy,$Taperecall);
$leg = array("Resident Files","Pool Copies","Tape Recalls"); 
$graph = new PieGraph(420,200,"auto");
$graph->SetShadow();
$graph->title-> Set( "File Requests Percentages");
$p1 = new PiePlot3D($data);
$p1->SetLegends($leg);  
$p1->SetTheme("sand");
$p1->SetAngle(50);
$graph->Add( $p1); 
$graph->Stroke(); 
?>
