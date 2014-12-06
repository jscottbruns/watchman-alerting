#!/usr/bin/env perl
#
# Modbus/TCP Server query
#
# Issues a read coils request on an IP address / port
# Here is demonstrated the full Modbus transaction API
#
# Cosimo  Feb 2st, 2007
#
# $Id: read_coils_full_api.pl,v 1.4 2007/02/16 10:05:22 cosimo Exp $

use strict;
use warnings;
use lib '../blib/lib';
use Getopt::Long;
use Protocol::Modbus;

$| = 1;
$ARGV[0] ||= '';

GetOptions(
    'ip:s'      => \my $ip,
    'port:s'    => \my $port,
    'address:s' => \my $address,
    'count:i'   => \my $count,
    'poll'      => \my $poll,
    'rate:f'    => \my $rate
) or die "Wrong options!";

$ip ||= '127.0.0.1';
$port ||= '502';
$address ||= 1;
$count ||= 1;
$rate ||= 1.0;

my $modbus = Protocol::Modbus->new(driver=>'TCP', transport=>'TCP');

# Create transport class
my $trs = Protocol::Modbus::Transport->new(
    driver  => 'TCP',
    address => $ip,
    port    => $port,
    timeout => 3,
);

# with explicit method name
my $req = $modbus->readCoilsRequest(
    address  => $address,
    quantity => $count
);

# Open a new modbus transaction...
my $trn = $modbus->transaction($trs, $req);
my $CONT = 1;

while($CONT)
{
    my $res = $trn->execute();

    my $coils = $res->coils();
    print join(',', @{ $coils } ) . "\n";

    $CONT = undef unless $poll;
    select(undef, undef, undef, $rate) if $poll;
}

