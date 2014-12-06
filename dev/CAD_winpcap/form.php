<?php

if ( $_POST['s'] ) {
	echo "Form POST Contents: \n";
	print_r($_POST);
	//setcookie("TestCookie", $_POST['in_eid']);
}

?>