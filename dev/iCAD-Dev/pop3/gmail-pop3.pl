#!/usr/bin/perl
use Net::POP3::SSLWrapper;
use Data::Dumper;

pop3s {
    my $pop3 = Net::POP3->new('pop.gmail.com', Port => 995) or die ("Can't connect");
    if ( $pop3->login('pgmd-incident@fhwm.net', 'fhwm123321') > 0 )
    {
        print "Login Successful\n";
        print Dumper($pop3->list);
        my $list = $pop3->list;
        foreach my $_i ( keys %$list )
        {
            my $msg = $pop3->get($_i);
            print "Dumping Message [$_i]\n";
            print @$msg;
            print "\n";
        }
    }
    else
    {
        print "Login Unsucessful\n";
    }
    $pop3->quit;
}
