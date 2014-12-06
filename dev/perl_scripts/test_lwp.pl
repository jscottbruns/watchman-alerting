#!/usr/bin/perl

use LWP::UserAgent;
use HTTP::Headers;
use HTTP::Message;
use HTTP::Cookies;
use HTML::Entities;
use Data::Dumper;

$ua = LWP::UserAgent->new(
    'agent'             => "iCAD Client/1.1 (WatchmanAlerting AlertQueue Polling Agent; Standard HTTP Agent Object)",
    'default_headers'   => HTTP::Headers->new(
        'Accept-Language'   =>  'en-US,en',
        'Accept-Charset'    =>  'iso-8859-1, utf-8, utf-16, *',
        'Accept-Encoding'   =>  scalar HTTP::Message::decodable(),
        'Accept'            =>  'text/html, application/xml, application/xhtml+xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*',
        'Content-Type'      =>  'application/x-www-form-urlencoded',
    )
);

if ( $ua )
{

    my $response = $ua->post(
        'http://icad.pittsburghpa.fhwm.net:8080/login.php',
        Content => 'user_name=ENG10&password=ENG10'
    );

    print "Response: $response->{_content}\n";
    print Dumper($response->{_headers});
}
