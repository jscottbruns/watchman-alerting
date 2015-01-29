#!perl
use strict;
use Net::Pcap;
use NetPacket::Ethernet qw( :strip );
use NetPacket::IP qw( IP_PROTO_TCP );
use NetPacket::TCP qw( FIN SYN ACK PSH URG RST );
use File::Temp qw/tempfile/;
use Data::Dumper;
use XML::Generator;
use POSIX;
use URI::Escape;
use HTML::Entities;
use IO::Socket qw(:DEFAULT :crlf);

$| = 1;

my $DEBUG = 1;
my $self = {
    ip_addr         => '10.50.28.35',
    port            => '9100',
    iface           => 'br0',
    seqnum          => -1,
    test_file       => $ARGV[0],
    station_units   => ['E828', 'E828B', 'TK828', 'MP828']
};

my ($err, $network, $netmask, $pcap_t);

if ( $self->{test_file} )
{
    die "Error opening local file $self->{test_file}" unless -f $self->{test_file};
    &main::write_log("Opening network stream on local input file $self->{test_file}");
    $pcap_t = Net::Pcap::open_offline($self->{test_file}, \$err);
}
else
{
    &main::write_log("Opening network stream on interface $self->{iface}");
    $pcap_t = Net::Pcap::pcap_open_live($self->{'iface'}, 4096, 1, -1, \$err);
}

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
                    write_log("\tChecksum is valid, processing data payload");

                	undef $self->{'__packet__'};
                	undef $self->{'__checksum__'};
                	$self->{'__port__'} = 0;

    				my($fh, $filename) = tempfile(DIR => './capture-data');
    				print $fh $self->{'packet'};
    				close $fh;

                    &process_payload($filename);

    				write_log( "\tReassembled $i packets\n\t\tWriting raw packet payload to file: $filename");

    				my $txtfile = $filename . ".txt";
    				`pcl6 -dNOPAUSE -sOutputFile=$txtfile -sDEVICE=txtwrite $filename`;

    				write_log( "\tWriting decoded payload to file: $txtfile");
                }
                else
                {
                    write_log("Checksum is not valid, unable to parse packet data");
                }
            }
		}
	}
}

