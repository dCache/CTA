/******************************************************************************
 *              migsperpool.php
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
//connection
$conn = ocilogon(username,pass,serv);
if(!$conn) {
	$e = oci_error();
	print htmlentities($e['message']);
	exit;
}
$query1 = "select svcclass, count(*) migs
	   from migration
	   where timestamp > sysdate - $period
	   group by svcclass
	   order by migs desc";

if (!($parsed1 = OCIParse($conn, $query1))) 
	{ echo "Error Parsing Query";exit();}
if (!OCIExecute($parsed1))
	{ echo "Error Executing Query";exit();}
$i = 0;
while (OCIFetch($parsed1)) {
	$pool_names[$i] = OCIResult($parsed1,1);
	$num_migs[$i] = OCIResult($parsed1,2);
	$i++;		
}
if(empty($pool_names)) {
	No_Data_Image();
	exit();
};	   
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
else 
	$graph = new Graph(700,350,"auto",360);
$graph->SetShadow();
$graph->SetScale("textlin");
$graph->title->Set("Files Migrated per POOL");
$graph->title->SetFont(FF_FONT1,FS_BOLD);
$graph->img->SetMargin(60,40,40,120);
// $graph->yaxis->title->Set("Number of Requests" );
$graph->yaxis->title->SetFont(FF_FONT1,FS_BOLD);
$graph->yaxis->SetTitleMargin(40);
$graph->yaxis->SetFont(FF_FONT1,FS_BOLD,8);
$graph->xaxis->SetTickLabels($pool_names);
$graph->xaxis->SetLabelAngle(90);
$graph->xaxis->SetFont(FF_FONT1,FS_BOLD,8);
// $graph->xaxis->title->Set("POOL");
$graph->xaxis->SetTitleMargin(80);
$graph->xaxis->title->SetFont(FF_FONT1,FS_BOLD);
$b1 = new BarPlot($num_migs);
$b1->SetFillColor("orangered");
$b1->value->Show(); 
$b1->value->SetFormat('%0.0f');
$graph->Add($b1);
$graph->Stroke();

?> 



	
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	
