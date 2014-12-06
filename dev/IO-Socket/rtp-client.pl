#!/usr/bin/perl
use Net::RTP;

print "Openining RTP Stream...\n";

my $rtp = new Net::RTP(
	LocalPort	=>	4444,
	LocalAddr	=>	'localhost',
);

die "Unable to open RTP client stream $!\n" if ( ! defined $rtp );
  
print "Listening for incoming RTP packets";
  
$rtp->autoflush(1);

print "Receiving RTP packets\n";

my ($buf, $packet, $inc, $mp3);

open MP3, ">stream.mp3";

while ( $packet = $rtp->recv() ) {

	print "Packet " . ++$inc . " : \n";
	#print "Payload size: ".$packet->payload_size()." Curr Seq: " . $packet->seq_num() . "\n";
	
	print MP3 $packet->payload();
}

close MP3;
 