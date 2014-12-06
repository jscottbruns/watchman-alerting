#!/usr/bin/perl
use XML::LibXML;
use Data::Dumper;

my $parser = XML::LibXML->new();
my $doc = $parser->parse_file('./resp.xml');

my $xc = XML::LibXML::XPathContext->new( $doc->documentElement()  );

# <response resultCode="0" resultString="success" conversionNumber="6" status="Queued" statusCode="1"/>

print $xc->findvalue('//response/@resultString');
exit;

my @n = $xc->findnodes('//response');
print "ResultCode: " . $n[0]->getAttribute("resultCode") . "\n";
print "ResultString: " . $n[0]->getAttribute("resultString") . "\n";
print "Convert Number: " . $n[0]->getAttribute("conversionNumber") . "\n";
print "Status: " . $n[0]->getAttribute("status") . "\n";
print "Status Code: " . $n[0]->getAttribute("statusCode") . "\n";
