#! /usr/bin/perl
use warnings;
use strict;

use Socket qw(:all);

$|++;

my $udp_port = 5050;

socket( UDPSOCK, PF_INET, SOCK_DGRAM, getprotobyname('udp') ) or die "
+socket: $!";

select( ( select(UDPSOCK), $|=1 )[0] ); # no suffering from buffering

setsockopt( UDPSOCK, SOL_SOCKET, SO_REUSEADDR, 1 )
    or die "setsockopt SO_REUSEADDR: $!";
#setsockopt( UDPSOCK, SOL_SOCKET, SO_BROADCAST, 1 )
#    or die "setsockopt SO_BROADCAST: $!";

# my $broadcastAddr = sockaddr_in( $udp_port, INADDR_BROADCAST );
my $broadcastAddr = sockaddr_in( $udp_port, INADDR_ANY );
bind( UDPSOCK, $broadcastAddr ) or die "bind failed: $!\n";

my $input;
while( my $addr = recv( UDPSOCK, $input, 4096, 0 ) ) {
    print "$addr => $input\n";
}