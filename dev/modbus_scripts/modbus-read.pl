#!/usr/bin/perl
use Modbus::Client;
use Data::Dumper;

#my $bus = new Modbus::Client "COM4";, POSIX::B9600 or die "Can't open Modbus client: $@ $!\n";
my $bus = new Modbus::Client "COM4" or die "Can't open Modbus client: $@ $!\n";
$bus->{'debug'} = 1;
my $unit = $bus->device(1) or die "Can't set new device: $@ $!\n";

# Function Code (tM-P4C4):
# 1 (0x01) = Read Coils
# 2 (0x02) = Read Discrete Inputs
# 3 (0x03) = Read Mult Registers
# 4 (0x04) = Read Mult Input Registers
# 5 (0x05) = Write Single Coil
# 15 (0x0F)= Write Mult Coils
# 70 (0x46)= Read/Write Module Settings
#
# Channel Syntax (10001): 
#                 1  - Function Code (1-byte decimal)
#                 00 - Channel Number (2-byte decimal, Byte 1 of 2)
#                 01 - Channel Number (2-byte decimal, Byte 2 of 2)
# ** Under umbrella of function 01 (read coils), multiple channel assignments can be assigned to each physical channel on device.
# ** Example, on tM-P4C4 the same physical output channel uses the following modbus channel values for reading different data 
# ** values from the same physical channel:
# ** (1) DO readback value on channel DO1: 0001 (0x0001) ... 0002 for DO2, 0003 for DO3, etc.
# ** (2) DO latch high value on channel DO1: 0072 (0x0048)
# ** (3) DO latch low value on channel DO1: 0104 (0x0068)

my $_hash  = $unit->read_one(10001);
print "Response: : [$_hash]\n";

for my $_i ( sort keys %{ $_hash } )
{
	print "$_i => $_hash->{ $_i } \n";
}
