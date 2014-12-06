#!/usr/bin/perl
use POSIX;
use LWP::UserAgent;
use Data::Dumper;
use HTML::Entities;

my $uri = 'https://{:}@wctp.sprint.com/servlet/wctp';

if ( $uri =~ /^(http(?:s)?):\/\/(?:{(.*)?:(.*)?})@(.*)$/ )
{
    print "proto: $1\n";
    print "User: $2\n";
    print "Pass: $3\n";
    print "Host: $4\n";
    exit;
}

print "N";
exit;

my $uri = 'http://wctp.sprint.com/servlet/wctp';
my $wctp_user = 'jsbruns';
my $text = "[231-110] HOUSE FIRE 1929 LETSCHE ST #59 (BELLEAU ST & MERCY ST) 231EN38 231EN37 231EN32 231TK32 2314011 231MAC1 231EN03 2314044 http://fhwm.net/g/1132 http://fhwm.net/c/1133";
my $msg_id = '12';

my $gmt_time = POSIX::strftime("%Y-%m-%dT%H:%M:%S", gmtime);
my $recip_xml = "<wctp-Recipient recipientID=\"+14432501272\" /><wctp-Recipient recipientID=\"+14432572223\" />";
my $message = HTML::Entities::encode_entities( $text );

my $xml = <<XML;
<?xml version="1.0" ?>
<!DOCTYPE wctp-Operation SYSTEM "http://www.telemessage.com/dtd/wctp/wctp-dtd-v1r3.dtd">
<wctp-Operation wctpVersion="wctp-dtd-v1r3">
    <wctp-SendMsgMulti>
        <wctp-MsgMultiHeader>
            <wctp-Originator senderID="$wctp_user" />
            <wctp-Recipient recipientID="+14432501272" />
        </wctp-MsgMultiHeader>
        <wctp-Payload>
            <wctp-Alphanumeric>$text</wctp-Alphanumeric>
        </wctp-Payload>
    </wctp-SendMsgMulti>
</wctp-Operation>
XML

my $ua = LWP::UserAgent->new;
my $req = HTTP::Request->new( POST => $uri );

$req->content_type('text/xml');
$req->content( $xml );

my $response = $ua->request( $req );

# Check the outcome of the response
if ($response->is_success) {
    print $response->content;
}
else {
    print $response->status_line, "\n";
}
