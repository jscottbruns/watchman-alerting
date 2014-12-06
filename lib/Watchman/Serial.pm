package Watchman::Serial;

use Win32::SerialPort;
use File::Temp qw/ tempfile /;
use File::Slurp;

sub new
{
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;
    my $self = {};

    $self->{'version'}          =   $::VERSION;
    $self->{'path'}             =   $::PATH;
    $self->{'debug'}            =   $::DEBUG;
    $self->{'temp_dir'}         =   $::TEMP_DIR;
    $self->{'errfile'}          =   "$self->{temp_dir}\\serial_err.log";
    $self->{'base_dir'}         =   "$self->{'path'}\\xml";
    $self->{'xslt_bin'}         =   "$self->{'path'}\\xml\\libxml\\xsltproc.exe";

    mkdir( $self->{'temp_dir'} ) if ! -d $self->{'temp_dir'};

    my $serial_port             =   $params{'port'} || 'COM1';
    my $baudrate                =   $params{'baudrate'} || '9600';
    my $parity                  =   $params{'parity'} || 'even';
    my $databits                =   $params{'databits'} || 7;
    my $stopbits                =   $params{'stopbits'} || 2;
    my $protocol				=	$params{'protocol'};

	my $xml_file				=	$params{'xml_file'};
    my $xml_msg                 =   $params{'xml_msg'};
    my $wait_pid                =   $params{'wait_pid'};
    my $xsl_file                =   "$self->{'base_dir'}\\alphasign.xsl";

    my ($serial_data, $serial_write);

    bless $self, $class;

    &main::write_log("Starting Watchman::Serial - Signboard communication service \n");

    if ( my $serial_data = $self->convert_msg (
        'xsl_file'  	=>  $xsl_file,
        'xml_file'  	=>  $xml_file,
        'xml_msg'   	=>  $xml_msg,
        'protocol'		=>	$protocol
    ) ) {

        unless ( $self->write_serial (
            'device'    =>  $serial_port,
            'baudrate'  =>  $baudrate,
            'parity'    =>  $parity,
            'databits'  =>  $databits,
            'stopbits'  =>  $stopbits,
            'message'   =>  $serial_data,
            'xsl_file'	=>	$xsl_file
        ) ) {

            &main::write_log("Message delivery was unsuccessful\n", 1);
            return undef;
        }

        &main::write_log("Message delivery was successful\n");
        return 1;
    }

    &main::write_log("XSLT message conversion failure. Unable to write to serial device. \n", 1);
    return undef;
}

