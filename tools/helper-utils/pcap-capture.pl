#!/usr/bin/perl
use strict;
use Getops::Long;
use Net::PCap;

my $interface;

my ($err, $network, $netmask);

if ( ! $interface )
{
	warn "No interface specified, looking up default interface\n";
	$interface = Net::Pcap::lookupdev( \$err );
            unless ( defined( $interface ) ) {

                &main::write_log("Unable to look up device. $err - Error Net101\n", 1);
                Win32::MsgBox("The Watchman was unable to lookup the network interface list. Please check your network connections and make sure all connections have been properly configured. Error Net101");
                return undef;
            }
        }

        &main::write_log("Using device: $interface\n");

        &main::write_log("Looking up network...\n");
        if ( Net::Pcap::lookupnet($interface, \$network, \$netmask, \$err) == -1 ) {

            &main::write_log("Unable to look up device information for $interface: $err - Error NET102", 1);
            Win32::MsgBox("The Watchman was unable to bind to the local network. Please check your system settings and make sure you have configured the network settings correctly. Error: NET102");
            return undef;
        }

        &main::write_log("Assigning Network Broadcast: $network\n");
        &main::write_log("Assigning Netmask: $netmask\n");

        &main::write_log("Opening live network capture stream\n");
        $pcap_t = Net::Pcap::open_live($interface, $self->{'mtu'}, $self->{'promisc'}, $self->{'to_ms'}, \$err);
        unless ( defined($pcap_t) ) {

            &main::write_log("Unable to open device for packet capture: $err - Error NET103", 1);
            Win32::MsgBox("The Watchman was unable to lookup the specified network interface. Please make sure you have assigned the correct network interface from your computer's interface list. Error NET103");
            return undef;
        }
    } else {

        &main::write_log("Opening offline network capture stream on capture file: $self->{'captureFile'}\n");
        if ( $self->{'captureFile'} && -f $self->{'captureFile'} ) {

            $pcap_t = Net::Pcap::open_offline($self->{'captureFile'}, \$err);
            unless ( defined($pcap_t) ) {

                &main::write_log("Unable to inititate Net::Pcap::open_offline for offline packet capture: $err - Error NET104", 1);
                Win32::MsgBox("The Watchman was unable to initiate offline packet capture for system testing. Please check your system settings and make sure you have configured the network settings correctly. Error NET104");
                return undef;
            }
        } else {

            &main::write_log("Error. Cannot stat capture file $self->{captureFile} - Error NET105\n", 1);
            Win32::MsgBox("In order to run the system in testing mode a sample incident file must be used to emulate incident alerting. The system was unable to locate this required file: $self->{captureFile} Error NET105\n");
            return undef;
        }


        $net_checked = 'Checked';
        undef $net_interupt;

        my $capture_err = 0;
        my $last_error = 0;

        while ( 1 ) {

            unless ( &main::start_capture ) {

                &main::write_log("Error. Cannot start packet capture loop: " . Net::Pcap::geterr( $pcap_t ) . " \n", 1);
                Win32::MsgBox("Error stating network capture loop. " . Net::Pcap::geterr( $pcap_t ) );
            }

            last if ( ! $self->{'live'} || $net_interupt );

            &main::write_log("Capture loop interrupted...\n");

            $capture_err = 0 if ( $capture_err > 0 && int( ( time - $last_error ) / 60 ) > 2 );

            $capture_err++;
            $last_error = time;

            if ( $capture_err > 30 ) {

                &main::write_log("Unable to restart Pcap capture loop. Aborting\n", 1);
                $capture_err = $last_error = 0;
                last;
            }
        }

        &main::write_log("Closing Pcap packet descriptor\n");
        Net::Pcap::close( $pcap_t );

        exit(0);
    }

    &main::write_log("Pushing net capture process ID onto child process array: $net_capture\n");
    push( @childpids, $net_capture );
}
