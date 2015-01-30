use utf8;
package Watchman::Schema::Result::IoDevice;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::IoDevice

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<IO_Devices>

=cut

__PACKAGE__->table("IO_Devices");

=head1 ACCESSORS

=head2 triggerid

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 eventtype

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=head2 unitid

  data_type: 'integer'
  is_foreign_key: 1
  is_nullable: 1

=head2 toneid

  data_type: 'integer'
  is_foreign_key: 1
  is_nullable: 1

=head2 triggertype

  data_type: 'varchar'
  default_value: 'MB_TCP'
  is_nullable: 0
  size: 10

=head2 deviceaddr

  data_type: 'varchar'
  is_nullable: 0
  size: 32

=head2 deviceport

  data_type: 'varchar'
  is_nullable: 1
  size: 5

=head2 deviceid

  data_type: 'varchar'
  is_nullable: 0
  size: 12

=head2 devicechan

  data_type: 'varchar'
  is_nullable: 0
  size: 255

=head2 deviceonval

  data_type: 'varchar'
  is_nullable: 1
  size: 8

=head2 deviceoffval

  data_type: 'varchar'
  is_nullable: 1
  size: 8

=head2 resettime

  data_type: 'varchar'
  is_nullable: 1
  size: 3

=head2 inactive

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=cut

__PACKAGE__->add_columns(
  "triggerid",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "eventtype",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
  "unitid",
  { data_type => "integer", is_foreign_key => 1, is_nullable => 1 },
  "toneid",
  { data_type => "integer", is_foreign_key => 1, is_nullable => 1 },
  "triggertype",
  {
    data_type => "varchar",
    default_value => "MB_TCP",
    is_nullable => 0,
    size => 10,
  },
  "deviceaddr",
  { data_type => "varchar", is_nullable => 0, size => 32 },
  "deviceport",
  { data_type => "varchar", is_nullable => 1, size => 5 },
  "deviceid",
  { data_type => "varchar", is_nullable => 0, size => 12 },
  "devicechan",
  { data_type => "varchar", is_nullable => 0, size => 255 },
  "deviceonval",
  { data_type => "varchar", is_nullable => 1, size => 8 },
  "deviceoffval",
  { data_type => "varchar", is_nullable => 1, size => 8 },
  "resettime",
  { data_type => "varchar", is_nullable => 1, size => 3 },
  "inactive",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
);

=head1 PRIMARY KEY

=over 4

=item * L</triggerid>

=back

=cut

__PACKAGE__->set_primary_key("triggerid");

=head1 RELATIONS

=head2 toneid

Type: belongs_to

Related object: L<Watchman::Schema::Result::RfAlert>

=cut

__PACKAGE__->belongs_to(
  "toneid",
  "Watchman::Schema::Result::RfAlert",
  { id => "toneid" },
  {
    is_deferrable => 1,
    join_type     => "LEFT",
    on_delete     => "SET NULL",
    on_update     => "RESTRICT",
  },
);

=head2 unitid

Type: belongs_to

Related object: L<Watchman::Schema::Result::Unit>

=cut

__PACKAGE__->belongs_to(
  "unitid",
  "Watchman::Schema::Result::Unit",
  { id => "unitid" },
  {
    is_deferrable => 1,
    join_type     => "LEFT",
    on_delete     => "SET NULL",
    on_update     => "RESTRICT",
  },
);


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:YP5ZzjklB7ntnnCfmUTAOA


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