sub convert_msg
{
    my $self = shift;
    my %params = @_;

    my $xsl_file = $params{'xsl_file'};
    my $xml_file = $params{'xml_file'};
    my $xml_msg = $params{'xml_msg'};
	my $protocol = $params{'protocol'};

    &main::write_log("Checking input arguments for XML [ message | file ]");
    if ( $xml_file ) {

        &main::write_log("XML file provided '$xml_file', checking if exists\n");
        if ( ! -f "$self->{'temp_dir'}\\$xml_file" ) {

            &main::write_log("Error: Cannot stat XML file '$self->{'temp_dir'}\\$xml_file'\n", 1);
            return undef;
        }

        $xml_file = "$self->{'temp_dir'}\\$xml_file";

    } elsif ( $xml_msg && ! $xml_file ) {

        &main::write_log("Raw XML provided, writing to output file '$self->{temp_dir}\\serial.xml'\n");

        if ( open(XML_FH, ">$self->{temp_dir}\\serial.xml") ) {

            print XML_FH $xml_msg;
            close XML_FH;

            $xml_file = "$self->{temp_dir}\\serial.xml";

        } else {

            &main::write_log("Error writing to logfile '$self->{'temp_dir'}\\serial.xml': " . $@, 1);
            return undef;
        }
    }

    if ( $xml_file && -f $xml_file ) {

		my (undef, $tmpfile) = tempfile(
			'serialXXXXXX',
			DIR		=>	$self->{'temp_dir'},
			SUFFIX	=>	".out",
			OPEN	=>	0
		);

        &main::write_log("Calling XSLT preprocessor for message transformation: [ `\"$self->{xslt_bin}\" \"$xsl_file\" \"$xml_file\" 2>&1 >\"$tmpfile\"` ]\n");

		`"$self->{xslt_bin}" "$xsl_file" "$xml_file" 2>&1 >"$tmpfile"`;

		if ( $? == 0 ) {

			my $outstr = read_file( $tmpfile );
			unlink($tmpfile);

			if ( $outstr ) {

				&main::write_log("XSLT transformation successful \n");
				&main::write_log("Message (3-byte): $outstr \n") if $self->{'debug'};

				if ( $protocol && $protocol eq '1-byte' ) {

					&main::write_log("Converting message into 1-byte protocol...\n");

					$outstr = convert_3to1($outstr);

					&main::write_log("Message convertion successful \n");
					&main::write_log("Message (1-byte): $outstr \n") if $self->{'debug'};

				} elsif ( $protocol && $protocol eq '2-byte' ) {

					&main::write_log("Converting message into 2-byte protocol...\n");

					$outstr = convert_3to2($outstr);

					&main::write_log("Message convertion successful \n");
					&main::write_log("Message (2-byte): $outstr \n") if $self->{'debug'};
				}

				if ( $self->{'debug'} ) {

					&main::write_log("Debug flag is set, writing raw message to tmp\\serial.out");

					if ( open(OUTPUT, ">$self->{'temp_dir'}\\serial.out") ) {

						print OUTPUT $outstr;
						close OUTPUT;
					}
				}

				return $outstr;
			}

			&main::write_log("Command failed with unexpected result " . ( defined $? ? "($?)" : undef ) . ( defined $! || defined $@ ? ": $! $@" : undef ) . "\n", 1);

			return undef;
		}

        &main::write_log("XSLT transformation failed ($?)" . ( defined $! || defined $@ ? ": $! $@" : undef ) . " \n", 1);

        unlink($tmpfile) if ( $tmpfile && -f $tmpfile );

        return undef;
    }

    &main::write_log("Cannot stat XML file [$xml_file] for XSLT preprocessing \n", 1);

    return undef;
}

sub convert_3to1
{
	my $text = shift;

	$text =~ s/_([0-9A-Fa-f]{2})/chr( oct("0x$1") )/eg;

	return "\0\0\0\0\0" . $text;
}

sub convert_3to2
{
	my $text = shift;

	$text =~ s/_([0-9A-Fa-f]{2})/"]" . chr(oct("0x$1") + oct("0x20"))/eg;

	return $text;
}

sub write_serial
{
    my $self = shift;
    my %params = @_;

    &main::write_log("Preparing to write to serial device on port $params{device} \n");

    my $ob;
    if ( $ob = Win32::SerialPort->new( $params{'device'} ) ) {

	    eval { $ob->baudrate( $params{'baudrate'} ) };
	    &main::write_log("Can't set baudrate: $@\n", 1) if ( $@ );

	    eval { $ob->parity( $params{'parity'} ) };
	    &main::write_log("Can't set parity: $@\n", 1) if ( $@ );

	    eval { $ob->databits( $params{'databits'} ) };
	    &main::write_log("Can't set databits: $@\n", 1) if ( $@ );

	    eval { $ob->stopbits( $params{'stopbits'} ) };
	    &main::write_log("Can't set stopbits: $@\n", 1) if ( $@ );

	    eval { $ob->write_settings };
	    &main::write_log("Error writing port settings: $@\n", 1) if ( $@ );

	    &main::write_log("Writing message to serial port\n");
	    eval { $ob->write( $params{'message'} ) };
	    if ( $@ ) {

	        &main::write_log("Error writing to serial port: $@\n", 1);
	        return undef;
	    }

	    return 1;
    }

    &main::write_log("Error writing to serial port\n", 1);
    return undef;
}
1;