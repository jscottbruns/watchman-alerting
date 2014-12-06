package Watchman::Connect;

use LWP::UserAgent;
use HTTP::Request::Common;
use HTTP::Headers;

use XML::Simple;
use XML::XPath;
use HTML::Entities;
use Storable;

use Data::Dumper;

sub new
{
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;
    my $self = {};

    &main::write_log("Starting Watchman::Connect - Data Relay Service\n");

    $self->{'version'}          =   $::VERSION;
    $self->{'path'}             =   $::PATH;
    $self->{'debug'}            =   $::DEBUG || 0;
    $self->{'license'}          =   $::LICENSE;

	my $datafile				=	$params{'datafile'};
	my $host_uri				=	$params{'uri'};
	my $host_proxy				=	$params{'proxy'};
	my $timeout					=	$params{'timeout'};

	if ( ! $datafile || ! -f $datafile ) {

		&main::write_log("Error. Cannot stat file for hash data unserialization\n", 1);
		return undef;
	}

	my $dataset;
	unless ( $dataset = retrieve($datafile) ) {

		&main::write_log("Error encountered while retrieving serialized data file $@ - Unable to proceed with data relay \n", 1);
		return undef;
	}

	if ( ! $dataset || ref( $dataset ) ne 'ARRAY' ) {

		&main::write_error("Data retrieval error - Unexpected data type (" . ref($dataset) . ") resulting from unserialized data. Unable to proceed with data relay \n", 1);
		return undef;
	}

	my $count = 0;
	my $envelope = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" soap:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">
	<soap:Body>
		<init xmlns=\"$host_uri\">
			<c-gensym3>
				<license xsi:type=\"xsd:string\">$self->{license}</license>
				<debug xsi:type=\"xsd:boolean\">$self->{debug}</debug>
				<inc_no xsi:type=\"xsd:string\">@{ $dataset }[0]->{inc_no}->{content}</inc_no>
				<call_no xsi:type=\"xsd:string\">@{ $dataset }[0]->{call_no}->{content}</call_no>
				<req_count xsi:type=\"xsd:integer\">" . ( $#{ $dataset } + 1 ) . "</req_count>";

	foreach my $_hashmap ( @{ $dataset } ) {

		$envelope .= "
		<request_$count>
		<service>$_hashmap->{service}</service>";

		for my $_j ( keys %{ $_hashmap } ) {

			if ( ref( $_hashmap->{ $_j } ) eq 'HASH' ) {

				if ( $_hashmap->{ $_j }->{'type'} eq 'string' ) {
					$envelope .= "<$_j xsi:type=\"xsd:string\">" . encode_entities($_hashmap->{ $_j }->{'content'}) . "</$_j>\n";
				} elsif ( $_hashmap->{ $_j }->{'type'} eq 'cdata' ) {
					$envelope .= "<$_j xsi:type=\"xsd:string\"><![CDATA[ $_hashmap->{ $_j }->{content} ]]></$_j>\n\r";
				} elsif ( $_hashmap->{ $_j }->{'type'} eq 'array' ) {
					$envelope .= "<$_j soapenc:arrayType=\"xsd:string[$_hashmap->{ $_j }->{size}]\" xsi:type=\"soapenc:Array\">$_hashmap->{ $_j }->{content}</$_j>";
				}
			}
		}

		$envelope .= "
		</request_$count>";

		$count++;
	}

	$envelope .= "
			</c-gensym3>
		</init>
	</soap:Body>
</soap:Envelope>";

	&main::write_log("Message envelope constructed, validating XML...\n");

	eval { XMLin($envelope) };

	if ( $@ ) {

		&main::write_log("Error parsing	XML: $@ \n", 1);
		&main::write_log("Message delivery failed \n", 1);

		return undef;
	}

	&main::write_log("Initiating LWP User Agent \n");

	my $ua = LWP::UserAgent->new(
		'agent'		=>	'WatchmanConnect SOAP Client/1.0',
        'timeout'   =>	$timeout,
	    'default_headers'	=>	HTTP::Headers->new(
		    'Content-Type'		=>	"text/xml; charset=utf-8",
	    	'Accept-Language'	=>	'en-US,en',
	    	'Accept-Charset'	=>	'iso-8859-1, utf-8, utf-16, *',
	    	'Accept-Encoding'	=>	scalar HTTP::Message::decodable(),
	    	'Accept'			=>	"text/xml, multipart/*, application/soap"
	    )
    );

    &main::write_log("Sending HTTP request\n");

	my $response = $ua->post(
		$host_proxy,
		Content_Type	=>	'text/xml; charset=utf-8',
		Content			=>	$envelope
	);

	&main::write_log("HTTP response received (" . $response->status_line . ")");

	if ( $response->is_success ) {

		my $respobj;

		eval { $respobj = XMLin($response->decoded_content) };

		if ( $@ ) {

			&main::write_log("Could not parse server response, XML not valid\n", 1);
			return undef;
		}

		if ( $respobj->{'SOAP-ENV:Body'}->{'SOAP-ENV:Fault'} ) {

			&main::write_log("Fault response received from server: ($respobj->{'SOAP-ENV:Body'}->{'SOAP-ENV:Fault'}->{faultcode}) $respobj->{'SOAP-ENV:Body'}->{'SOAP-ENV:Fault'}->{faultstring}", 1);
			return undef;
		}

		return 1;
	}

	return undef;
}



1;