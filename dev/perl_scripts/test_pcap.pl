#!perl
use strict;

use DBI;

use Net::Pcap;
use NetPacket::Ethernet qw( :strip );
use NetPacket::IP qw( IP_PROTO_TCP );
use NetPacket::TCP qw( FIN SYN ACK PSH URG RST );

my $PATH = $ENV{'WATCHMAN_HOME'};

my $self = {};

$self->{'seqnum'} = -1;

my $dbh = DBI->connect(
	"dbi:SQLite:dbname=$PATH\\watchman.s3db",
	"",
	"",
	{
	   ShowErrorStatement => 1,
	   PrintError => 1
	}
);
unless ( $dbh ) {

	&main::write_log( "Error connecting to database: " . $DBI::errstr, 1 );
	exit;
}

my %row;
my $sth = $dbh->prepare("SELECT *
						 FROM settings
						 WHERE category IN ('system', 'network')") or die "DBH Query Error: " . $dbh->errstr;
if ( $sth->execute ) {

	$sth->bind_columns( \( @row{ @{$sth->{NAME} } } ) );
	while ( $sth->fetch ) {
		$self->{ $row{'name'} } = $row{'setting'};
	}
}

my ($err, $network, $netmask, $pcap_t);

$self->{'captureFile'} = "$PATH\\$self->{captureFile}";

&main::write_log("Opening offline network capture stream on capture file: $self->{'captureFile'}\n");
if ( $self->{'captureFile'} && -f $self->{'captureFile'} ) {

	$pcap_t = Net::Pcap::open_offline($self->{'captureFile'}, \$err);
	unless ( defined($pcap_t) ) {

		&main::write_log("Unable to inititate Net::Pcap::open_offline for offline packet capture: $err - Error NET104", 1);
		exit;
	}
} else {

	&main::write_log("Error. Cannot stat capture file $self->{captureFile} - Error NET105\n", 1);
	exit;
}

my $filter;
if ( $self->{'ip_addr'} ) {

	$filter = "host $self->{ip_addr} ";

	&main::write_log("Creating network filter: [ $filter ]\n");
	if ($filter) {
		my $filter_t;

		&main::write_log("Compiling filter\n");
		if ( Net::Pcap::compile($pcap_t, \$filter_t, $filter, 0, $netmask) == -1 ) {

			&main::write_log("Unable to compile packet capture filter: " . Net::Pcap::geterr( $pcap_t ) . ". Error NET107\n", 1);
			exit;
		}

		&main::write_log("Applying filter\n");
		if (Net::Pcap::setfilter($pcap_t, $filter_t) == -1) {

			&main::write_log("Unable to apply compiled filter on packet capture device. Erro NET108\n");
			exit;
		}
	}
}

&main::write_log("Starting network capture loop\n");
unless ( Net::Pcap::loop($pcap_t, -1, \&process_pkt, \%{ $self } ) ) {

	# &main::write_log("Error initiating network capture loop. Pcap reported: " . Net::Pcap::geterr( $pcap_t ) . "\n", 1);
	exit;
}

sub process_pkt
{
    my ($self, $header, $packet) = @_;

    my $ip = NetPacket::IP->decode( eth_strip($packet) );

    if ( $ip->{'proto'} == IP_PROTO_TCP && ( $ip->{'dest_ip'} eq $self->{'ip_addr'} || $ip->{'src_ip'} eq $self->{'ip_addr'} ) ) {

        my $tcp = NetPacket::TCP->decode( $ip->{'data'} );

		#print "$ip->{src_ip}:$tcp->{src_port} => $ip->{dest_ip}:$tcp->{dest_port} SEQ: $tcp->{seqnum} LEN: " . length( $tcp->{'data'} ) . " " . ( $tcp->{'flags'} & SYN ? "SYN, " : undef ) . ( $tcp->{'flags'} & ACK ? "ACK, " : undef ) . ( $tcp->{'flags'} & FIN ? "FIN, " : undef ) . ( $tcp->{'flags'} & PSH ? "PSH, " : undef ) . ( $tcp->{'flags'} & URG ? "URG, " : undef ) . ( $tcp->{'flags'} & RST ? "RST, " : undef ) . "\n";

		if ( $ip->{'dest_ip'} eq $self->{'ip_addr'} && ( $tcp->{'flags'} & SYN ) ) {

			$self->{'__packet__'} = {};
			$self->{'__checksum__'} = {};
			$self->{'__port__'} = $tcp->{'src_port'};
            $self->{'__checksum__'}->{ ( $tcp->{'seqnum'} + 1 ) } = 0;

			print "Incomming message from [$ip->{src_ip}:$tcp->{src_port}]=>[$ip->{dest_ip}:$tcp->{dest_port}]\n";
			print "      Setting Connection Port: $self->{__port__}\n";

		} elsif ( $tcp->{'src_port'} eq $self->{'__port__'} && $ip->{'dest_ip'} eq $self->{'ip_addr'} && ( $tcp->{'flags'} & ACK ) ) {

            if ( $tcp->{'data'} ) {

                $self->{'__packet__'}->{ $tcp->{'seqnum'} } = $tcp->{'data'};
                $self->{'__checksum__'}->{ ( $tcp->{'seqnum'} + length( $tcp->{'data'} ) ) } = 0 if ( ! ( $tcp->{'flags'} & FIN ) );
            }

            $self->{'__checksum__'}->{ $tcp->{'seqnum'} } = 1 if ( $tcp->{'data'} || ( $tcp->{'flags'} & FIN ) );

            print "  ACK data packet from [$ip->{src_ip}:$tcp->{src_port}]=>[$ip->{dest_ip}:$tcp->{dest_port}]\n";
            print "      SEQNUM: $tcp->{seqnum} FLAGS: " . ( $tcp->{'flags'} & SYN ? "SYN, " : undef ) . ( $tcp->{'flags'} & ACK ? "ACK, " : undef ) . ( $tcp->{'flags'} & FIN ? "FIN, " : undef ) . ( $tcp->{'flags'} & PSH ? "PSH, " : undef ) . ( $tcp->{'flags'} & URG ? "URG, " : undef ) . ( $tcp->{'flags'} & RST ? "RST, " : undef ) . "\n";

		}

		if ( ( ( $tcp->{'src_port'} eq $self->{'__port__'} && $ip->{'dest_ip'} eq $self->{'ip_addr'} ) || ( $tcp->{'dest_port'} eq $self->{'__port__'} && $ip->{'src_ip'} eq $self->{'ip_addr'} ) ) && ( $tcp->{'flags'} & FIN ) ) {

           $self->{'__checksum__'}->{'30266130'} = 0;

            # If the FIN flag has been set by the sender, validate the checksum for out of order packet transmission
            if ( $tcp->{'src_port'} eq $self->{'__port__'} && $ip->{'dest_ip'} eq $self->{'ip_addr'} ) {

                print "Finish flag set by TCP sender, validating packet checksum\n";

                $self->{'packet'} = '';
                my $checksum = 1;
                my $i = 0;

                for my $_seq ( sort keys %{ $self->{'__checksum__'} } ) {

                    if ( ! $self->{'__checksum__'}->{ $_seq } ) {
                        $checksum = 0;
                        last;
                    }

                    $self->{'packet'} .= $self->{'__packet__'}->{ $_seq };
                    $i++;
                }

                if ( $checksum == 1 ) {

                    print "Checksum is valid\n";

                	undef $self->{'__packet__'};
                	undef $self->{'__checksum__'};
                	$self->{'__port__'} = 0;

                    print "Reassembled $i packet containing incident data. Invoking alert class\n";

                    #print $self->{'packet'};
                }

                print "Checksum was not valid, posting packet reassembly\n";

            } elsif ( $tcp->{'dest_port'} eq $self->{'__port__'} && $ip->{'src_ip'} eq $self->{'ip_addr'} ) {

                print "Connection close acknowledgement from [$ip->{src_ip}:$tcp->{src_port}]=>[$ip->{dest_ip}:$tcp->{dest_port}]\n\n";

                $self->{'packet'} = '';
                my $i = 0;
	            for ( sort keys %{ $self->{'__packet__'} } ) {
	                $self->{'packet'} .= $self->{'__packet__'}->{ $_ };
	                $i++;
	            }

                undef $self->{'__packet__'};
                undef $self->{'__checksum__'};
                $self->{'__port__'} = 0;

                print "Reassembled $i packets\n";
            }
		}
	}
}








sub write_log
{
	my $msg = shift;

	print $msg;
}