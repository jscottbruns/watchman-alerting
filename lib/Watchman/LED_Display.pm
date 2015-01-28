package Watchman::LED_Display;
use Data::Dumper;
BEGIN
{
	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warning';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}

use File::Temp qw/ tempfile /;
use IO::Socket qw(:DEFAULT :crlf);
use File::Spec;
use File::Basename;
use IO::Termios;

sub new
{
    my $this = shift;
    my %params = @_;

	&main::log("Starting LED serial display service");

    my $class = ref($this) || $this;
    my $self = {
    	path		=> $::PATH,
    	debug		=> $::DEBUG,
    	temp_dir	=> $::TEMP_DIR,
    	dbh			=> $::DBH,
    	main		=> $::self,
    	log_dir		=> $::LOG_DIR
    };

    $self->{'settings'} = $::self->{'serial'};

	my $stty_init	= $params{'sttyinit'};
	my $xml_file 	= $params{'xml_file'};
	my $xml_msg		= $params{'xml_msg'};
	my $xsltbin		= $params{'xsltbin'} || 'xsltproc';
    my $serial_port	= $params{'port'} || $self->{'settings'}->{'LED_DefaultTTY_Port'};
    my $xsl_file	= $params{'xsl_file'} || $self->{'settings'}->{'LED_DefaultXSL'};
    my $wait_pid	= $params{'wait_pid'};
	my $protocol	= $params{'protocol'} || $self->{'settings'}->{'LED_DefaultProtocol'};
	my $ncparams 	= $self->{'settings'}->{'Netcat_Params'};
	my $led_addr	= $::self->{'ledalerting'}->{'LED_DefaultTextPage'};

	unless ( $xml_msg || ( $params{'xml_file'} && -f $params{'xml_file'} ) )
	{
		unless ( $params{'reset'} )
		{
			&main::log("Invalid or missing XML - Can't run preprocessor against invalid XML", E_CRIT);
			return undef;
		}
	}

	eval {
		$self->{'Displays'} = $self->{'dbh'}->run( sub {
			return $_->selectall_arrayref(
				qq{
					SELECT *
					FROM LED_Displays
				},
				{ Slice => {} }
			);
		} )
	};

	if ( my $ex = $@ )
	{
		&main::log("Database exception received while looking up NetAudio groups for audio delivery - Unable to proceed with audible alerting " . $ex->error, E_ERROR);
		return undef;
	}

	my ($tcp, $udp, $tty);

	foreach my $_i ( @{ $self->{Displays} } )
	{
		if ( $params{'reset'} )
		{
			&main::log("LED reset requested");

			$xml_msg = "<signboard>" . ( $xml_msg ? $xml_msg : '' );
			$xml_msg .= "<setDateTime><timeOfDay hour=\"@{[ POSIX::strftime('%H', localtime) ]}\" minute=\"@{[ POSIX::strftime('%M', localtime) ]}\" /><timeFormat format=\"$_i->{TimeFormat}\" /><calendarDate month=\"@{[ POSIX::strftime('%m', localtime) ]}\" day=\"@{[ POSIX::strftime('%d', localtime) ]}\" year=\"@{[ POSIX::strftime('%y', localtime) ]}\"/><dayOfWeek day=\"@{[ POSIX::strftime('%A', localtime) ]}\"/></setDateTime>" if $_i->{'DefaultMsg'} =~ /<(date|time)/;

			if ( $_i->{'DefaultMsg'} =~ /^<(time|date)/ )
			{
				$xml_msg .= "<text label=\"$led_addr\">$_i->{DefaultMsg}</text>";
			}
			else
			{
				$xml_msg .= $_i->{'DefaultMsg'};
			}

			$xml_msg .= "</signboard>";
		}

		if ( $_i->{'Protocol'} eq 'tcp' )
		{
			if ( ! ( fork ) )
			{
				my ($_addr, $_port) = split ':', $_i->{'Address'};
				my $xsl = File::Spec->catfile( $self->{'path'}, $_i->{'Stylesheet'} );

				&main::log("Preparing TCP/IP network display [$_i->{Address}]");

				unless ( $_addr && $_port )
				{
					&main::log("Error - missing TCP/IP port and/or address", E_CRIT);
					exit;
				}

				unless ( $_i->{'Stylesheet'} && -f $xsl )
				{
					&main::log("Error - cannot stat XSL stylesheet", E_CRIT);
					exit;
				}

				if ( $xml_msg )
				{
					unless ( open(XSLT, "| $xsltbin $xsl - | nc $ncparams $_addr $_port") )
					{
						&main::log("Error opening XSLT pipe command ($?) $@ $!", E_CRIT);
						exit;
					}

					print XSLT $xml_msg;
					close(XSLT);

					if ( $? )
					{
						&main::log("XSLT/Network error ($?) $@ $!", E_CRIT);
					}
				}
				elsif ( $params{'xml_file'} )
				{
					unless ( open(XSLT, "$xsltbin $xsl $xml_file | nc $ncparams $_addr $_port |") )
					{
						&main::log("Error opening XSLT pipe command ($?) $@ $!", E_CRIT);
						exit;
					}

					while ( <XSLT> )
					{
						&main::log("Network display processing reported: [$_]", E_ERROR);
					}

					close(XSLT);

					if ( $? )
					{
						&main::log("XSLT/Network error ($?) $@ $!", E_CRIT);
					}
				}

				&main::log("Finished processing TCP/IP network display [$_i->{Address}]");

				exit 0;
			}
		}
		elsif ( $_i->{'Protocol'} eq 'udp' )
		{
			if ( ! ( fork ) )
			{
				my ($_addr, $_port) = split ':', $_i->{'Address'};
				my $xsl = File::Spec->catfile( $self->{'path'}, $_i->{'Stylesheet'} );

				&main::log("Preparing UDP/IP network display [$_i->{Address}]");

				unless ( $_addr && $_port )
				{
					&main::log("Error - missing UDP/IP port and/or address", E_CRIT);
					exit;
				}

				unless ( $_i->{'Stylesheet'} && -f $xsl )
				{
					&main::log("Error - cannot stat XSL stylesheet", E_CRIT);
				}

				if ( $xml_msg )
				{
					unless ( open(XSLT, "| $xsltbin $xsl - | nc $ncparams $_addr $_port") )
					{
						&main::log("Error opening XSLT pipe command ($?) $@ $!", E_CRIT);
						exit;
					}

					print XSLT $xml_msg;
					close(XSLT);

					if ( $? )
					{
						&main::log("XSLT/Network error ($?) $@ $!", E_CRIT);
					}
				}
				elsif ( $params{'xml_file'} )
				{
					unless ( open(XSLT, "$xsltbin $xsl $xml_file | nc $ncparams $_addr $_port |") )
					{
						&main::log("Error opening XSLT pipe command ($?) $@ $!", E_CRIT);
						exit;
					}

					while ( <XSLT> )
					{
						&main::log("Network display processing reported: [$_]", E_ERROR);
					}

					close(XSLT);

					if ( $? )
					{
						&main::log("XSLT/Network error ($?) $@ $!", E_CRIT);
					}
				}

				&main::log("Finished processing UDP/IP network display [$_i->{Address}]");

				exit 0;
			}
		}
		elsif ( $_i->{'Protocol'} eq 'tty' )
		{
			if ( ! ( fork ) )
			{
				my ($_addr, $_config) = split ':', $_i->{'Address'};
				my $xsl = File::Spec->catfile( $self->{'path'}, $_i->{'Stylesheet'} );

				&main::log("Preparing TTY serial display [$_i->{Address}]");

				unless ( $_addr )
				{
					&main::log("Error - missing TTY serial device", E_CRIT);
					exit;
				}

				unless ( $_i->{'Stylesheet'} && -f $xsl )
				{
					&main::log("Error - cannot stat XSL stylesheet", E_CRIT);
					exit;
				}

				if ( $xml_msg )
				{
					unless ( open(XSLT, "| $xsltbin $xsl - > $_addr") )
					{
						&main::log("Error opening XSLT pipe command ($?) $@ $!", E_CRIT);
						exit;
					}

					print XSLT $xml_msg;
					close(XSLT);

					if ( $? )
					{
						&main::log("XSLT/Serial error ($?) $@ $!", E_CRIT);
					}
				}
				elsif ( $params{'xml_file'} )
				{
					unless ( open(XSLT, "$xsltbin $xsl $xml_file > $_addr |") )
					{
						&main::log("Error opening XSLT pipe command ($?) $@ $!", E_CRIT);
						exit;
					}

					while ( <XSLT> )
					{
						&main::log("Network display processing reported: [$_]", E_ERROR);
					}

					close(XSLT);

					if ( $? )
					{
						&main::log("XSLT/Serial error ($?) $@ $!", E_CRIT);
					}
				}

				#my $tty;
				#
				#eval
				#{
				#	$tty = IO::Termios->open( $_addr )
				#};
				#
				#if ( $@ )
				#{
				#	&main::log("Error initiating TTY IO $@", E_CRIT);
				#	exit;
				#}

				#my ($speed, $databits, $stopbits, $parity);

				#if ( $_config =~ /^([0-9]{4,}),([5-8]{1})([NEO]{1})([1-2]{1})$/ )
				#{
				#	$speed = $1;
				#	$databits = $2;
				#	$parity = $3;
				#	$stopbits = $4;
				#}

				#$tty->setbaud( $speed ) if $speed;
				#$tty->setcsize( $databits ) if $databits;
				#$tty->setparity( $parity ) if $parity && lc($parity) ne 'n';
				#$tty->setstop( $stopbits ) if $stopbits;

				#my $out = `$xsltbin $xsl $xml_file`;

				#unless ( $out )
				#{
				#	&main::log("Error rendering XML file - XSLT preprocessor failed to return output ($?)", E_CRIT);
				#	exit;
				#}

				#&main::log("Writing " . length( $out ) . " bytes to serial port") if $self->{'debug'};

				#$tty->print( $out );

				&main::log("Finished processing TTY serial display [$_i->{Address}]");

				exit 0;
			}
		}
	}

	return 1;
}

1;