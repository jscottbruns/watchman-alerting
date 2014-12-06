#!/usr/bin/perl
use strict;
use DBIx::Connector;
use Exception::Class::DBI;
use Time::HiRes;

BEGIN {
	push @INC, '/home/jsbruns/workspace/WatchmanAlerting/lib';

	use vars qw( $PATH $TEMP_DIR $DEBUG $log $DBH $self $LOG_DIR );

	use constant DAEMON		=> 'watchman-alerting';
	use constant ROOT_DIR	=> '/usr/local/watchman-alerting';
	use constant LOG_DIR	=> '/var/log/watchman-alerting';
	use constant LOG_FILE	=> 'watchman-alerting.log';
	use constant PID_FILE	=> '/var/run/watchman-alerting.pid';
	use constant TEMP_DIR	=> '/tmp';
	use constant INI_CONFIG	=> '/etc/watchman.ini';
	
	use vars qw( $PATH $TEMP_DIR $DEBUG $DB_FILE $log $DBH $self $LOG_DIR );

	use constant E_ERROR	=> 'error';
	use constant E_WARN		=> 'warning';
	use constant E_CRIT		=> 'critical';
	use constant E_DEBUG	=> 'debug';
	use constant E_INFO		=> 'info';
}

use Watchman::NetAudio;

$PATH = ROOT_DIR;
$TEMP_DIR = TEMP_DIR;
$LOG_DIR = LOG_DIR;
$DEBUG = 1;

unless (
	$DBH = DBIx::Connector->new(
		'dbi:mysql:WatchmanAlerting',
        'root',
        'ava457',
        {
        	PrintError	=> 0,
        	RaiseError	=> 0,
        	HandleError	=> Exception::Class::DBI->handler,
    	    AutoCommit	=> 1,
		}
	)
) {
	die "Database connection error: " . $DBI::errstr;
}


$self = {};
eval {
	$self = $DBH->run( sub {
		my $arr = $_->selectall_arrayref(
			q{ SELECT * FROM Settings },
			{ Slice => {} }
		);
		my $_self = {};
		foreach my $_i ( @$arr )
		{
		    $_self->{ $_i->{Category} }->{ $_i->{Name} } = $_i->{Setting};
		}
		return $_self;
	} )
};

if ( my $ex = $@ ) 
{
	die "Error ", $ex->error, "\n";
	exit;
}

my $__audio = [];
push @{ $__audio }, 'audio/Informer_beep.mp3';
push @{ $__audio }, 'audio/tts/rich/units/enginecompany.mp3';
push @{ $__audio }, 'audio/tts/rich/callType/autofire.mp3';
Watchman::NetAudio->new(
	IncType		=> 'assigned', #'active',
	AudioSrc	=> $__audio
);

sub log2
{
	my @params = @_;
	foreach ( @params )
	{	
		print "Param => $_ \n";
	}
}

sub log
{
    my $msg = shift;
    my $level = shift;
    my ($package, $file, $line) = caller;

    $msg .= " ($$)";
    $msg = "[$package:$line] $msg ($$)";
	print STDOUT "$msg\n";
}
