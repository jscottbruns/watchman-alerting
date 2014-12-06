#!/usr/bin/perl
use Data::Dumper;
use REST::Client;

my $ssml = $ARGV[0] || die "Failed to specify valid SSML for conversion\n";
my $dir = $ARGV[1] || '.';
my $debug = $ARGV[2];
#my $ssml = '<s>reported building fire</s><s><say-as interpret-as="characters" format="vxml:digits">644</say-as>elmore street</s><s>between ishar way and webster avenue</s><s>fire zone 2 <say-as interpret-as="vxml:number">1</say-as></s><s><say-as interpret-as="vxml:number">04</say-as> engine<break/><say-as interpret-as="vxml:number">10</say-as> engine<break/><say-as interpret-as="vxml:number">24</say-as> engine<break/><say-as interpret-as="vxml:number">04</say-as> truck<break/>unit <say-as interpret-as="vxml:number">40</say-as><say-as interpret-as="vxml:number">32</say-as><break/><say-as interpret-as="vxml:number">06</say-as> engine<break/>MAC1<break/>unit <say-as interpret-as="vxml:number">40</say-as><say-as interpret-as="vxml:number">34</say-as> respond</s>';

#$ssml = '<s>Ambulance';
die "Cannot stat audio directory [$dir]\n" unless -d $dir;

my $rest_xml = "<?xml version=\"1.0\"?><ConvertSsml>
<email>jeff.bruns\@firehouseautomation.com</email>
<accountId>ef5674a4c1</accountId>
<loginKey>fhwmtts</loginKey>
<loginPassword>36159d9079b5a4af9081</loginPassword>
<voice>TTS_JULIE_DB</voice>
<outputFormat>FORMAT_WAV</outputFormat>
<sampleRate>8</sampleRate>
<ssml>
    <speak version=\"1.0\">$ssml</speak>
</ssml>
<useUserDictionary>false</useUserDictionary>
</ConvertSsml>";

print "  Submitting TTS Request: \n";
print $rest_xml if $debug;

my $ws_client = REST::Client->new( {
    timeout => 5
} );

$ws_client->POST( 'https://tts.neospeech.com/rest_1_1.php', $rest_xml );

my $tts_content = $ws_client->responseContent();

if ( $ws_client->responseCode() eq '200' ) 
{
    print "TTS Conversion Successful\n" if $debug;

    my $tts_result = $ws_client->responseXpath()->findvalue('//response/@resultCode');
    my $tts_msg = $ws_client->responseXpath()->findvalue('//response/@resultString');
    my $tts_descr = $ws_client->responseXpath()->findvalue('//response/@resultDescription');
    my $tts_id = $ws_client->responseXpath()->findvalue('//response/@conversionNumber');

    print "TTS Conversion Successful ($tts_result) $tts_msg $tts_descr \n" if $debug;

    if ( $tts_result eq '0' && $tts_id )
    {
        print "Checking conversion status for TTS ID: $tts_id\n" if $debug;
        sleep 1;    

        $ws_client->GET("https://tts.neospeech.com/rest_1_1.php?method=GetConversionStatus&email=jeff.bruns\@firehouseautomation.com&accountId=ef5674a4c1&conversionNumber=$tts_id");
    
        print $ws_client->responseContent() if $debug;

        if ( $ws_client->responseCode() eq '200' )
        {
            my $statxpath = $ws_client->responseXpath();

            my $tts_statcode = $statxpath->findvalue('//response/@statusCode');
            my $tts_statstring = $statxpath->findvalue('//response/@status');
            my $tts_url = $statxpath->findvalue('//response/@downloadUrl');

            print "TTS Server returned: $tts_statcode\n" if $debug;

            if ( $tts_statcode eq '4' )
            {
                print "TTS Conversion Complete, Downloading from URL: $tts_url \n" if $debug;
                `wget $tts_url`;
            }
            else
            {
                print "TTS Conversion Error: $tts_statcode - $tts_statstring\n";
            }
        }
    }
}
else
{
    print "TTS Conversion Error: " . $ws_client->responseCode() . "\n";
}

