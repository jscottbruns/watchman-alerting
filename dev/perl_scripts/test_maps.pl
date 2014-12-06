#!perl
use strict;
use LWP::Simple;
use XML::XPath;

my $result_no;
my $county_list = "\\bPrince George's\\b";

my $url = "http://maps.google.com/maps/api/geocode/xml?address=7521+RIVERDALE+RD%2C+NEW+CAROLLTON++PG&sensor=false";
# my $url = "http://maps.google.com/maps/api/geocode/xml?address=CAPITAL BELTWAY/BALTIMORE WASHINGTON PKWY&sensor=false";

my $content = get $url;

open(FH, ">mapdump.xml");
print FH $content;
close FH;

my $xp = XML::XPath->new(
    xml => $content
);

my $stat = $xp->findvalue("//status");
if ( $stat eq '620' ) {
    print "ABUSE WARNING!\n";
    exit;
}
print "Status OK\n" if ( $stat eq 'OK' );
print "Status NO RESULTS\n" if ( $stat eq 'ZERO_RESULTS' );
print "MALFORMATTED ADDRESS\n" if ( $stat eq 'INVALID_REQUEST' );

my $results = $xp->find("//result");

if ( $stat eq 'OK' ) {

	my ($address, $lat, $lng);

	if ( $results->size > 1 && $county_list ) {

		print "Multi-result set\n";
		my $i;
		for ( $i = 1; $i <= $results->size; $i++ ) {

		    if ( $xp->findvalue("/GeocodeResponse/result[$i]/address_component[type/text()='administrative_area_level_2']/long_name") =~ /$county_list/i ) {

                $address = $xp->findvalue("//result[$i]/formatted_address/text()");
		        $lat = $xp->findvalue("//result[$i]//location/lat");
		        $lng = $xp->findvalue("//result[$i]//location/lng");

			    last;
		    }
		}
	} else {

        print "Single result set\n";
        $address = $xp->findvalue("//result[1]/formatted_address/text()");
        $lat = $xp->findvalue("//result[1]//location/lat");
        $lng = $xp->findvalue("//result[1]//location/lng");
	}

	print "Address: $address\n";
	print "Latitude: $lat\n";
	print "Longitude: $lng\n";

}