sub process_payload
{
    my $datafile = shift;
    my $payload = {};
    my $data;

    &write_log("Decoding PCL packet data");

    open(F, "cat $datafile | pcl6 -dNOPAUSE -sOutputFile=/dev/stdout -sDEVICE=txtwrite -|");
    $data .= $_ while (<F>);

    my $ns = {
        ns1 => [ ns1 => "http://niem.gov/niem/structures/2.0" ],
        ns2 => [ ns2 => "http://niem.gov/niem/domains/emergencyManagement/2.0" ],
        ns3 => [ ns3 => "http://niem.gov/niem/niem-core/2.0" ],
        ns4 => [ ns4 => "http://fhwm.net/xsd/ICadDispatch" ],
        ns6 => [ ns6 => "http://niem.gov/niem/domains/jxdm/4.0" ],
        ns7 => [ ns7 => "http://niem.gov/niem/ansi-nist/2.0" ]
    };

    ($payload->{'EventNo'}, $payload->{'EntryDateTime'}) = $data =~ /Call #([F0-9]*) has been assigned\s+As of\s(.*)/;
    ($payload->{'CallType'}, $payload->{'CallNature'}, $payload->{'Priority'}) = $data =~ /Type:([a-zA-Z0-9]*?)\s-\s(.*?)\s+Priority:([A-Z0-9]*)/;
    ($payload->{'LocationAddress'}) = $data =~ /Location:(.*)/;
    ($payload->{'LocationDescr'}) = $data =~ /LocDesc:(.*)/;
    ($payload->{'MapGrid'}) = $data =~ /Map:(.*)/;
    ($payload->{'Agency'}, $payload->{'RadioTac'}) = $data =~ /Agency:([A-Z]*)\s+Darea:(TG[A-Z0-9]*)/;
    ($payload->{'StationGrid'}, $payload->{'BoxArea'}) = $data =~ /Station:([0-9]*)\s+Box:([0-9]*)/;
    ($payload->{'IncidentNo'}) = $data =~ /TIBNUM\s+.*?Number:(PF[0-9]*)/;
    ($payload->{'GPSLongitude'}, $payload->{'GPSLatitude'}) = $data =~ /ALIGEO\s+.*?GeoLong:([0-9\.\-]*)\sGeoLat:([0-9\.\-]*)/;

    # Trim each of the hash values
    while ( my ($key, $value) = each %{ $payload } ) { $payload->{ $key } = &trim($value) }

    unless ( $payload->{'EventNo'} )
    {
        &write_log("Non-incident data received, no need to continue");
        return;
    }

    # Extract Xstreets if they exit
    if ( $payload->{'LocationAddress'} =~ /^(.*?) - btwn (.*?) and (.*)$/ )
    {
        $payload->{'CrossSt1'} = &trim($2);
        $payload->{'CrossSt2'} = &trim($3);
        $payload->{'LocationAddress'} = &trim($1);
    }

    my @datetime = $payload->{'EntryDateTime'} =~ /^([0-9]{2})\/([0-9]{2})\/([0-9]{4})\s([0-9]{2}:[0-9]{2}:[0-9]{2})$/;
    $payload->{'EntryDate'} = sprintf("%s-%s-%s", $datetime[2], $datetime[0], $datetime[1]);
    $payload->{'EntryTime'} = $datetime[3];

    my ($dispatch_time, @units) = &parse_units($data, $payload);

    my $xml = XML::Generator->new;
    my $unitxml;
    foreach my $_unit ( @units )
    {
        $unitxml .= $xml->ServiceCallAssignedUnit(
            $ns->{ns4},
            $xml->OrganizationIdentification(
                $ns->{ns3},
                $xml->IdentificationID(
                    $ns->{ns3},
                    encode_entities( $_unit )
                )
            )
        ) if grep $_ eq $_unit, @{ $self->{station_units} };
    }

    my ($created_time, $open_comment, @comments) = &parse_comments($data, $payload);

    if ( $DEBUG )
    {
        print "------------ START DEBUG -----------\n";
        print Dumper($payload);
        print Dumper(@units);
        print Dumper(@comments);
        print "------------ END DEBUG -----------\n";
    }

    my $inc_xml;
    foreach my $_note ( @comments )
    {
        $inc_xml .= $xml->Comment(
            $ns->{ns4},
            $xml->CommentText(
                $ns->{ns3},
                encode_entities( $_note->{CommentText} ),
            ),
            $xml->CommentDateTime(
                $ns->{ns4},
                $xml->DateTime(
                    $ns->{ns3},
                    $_note->{CommentDateTime}
                )
            ),
            $xml->ServiceCallOperator(
                $ns->{ns6},
                $xml->PersonName(
                    $ns->{ns3},
                    $xml->PersonNamePrefixText(
                        $ns->{ns3},
                        encode_entities( $_note->{Operator} )
                    ),
                    $xml->PersonFullName(
                        $ns->{ns3},
                        encode_entities( $_note->{Operator} )
                    )
                )
            ),
            $xml->OrganizationIdentification(
                $ns->{ns3},
                $xml->IdentificationID(
                    $ns->{ns3},
                    $_note->{Sequence}
                )
            ),
            $xml->SourceIDText(
                $ns->{ns3},
                $_note->{SourceIDText}
            )
        );
    }

    my $StatusXML = $xml->ActivityStatus(
        $ns->{ns3},
        $xml->StatusText(
            $ns->{ns3},
            'INITIATE'
        ),
        $xml->StatusDate(
            $ns->{ns3},
            $xml->DateTime(
                $ns->{ns3},
                $created_time
            )
        ),
    ) .
    $xml->ActivityStatus(
        $ns->{ns3},
        $xml->StatusText(
            $ns->{ns3},
            'ENTRY'
        ),
        $xml->StatusDate(
            $ns->{ns3},
            $xml->DateTime(
                $ns->{ns3},
                &ts_format($payload->{'EntryDate'}, $payload->{'EntryTime'})
            )
        ),
    );

    $StatusXML .= $xml->ActivityStatus(
        $ns->{ns3},
        $xml->StatusText(
            $ns->{ns3},
            'DISPATCH'
        ),
        $xml->StatusDate(
            $ns->{ns3},
            $xml->DateTime(
                $ns->{ns3},
                $dispatch_time
            )
        ),
    ) if $dispatch_time;



    my $xmlout = $xml->ICadDispatch(
        [
            fhwm    => "http://fhwm.net/xsd/1.2/ICadDispatch",
            @{ $ns->{ns1} },
            @{ $ns->{ns2} },
            @{ $ns->{ns3} },
            @{ $ns->{ns4} },
            @{ $ns->{ns6} },
            @{ $ns->{ns7} }
        ],
        $xml->Payload(
            $ns->{ns4},
            $xml->ServiceCall(
                $ns->{ns4},
                $xml->ActivityIdentification(
                    $ns->{ns3},
                    $xml->IdentificationID(
                        $ns->{ns3},
                        $payload->{EventNo}
                    )
                ),
                $xml->ActivityDescriptionText(
                    $ns->{ns3},
                    encode_entities( $payload->{CallNature} )
                ),
                $StatusXML,
                $xml->ActivityReasonText(
                    $ns->{ns3},
                    encode_entities( ( @units ? join(' ', @units ) : '' ) )
                ),
                $xml->ServiceCallDispatchedDate(
                    $ns->{ns6},
                    $xml->DateTime(
                        $ns->{ns3},
                        $dispatch_time
                    )
                ),
                $xml->ServiceCallAugmentation(
                    $ns->{ns4},
                    $xml->CurrentStatus(
                        $ns->{ns4},
                        $xml->StatusText(
                            $ns->{ns3},
                            'DISPATCHED'
                        )
                    ),
                    $xml->CallTypeText(
                        $ns->{ns4},
                        encode_entities( $payload->{CallType} )
                    ),
                    $xml->CallSubTypeText(
                        $ns->{ns4},
                        encode_entities( $open_comment )
                    ),
                    $xml->CallPriorityText(
                        $ns->{ns4},
                        $payload->{Priority}
                    ),
                    ( $inc_xml ? $inc_xml : undef ),
                    $xml->Staging(
                        $ns->{ns4},
                        $xml->ContactRadioChannelText(
                            $ns->{ns3},
                            $payload->{RadioTac}
                        )
                    ),
                    $xml->IncidentId(
                        $ns->{ns4},
                        $xml->IdentificationID(
                            $ns->{ns3},
                            $payload->{IncidentNo}
                        )
                    ),
                    $xml->LocalIncidentId(
                        $ns->{ns4},
                        $xml->IdentificationID(
                            $ns->{ns3},
                            $payload->{ReportNo}
                        )
                    )
                ),
                $xml->ServiceCallResponseLocation(
                    $ns->{ns4},
                    $xml->LocationAddress(
                        $ns->{ns3},
                        $xml->StructuredAddress(
                            $ns->{ns3},
                            $xml->LocationStreet(
                                $ns->{ns3},
                                $xml->StreetName(
                                    $ns->{ns3},
                                    encode_entities( $payload->{LocationAddress} )
                                ),
                                $xml->StreetName(
                                    $ns->{ns3},
                                    encode_entities( $payload->{LocationDescr} )
                                ),
                                ( $payload->{LocationNote} ?
                                    $xml->StreetName(
                                        $ns->{ns3},
                                        encode_entities( $payload->{LocationNote} )
                                    ) : undef
                                ),
                            )
                        )
                    ),
                    $xml->ServiceCallResponseLocationAugmentation(
                        $ns->{ns4},
                        $xml->Firebox(
                            $ns->{ns4},
                            $payload->{BoxArea}
                        ),
                        $xml->MapGrid(
                            $ns->{ns4},
                            $payload->{MapGrid}
                        ),
                        $xml->StationGrid(
                            $ns->{ns4},
                            $payload->{StationGrid}
                        ),
                        $xml->GPSLatitudeDecimal(
                            $ns->{ns4},
                            ( $payload->{GPSLatitude} ? $payload->{GPSLatitude} : 0 )
                        ),
                        $xml->GPSLongitudeDecimal(
                            $ns->{ns4},
                            ( $payload->{GPSLongitude} ? $payload->{GPSLongitude} : 0 )
                        ),
                    ),
                    $xml->ServiceCallResponseLocationAugmentation(
                        $ns->{ns4},
                        $xml->LocationCrossStreet(
                            $ns->{ns4},
                            $xml->StreetName(
                                $ns->{ns3},
                                ( $payload->{CrossSt1} ? encode_entities( $payload->{CrossSt1} ) : undef )
                            ),
                            $xml->StreetName(
                                $ns->{ns3},
                                ( $payload->{CrossSt2} ? encode_entities( $payload->{CrossSt2} ) : undef )
                            )
                        )
                    )
                )
            ),
            $unitxml
        ),
        $xml->ExchangeMetadata(
            $ns->{ns4},
            $xml->DataSubmitterMetadata(
                $ns->{ns4},
                $xml->OrganizationIdentification(
                    $ns->{ns3},
                    $xml->IdentificationID(
                        $ns->{ns3},
                        'ICAD-DISPATCH'
                    )
                ),
                $xml->OrganizationName(
                    $ns->{ns3},
                    'Prince George\'s County, MD'
                )
            )
        ),
        $xml->ExchangeMetadata(
            $ns->{ns4},
            $xml->TransactionMetadata(
                $ns->{ns4},
                $xml->MetadataAugmentation(
                    $ns->{ns4},
                    $xml->SubmissionDateTime(
                        $ns->{ns4},
                        $xml->DateTime(
                            $ns->{ns3},
                            scalar localtime
                        )
                    ),
                    $xml->MessageSequenceNumber(
                        $ns->{ns4},
                        $xml->IdentificationID(
                            $ns->{ns3},
                            1234
                        )
                    )
                )
            )
        )
    );

    if ( $DEBUG )
    {
        my ($fh, $filename) = tempfile(
            'iCadDispatch_XXXXXX',
            SUFFIX      => '.xml',
            DIR         => 'tmp/'
        );
        &write_log("Dumping NIEM iCAD dispatch stream to temporary file => $filename") if $DEBUG;

        if ( $fh )
        {
            print $fh $xmlout;
            close $fh;
        }
    }

    &write_log("Opening remote INET socket to host 127.0.0.1:8888");

    my ($sock, $confirm);
    unless (
        $sock = new IO::Socket::INET(
            PeerHost    => '127.0.0.1',
            PeerPort    => 8888,
            Proto       => 'tcp',
            Type        => SOCK_STREAM,
            Timeout     => 30
        )
    )
    {
        &write_log("Failed to open INET socket to remote host 127.0.0.1:8888 $@ ");
        return undef;
    }

    $sock->autoflush(1);

    &write_log("Connected to remote host " . $sock->peerhost . ":" . $sock->peerport . " - Preparing NIEM/iCAD data stream");

    print $sock $xmlout,CRLF,CRLF;

    &write_log("Waiting for confirmation message...");

    {
        local $/ = CRLF;
        chomp( $confirm = <$sock> );
    }

    close($sock);

    &write_log("iCAD data transfer complete with confirmation => [$confirm] - Updating iCAD alert transaction status");

    if ( $DEBUG )
    {
        my $txtmsg = sprintf("echo \"%s %s\" | mutt -s \"[%s] %s\" -- ", $payload->{'LocationAddress'}, ( @units ? join(' ', @units ) : '' ), $payload->{'StationGrid'}, $payload->{'CallType'});
        `$txtmsg 4432501272\@vtext.com`;
        `$txtmsg jscottbruns\@gmail.com`;

        &write_log("Dispatching incident text message -> $txtmsg");
    }

    return $confirm;
}

