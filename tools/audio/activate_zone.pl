#!/usr/bin/perl
use strict;
use Getopt::Long;
use File::Basename;

my ($host, $verbose, $timeout, $delay, $sleeptype);
my $port = '4446';
my @audio;
my $gstbin = '/usr/bin/gst-launch-0.10';

GetOptions (
    "host|h=s"      => \$host,
    "port|p=i"      => \$port,
    "v:i"           => \$verbose,
    "audio|a=s{}"   => \@audio,
    "delay|d:s"     => \$delay,
    "gstbin|g=s"    => \$gstbin,
    "timeout|t:i"   => \$timeout
);

unless ( $host && $port && @audio && $gstbin )
{
    print "WatchmanAlerting Alert Zone Activation Utility\n";
    print "Usage: activate_zone -h host -p port -a path_to_audio [-a path_to_audio] [-v 1] [-d interval_delay] [-g path_to_gstbin] [-t timeout]\n";
}

die "GST application [$gstbin] doesn't exist or not executable\n" unless -f $gstbin && -X $gstbin;

my ($cmd, $out, $LOG);

if ( $delay && $delay =~ /^([0-9]*)(s|ms)$/ )
{
    $delay = $1;
    $sleeptype = $2;
}

$delay = undef if ( $delay && $sleeptype eq 's' && $delay > 5 );

open($LOG, ">>/var/log/watchman-alerting/activate_zone.log");

for ( @audio )
{
    my $filepath = $_;
    my ($filetype, $cmd);

    $filepath =~ /\.([a-zA-Z0-9]{3})$/ and $filetype = $1;

    if ( -f $filepath && ( lc($filetype) eq 'mp3' || lc($filetype) eq 'wav' ) )
    {
        $cmd = "$gstbin" . ( $verbose ? ' -v' : undef ) . " filesrc location=$filepath ";

        if ( $filetype eq 'wav' )
        {
            $cmd .= "! wavparse ! audioconvert ! audioresample ! mulawenc ! rtppcmupay ! udpsink ";
        }
        elsif ( $filetype eq 'mp3' )
        {
            $cmd .= "! mad ! audioconvert ! audioresample ! mulawenc ! rtppcmupay ! udpsink ";
        }

        $cmd .= "host=$host auto-multicast=true port=$port";

        $out = `$cmd 2>&1`;

		if ( $LOG )
		{
			print $LOG strftime("%Y-%m-%d %H:%M:%S", localtime()) . " Issuing audio command [$cmd] ($?) $out\n"; 	
		}

        if ( $? == 0 )
        {
            sleep $delay if ( $delay && $sleeptype eq 's' );           
            if ( $delay && $sleeptype eq 'ms' )
            {
            	$delay *= 0.001;
            	select(undef, undef, undef, $delay);	
            }
        }
    }
    else
    {
    	if ( $LOG )
    	{
	        print $LOG "Error [$filepath] Cannot stat audio file\n" unless -f $filepath;
	        print $LOG "Error [$filepath] Invalid file type\n" unless ( lc($filetype) eq 'mp3' || lc($filetype) eq 'wav' );    		
    	}
        print STDERR "Error [$filepath] Cannot stat audio file\n" unless -f $filepath;
        print STDERR "Error [$filepath] Invalid file type\n" unless ( lc($filetype) eq 'mp3' || lc($filetype) eq 'wav' );
    }
}

close $LOG if $LOG;