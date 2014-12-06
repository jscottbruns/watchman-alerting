<?php
require_once('database.php');

$db = new Database();

$sql = "SELECT * FROM tbl_user WHERE userId='1' ";

$db->query($sql);

$db->singleRecord(); //call this if the query will only return a single row

echo $db->Record['userId']; // use the field name for example or;

echo $db->Record[0]; //use indexes