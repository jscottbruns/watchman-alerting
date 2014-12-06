#!/usr/bin/perl

my $self = {
	'Location' => '123 BELMONT ST',
    'CrossSt'  => 
	[
	#'CLEARWATER ST',
		#	'BERNARD ST'
	]
};

$self->{'Location'} .= " (" . join(' & ', @{ $self->{'CrossSt'} } ) . ")" if ( $self->{'CrossSt'}->[0] || $self->{'CrossSt'}->[1] );
print $self->{'Location'};
