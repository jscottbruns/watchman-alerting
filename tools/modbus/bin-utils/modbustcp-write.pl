#!/usr/bin/env perl
#
# Modbus/TCP Server query
# Issues a write coil request on a Modbus server
#
# Cosimo  Feb 5st, 2007
#
# $Id: write_coil.pl,v 1.3 2007/02/16 15:02:26 cosimo Exp $

use strict;
use warnings;
use lib '../blib/lib';
use Getopt::Long;
use Protocol::Modbus;

$ARGV[0] ||= '';

GetOptions(
    'ip:s'      => \my $ip,
    'port:s'    => \my $port,
    'timeout:i' => \my $timeout,
    'address:s' => \my $address,
    'value:s'   => \my $value,
) or die "Wrong options!";

$ip ||= '127.0.0.1';
$port ||= '502';
$timeout ||= 3;
$value ||= 0;

die "Missing required address parameters\n" unless $address;

my $modbus = Protocol::Modbus->new(driver=>'TCP', transport=>'TCP');

# Create transport class
my $trs = Protocol::Modbus::Transport->new(
    driver  => 'TCP',
    address => $ip,
    port    => $port,
    timeout => $timeout,
);

# with explicit method name
my $req = $modbus->writeCoilRequest(
    address  => $address,
    value    => $value
);
# Open a new modbus transaction...
my $trn = $modbus->transaction($trs, $req);

# ... issue the request and get response
my $res = $trn->execute();

print 'Response: ', $res, "\n";

$trs->disconnect();

