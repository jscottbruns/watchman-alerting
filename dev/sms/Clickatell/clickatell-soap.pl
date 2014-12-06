#!/usr/bin/perl
use SOAP::Lite +trace => 'debug';
use Data::Dumper;

my $sms = new SOAP::Lite(
    proxy   => "http://api.clickatell.com/soap/webservice.php",
    service => "http://api.clickatell.com/soap/webservice.php?wsdl"
);

my $response = $sms->call(
    auth => 
        SOAP::Data->name( 'user'        => 'jsbruns' ),
        SOAP::Data->name( 'password'    => 'aci123ava' ),
        SOAP::Data->name( 'api_id'      => '3406044' ),
);

if ( $response->result =~ /^OK: (.*)$/ )
{
    $sess_id = $1;
}

my $resp = $sms->call(
    sendmsg =>
        SOAP::Data->name( 'session_id'  => $sess_id ),
        SOAP::Data->name( 'to'          => '04432501272' ),
        SOAP::Data->name( 'text'        => 'TEST MESSAGE' )
    );
print "R: " . Dumper( $resp->result );
