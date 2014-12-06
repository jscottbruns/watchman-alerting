#!/usr/bin/perl
use Data::Dumper;
use REST::Client;
 
#The basic use case
my $client = REST::Client->new();
$client->POST("http://api.ispeech.org/api/rest", "apikey=55f3776076dc1730b51c86eb5eff2ae5&action=convert&text=17+Engine");
print length $client->responseContent();

open(FH, ">17-Engine.mp3");
print FH $client->responseContent();
close FH;
