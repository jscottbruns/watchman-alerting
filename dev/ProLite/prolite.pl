#!/usr/bin/perl
use ProLite;

my $s = new ProLite(
    id          => 1,
    device      => '/dev/ttyUSB0', 
    debug       => 1,
    maxReadAttempts => 1
);

$err = $s->connect();
die "Can't connect to device - $err" if $err;
#$s->wakeUp();
$s->setPage(1, RESET, yellow, "Hello ", green, "world");
$s->runPage(1);
