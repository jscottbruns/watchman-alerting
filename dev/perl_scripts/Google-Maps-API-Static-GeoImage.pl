#!/usr/bin/perl
use LWP::Simple;

my $lat = '38.9522684';
my $lng = '-76.8822774';
my $thumb_zoom = '14' || $params->{'thumbzoom'};
my $thumb_size = '200x150' || $params->{'thumbsize'};
my $thumb_scale = '2' || $params->{'thumbscale'};

my $mapthumb = LWP::Simple::get("http://maps.googleapis.com/maps/api/staticmap?markers=size:small%7C$lat,$lng&zoom=$thumb_zoom&size=$thumb_size&scale=$thumb_scale&sensor=false");

if ( $mapthumb )
{
	print "Successful result\n";
	if ( open(FH, ">geomap.png") )
	{
		print FH $mapthumb;
		close FH;
	}
	print "Done\n";
}
