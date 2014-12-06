package Watchman::Printing;
use strict;

BEGIN
{
	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warning';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}

use IPC::Run qw( run start timeout );

sub new
{
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;
    my $self = {
    	'path'		=> $::PATH,
    	'debug'		=> $::DEBUG,
    	'temp_dir'	=> $::TEMP_DIR,
    	'dbh'		=> $::DBH,
    	'main'		=> $::self,
    	'license'	=> $::LICENSE,
    };

	$self->{'settings'} = $::self->{'printing'};

    &main::log("Starting fire station print service");

	$self->{'IncidentNo'}	= $params{'IncidentNo'};
	$self->{'StreamData'}	= $params{'StreamData'};
	$self->{'Printout'}		= $params{'Printout'};
	$self->{'PrintBin'}		= $self->{'settings'}->{'PrintBin'} || 'rlpr';

	return if ( ! $self->{'IncidentNo'} && ! $self->{'Printout'} && ! $self->{'settings'}->{'PrintPassThru'} );
	$self->{'settings'}->{'TotalPrintDevices'} = $self->{'settings'}->{'TotalPrintDevices'} || 1 if $self->{'settings'}->{'RemotePrintingEnabled'};
	$self->{'settings'}->{'PrintTimeout'} = $self->{'settings'}->{'PrintTimeout'} || 5;

	unless ( $self->{'settings'}->{'RemotePrintingEnabled'} )
	{
		&main::log("Remote printing is not enabled - Nothing to do at this time", E_ERROR) unless ( $self->{'settings'}->{'RemotePrintingEnabled'} );
		&main::log("No print devices have been set up! Please check system settings to ensure print devices are properly configured", E_ERROR) unless ( $self->{'settings'}->{'TotalPrintDevices'} );

		return undef;
	}

	if ( ! $self->{'IncidentNo'} || ( ( $self->{'settings'}->{'PrintPassThru'} && ! $self->{'StreamData'} ) || ( ! $self->{'settings'}->{'PrintPassThru'} && ! $self->{'Printout'} ) ) )
	{
		&main::log("Incident number not specified - Unable to continue with incident printing", E_ERROR) unless ( $self->{'IncidentNo'} );
		&main::log("Unable to continue with passthru printing of unmodified dispatch data. No stream dispatch data specified for printing", E_ERROR) if ( $self->{'settings'}->{'PrintPassThru'} && ! $self->{'StreamData'} );
		&main::log("Unable to continue with incident printing. No incident printout data specified", E_ERROR) if ( ! $self->{'settings'}->{'PrintPassThru'} && ! $self->{'Printout'} );

		return undef;
	}

	my $ipcerr;

	&main::log("Preparing incident [$self->{IncidentNo}] for printing to [$self->{settings}->{TotalPrintDevices}] remote print device(s)") if $self->{'IncidentNo'};
	&main::log("Preparing non-incident printjob for printing to [$self->{settings}->{TotalPrintDevices}] remote print device(s)") if ( ! $self->{'IncidentNo'} && $self->{'settings'}->{'PrintPassThru'} );

	for ( my $_i = 0; $_i < $self->{'settings'}->{'TotalPrintDevices'}; $_i++ )
	{
		 my $dev = $_i || 'Default';

		 my $hostaddr = $self->{'settings'}->{'Printer_' . $dev . 'HostAddr'} || $self->{'settings'}->{'Printer_' . $dev . 'Name'} . '@' . $self->{'settings'}->{'Printer_' . $dev . 'Addr'};
		 my $devname = $self->{'settings'}->{'Printer_' . $dev . 'Name'};
		 my $devaddr = $self->{'settings'}->{'Printer_' . $dev . 'Addr'};
		 my $devport = $self->{'settings'}->{'Printer_' . $dev . 'Port'};

		 &main::log("Preparing remote print job for device [$dev] --> [$hostaddr]");

		if ( $self->{'settings'}->{'PrintPassThru'} )
		{
			&main::log("Sending passthru stream data to device --> [$hostaddr]");
			run [ $self->{'PrintBin'}, "--printer=$hostaddr", '-r' ], '<', \$self->{'StreamData'}, '2>', \$ipcerr, timeout( $self->{'settings'}->{'PrintTimeout'} );
		}
		else
		{
			&main::log("Running print data through postscript conversion utility and sending to device --> [$hostaddr]");
			run [ 'enscript', '-B', '-p', '-', '--word-wrap' ], '<', \$self->{'Printout'}, '|', [ $self->{'PrintBin'}, "--quiet", "--printer=$hostaddr" ], '2>', \$ipcerr, timeout( $self->{'settings'}->{'PrintTimeout'} );
		}

		if ( $ipcerr )
		{
			chomp( $ipcerr );
			&main::log("Remote print job on device [$dev] reported errors: $ipcerr", E_ERROR);
		}
	}

	return;
}

1;