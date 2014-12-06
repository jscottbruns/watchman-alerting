#!perl
# **Note - Modification is required in order to maintain a persistent LWP HTTP session. Manual header alteration added 
# to LWP::Protocol::http on line 206. Regular expression match used to remove header "Keep-Alive: 300". 
# package PreAlertNet::CAD
use strict;

local $| = 1;

use HTTP::Headers;
use HTTP::Request;
use LWP;
use LWP::ConnCache;
use HTTP::Cookies;

open(LOG_FILE, ">>http.log");

my $username = '212';
my $password = '212';
my $incident_no = ''; #P093180409';
my $valid_session;

my $CAD = {
	'agent'			=>		'Opera/9.64 (Windows NT 5.1; U; en) Presto/2.1.1',
	'host'			=>		'http://10.11.1.144',
	'cookie_name'	=>		'ASP.NET_SessionId',
	'login'			=>		{
		'url'			=>		'/PRD/Html/SystemDocs/Logon.aspx',
		'query_string'	=>		'',
		'failure_msg'	=>		'Logon failed',
	},
	'incident'		=>		{
		'url'			=>		'/PRD/Html/SystemDocs/SSInterface.aspx',
		'query_string'	=>		'ACTION=X',
		'lookup_field'	=>		'RDS1',
	},
	'username'		=>		{
		'field'			=>		'UserName',
		'value'			=>		$username,
	},
	'password'		=>		{
		'field'			=>		'Password',
		'value'			=>		$password,
	},
};

die "Malformed host URL. Host must follow standard format: http://host.url.tld \n" unless $CAD->{'host'} =~ /^http:\/\/.*[^\/]$/;
die "Malformed login PATH info. Path must follow standard URL path format: /path/to/destination/script" unless $CAD->{'login'}->{'url'} =~ /^\//;

my $cookie_jar = HTTP::Cookies->new(    
	'file'				=>	"http_cookies.lwp",    
	'ignore_discard' 	=>	1,
	'autosave'			=>	1
);
my $ua = LWP::UserAgent->new( 
	'cookie_jar'		=>	$cookie_jar,
	'timeout'			=>	10,
	'conn_cache'		=>	LWP::ConnCache->new,
);

$cookie_jar->scan( \&validate_cookie );
if ( ! defined $valid_session  ) {

	if ( ! &main::init_login( &main::init_request( $CAD->{'host'} . $CAD->{'login'}->{'url'} . ( $CAD->{'login'}->{'query_string'} ? "?" . $CAD->{'login'}->{'query_string'} : undef ) ) ) ) {
	
		print LOG_FILE "Error received during authentication request\n";
		print "Error received during authentication request\n";
		exit;
	}
} 

&main::query_incident( &main::init_request( $CAD->{'host'} . $CAD->{'incident'}->{'url'} . ( $CAD->{'incident'}->{'query_string'} ? "?" . $CAD->{'incident'}->{'query_string'} : undef ) ), $incident_no );

sub init_login
{
	my $request = shift;
	# Load the content to post
	print LOG_FILE "Sending authentication request:\n";

	$request->content( "$CAD->{'username'}->{'field'}=$CAD->{'username'}->{'value'}&$CAD->{'password'}->{'field'}=$CAD->{'password'}->{'value'}" );
	my $response = $ua->request( $request );

	if ( $response->status_line eq '200 OK' || $response->status_line eq '302 Found' ) {
		
		print "HTTP request successful\n";
		
		print LOG_FILE "Request status: ". $response->status_line . "\n";
		print LOG_FILE "Headers:\n" . $response->headers_as_string . "\n\n";
		print LOG_FILE "Content:\n" . $response->content . "\n\n";
		
		if ( $response->content =~ /Logon failed:(.*?)<\//si ) {
			
			print LOG_FILE "Authentication failed\n"; 
			print "Authentication failed\n"; 

			return undef;
		}	
		
		return 1;
	} else {

		print LOG_FILE "HTTP request unsuccessful: " . $response->status_line . "\n";
		print "HTTP request unsuccessful: " . $response->status_line . "\n";
		return undef;
	}	
	
	return undef;
}
	
sub query_incident
{
	my $request = shift;
	my $inc_no = shift;
	
	print "Sending incident number lookup: \n";	

	# Incident lookup query
	$request->content( "RDS0=QIH  /&RDS1=$inc_no" );
	my $response = $ua->request( $request );

	if ( $response->status_line eq '200 OK' || $response->status_line eq '302 Found' ) {
		
		print "HTTP request successful\n";
			
		print LOG_FILE "Request status: ". $response->status_line . "\n";
		print LOG_FILE "Headers:\n" . $response->headers_as_string . "\n\n";
		print LOG_FILE "Content:\n" . $response->content . "\n\n";
		
	} else {

		print LOG_FILE "HTTP request unsuccessful: " . $response->status_line . "\n";
		print LOG_FILE "Headers:\n" . $response->headers_as_string . "\n\n";
		print LOG_FILE "Content:\n" . $response->content . "\n\n";

		print "HTTP request unsuccessful: " . $response->status_line . "\n";
		exit;
	}
	
}	

sub init_request
{
	my $host_url = shift;
	
	my $request = HTTP::Request->new( 
		POST 				=>	$host_url,
		HTTP::Headers->new(
			'User-Agent' 		=>	'Opera/9.64 (Windows NT 5.1; U; en) Presto/2.1.1',
			'Accept-Language' 	=>	'en-US,en',
			'Accept-Charset'	=>  'iso-8859-1, utf-8, utf-16, *',
			'Accept-Encoding' 	=>	'deflate, gzip',
			'Accept' 			=>	'text/html, application/xml, application/xhtml+xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*',
			'Content-Type'		=>	'application/x-www-form-urlencoded',
		),
	);
	
	return $request;
}

sub trim
{
    my $string = shift;
	if ( $string ) {
		$string =~ s/^\s+//;
		$string =~ s/\s+$//;
		return $string;
	}
}

sub validate_cookie
{
    my @params = @_;
	my $host = $CAD->{'host'};	
	$host =~ s/^http:\/\/(.*)/$1/;
	
	if ( $params[4] eq $host && $params[1] eq $CAD->{'cookie_name'} ) {

		if ( $params[8] && time >= $params[8] ) {
		
			undef $valid_session;
		}	
		
		$valid_session = 1;	
		return;
	}	
	
	return;
}