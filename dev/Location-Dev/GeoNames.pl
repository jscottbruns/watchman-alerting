#!/usr/bin/perl
use Geo::GeoNames;
use Data::Dumper;

my $geo;

if ( $geo = new Geo::GeoNames( 'username' => 'fhwm' ) )
{ 
    my $result = $geo->find_nearest_intersection( lat => '40.469180', lng => '-79.871722' );

    if ( $result && $result->[0] )
    { 
        print "GeoNames service request appears successful";

        if ( $result->[0]->{'street1'} || $result->[0]->{'street2'} )
        {
            $geo_update = 1;

            print "Xstreet 1: $result->[0]->{'street1'} \n";
            print "Xstreet 2: $result->[0]->{'street2'} \n";
        }
        else
        {
            print "*ERROR* Geo request did not contain valid response - Cannot continue with geolocation service $@ $!";
        }
    }
    else
    {
        print "*ERROR* Geonames service request returned empty response ";
    }
}
