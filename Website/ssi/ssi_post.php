<?php

$do = FALSE;

// check the data sent from url
if(isset($_POST['gid'])
	&& isset($_POST['nid'])
	&& isset($_POST['sid'])
	&& isset($_POST['date'])
	&& isset($_POST['time'])
	&& isset($_POST['data0'])
	&& isset($_POST['data1'])
	&& isset($_POST['data2'])
	&& isset($_POST['data3'])
	&& isset($_POST['data4'])
	&& isset($_POST['data5'])
	&& isset($_POST['data6'])
	&& isset($_POST['data7'])
	&& isset($_POST['data8'])
	&& isset($_POST['data9'])
	&& isset($_POST['data10'])
	&& isset($_POST['data11'])
	&& isset($_POST['data12'])
	&& isset($_POST['data13'])
	&& isset($_POST['data14']))
{

	if(is_numeric($_POST['gid'])
		&& is_numeric($_POST['nid'])
		&& is_numeric($_POST['sid'])
		&& is_numeric($_POST['data0'])
		&& is_numeric($_POST['data1'])
		&& is_numeric($_POST['data2'])
		&& is_numeric($_POST['data3'])
		&& is_numeric($_POST['data4'])
		&& is_numeric($_POST['data5'])
		&& is_numeric($_POST['data6'])
		&& is_numeric($_POST['data7'])
		&& is_numeric($_POST['data8'])
		&& is_numeric($_POST['data9'])
		&& is_numeric($_POST['data10'])
		&& is_numeric($_POST['data11'])
		&& is_numeric($_POST['data12'])
		&& is_numeric($_POST['data13'])
		&& is_numeric($_POST['data14']))
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
	
	$gid = (int) $_POST['gid'];
	if($gid < 10){ $tbname .= "0"; }
	if($gid < 100){ $tbname .= "0"; }
	if($gid < 1000){ $tbname .= "0"; }
	if($gid < 10000){ $tbname .= "0"; }
	$tbname .= $gid;
	$tbname .= "_";
	
	$nid = (int) $_POST['nid'];
	if($nid < 10){ $tbname .= "0"; }
	if($nid < 100){ $tbname .= "0"; }
	$tbname .= $nid;
	$tbname .= "_";
	
	$sid = (int) $_POST['sid'];
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
	$sql .= $_POST['date'];
	$sql .= " ";
	$sql .= $_POST['time'];
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
		$sql .= $_POST['date'];
		$sql .= " ";
		$sql .= $_POST['time'];
		$sql .= "',";
		$sql .= $_POST['data0'];
		$sql .= ",";
		$sql .= $_POST['data1'];
		$sql .= ",";
		$sql .= $_POST['data2'];
		$sql .= ",";
		$sql .= $_POST['data3'];
		$sql .= ",";
		$sql .= $_POST['data4'];
		$sql .= ",";
		$sql .= $_POST['data5'];
		$sql .= ",";
		$sql .= $_POST['data6'];
		$sql .= ",";
		$sql .= $_POST['data7'];
		$sql .= ",";
		$sql .= $_POST['data8'];
		$sql .= ",";
		$sql .= $_POST['data9'];
		$sql .= ",";
		$sql .= $_POST['data10'];
		$sql .= ",";
		$sql .= $_POST['data11'];
		$sql .= ",";
		$sql .= $_POST['data12'];
		$sql .= ",";
		$sql .= $_POST['data13'];
		$sql .= ",";
		$sql .= $_POST['data14'];
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
	echo "error POSTdata";
}	
?>