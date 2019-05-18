<?php

$do = FALSE;

if(isset($_POST['gid'])
	&& isset($_POST['nid'])
	&& isset($_POST['sid']))
{
	if(is_numeric($_POST['gid'])
		&& is_numeric($_POST['nid'])
		&& is_numeric($_POST['sid']))
	{
		$gid = (int) $_POST['gid'];
		$nid = (int) $_POST['nid'];
		$sid = (int) $_POST['sid'];
		$do = TRUE;
	}
}

if($do)
{
	$host = "localhost";
	$username = "root";
	$password = "zahra105";
	$dbname = "soil_inclinometer";
	$tbname = "gateway_instruction";

	$conn = mysqli_connect($host, $username, $password, $dbname);
	if(!$conn) {
		die("error connection");
	}
	
	$sql = "SELECT instruction_ID, instruction_code, instruction_data FROM gateway_instruction WHERE gateway_ID='";
	$sql .= $gid;
	$sql .= "' AND node_ID='";
	$sql .= $nid;
	$sql .= "' AND sensor_ID='";
	$sql .= $sid;
	$sql .= "' AND status='None' ORDER BY time ASC LIMIT 1";
	
	$result = mysqli_query($conn, $sql);
	
	if(mysqli_num_rows($result) > 0){
		while($row = mysqli_fetch_assoc($result)){
			echo $gid;
			echo ",";
			echo $nid;
			echo ",";
			echo $sid;
			echo ",";
			echo $row['instruction_ID'];
			echo ",";
			echo $row['instruction_code'];
			echo ",";
			echo $row['instruction_data'];
		}
	}else{
		echo "no instruction";
	}
}
else
{
	echo "error URLdata";
}

?>