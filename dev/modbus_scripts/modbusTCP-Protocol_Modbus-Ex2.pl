#!/usr/bin/perl -l

use strict;
use warnings;
use Protocol::Modbus;

$|=1;

my $proto = Protocol::Modbus->new( driver => 'TCP',  transport => 'TCP
+' );

my $request1 = $proto->request(
    function => 1,
    address  => 1,
    quantity => 1,
);

my $request2 = $proto->request(
   function  => 2,
   address   => 1,
   quantity  => 1,
);

my $request3 = $proto->request(
   function  => 3,
   address   => 1,
   uantity   => 1,
);

my $request4 = $proto->request(
   function  => 4,
   address   => 1,
   quantity  => 1,
); 

print "=" x 60;
print "Using unpack: \n";
print unpack ('H*', $request1->pdu());
print unpack ('H*', $request2->pdu());
print unpack ('H*', $request3->pdu());
print unpack ('H*', $request4->pdu());
print "=" x 60;
print "Using Pretty-Print: \n";
print $request1;
print $request2;
print $request3;
print $request4;
print "=" x 60;
print "stringify: ";
print $request1->stringify();
print $request2->stringify();
print $request3->stringify();
print $request4->stringify();
print "=" x 60;
