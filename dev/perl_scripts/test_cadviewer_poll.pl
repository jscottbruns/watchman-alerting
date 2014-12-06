#!perl
use LWP;

use HTTP::Headers;
use HTTP::Cookies;

my $cookie_jar = HTTP::Cookies->new(
	'file'              =>  'testcookie.lwp',
	'ignore_discard'    =>  1
);

$cookie_jar->clear;
my $can_accept = HTTP::Message::decodable;

my $ua = LWP::UserAgent->new(
	'agent'				=>	'WatchmanCAD Client',
	'timeout'           =>  5,
	'cookie_jar'		=>	$cookie_jar,
	'default_header'	=>	HTTP::Headers->new(
		User-Agent			=>	'WatchmanCAD Client',
		Accept-Language		=>  'en-US,en',
		Accept-Charset		=>  'iso-8859-1, utf-8, utf-16, *',
		Accept-Encoding		=>	$can_accept,
		Accept				=>  'text/html, application/xml, application/xhtml+xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*',
		Content-Type		=>  'application/x-www-form-urlencoded',
	)
);

my $response = $ua->post(
	'http://cadviewer.firehousewatchman.com/login.php',
	Content	=>	"user_name=usertest&password=123123&r=poll&login_button_x=1"
);

if ( $response->status_line eq '200 OK' || $response->status_line eq '302 Found' ) {

	print "Saving cookies\n";
	$cookie_jar->save;
}

print "Calling HTTP poll\n";

my $response = $ua->post(
	'http://cadviewer.firehousewatchman.com/poll.php',
	Content	=>	"date=2011-03-23"
);

if ( $response->status_line eq '200 OK' ) {

	print "HTTP request successful\n";
	print "Content Type: ";
	print $response->header('Content-Type');

	if ( $response->header('Content-Type') =~ /text\/xml/ ) {
			print "Y";
	}

	if ( open(FH, ">httpresp.txt") ) {

		print FH "Content: \n";
		print FH $response->content;
		print FH "\n\n";

		print FH "Decoded: \n";
		print FH $response->decoded_content;
		print FH "\n\n";
		close FH;
	}

}