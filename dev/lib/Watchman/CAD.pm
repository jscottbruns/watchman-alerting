#!perl
package Watchman::CAD;

sub new {
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;
    my $self = {};
	
	$self->{'preinc_module'} = $params{'preinc_module'};
	
	$self->{'call_alert'} = {};
	$self->{'preIncAlert'} = {};	
	$self->{'preIncAlert'}->{'30'}->{'HOUSEF'} = 1;
	
	print "Watchman::CAD initiating module using $preinc_module lookup module\n";	
	
	bless $self, $class;
	
    if ( ! $self->init_cookie ) {
        print "Error creating cookie object\n";
        return undef;
    }
	
    if ( ! $self->init_lwp ) {
        print "Error creating LWP object\n";
        return undef;
    }	
	
	$self->{'request'} = $self->init_request( 'http://firehousewatchman.com/listing.html' );
	
	# Subroutines called from child
	sub request { $self->{'request'} };
	sub ua { $self->{'ua'} };
	sub preIncAlert { $self->{'preIncAlert'} };
	
	return $self;
}

sub fetch 
{
	my $self = shift;
	
	if ( $self->{'call_alert'} = "Watchman::CAD::$self->{preinc_module}"->fetch ) {
	
		print "Incident alert returned...\n";
		print "Dumping call alert\n";
		for $_i ( keys %{ $self->{'call_alert'} } ) {
			print "$_i => $self->{call_alert}->{ $_i } \n";
		}		
	} else {
		print "Nothing returned\n";
	}
}

sub init_request
{
	my $self = shift;	
	my $host = shift;

    my $request = HTTP::Request->new(
        POST                =>  $host,
        HTTP::Headers->new(
            'Accept-Language'   =>  'en-US,en',
            'Accept-Charset'    =>  'iso-8859-1, utf-8, utf-16, *',
            'Accept-Encoding'   =>  'deflate, gzip',
            'Accept'            =>  'text/html, application/xml, application/xhtml+xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*',
            'Content-Type'      =>  'application/x-www-form-urlencoded',
        ),
    );

    return $request;
}

sub init_cookie
{
    my $self = shift;

	$self->{'cookie_jar'} = HTTP::Cookies->new(
	    'file'              =>  'cookie.lwp',
	    'ignore_discard'    =>  1,
	    'autosave'          =>  1
	);

	return undef unless $self->{'cookie_jar'};
}

sub init_lwp
{
    my $self = shift;

	$self->{'ua'} = LWP::UserAgent->new(
	    'cookie_jar'        =>  $self->{'cookie_jar'},
	    'timeout'           =>  10,
	    'conn_cache'        =>  LWP::ConnCache->new,
	);

	return undef unless $self->{'ua'};
}

1;