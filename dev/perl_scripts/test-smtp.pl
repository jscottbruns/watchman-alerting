#!/usr/bin/perl
use Data::Dumper;

my $msg = <<MSG;
ALRM LVL: 1
LOC:
PPG INDUSTRIES,
440 COLLEGE PARK DR
MONROEVILLE
BTWN: NOEL DR & DRIVEWAY

RCVD AS Phone

COM:
GENERAL FIRE

CT:
MD100 at POS 02
MSG

print $msg;
print "\n\n";

if ( $msg =~ /^ALRM LVL:\s([0-9])\nLOC:\s(.*?)\nBTWN:\s(.*?)\s&\s(.*?)\n(.*?)CT:\s?\n(.*)$/s )
{
    $inc_data = {
        'Level'     => &main::trim( $1 ),
        'Location'  => &main::trim( $2 ),
        'Address'   => undef,
        'CityCode'  => undef,
        'Type'      => undef,
        'XStreet1'  => &main::trim( $3 ),
        'XStreet2'  => &main::trim( $4 ),
        'Comment'   => &main::trim( $5 ),           
        'DispId'    => &main::trim( $6 )
    };
    
    $inc_units = [ 'STA5' ];
    
    if ( $inc_data->{'Location'} =~ /^(.*?),\n(.*?)\n(.*)$/s )
    {
        $inc_data->{'Location'} = &main::trim( $1 );
        $inc_data->{'Address'} = &main::trim( $2 );
        $inc_data->{'CityCode'} = &main::trim( $3 );   
    }

    if ( $inc_data->{'Comment'} =~ /^(.*?)\n{1,}COM:\n(.*)$/s )
    {
        $inc_data->{'Comment'} = &main::trim( $1 ) . ' - ' . &main::trim( $2 );
    }

    my $sub = "(Alert: Fire-Commerical)" =~ /^\(Alert:\s(.*?)\)$/ and $inc_data->{'Type'} = &main::trim($1);
}

print Dumper( $inc_data );

sub trim($)
{
    my $string = shift;
    return unless $string;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    return $string;
}

