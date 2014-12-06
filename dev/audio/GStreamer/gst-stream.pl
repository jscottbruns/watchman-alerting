#!/usr/bin/perl

use strict;
use warnings;
use GStreamer -init;

my $loop = Glib::MainLoop->new;

my $pipeline = GStreamer::Pipeline->new("wavparse");
my $caps = GStreamer::Caps->from_string("audio/x-raw-int,channels=2");

my $filesrc = GStreamer::ElementFactory->make("filesrc", "audio/preIncAlert.wav");
my $convert = GStreamer::ElementFactory->make("audioconvert", "audioresample");
my $level = GStreamer::ElementFactory->make("level", "level");
my $sink = GStreamer::ElementFactory->make("fakesink", "sink");

$pipeline->add($testsrc, $convert, $level, $sink);
$testsrc->link($convert);
$convert->link_filtered($level, $caps);
$level->link($sink);

$level->set("message", 1);
$level->set("interval", 200000000000); # 1 second interval
$pipeline->get_bus->add_watch(\&hdl_level, $loop);
$pipeline->set_state("playing");

$loop->run;
$pipeline->set_state("null");
exit;

sub hdl_level {
  my ($bus, $message, $loop) = @_;
  return 1 unless $message->type & 'element' && $message->get_structure->{name} eq "level";
  my $num_channels = scalar @{$message->get_structure->{fields}[5][2]};
  printf "%s: " . join(', ', ('%.4f') x $num_channels) . "\n", scalar localtime, map { $_->[0] } @{$message->get_structure->{fields}[5][2]};
  return 1;
}
