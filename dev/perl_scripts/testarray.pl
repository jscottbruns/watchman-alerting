#!perl

BEGIN {
	push @INC, 'lib/';

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

print "Y";