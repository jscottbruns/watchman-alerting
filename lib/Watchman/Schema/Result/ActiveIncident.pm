use utf8;
package Watchman::Schema::Result::ActiveIncident;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::ActiveIncident

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<ActiveIncidents>

=cut

__PACKAGE__->table("ActiveIncidents");

=head1 ACCESSORS

=head2 id

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 eventno

  data_type: 'varchar'
  is_nullable: 0
  size: 24

=head2 timestamp

  data_type: 'timestamp'
  datetime_undef_if_invalid: 1
  default_value: current_timestamp
  is_nullable: 0

=head2 incidentno

  data_type: 'varchar'
  is_nullable: 0
  size: 24

=head2 opentime

  data_type: 'datetime'
  datetime_undef_if_invalid: 1
  is_nullable: 1

=head2 closetime

  data_type: 'datetime'
  datetime_undef_if_invalid: 1
  is_nullable: 1

=head2 calltype

  data_type: 'varchar'
  is_nullable: 0
  size: 12

=head2 nature

  data_type: 'varchar'
  is_nullable: 1
  size: 18

=head2 boxarea

  data_type: 'varchar'
  is_nullable: 0
  size: 12

=head2 location

  data_type: 'varchar'
  is_nullable: 0
  size: 255

=head2 priority

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=head2 update

  accessor: undef
  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=head2 cancel

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=cut

__PACKAGE__->add_columns(
  "id",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "eventno",
  { data_type => "varchar", is_nullable => 0, size => 24 },
  "timestamp",
  {
    data_type => "timestamp",
    datetime_undef_if_invalid => 1,
    default_value => \"current_timestamp",
    is_nullable => 0,
  },
  "incidentno",
  { data_type => "varchar", is_nullable => 0, size => 24 },
  "opentime",
  {
    data_type => "datetime",
    datetime_undef_if_invalid => 1,
    is_nullable => 1,
  },
  "closetime",
  {
    data_type => "datetime",
    datetime_undef_if_invalid => 1,
    is_nullable => 1,
  },
  "calltype",
  { data_type => "varchar", is_nullable => 0, size => 12 },
  "nature",
  { data_type => "varchar", is_nullable => 1, size => 18 },
  "boxarea",
  { data_type => "varchar", is_nullable => 0, size => 12 },
  "location",
  { data_type => "varchar", is_nullable => 0, size => 255 },
  "priority",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
  "update",
  {
    accessor      => undef,
    data_type     => "tinyint",
    default_value => 0,
    is_nullable   => 0,
  },
  "cancel",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
);

=head1 PRIMARY KEY

=over 4

=item * L</id>

=back

=cut

__PACKAGE__->set_primary_key("id");


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:h1zd8b8XkGQoHKfsqToefg


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
