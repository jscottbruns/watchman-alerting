#!perl

use LWP::UserAgent;
use HTTP::Headers;
use HTTP::Message;
use HTTP::Cookies;
use HTML::Entities;
use URI::Escape;

use XML::XPath;

my $cookie_jar = HTTP::Cookies->new(
	file			=>	'GarminHttpCookie.lwp',
	ignore_discard	=>	1
);

my $ua = LWP::UserAgent->new(
	agent			=>	'WatchmanGPS Client/1.2.1 (compatible; Mozilla/4.0; MSIE 7.0; Windows NT 5.1; GTB6; .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; MDDC; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)',
	cookie_jar		=>	$cookie_jar,
	timeout			=>	15,
	default_headers	=>	HTTP::Headers->new(
    	'Accept-Language'	=>	'en-US,en',
    	'Accept-Charset'	=>	'iso-8859-1, utf-8, utf-16, *',
    	'Accept-Encoding'	=>	scalar HTTP::Message::decodable(),
    	'Accept'			=>	'text/html, application/xml, application/xhtml+xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*'
	)
);


my $api_key = 'ca10609abba0158d758eff73846f2093';
my $device_id = '3766180185';

my $loc_name = "505 Suffolk Ave, Capitol Heights, MD 20743";
my $lat = '38.883645';
my $long = '-76.911605';
my $attribution = '';
my $province = 'MD';
my $country = 'US';
my $city = 'Capitol Heights';
my $street = 'Suffolk Ave';
my $streetnum = '505';
my $street_addr = "$streetnum $street";
my $postcode = '20743';
my $phone = '';
my $source = uri_escape("http://maps.google.com");
my $notes = '';
my $c0_id = '0';
my $user = 'string:jsbruns';
my $pwd = 'string:aci123';
my $batchId = 1;
my $callCount = '1';
my $page = uri_escape("/locate/savePOI.htm?action=redirect");

#my $httpSessID = 'FDDBC3EE716701AF0D085A3DDE13BC18';
#my $scriptSessionId = '5D0B2AC1898EB2630488FBF40AA0DAB5973';

my $login_url = "http://my.garmin.com/locate/dwr/call/plaincall/MygarminAuth.login.dwr?callCount=$callCount&page=$page&a_key=$api_key&name=$loc_name&hl=en&lat=$lat&long=$long&attribution=$attribution&province=$province&country=$country&city=$city&street=$street&streetnum=$streetnum&postcode=$postcode&phone=$phone&source=$source&notes=$notes&httpSessionId=$httpSessID&scriptSessionId=$scriptSessionId&c0-scriptName=MygarminAuth&c0-methodName=login&c0-id=$c0_id&c0-param0=$user&c0-param1=$pwd&batchId=$batchId";
my $getUName_url = "http://my.garmin.com/locate/dwr/call/plaincall/MygarminAuth.getUsername.dwr?callCount=$callCount&page=$page&a_key=$api_key&name=$loc_name&hl=en&lat=$lat&long=$long&attribution=$attribution&province=$province&country=$country&city=$city&street=$street&streetnum=$streetnum&postcode=$postcode&phone=$phone&source=$source&notes=$notes&httpSessionId=$httpSessID&scriptSessionId=$scriptSessionId&c0-scriptName=MygarminAuth&c0-methodName=getUsername&c0-id=$c0_id&batchId=$batchId";
my $isAuth_url = "http://my.garmin.com/locate/dwr/call/plaincall/MygarminAuth.isAuthenticated.dwr?callCount=$callCount&page=$page&a_key=$api_key&name=$loc_name&hl=en&lat=$lat&long=$long&attribution=$attribution&province=$province&country=$country&city=$city&street=$street&streetnum=$streetnum&postcode=$postcode&phone=$phone&source=$source&notes=$notes&httpSessionId=$httpSessID&scriptSessionId=$scriptSessionId&c0-scriptName=MygarminAuth&c0-methodName=isAuthenticated&c0-id=$c0_id&batchId=$batchId";
my $send_gps = "http://my.garmin.com/locate/dwr/call/plaincall/ManageLocations.sendSingleWaypointWirelessly.dwr?callCount=$callCount&page=$page&a_key=$api_key&name=$loc_name&hl=en&lat=$lat&long=$long&attribution=$attribution&province=$province&country=$country&city=$city&street=$street&streetnum=$streetnum&postcode=$postcode&phone=$phone&source=$source&notes=$notes&httpSessionId=$httpSessID&scriptSessionId=$scriptSessionId&c0-scriptName=ManageLocations&c0-methodName=sendSingleWaypointWirelessly&c0-id=$c0_id&c0-param0=boolean:false&c0-e1=string:$device_id&c0-param1=Array:[reference:c0-e1]&c0-e2=number:$lat&c0-e3=number:$long&c0-e4=string:$loc_name&c0-e5=string:$street_addr&c0-e6=string:&c0-e7=string:$city&c0-e8=string:$province&c0-e9=string:$country&c0-e10=string:$postcode&c0-e11=string:$phone&c0-e12=string:$notes&c0-param2=Object_Object:{latitude:reference:c0-e2, longitude:reference:c0-e3, name:reference:c0-e4, address1:reference:c0-e5, address2:reference:c0-e6, city:reference:c0-e7, stateCode:reference:c0-e8, countryCode:reference:c0-e9, postalCode:reference:c0-e10, phoneNumber:reference:c0-e11, note:reference:c0-e12}&batchId=$batchId";

if ( open(FH, ">GarmingSendToGPS.out.txt") ) {
	print FH $send_gps;
	close FH;
}

print "Initiating HTTP login [ $login_url ]\n";
sleep 1;

my $resp = $ua->get($login_url);

print "Login Response: \n\n";
print "Headers: \n";
print $resp->headers_as_string;
print "Content: \n";
print $resp->decoded_content;
sleep 2;

print "\n\n";

print "Initiating HTTP authentication check [ $isAuth_url ]\n";
sleep 1;

my $resp = $ua->get($isAuth_url);

print "Login Response: \n\n";
print "Headers: \n";
print $resp->headers_as_string;
print "Content: \n";
print $resp->decoded_content;
sleep 2;

print "\n\n";


print "Initiating HTTP username request [ $getUName_url ]\n";
sleep 1;

my $resp = $ua->get($getUName_url);

print "Login Response: \n\n";
print "Headers: \n";
print $resp->headers_as_string;
print "Content: \n";
print $resp->decoded_content;
sleep 2;

print "\n\n";


print "Initiating HTTP send waypoint request [ $send_gps ]\n";
sleep 1;

my $resp = $ua->get($send_gps);

print "Login Response: \n\n";
print "Headers: \n";
print $resp->headers_as_string;
print "Content: \n";
print $resp->decoded_content;
sleep 2;

print "\n\n";

#print "\nPreparing cookie scan...\n";
#$cookie_jar->scan( \&validate );



exit;


sub validate
{
	my @params = @_;
	my $i = 0;

	print "Cookie Dump: \n";

	foreach ( @params ) {
		print ( $i++ );
		print ": $_ \n"
	}
}
