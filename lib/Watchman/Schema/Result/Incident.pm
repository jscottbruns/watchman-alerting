use utf8;
package Watchman::Schema::Result::Incident;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::Incident

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<Incidents>

=cut

__PACKAGE__->table("Incidents");

=head1 ACCESSORS

=head2 id

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 eventno

  data_type: 'varchar'
  is_nullable: 1
  size: 32

=head2 incidentno

  data_type: 'varchar'
  is_nullable: 1
  size: 25

=head2 reportno

  data_type: 'varchar'
  is_nullable: 1
  size: 25

=head2 timestamp

  data_type: 'timestamp'
  datetime_undef_if_invalid: 1
  default_value: current_timestamp
  is_nullable: 0

=head2 entrytime

  data_type: 'datetime'
  datetime_undef_if_invalid: 1
  is_nullable: 1

=head2 opentime

  data_type: 'datetime'
  datetime_undef_if_invalid: 1
  is_nullable: 1

=head2 dispatchtime

  data_type: 'datetime'
  datetime_undef_if_invalid: 1
  is_nullable: 1

=head2 enroutetime

  data_type: 'datetime'
  datetime_undef_if_invalid: 1
  is_nullable: 1

=head2 onscenetime

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
  size: 100

=head2 station

  data_type: 'varchar'
  is_nullable: 1
  size: 6

=head2 boxarea

  data_type: 'varchar'
  is_nullable: 1
  size: 6

=head2 location

  data_type: 'varchar'
  is_nullable: 0
  size: 255

=head2 locationnote

  data_type: 'varchar'
  is_nullable: 1
  size: 255

=head2 crossst1

  data_type: 'varchar'
  is_nullable: 0
  size: 128

=head2 crossst2

  data_type: 'varchar'
  is_nullable: 0
  size: 128

=head2 gpslatitude

  data_type: 'varchar'
  is_nullable: 1
  size: 64

=head2 gpslongitude

  data_type: 'varchar'
  is_nullable: 1
  size: 64

=head2 unitlist

  data_type: 'varchar'
  is_nullable: 1
  size: 255

=head2 comments

  data_type: 'text'
  is_nullable: 1

=cut

__PACKAGE__->add_columns(
  "id",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "eventno",
  { data_type => "varchar", is_nullable => 1, size => 32 },
  "incidentno",
  { data_type => "varchar", is_nullable => 1, size => 25 },
  "reportno",
  { data_type => "varchar", is_nullable => 1, size => 25 },
  "timestamp",
  {
    data_type => "timestamp",
    datetime_undef_if_invalid => 1,
    default_value => \"current_timestamp",
    is_nullable => 0,
  },
  "entrytime",
  {
    data_type => "datetime",
    datetime_undef_if_invalid => 1,
    is_nullable => 1,
  },
  "opentime",
  {
    data_type => "datetime",
    datetime_undef_if_invalid => 1,
    is_nullable => 1,
  },
  "dispatchtime",
  {
    data_type => "datetime",
    datetime_undef_if_invalid => 1,
    is_nullable => 1,
  },
  "enroutetime",
  {
    data_type => "datetime",
    datetime_undef_if_invalid => 1,
    is_nullable => 1,
  },
  "onscenetime",
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
  { data_type => "varchar", is_nullable => 1, size => 100 },
  "station",
  { data_type => "varchar", is_nullable => 1, size => 6 },
  "boxarea",
  { data_type => "varchar", is_nullable => 1, size => 6 },
  "location",
  { data_type => "varchar", is_nullable => 0, size => 255 },
  "locationnote",
  { data_type => "varchar", is_nullable => 1, size => 255 },
  "crossst1",
  { data_type => "varchar", is_nullable => 0, size => 128 },
  "crossst2",
  { data_type => "varchar", is_nullable => 0, size => 128 },
  "gpslatitude",
  { data_type => "varchar", is_nullable => 1, size => 64 },
  "gpslongitude",
  { data_type => "varchar", is_nullable => 1, size => 64 },
  "unitlist",
  { data_type => "varchar", is_nullable => 1, size => 255 },
  "comments",
  { data_type => "text", is_nullable => 1 },
);

=head1 PRIMARY KEY

=over 4

=item * L</id>

=back

=cut

__PACKAGE__->set_primary_key("id");

=head1 UNIQUE CONSTRAINTS

=head2 C<index_event_no>

=over 4

=item * L</eventno>

=back

=cut

__PACKAGE__->add_unique_constraint("index_event_no", ["eventno"]);

=head2 C<index_incident_no>

=over 4

=item * L</incidentno>

=back

=cut

__PACKAGE__->add_unique_constraint("index_incident_no", ["incidentno"]);

=head1 RELATIONS

=head2 incident_units

Type: has_many

Related object: L<Watchman::Schema::Result::IncidentUnit>

=cut

__PACKAGE__->has_many(
  "incident_units",
  "Watchman::Schema::Result::IncidentUnit",
  { "foreign.incidentid" => "self.id" },
  { cascade_copy => 0, cascade_delete => 0 },
);

=head2 rssfeeds

Type: has_many

Related object: L<Watchman::Schema::Result::Rssfeed>

=cut

__PACKAGE__->has_many(
  "rssfeeds",
  "Watchman::Schema::Result::Rssfeed",
  { "foreign.incidentno" => "self.id" },
  { cascade_copy => 0, cascade_delete => 0 },
);


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:3SI+tNAKVldzYsW3RBU3cA


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
