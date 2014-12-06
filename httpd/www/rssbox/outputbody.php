<?
//Function for ouputting the body of each RSS item displayed (inside loop)- DynamicDrive.com
//For syntax pf bpdu, see: http://simplepie.org/docs/installation/from-scratch/ and http://simplepie.org/docs/reference/
//Function by default defines 3 different body outputs (templates). Modify or add additional templates as desired


function outputbody($item, $template=""){
if ($template=="" || $template=="default"){ //DEFAULT TEMPLATE
	?>
	<DIV class="rsscontainer">
	<div class="rsstitle"><a href="<?php echo $item->get_permalink(); ?>"><?php echo $item->get_title(); ?></a></div>
	<div class="rssdate"><?php echo $item->get_date('d M Y g:i a'); ?></div>
	<div class="rssdescription"><?php echo $item->get_description(); ?></div>
	</DIV>
	<?
} //end default template
else if ($template=="titles"){ //"TITLES" TEMPLATE
	?>
	<DIV class="rsscontainer">
	<div class="rsstitle"><a href="<?php echo $item->get_permalink(); ?>" target="_new"><?php echo $item->get_title(); ?></a></div> 
	<div>Category: <?php echo $item->get_category(); ?></div>
	</DIV>
	<?
} //end titles template
else if ($template=="titlesdates"){ //"TITLESDATES" TEMPLATE
	?>
	<DIV class="rsscontainer">
	<span class="rsstitle"><a href="<?php echo $item->get_permalink(); ?>"><?php echo $item->get_title(); ?></a></span> 
	<span class="rssdate"><?php echo $item->get_date('m/d/y g:i a'); ?></span>
	</DIV>
	<?
} //end titlesdates template
else if ($template=="mytemplatename"){ //"mytemplatename" TEMPLATE
	?>
	//DEFINE ADDITIONAL CUSTOM TEMPLATE(s) USING SAME LOGIC STRUCTURE AS ABOVE
	//For syntax of template body, see SimplePie docs: http://simplepie.org/docs/installation/from-scratch/ and http://simplepie.org/docs/reference/
	<?
}




else
die ("No template exists with such name!");
} //Closing function bracket
?>