sub parse_units
{
    my ($data, $payload) = @_;
    my (@units, $dispatch_time);

    while ( $data =~ /([0-9]{2}:[0-9]{2}:[0-9]{2})\s+(DISP|BACKUP|PRMPT)\s+\([A-Z0-9\/]*\)\s([A-Z0-9]*)\s/gs )
    {
        if ( $2 eq 'PRMPT' )
        {
            @units = grep { $_ ne $3 } @units;
        }
        else
        {
            push @units, $3;
            $dispatch_time = &ts_format($payload->{'EntryDate'}, &trim($1)) unless $dispatch_time;
        }
    }

    ($dispatch_time, @units);
}

sub parse_comments
{
    my ($data, $payload) = @_;
    my (@comments, $comment, $type, $time, $oper, $i, $created_time, $open_comment);

    $data =~ s/(\s{2}[0-9]{2}:[0-9]{2}:[0-9]{2}\s+[\-A-Z]*\s+.*?)/[**BREAK**]\r\n\1/mg;
    while ( $data =~ /\s{2}([0-9]{2}:[0-9]{2}:[0-9]{2})\s+([\-A-Z]*)\s+(.*?)\[\*\*BREAK\*\*\]/gsm )
    {
        $time = &trim($1);
        $type = &trim($2);
        $comment = &trim($3);
        $oper = undef;

        if ( $comment =~ /^\((.*?)\)\s?/ )
        {
            $oper = $1;
            $comment =~ s/^(\(.*?\)\s?)//;
        }

        $comment =~ s/\r\n/ /g;
        $comment =~ s/\s+/ /g;

        push @comments, {
            CommentText     => &trim($comment),
            CommentDateTime => &ts_format($payload->{'CreationDate'}, $time),
            SourceIDText    => $type,
            Operator        => $oper,
            Sequence        => $i++
        };

        $created_time = &ts_format($payload->{'EntryDate'}, &trim($time)) unless $created_time;
        ($open_comment) = $comment =~ /Text: (.*)/ unless $open_comment;
    }

    ($created_time, $open_comment, @comments);
}

sub ts_format
{
    my ($date, $time) = @_;
    sprintf("%s %s", $date, $time);
}

sub trim($)
{
    my $string = shift;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    return $string;
}

sub write_log
{
	my $msg = shift;
	printf("[%s] %s\n", scalar localtime, $msg);
}
