#!/usr/bin/perl
# How to convert OGG to MP3 (Why you want to do that?)
# But good way to make exampe in Gstreamer in perl
# Taken from: http://lizards.opensuse.org/2009/03/10/gstreamer-perl-bindings-and-converting-ogg-to-mp3/
# gst-launch pipeline examples:  http://docs.gstreamer.com/display/GstSDK/gst-launch
# Perl interface to GStreamer Library: http://gtk2-perl.sourceforge.net/doc/pod/GStreamer.html
# gtk2-perl Docs: http://gtk2-perl.sourceforge.net/doc/pod/GStreamer/ElementFactory.html

use strict;
use warnings;
use Glib qw(TRUE FALSE);
use GStreamer;

GStreamer -> init();

if ($#ARGV < 1) {
  printf "usage: %s   %dn", $0, $#ARGV;
  exit -1;
}

print $ARGV[0]." to ";
print $ARGV[1];
print "n";

# create a new pipeline to hold the elements
my $l_SPipeline = GStreamer::Pipeline -> new("pipeline");

# Create Source read file
my $l_SSource = GStreamer::ElementFactory->make(filesrc  =>  "file-source");
# Mux OGG
my $l_SOggDemuxer = GStreamer::ElementFactory->make(oggdemux => "ogg-demuxer");
# Decode Vorbis audio
my $l_SVorbisDecoder = GStreamer::ElementFactory->make(vorbisdec => "vorbis-decoder");
# If needed convert audio
my $l_SAudioConverter = GStreamer::ElementFactory->make(audioconvert => "converter");
# Queue it if needed
my $l_SQueue = GStreamer::ElementFactory -> make(queue => "queue");
# Sink it to file
my $l_SSink = GStreamer::ElementFactory->make(filesink  =>  "file-sink");
# Mp3 encode
my $l_SEncode = GStreamer::ElementFactory->make(lame  =>  "mp3-encoder");

# Set up lame
$l_SEncode->set(bitrate => 320);

# Set filenames
$l_SSource->set(location => Glib::filename_to_unicode $ARGV[0]);
# Sink
$l_SSink->set(location => Glib::filename_to_unicode $ARGV[1]);

# add objects to the main pipeline
$l_SPipeline -> add($l_SSource, $l_SOggDemuxer, $l_SVorbisDecoder, $l_SAudioConverter, $l_SQueue, $l_SEncode, $l_SSink);

# Link source to Ogg muxer
$l_SSource->link($l_SOggDemuxer) or die "Can't link source to OGG-muxer";
# Link rest of pipeline
$l_SVorbisDecoder->link($l_SQueue, $l_SAudioConverter, $l_SEncode, $l_SSink)  or die "Can't link rest of stuff";

# Link Pads from Source to Sink
$l_SOggDemuxer->link_pads("src", $l_SVorbisDecoder, "sink");

# listen for newly created pads
$l_SOggDemuxer->signal_connect(pad_added => &pgotm_addPads) or die "Can't link Pads to vorbis";;

# start playing
$l_SPipeline -> set_state("playing");

# Listen for EOS
pgotm_eventLoop($l_SPipeline);

$l_SPipeline -> set_state("null");

# Add pads
sub pgotm_addPads {
  my ($element, $pad, $data) = @_;

  my $l_SCaps = $pad->get_caps();
  my $l_SMime = $l_SCaps->get_structure(0) -> { name };

  printf "Mime that is added is: %sn", $l_SMime;
  printf "A new pad %s was created %sn", $pad->get_name(), $pad->get_parent()->get_name();

  # If it's vorbis audio
  if( $l_SMime eq "audio/x-vorbis" ) {
    # Can it link to the audiopad?
    my $l_SSinkPad = $l_SVorbisDecoder->get_pad("sink");

   # Check if vorbis not linked..
   if(  !$l_SSinkPad->is_linked() ) {
      if( $pad->link( $l_SSinkPad ) ) {
         die "Can't link pads!rn";
     }
     print "Audio pads linked!n";
   } else {
     print "Audio is allready linked.. can't do it!n";
   }
  }
}

# Event loop to listen to events posted on the GstBus from the pipeline. Exits
# on EOS or ERROR events
sub pgotm_eventLoop {
  my ($l_SPipe) = @_;

  my $l_SBus = $l_SPipe->get_bus();

  while (TRUE) {
    my $l_SMessage = $l_SBus->poll("any", -1);

    if ($l_SMessage->type & "eos") {
      print "EOSn";
      return;
    } elsif ($l_SMessage->type & "warning") {
      print "Warning".$l_SMessage -> error."n";
    } elsif ($l_SMessage->type & "error") {
      $l_SSource->set_state("null");
      $l_SAudioConverter->set_state("null");
      $l_SVorbisDecoder->set_state("null");
      $l_SPipeline->set_state("null");
      die $l_SMessage -> error;
    }
  }
}
