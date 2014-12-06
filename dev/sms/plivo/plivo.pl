#!/usr/bin/perl
use LWP::UserAgent;
use JSON;
use Data::Dumper;

my $uri = 'https://MAMTI0YZCZOGM3YWQ2MZ:MDNlYWFiM2FiNmM4NTNhMzJiNjcxOGNmNWEwMTJh@api.plivo.com/v1/Account/MAMTI0YZCZOGM3YWQ2MZ/Message/';
my $secret = 'MDNlYWFiM2FiNmM4NTNhMzJiNjcxOGNmNWEwMTJh';
my $text = "[231-110] HOUSE FIRE 1929 LETSCHE ST #59 (BELLEAU ST & MERCY ST) 231EN38 231EN37 231EN32 231TK32 2314011 231MAC1 231EN03 2314044 http://fhwm.net/g/1132 http://fhwm.net/c/1133";

my $args = {
    src     => '14432610284',
    dst     => '14432501272',
    text    => $text,
    type    => 'sms',
    url     => 'http://services.fhwm.net/sms/63ac4f25'
};

my $ua = LWP::UserAgent->new;

my $req = HTTP::Request->new( POST => $uri );
$req->content_type('application/json');
$req->content( to_json($args) );

my $response = $ua->request( $req );

print "Result: " . $response->is_success . "\n";
my $r = from_json($response->content);
print Dumper($r);
#print join ',', @{ $r->{'message_uuid'} };

