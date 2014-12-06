package Watchman::GTalk;
use strict;
use Net::XMPP;

use constant DEBUG => 1;

my $username = "jeff.bruns";
my $password = "5130_ozte";

my $to = "28rick";
my $body = "send me a text if you get this. this is an auto im test from prealert software.";

my $resource = "PerlBot";

# Google Talk & Jabber parameters
my $hostname = 'talk.google.com';
my $port = 5222;
my $componentname = 'gmail.com';
my $connectiontype = 'tcpip';
my $tls = 1;

my $Connection = new Net::XMPP::Client();

# Connect to talk.google.com
my $status = $Connection->Connect(
   hostname 		=> $hostname,
   port 			=> $port,
   componentname 	=> $componentname,
   connectiontype 	=> $connectiontype,
   tls 				=> $tls
);

if ( ! ( defined($status) ) ) {
   print "ERROR:  XMPP connection failed: $!\n";
   exit(0);
}

# Change hostname
my $sid = $Connection->{SESSION}->{id};
$Connection->{STREAM}->{SIDS}->{$sid}->{hostname} = $componentname;

# Authenticate
my @result = $Connection->AuthSend(
	username => $username,
	password => $password,
	resource => $resource
);

if ( $result[0] ne "ok" ) {
   print "ERROR: Authorization failed: $result[0] - $result[1]\n";
   exit(0);
}

# Send message
$Connection->MessageSend(
	to 			=> "$to\@$componentname",
	body 		=> $body,
	resource 	=> $resource
);

sub write_log
{
    my $self = shift;
    my $msg = shift;
    my $err = shift;

    my $pid = $$;
    $msg =~ s/\n$/ ($pid)\n/;
    my $time = localtime();

    print LOGFILE "[$time] " . ( $err ? "**" : "" ) . $msg;
    print ( $err ? "**" : "" ) . $msg if DEBUG;
}
1;