#!perl
use HTTP::Request;
use LWP::UserAgent;
use HTTP::Cookies;

open(HTM, ">htm_response.txt");

my $request;

my $cookie_file = 'http_cookies_genpop.lwp';
my $cookie_jar = HTTP::Cookies->new(
	'file'              =>  $cookie_file,
	'ignore_discard'    =>  1,
	'autosave'          =>  1
);

my $ua = LWP::UserAgent->new(
	'cookie_jar'        =>  $cookie_jar,
	'timeout'           =>  10,
);

if ( ! -f $cookie_file ) {

	print "Logging In...\n\n";

	my $content = "UserName=fire&Password=fire";
	$request = init_request( '/PRD/Html/SystemDocs/Logon.aspx', length( $content ) );
	$request->content( $content );

	print HTM "Login Request:\n";
	print HTM "Headers: " . $request->headers_as_string . "\n\n";
	print HTM "Content: " . $request->content . "\n";

	my $response = $ua->request( $request );

	print HTM "Login Response:\n";
	print HTM "Headers: " . $response->headers_as_string . "\n\n";
	print HTM "Content: " . $response->content . "\n";

	if ( $response->status_line eq '200 OK' || $response->status_line eq '302 Found' ) {

		print "Request status: ". $response->status_line . "\n";

		my $resp_headers = $response->headers_as_string;
		while ( $resp_headers =~ /^([a-zA-Z0-9-_]*):([a-zA-Z0-9 -_:=;\(\)\\\/,\.]*)/gsm ) {
			my $h_1 = $1;
			my $h_2 = $2;
			if ( $h_1 =~ /Set-Cookie/i ) {
				$h_2 =~ s/(.*);(.*)/$1/;
				print "Assigning session cookie: $h_2\n";
				last;
			}
		}

		if ( $response->content =~ /Logon failed(.*?)<\//si ) {

			print "Server response included login failure message: $self->{CAD}->{login}->{login_failureMsg}\n";
			exit;
		}

		# Save the cookies to a file if loging was successful
		$cookie_jar->save;

	} else {

		print "HTTP request unsuccessful: " . $response->status_line . "\n";
		exit;
	}
}
exit;
print "Submitting HTTP request: \n";

my $content = "RDS0=QIH++%2F&RDS1=&RDS2=&RDS3=&RDS4=&RDS5=&RDS6=01&RDS7=24&RDS8=10&RDS9=12&RDS10=15&RDS11=&RDS12=&RDS13=&RDS14=&RDS15=&RDS16=&RDS17=&RDS18=&RDS19=&RDS20=&RDS21=&RDS22=&RDS23=&RDS24=&RDS25=&RDS26=&RDS27=&RDS28=&RDS29=&RDS30=&RDS31=&RDS32=&RDS33=&RDS34=";

$request = &init_request( '/PRD/Html/SystemDocs/SSInterface.aspx?ACTION=X', length( $content ) );

$request->content( $content );
print HTM $request->headers_as_string;
print HTM "\n\n";
print HTM $request->content;

my $response = $ua->request( $request );

print HTM "After request: \n";
print HTM $request->headers_as_string;
print HTM "\n\n";
print HTM $request->content;

print "Response status: " . $response->status_line . "\n";

print HTM "Request result: " . $response->status_line . "\n";
print HTM "Response Headers: " . $response->headers_as_string . "\n";
print HTM "Response Content: " . $response->content . "\n";

sub init_request
{
	my $uri = shift;
	my $length = shift;
	
	my $url = 'http://10.11.1.144' . $uri;
	
	print "Initiating HTTP request to $url\n";
	
	my $req = HTTP::Request->new(
		POST                =>  $url,
		HTTP::Headers->new(
			'Accept'            =>  'image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/xaml+xml, application/vnd.ms-xpsdocument, application/x-ms-xbap, application/x-ms-application, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*',
			'Referer'			=>	'http://10.11.1.144/PRD/Html/SystemDocs/SSInterface.aspx',		
			'Accept-Language'   =>  'en-US',		
			'User-Agent'        =>  'Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)',
			'Content-Type'      =>  'application/x-www-form-urlencoded',		
			'Accept-Encoding'   =>  'deflate, gzip',
			'Host'				=>	'10.11.1.144',
			'Content-Length'	=>	$length,
			'Connection'		=>	'Keep-Alive',
			'Cache-Control'		=>	'no-cache'
		),
	);

	return $req;
}
