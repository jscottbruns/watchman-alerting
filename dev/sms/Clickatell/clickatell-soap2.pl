#!/usr/bin/perl
use Net::SMS::Clickatell::SOAP;
use Data::Dumper;

my $sms = new Net::SMS::Clickatell::SOAP(
    proxy       => 'http://api.clickatell.com/soap/webservice.php',
    service     => 'http://api.clickatell.com/soap/webservice.php?wsdl',
    verbose     => 1
);

my $r = $sms->routecoverage( msisdn => '4432501272' );
print "R: $r\n";exit;

my $id = $sms->auth(
    user        => 'jsbruns',
    password    => 'aci123ava',
    api_id      => '3406044'
);

print "Auth: $id\n";exit;
print "Sess ID: " . $sms->sessionId();
