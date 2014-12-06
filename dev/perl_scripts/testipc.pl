#!perl
my $_j = "audio_file.wav";
my ($_fname, $_fext) = $_j =~ /^(.*)\.([^.]+)$/;

print "File: $_j FName: $_fname FExt: $_fext \n";exit;

my $self = {};

$self->{'AlertGroups'} = [];
push @{ $self->{'AlertGroups'} }, {
	'Group'	=> 'DEFAULT',
	'ADDR'	=> '239.255.1.1'
};

push @{ $self->{'AlertGroups'} }, {
	'Group'	=> 'CHIEF',
	'ADDR'	=> '239.255.1.2'
};

push @{ $self->{'AlertGroups'} }, {
	'Group'	=> 'BUNK',
	'ADDR'	=> '239.255.1.3'
};

print $#{ $self->{AlertGroups} };
exit;
