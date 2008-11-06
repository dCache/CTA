/******************************************************************************
 *              gcsizedistr.php
 *
 * This file is part of the Castor Monitoring project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2008  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Author Theodoros Rekatsinas, 
 *****************************************************************************/
<?php 
include ("lib.php");
include ("../jpgraph-2.3.3/src/jpgraph.php");
include ("../jpgraph-2.3.3/src/jpgraph_bar.php");
include("no_data.php");
include ("user.php");
$period = $_GET['period'];
if($period == NULL) $period = 10000;
//initialization
$bins = array( 0 =>"<1Kb",1 =>"[1-10)Kb",2 =>"[10-100)Kb",3 =>"[100Kb-1Mb)",4 =>"[1-10)Mb",5 =>"[10-100)Mb",6 =>"[100Mb-1Gb]", 7=> ">1Gb");
//connection
$conn = ocilogon(username,pass,serv);
if(!$conn) {
	$e = oci_error();
	print htmlentities($e['message']);
	exit;
}
for($i = 0;$i < 8; $i++)
	$fsize[$i] = 0;
$query1 = "select distinct bin, count(bin) over (Partition by bin) reqs 
	from (
	select round(filesize/1024,4) , case when filesize < 1024 then 1
	  when filesize >= 1024 and filesize < 10240 then 2
	  when filesize >= 10240 and filesize < 102400 then 3
	  when filesize >= 102400 and filesize < 1048576 then 4
	  when filesize >= 1048576 and filesize < 10485760 then 5
	  when filesize >= 10485760 and filesize < 104857600 then 6
	  when filesize >= 104857600 and filesize <= 1073741824 then 7
	  else 8 end bin
	from gcfiles
	where timestamp > sysdate - $period
	and filesize!=0)
	order by bin";

if (!($parsed1 = OCIParse($conn, $query1))) 
	{ echo "Error Parsing Query";exit();}
if (!OCIExecute($parsed1))
	{ echo "Error Executing Query";exit();}
while (OCIFetch($parsed1)) {
	$num_bin = OCIResult($parsed1,1);
	$fsize[$num_bin - 1] = OCIResult($parsed1,2);		
}

if(empty($num_bin)) {
	No_Data_Image();
	exit();
};

//data processing

//plot
if ($period == 10/1440) 
	$graph = new Graph(730,300,"auto",1);
else if ($period == 1/24)
	$graph = new Graph(730,300,"auto",5);
else if ($period == 1)
	$graph = new Graph(730,300,"auto",30);
else if ($period == 7)
	$graph = new Graph(730,300,"auto",60);
else if ($period == 30)
	$graph = new Graph(730,300,"auto",360);
else 
	$graph = new Graph(730,300,"auto",360);
$graph->SetShadow();
$graph->SetScale("textlin");
$graph->title->Set("Size Distribution of Garbage Collected Files");
$graph->title->SetFont(FF_FONT1,FS_BOLD);
$graph->img->SetMargin(60,40,40,120);
$graph->yaxis->title->Set("Number of Files" );
$graph->yaxis->title->SetFont(FF_FONT1,FS_BOLD);
$graph->yaxis->SetTitleMargin(40);

$graph->xaxis->SetTickLabels($bins);
$graph->xaxis->SetLabelAngle(0);
$graph->xaxis->SetFont(FF_FONT1,FS_BOLD);
$graph->xaxis->title->Set("FileSize");
$graph->xaxis->SetTitleMargin(80);
$graph->xaxis->title->SetFont(FF_FONT1,FS_BOLD);
$b1 = new BarPlot($fsize);
$b1->SetFillColor("red");
$b1->value->Show();
$b1->value->SetFormat('%01.02f');
$graph->Add($b1);
$graph->Stroke();
?> 



	
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	
