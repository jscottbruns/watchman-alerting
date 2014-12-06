#!/usr/bin/perl
use PostScript::Simple;

my $p = new PostScript::Simple(
    'units' => 'pt',
    'xsize' => '215',
    'ysize' => '400',
    'coordorigin'   => 'LeftTop',
    'direction' => 'RightDown',
    'eps'   => 0
) or die "Postscript Error\n";

$p->newpage(1);

$p->setfont("Helvetica-Bold", 10);
$p->text(5, 15, "INCIDENT DISPATCH: F1234567");

#$p->setlinewidth(1);
#$p->line(5, 17, 210, 17);

#$p->setfont("Helvetica", 4);
#$p->text( 5, 22, "11/08/2012 12:23:04");

# Call Nature
$p->box(5, 36, 210, 27);
$p->setfont("Helvetica-Bold", 7);
$p->text( 7, 34, "INCIDENT TYPE ");
$p->setfont("Helvetica-Bold", 6);
$p->text( 7, 43, "COMMERCIAL FIRE ALARM ACTIVATION");

# Call Nature
$p->box(5, 59, 210, 50);
$p->setfont("Helvetica-Bold", 7);
$p->text( 7, 57, "LOCATION");
$p->setfont("Helvetica-Bold", 6);
$p->text( 7, 66, "THE RITTENHOUSE APARTMENTS");
$p->text( 7, 73, "5193 FORBES AVE" );
$p->text( 7, 80, "5TH AVE & PENN STATION PL" );

$p->box( 5, 95, 210, 86 );
$p->setfont("Helvetica-Bold", 7);
$p->text( 7, 93, "NATURE");


$p->output('./Incident-Dispatch');
