<?php

$do = FALSE;

// check the data sent from url
if(isset($_GET['gid'])
	&& isset($_GET['nid'])
	&& isset($_GET['sid'])
	&& isset($_GET['date'])
	&& isset($_GET['time'])
	&& isset($_GET['data0'])
	&& isset($_GET['data1'])
	&& isset($_GET['data2'])
	&& isset($_GET['data3'])
	&& isset($_GET['data4'])
	&& isset($_GET['data5'])
	&& isset($_GET['data6'])
	&& isset($_GET['data7'])
	&& isset($_GET['data8'])
	&& isset($_GET['data9'])
	&& isset($_GET['data10'])
	&& isset($_GET['data11'])
	&& isset($_GET['data12'])
	&& isset($_GET['data13'])
	&& isset($_GET['data14']))
{

	if(is_numeric($_GET['gid'])
		&& is_numeric($_GET['nid'])
		&& is_numeric($_GET['sid'])
		&& is_numeric($_GET['data0'])
		&& is_numeric($_GET['data1'])
		&& is_numeric($_GET['data2'])
		&& is_numeric($_GET['data3'])
		&& is_numeric($_GET['data4'])
		&& is_numeric($_GET['data5'])
		&& is_numeric($_GET['data6'])
		&& is_numeric($_GET['data7'])
		&& is_numeric($_GET['data8'])
		&& is_numeric($_GET['data9'])
		&& is_numeric($_GET['data10'])
		&& is_numeric($_GET['data11'])
		&& is_numeric($_GET['data12'])
		&& is_numeric($_GET['data13'])
		&& is_numeric($_GET['data14']))
	{
		$do = TRUE;
	}
}

// insert the data sent from url to database
if($do)
{
	$host = "localhost";
	$username = "root";
	$password = "zahra105";
	$dbname = "soil_inclinometer";
	$tbname = "data_";
	
	$gid = (int) $_GET['gid'];
	if($gid < 10){ $tbname .= "0"; }
	if($gid < 100){ $tbname .= "0"; }
	if($gid < 1000){ $tbname .= "0"; }
	if($gid < 10000){ $tbname .= "0"; }
	$tbname .= $gid;
	$tbname .= "_";
	
	$nid = (int) $_GET['nid'];
	if($nid < 10){ $tbname .= "0"; }
	if($nid < 100){ $tbname .= "0"; }
	$tbname .= $nid;
	$tbname .= "_";
	
	$sid = (int) $_GET['sid'];
	if($sid < 10){ $tbname .= "0"; }
	$tbname .= $sid;
		
	$conn = mysqli_connect($host, $username, $password, $dbname);
	if(!$conn) {
		die("error connection");
	}
	
	$sql = "SHOW TABLES LIKE '";
	$sql .= $tbname;
	$sql .= "'";
	
	$result0 = mysqli_query($conn, $sql);
	if(mysqli_num_rows($result0) == 0)
	{
		die("error IDdata");
	}	
	
	$sql = "SELECT 1 FROM ";
	$sql .= $tbname;
	$sql .= " WHERE time='";
	$sql .= $_GET['date'];
	$sql .= " ";
	$sql .= $_GET['time'];
	$sql .= "' LIMIT 1";
	
	echo $sql;
	echo "<br/>";
	
	$result1 = mysqli_query($conn, $sql);
	
	// check if the data already exist
	if(mysqli_num_rows($result1) == 0)
	{
		$sql = "INSERT INTO ";
		$sql .= $tbname;
		$sql .= " (time,data0,data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14) VALUES ('";
		$sql .= $_GET['date'];
		$sql .= " ";
		$sql .= $_GET['time'];
		$sql .= "',";
		$sql .= $_GET['data0'];
		$sql .= ",";
		$sql .= $_GET['data1'];
		$sql .= ",";
		$sql .= $_GET['data2'];
		$sql .= ",";
		$sql .= $_GET['data3'];
		$sql .= ",";
		$sql .= $_GET['data4'];
		$sql .= ",";
		$sql .= $_GET['data5'];
		$sql .= ",";
		$sql .= $_GET['data6'];
		$sql .= ",";
		$sql .= $_GET['data7'];
		$sql .= ",";
		$sql .= $_GET['data8'];
		$sql .= ",";
		$sql .= $_GET['data9'];
		$sql .= ",";
		$sql .= $_GET['data10'];
		$sql .= ",";
		$sql .= $_GET['data11'];
		$sql .= ",";
		$sql .= $_GET['data12'];
		$sql .= ",";
		$sql .= $_GET['data13'];
		$sql .= ",";
		$sql .= $_GET['data14'];
		$sql .= ")";
	
		echo "<br/>";
		echo $sql;
		echo "<br/>";
		
		// insert data to database
		$result2 = mysqli_query($conn, $sql);
		if(!$result2) {
			echo "error insertData";
		} else {
			echo "success";
		}
		
}
	else
	{
		echo "error duplicate";
	}
	mysqli_close($conn);
}
else
{
	echo "error URLdata";
}	
?>