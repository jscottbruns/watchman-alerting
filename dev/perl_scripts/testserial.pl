#!/usr/bin/perl
use strict;
use warnings;

$| = 1;
use Data::Dumper;

BEGIN
{
    push @INC, "/usr/local/watchman-alerting/lib";

    use constant DAEMON     => 'watchman-alerting';
    use constant ROOT_DIR   => '/usr/local/watchman-alerting';
    use constant LOG_DIR    => '/var/log/watchman-alerting';
    use constant DEBUG_DIR  => '/var/log/watchman-alerting';
    use constant LOG_FILE   => 'watchman-alerting.log';
    use constant TEMP_DIR   => '/tmp';
    use constant CONF_FILE  => '/etc/watchman.conf';

    use vars qw( $PATH $TEMP_DIR $DEBUG $DEBUG_DIR $PID_FILE $CONF_FILE $log $DBH $self $LOG_DIR $LICENSE $CONTINUE $CONTINUEPOLL %PIDS );

    use constant E_ERROR    => 'error';
    use constant E_WARN     => 'warn';
    use constant E_CRIT     => 'critical';
    use constant E_DEBUG    => 'debug';
    use constant E_INFO     => 'info';
}

$PATH = ROOT_DIR;
$TEMP_DIR = TEMP_DIR;
$LOG_DIR = LOG_DIR;
$CONF_FILE = CONF_FILE;
$DEBUG_DIR = DEBUG_DIR;

use Watchman::LED_Display;

use POSIX;
use File::Spec;
use File::Touch;
use File::Path;
use File::Temp qw( tempfile );
use File::Basename;
use Config::Tiny;
use DBIx::Connector;
use DBD::mysql;
use Exception::Class::DBI;
use XML::LibXML;
use Date::Calc qw( check_time check_date );
use Date::Parse;

$DBH = &init_db;

&log("Loading system settings");
$self = {};

eval {
    $self = $DBH->run( sub {
        my $arr = $_->selectall_arrayref(
            q{ SELECT * FROM Settings },
            { Slice => {} }
        );
        my $self = {};
        foreach my $_i ( @$arr )
        {
            $self->{ $_i->{Category} }->{ $_i->{Name} } = $_i->{Setting};
        }
        return $self;
    } )
};

if ( my $ex = $@ )
{
    &log("Database exception received when loading system settings, unable to proceed with WatchmanAlerting startup " . $ex->error, E_ERROR);
    die "Database exception received when loading system settings, unable to proceed with WatchmanAlerting startup " . $ex->error;
}

&init_ledboards;


sub init_ledboards
{
    unlink File::Spec->catfile( $PATH, 'default.xml') if -f File::Spec->catfile( $PATH, 'default.xml');

    &log("Initiating LED signboard displays");

    my $defaultInit;
    
    $self->{'ledalerting'}->{'LED_DefaultTextPage'} = 'A' unless $self->{'ledalerting'}->{'LED_DefaultTextPage'};
    my $defaultTextSize = $self->{'ledalerting'}->{'LED_DefaultTextSize'} || '768';

    &log("Configuring default signboard message w/address space [$self->{ledalerting}->{LED_DefaultTextPage}] [ $self->{ledalerting}->{LED_DefaultDisplay} ] ");

    $defaultInit = "<memoryConfig><textConfig label=\"$self->{ledalerting}->{LED_DefaultTextPage}\" size=\"$defaultTextSize\" /></memoryConfig>" unless $self->{'ledalerting'}->{'LED_DefaultTextPage'} eq 'A'; 

    my $stty_done;
    if ( $defaultInit )
    {
        &log("Setting LED signboard date/time configuration w/ serial port initialization");

        $stty_done = 1;
        unless (
            Watchman::LED_Display->new(
                'reset'     => 1,
                'xml_msg'   => $defaultInit
            )
        )
        {
            &log("Unable to set initial signboard date/time", E_ERROR);
            $stty_done = 0;
        }
    }

    &log("Initializing LED displays");

    unless (
        Watchman::LED_Display->new(
            'reset'     => 1,
            'xml_msg'   => $defaultInit
        )
    ) {
        &log("Unable to initialize serial port for default LED display message", E_ERROR);
        return undef;
    }

    &log("Signboard initiation complete");

    return 1;
}

sub init_db
{
    my $conn;
    unless (
        $conn = DBIx::Connector->new(
            'dbi:mysql:WatchmanAlerting',
            'watchman',
            '',
            {
                PrintError  => 0,
                RaiseError  => 0,
                HandleError => Exception::Class::DBI->handler,
                AutoCommit  => 1,
            }
        )
    ) {

        &log("Database connection error: " . $DBI::errstr, E_ERROR);
        die "Database connection error: " . $DBI::errstr;
    }

    &log("Setting default DBIx mode => 'fixup' ");

    unless ( $conn->mode('fixup') )
    {
        &log("Error received when attempting to set default mode on line " . __LINE__ . ": " . $DBI::errstr, E_ERROR);
        die "Unable to set default SQL mode on line " . __LINE__ . ". Fatal.";
    }

    return $conn;
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
