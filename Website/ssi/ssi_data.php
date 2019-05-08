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

	$data0 = (int) $_GET['data0'];
	$data1 = (int) $_GET['data1'];
	$data2 = (int) $_GET['data2'];
	$data3 = (int) $_GET['data3'];
	$data4 = (int) $_GET['data4'];
	$data5 = (int) $_GET['data5'];
	$data6 = (int) $_GET['data6'];
	$data7 = (int) $_GET['data7'];
	$data8 = (int) $_GET['data8'];
	$data9 = (int) $_GET['data9'];
	$data10 = (int) $_GET['data10'];
	$data11 = (int) $_GET['data11'];
	$data12 = (int) $_GET['data12'];
	$data13 = (int) $_GET['data13'];
	$data14 = (int) $_GET['data14'];
	
	if($data0<32768 && $data0>=-32768
		&& $data1<32768 && $data1>=-32768
		&& $data2<32768 && $data2>=-32768
		&& $data3<32768 && $data3>=-32768
		&& $data4<32768 && $data4>=-32768
		&& $data5<32768 && $data5>=-32768
		&& $data6<32768 && $data6>=-32768
		&& $data7<32768 && $data7>=-32768
		&& $data8<32768 && $data8>=-32768
		&& $data9<32768 && $data9>=-32768
		&& $data10<32768 && $data10>=-32768
		&& $data11<32768 && $data11>=-32768
		&& $data12<32768 && $data12>=-32768
		&& $data13<32768 && $data13>=-32768
		&& $data14<32768 && $data14>=-32768)
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

	$conn = mysqli_connect($host, $username, $password, $dbname);
	if(!$conn) {
		echo "error connection";
	}
	
	$sql = "SELECT data0 FROM data_";
	$sql .= $_GET['gid'];
	$sql .= "_";
	$sql .= $_GET['nid'];
	$sql .= "_";
	$sql .= $_GET['sid'];
	$sql .= " WHERE time='";
	$sql .= $_GET['date'];
	$sql .= " ";
	$sql .= $_GET['time'];
	$sql .= "'";
	
	echo $sql;
	echo "<br/>";
	
	$result1 = mysqli_query($conn, $sql);
	
	// check if the data already exist
	if(mysqli_num_rows($result1) == 0)
	{
		$sql = "INSERT INTO data_";
		$sql .= $_GET['gid'];
		$sql .= "_";
		$sql .= $_GET['nid'];
		$sql .= "_";
		$sql .= $_GET['sid'];
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
		
		$result2 = mysqli_query($conn, $sql);
		if(!$result2) {
			echo "error serverdb";
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
	echo "error urldata";
}	
?>