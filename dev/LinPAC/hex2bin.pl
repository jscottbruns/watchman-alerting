#!/usr/bin/perl
# http://www.binaryhexconverter.com/hex-to-binary-converter

use strict;

my @hex_digits = (0..9, 'A'..'F');
my $valid_hex = join '', @hex_digits;
my %hex2binary;
for (0..15) {
    # Create the binary string for this number.
    $hex2binary{$hex_digits[$_]} = ($_>>3).($_%8>>2).($_%4>>1).$_%2;
}

sub hex2binary
{
    my ($hex_string) = @_;
    # Return an undefined value unless the input is hexadecimal.
    return unless $hex_string =~ /^[$valid_hex]+$/i;
    # Globally substitute the hexadecimal digits with binary equivalents.
    $hex_string =~ s/([$valid_hex])/$hex2binary{uc $1}/gi;
    return $hex_string;
}

my $hex = $ARGV[0] || die "No hex input\n";
print "Hex: [$hex] Bin: [" . &hex2binary($hex) . "]\n";

my $a;
my @arr = reverse( split('', &hex2binary($hex)) );

print "Bit Array: \n";
my $i = 0;
foreach ( @arr )
{
    print $i++;
    print " - $_\n";
}
