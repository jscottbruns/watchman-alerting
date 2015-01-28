#!perl
use strict;
use Net::Pcap;
use NetPacket::Ethernet qw( :strip );
use NetPacket::IP qw( IP_PROTO_TCP );
use NetPacket::TCP qw( FIN SYN ACK PSH URG RST );
use File::Temp qw/tempfile/;
use Data::Dumper;

$| = 1;

my $self = {
    ip_addr => '10.50.28.35',
    port    => '9100',
    iface => 'br0',
    seqnum => -1
};

my ($err, $network, $netmask, $pcap_t);

&main::write_log("Opening network stream on interface $self->{iface}");
$pcap_t = Net::Pcap::pcap_open_live($self->{'iface'}, 2048, 1, -1, \$err);
unless ( defined($pcap_t) )
{
    &main::write_log("Unable to inititate Net::Pcap::open_offline for offline packet capture: $err - Error NET104", 1);
    exit;
}

my $filter;
if ( $self->{'ip_addr'} )
{
    my $filter_opts = [];
	push @{ $filter_opts }, "host $self->{ip_addr}" if $self->{'ip_addr'};
    push @{ $filter_opts }, "port $self->{port}" if $self->{'port'};

    $filter = join ' and ', @{ $filter_opts };

	if ($filter)
    {
		my $filter_t;

        &main::write_log("Compiling network filter: [ $filter ]");

		if ( Net::Pcap::compile($pcap_t, \$filter_t, $filter, 0, $netmask) == -1 )
        {
			&main::write_log("Unable to compile packet capture filter: " . Net::Pcap::geterr( $pcap_t ) . ". Error NET107", 1);
			exit;
		}

		&main::write_log("Applying filter");
		if (Net::Pcap::setfilter($pcap_t, $filter_t) == -1)
        {
			&main::write_log("Unable to apply compiled filter on packet capture device. Erro NET108");
			exit;
		}
	}
}

&main::write_log("Starting network capture loop");
unless ( Net::Pcap::loop($pcap_t, -1, \&process_pkt, \%{ $self } ) )
{
	&main::write_log("Error initiating network capture loop. Pcap reported: " . Net::Pcap::geterr( $pcap_t ) . "", 1);
	exit;
}

sub process_pkt
{
    my ($self, $header, $packet) = @_;
    my $ip = NetPacket::IP->decode( eth_strip($packet) );

    if ( $ip->{'proto'} == IP_PROTO_TCP && ( $ip->{'dest_ip'} eq $self->{'ip_addr'} || $ip->{'src_ip'} eq $self->{'ip_addr'} ) )
    {
        my $tcp = NetPacket::TCP->decode( $ip->{'data'} );

		write_log("$ip->{src_ip}:$tcp->{src_port} => $ip->{dest_ip}:$tcp->{dest_port} SEQ: $tcp->{seqnum} LEN: " . length( $tcp->{'data'} ) . " " . ( $tcp->{'flags'} & SYN ? "SYN " : undef ) . ( $tcp->{'flags'} & ACK ? "ACK " : undef ) . ( $tcp->{'flags'} & FIN ? "FIN " : undef ) . ( $tcp->{'flags'} & PSH ? "PSH " : undef ) . ( $tcp->{'flags'} & URG ? "URG " : undef ) . ( $tcp->{'flags'} & RST ? "RST " : undef ));

		if ( $ip->{'dest_ip'} eq $self->{'ip_addr'} && ( $tcp->{'flags'} & SYN ) )
        {
			$self->{'__packet__'} = {};
			$self->{'__checksum__'} = {};
			$self->{'__port__'} = $tcp->{'src_port'};
            $self->{'__checksum__'}->{ ( $tcp->{'seqnum'} + 1 ) } = 0;

			#write_log( "\t**New incomming message**");

		}
        elsif ( $tcp->{'src_port'} eq $self->{'__port__'} && $ip->{'dest_ip'} eq $self->{'ip_addr'} && ( $tcp->{'flags'} & ACK ) )
        {
            if ( length($tcp->{'data'}) > 0 )
            {
                $self->{'__packet__'}->{ $tcp->{'seqnum'} } = $tcp->{'data'};
                $self->{'__checksum__'}->{ ( $tcp->{'seqnum'} + length( $tcp->{'data'} ) ) } = 0 unless ( $tcp->{'flags'} & FIN );
            }

            $self->{'__checksum__'}->{ $tcp->{'seqnum'} } = 1 if ( $tcp->{'data'} || ( $tcp->{'flags'} & FIN ) );
            #write_log("\tNext SEQNUM should be: " . ( $tcp->{'seqnum'} + length( $tcp->{'data'} ) ));
		}

		if ( ( $tcp->{'src_port'} eq $self->{'__port__'} && $ip->{'dest_ip'} eq $self->{'ip_addr'} ) && ( $tcp->{'flags'} & FIN ) )
        {
            # If the FIN flag has been set by the sender, validate the checksum for out of order packet transmission
            if ( $tcp->{'src_port'} eq $self->{'__port__'} && $ip->{'dest_ip'} eq $self->{'ip_addr'} )
            {
                write_log( "\tFinish flag set by sender, validating packet checksum");

                $self->{'packet'} = '';
                my $checksum = 1;
                my $i = 0;

                foreach my $_seq ( sort { $a <=> $b } keys %{ $self->{'__checksum__'} } )
                {
                    if ( ! $self->{'__checksum__'}->{ $_seq } )
                    {
                        write_log( "\t\t**Checksum failed on seqnum $_seq");
                        $checksum = 0;
                    }

                    $self->{'packet'} .= $self->{'__packet__'}->{ $_seq };
                    $i++;
                }

                if ( $checksum )
                {
                    write_log( sprintf("\tChecksum is %s", $checksum == 1 ? 'valid' : 'not valid'));

                	undef $self->{'__packet__'};
                	undef $self->{'__checksum__'};
                	$self->{'__port__'} = 0;

    				my($fh, $filename) = tempfile(DIR => './capture-data');
    				print $fh $self->{'packet'};
    				close $fh;

    				write_log( "\tReassembled $i packets\n\t\tWriting raw packet payload to file: $filename");

    				my $txtfile = $filename . ".txt";
    				`pcl6 -dNOPAUSE -sOutputFile=$txtfile -sDEVICE=txtwrite $filename`;

    				write_log( "\tWriting decoded payload to file: $txtfile");

                    my $rcpt = 'jscottbruns@gmail.com';
                    `echo "New incident received" | mutt -s "New WLHVFD Incident" -a $txtfile -- $rcpt`;
                }
                else
                {
                    write_log("Checksum is not valid, unable to parse packet data");
                }
            }
		}
	}
}

sub write_log
{
	my $msg = shift;
	printf("[%s] %s\n", scalar localtime, $msg);
}
