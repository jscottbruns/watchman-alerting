#!/usr/bin/perl
use LWP::UserAgent;
use HTTP::Request::Common;
use JSON;
use Data::Dumper;

my $uri = 'http://rest.nexmo.com/sms/json';
my $apikey = '07243b69';
my $secret = 'a0018d9b';
my $text = "[231-110] HOUSE FIRE 1929 LETSCHE ST #59 (BELLEAU ST & MERCY ST) 231EN38 231EN37 231EN32 231TK32 2314011 231MAC1 231EN03 2314044 http://fhwm.net/g/1132 http://fhwm.net/c/1133";

my $args = {
    api_key     => $apikey,
    api_secret  => $secret,
    from        => '14439556711',
    to          => '14123986404',
    text        => $text,
    #   'status-report-req'   => 1
};

my $ua = LWP::UserAgent->new;
my $response = $ua->request( POST $uri, $args );
my $result = from_json( $response->content );

print "Raw:\n";
print Dumper( $response->content );
print "\nJSON:\n";
print Dumper($result);

