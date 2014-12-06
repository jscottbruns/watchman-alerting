#!perl
my $url = "http://localhost:8080/";
my $commandline = qq{start "$url" "$url"};
system($commandline) == 0
    or die qq{Couldn't launch '$commandline': $!/$?};
