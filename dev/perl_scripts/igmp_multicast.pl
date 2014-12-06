#!/usr/bin/perl
use IO::Socket::Multicast;

# 224.1.0.0/16  ST Multicast Groups
# 229.1.1.1		Barix Default Multicast Group

my $s = IO::Socket::Multicast->new(
	LocalPort	=> 4446
) or die "Error: Initializing\n";

#$s->mcast_add('229.1.1.1') or die "Error adding group\n";
$s->mcast_add('229.1.1.1', 'eth0') or die "Error adding group to eth0\n";

$s->recv($data, 1024);

print "Received Data: " . length($data);
open(FH, ">/tmp/mcast_data");
print FH $data;
close FH;
