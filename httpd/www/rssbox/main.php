<?php
//Function for ouputting the body of each RSS item displayed (inside loop)- DynamicDrive.com

require('../simplepie/simplepie.inc'); //Specify path to simplepie.inc on your server (by default located inside "simplepie" directory)
$feed = new SimplePie();
$feed->cache_location('cache'); //Specify path to cache directory on your server relative to this file
$feed->handle_content_type();
require('outputbody.php'); //Path to custom "outputbody.php" file relative to this file

//Specify list of RSS URLs
$rsslist=array(
"cssdrive" => "http://www.cssdrive.com/index.php/news/rss_2.0/",
"bbc" => "http://newsrss.bbc.co.uk/rss/newsonline_uk_edition/front_page/rss.xml",
"nytimes" => "http://www.nytimes.com/services/xml/rss/nyt/HomePage.xml",
"dynamicdrive" => "http://www.dynamicdrive.com/export.php?type=new" //no trailing comma after last RSS URL!!
);

////Beginners don't need to configure past here. Edit outputbody.php to format the output of each RSS item's body ////////////////////

$rssid=$_GET['id'];
$rssurl=isset($rsslist[$rssid])? $rsslist[$rssid] : die("<b>Error:</b> Can't find requested RSS in list.");
$cachetime=isset($_GET["cachetime"])? (int)$_GET["cachetime"] : 30; //Get cachetime. Default to 30 if not specified
$feednumber=isset($_GET["limit"])? (int)$_GET["limit"] : ""; //Get number of items to fetch. If not specified (""), all is fetched.

$templatename=isset($_GET["template"])? $_GET["template"] : ""; //get name of template, which is used by if/else logic within function outputbody()
if ($templatename!="" && !preg_match("/^(\w|-)+$/i", $templatename)) //Template name can only contain alphanumeric characters, underscore or dash (-)
die ("<b>Error:</b> Template name can only consist of alphanumeric characters, underscore or dash");

$feed->cache_max_minutes($cachetime);
$feed->feed_url($rssurl);
$feed->init();
$max = $feed->get_item_quantity($feednumber);


function outputitems(){
global $feed, $feednumber, $templatename;
$max = $feed->get_item_quantity($feednumber);
for ($x = 0; $x < $max; $x++){
$item = $feed->get_item($x);
outputbody($item, $templatename); //call custom outputbody() function
}
}

?>

<?php
if ($feed->data){
outputitems();
}

?>


