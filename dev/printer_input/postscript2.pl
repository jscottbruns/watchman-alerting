#!/usr/bin/perl

use PostScript::Simple;

# create a new PostScript object
$p = new PostScript::Simple(
    papersize => "A4",
    colour => 1,
    eps => 0,
    units => "pt"
);

# create a new page
$p->newpage;

$p->polygon(10,10, 10,20, 20,20, 20,10, 10,10);

$p->output("file.ps");
