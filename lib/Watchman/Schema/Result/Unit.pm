use utf8;
package Watchman::Schema::Result::Unit;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::Unit

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<Units>

=cut

__PACKAGE__->table("Units");

=head1 ACCESSORS

=head2 id

  data_type: 'integer'
  is_nullable: 0

=head2 unitid

  data_type: 'varchar'
  is_nullable: 0
  size: 12

=head2 unitlabel

  data_type: 'varchar'
  is_nullable: 0
  size: 32

=head2 audioannounce

  data_type: 'varchar'
  is_nullable: 1
  size: 128

=head2 inactive

  data_type: 'tinyint'
  default_value: 1
  is_nullable: 0

=cut

__PACKAGE__->add_columns(
  "id",
  { data_type => "integer", is_nullable => 0 },
  "unitid",
  { data_type => "varchar", is_nullable => 0, size => 12 },
  "unitlabel",
  { data_type => "varchar", is_nullable => 0, size => 32 },
  "audioannounce",
  { data_type => "varchar", is_nullable => 1, size => 128 },
  "inactive",
  { data_type => "tinyint", default_value => 1, is_nullable => 0 },
);

=head1 PRIMARY KEY

=over 4

=item * L</id>

=back

=cut

__PACKAGE__->set_primary_key("id");

=head1 UNIQUE CONSTRAINTS

=head2 C<index_unit_id>

=over 4

=item * L</unitid>

=back

=cut

__PACKAGE__->add_unique_constraint("index_unit_id", ["unitid"]);

=head1 RELATIONS

=head2 alert_groups

Type: has_many

Related object: L<Watchman::Schema::Result::AlertGroup>

=cut

__PACKAGE__->has_many(
  "alert_groups",
  "Watchman::Schema::Result::AlertGroup",
  { "foreign.unitid" => "self.id" },
  { cascade_copy => 0, cascade_delete => 0 },
);

=head2 incident_units

Type: has_many

Related object: L<Watchman::Schema::Result::IncidentUnit>

=cut

__PACKAGE__->has_many(
  "incident_units",
  "Watchman::Schema::Result::IncidentUnit",
  { "foreign.unitid" => "self.id" },
  { cascade_copy => 0, cascade_delete => 0 },
);

=head2 io_devices

Type: has_many

Related object: L<Watchman::Schema::Result::IoDevice>

=cut

__PACKAGE__->has_many(
  "io_devices",
  "Watchman::Schema::Result::IoDevice",
  { "foreign.unitid" => "self.id" },
  { cascade_copy => 0, cascade_delete => 0 },
);


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:ovU/mlx/9lm5hMsQMw+rKg


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
