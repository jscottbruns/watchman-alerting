use utf8;
package Watchman::Schema::Result::LedDisplay;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::LedDisplay

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<LED_Displays>

=cut

__PACKAGE__->table("LED_Displays");

=head1 ACCESSORS

=head2 displayid

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 displaytype

  data_type: 'varchar'
  default_value: 'BETABRITE'
  is_nullable: 0
  size: 20

=head2 displayname

  data_type: 'varchar'
  is_nullable: 0
  size: 100

=head2 protocol

  data_type: 'varchar'
  is_nullable: 0
  size: 3

=head2 address

  data_type: 'varchar'
  is_nullable: 0
  size: 30

=head2 defaultmsg

  data_type: 'varchar'
  is_nullable: 1
  size: 255

=head2 timeformat

  data_type: 'varchar'
  default_value: '24-hour'
  is_nullable: 0
  size: 10

=head2 stylesheet

  data_type: 'varchar'
  default_value: 'xsl/alphasign.xsl'
  is_nullable: 0
  size: 200

=cut

__PACKAGE__->add_columns(
  "displayid",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "displaytype",
  {
    data_type => "varchar",
    default_value => "BETABRITE",
    is_nullable => 0,
    size => 20,
  },
  "displayname",
  { data_type => "varchar", is_nullable => 0, size => 100 },
  "protocol",
  { data_type => "varchar", is_nullable => 0, size => 3 },
  "address",
  { data_type => "varchar", is_nullable => 0, size => 30 },
  "defaultmsg",
  { data_type => "varchar", is_nullable => 1, size => 255 },
  "timeformat",
  {
    data_type => "varchar",
    default_value => "24-hour",
    is_nullable => 0,
    size => 10,
  },
  "stylesheet",
  {
    data_type => "varchar",
    default_value => "xsl/alphasign.xsl",
    is_nullable => 0,
    size => 200,
  },
);

=head1 PRIMARY KEY

=over 4

=item * L</displayid>

=back

=cut

__PACKAGE__->set_primary_key("displayid");


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:Exhg/LJtZ2O9/5bIkEaZKg


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
