#!/usr/bin/perl
use GMail::Checker;
use Data::Dumper;
my $gwrapper = new GMail::Checker();
my $gwrapper = new GMail::Checker(USERNAME => 'pgmd-incident@fhwm.net', PASSWORD => "fhwm123321");

# Let's log into our account (using SSL)
$gwrapper->login('pgmd-incident@fhwm.net', 'fhwm123321');

# Get the number of messages in the maildrop & their total size
my ($nb, $size) = $gwrapper->get_msg_nb_size();

# Do we have new messages ?
#my $alert =$gwrapper->get_pretty_nb_messages(ALERT => "TOTAL_MSG");
print "Total: $nb\n";

# Get the headers for a specific message (defaults to last message)
#my @headers = $gwrapper->get_msg_headers(HEADERS => "FULL", MSG => 74);

# Get a message size
#my ($msgnb, $msgsize) = $gwrapper->get_msg_size(MSG => 42);

# Retrieve a specific message
for ( my $i = 1; $i <= $nb; $i++ )
{
    print "Fetching Message $i \n";
    my @msg = $gwrapper->get_msg( MSG => $i );
    #print Dumper(@msg);
    print "CONTENT --> " . $msg[0]->{content}, "\n";
    print "BODY --> " . $msg[0]->{body};
    print "\n\n";
}

# Retrieve UIDL for a message
#my @uidl = $gwrapper->get_uidl(MSG => 10);
$gwrapper->close();
