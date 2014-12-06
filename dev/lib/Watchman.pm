#!perl
package Watchman;

sub new {
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;
    my $self = {};
	
	return $self;
}

1;