use utf8;
package Watchman::Schema::Result::IncidentUnit;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::IncidentUnit

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<IncidentUnits>

=cut

__PACKAGE__->table("IncidentUnits");

=head1 ACCESSORS

=head2 id

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 timestamp

  data_type: 'timestamp'
  datetime_undef_if_invalid: 1
  default_value: current_timestamp
  is_nullable: 0

=head2 incidentid

  data_type: 'integer'
  is_foreign_key: 1
  is_nullable: 0

=head2 unitid

  data_type: 'integer'
  is_foreign_key: 1
  is_nullable: 0

=head2 dispatchtime

  data_type: 'datetime'
  datetime_undef_if_invalid: 1
  is_nullable: 1

=head2 cancelled

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=cut

__PACKAGE__->add_columns(
  "id",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "timestamp",
  {
    data_type => "timestamp",
    datetime_undef_if_invalid => 1,
    default_value => \"current_timestamp",
    is_nullable => 0,
  },
  "incidentid",
  { data_type => "integer", is_foreign_key => 1, is_nullable => 0 },
  "unitid",
  { data_type => "integer", is_foreign_key => 1, is_nullable => 0 },
  "dispatchtime",
  {
    data_type => "datetime",
    datetime_undef_if_invalid => 1,
    is_nullable => 1,
  },
  "cancelled",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
);

=head1 PRIMARY KEY

=over 4

=item * L</id>

=back

=cut

__PACKAGE__->set_primary_key("id");

=head1 UNIQUE CONSTRAINTS

=head2 C<index_incident_no_unit_id>

=over 4

=item * L</incidentid>

=item * L</unitid>

=back

=cut

__PACKAGE__->add_unique_constraint("index_incident_no_unit_id", ["incidentid", "unitid"]);

=head1 RELATIONS

=head2 incidentid

Type: belongs_to

Related object: L<Watchman::Schema::Result::Incident>

=cut

__PACKAGE__->belongs_to(
  "incidentid",
  "Watchman::Schema::Result::Incident",
  { id => "incidentid" },
  { is_deferrable => 1, on_delete => "CASCADE", on_update => "RESTRICT" },
);

=head2 unitid

Type: belongs_to

Related object: L<Watchman::Schema::Result::Unit>

=cut

__PACKAGE__->belongs_to(
  "unitid",
  "Watchman::Schema::Result::Unit",
  { id => "unitid" },
  { is_deferrable => 1, on_delete => "CASCADE", on_update => "RESTRICT" },
);


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:QLyXdJowiRUpUYeNAWo3lA


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
