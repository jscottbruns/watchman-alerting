#!/usr/bin/perl -l

use strict;
use warnings;
use Protocol::Modbus;

$|=1;

my $modbus = Protocol::Modbus->new(
    driver    => 'TCP',
    transport => 'TCP',
);

my $trs = Protocol::Modbus::Transport->new(
    driver  => 'TCP',
    address => '10.10.0.50',
    port    => 502,
    timeout => 3,
);

my $req = $modbus->request(
    function => 1,
    address  => 1,
    quantity => 4,
    value    => 0x0000,
);
print "=" x 60;
print "Response: ", $req;
print "=" x 60;
$trs->disconnect;
