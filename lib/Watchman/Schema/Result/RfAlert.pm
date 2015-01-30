use utf8;
package Watchman::Schema::Result::RfAlert;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::RfAlert

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<RF_Alerts>

=cut

__PACKAGE__->table("RF_Alerts");

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

=head2 io_device

  data_type: 'varchar'
  is_nullable: 0
  size: 10

=head2 io_slot

  data_type: 'integer'
  is_nullable: 0

=head2 io_channel

  data_type: 'integer'
  is_nullable: 0

=head2 tonedescr

  data_type: 'varchar'
  is_nullable: 0
  size: 100

=head2 displaylabel

  data_type: 'varchar'
  is_nullable: 1
  size: 64

=head2 displaycolor

  data_type: 'varchar'
  is_nullable: 1
  size: 16

=head2 audiopreamble

  data_type: 'varchar'
  is_nullable: 1
  size: 255

=head2 audioannouncement

  data_type: 'varchar'
  is_nullable: 1
  size: 64

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
  "io_device",
  { data_type => "varchar", is_nullable => 0, size => 10 },
  "io_slot",
  { data_type => "integer", is_nullable => 0 },
  "io_channel",
  { data_type => "integer", is_nullable => 0 },
  "tonedescr",
  { data_type => "varchar", is_nullable => 0, size => 100 },
  "displaylabel",
  { data_type => "varchar", is_nullable => 1, size => 64 },
  "displaycolor",
  { data_type => "varchar", is_nullable => 1, size => 16 },
  "audiopreamble",
  { data_type => "varchar", is_nullable => 1, size => 255 },
  "audioannouncement",
  { data_type => "varchar", is_nullable => 1, size => 64 },
);

=head1 PRIMARY KEY

=over 4

=item * L</id>

=back

=cut

__PACKAGE__->set_primary_key("id");

=head1 UNIQUE CONSTRAINTS

=head2 C<index_rf_alerts_io_slot_io_channel>

=over 4

=item * L</io_slot>

=item * L</io_channel>

=back

=cut

__PACKAGE__->add_unique_constraint(
  "index_rf_alerts_io_slot_io_channel",
  ["io_slot", "io_channel"],
);

=head1 RELATIONS

=head2 alert_groups

Type: has_many

Related object: L<Watchman::Schema::Result::AlertGroup>

=cut

__PACKAGE__->has_many(
  "alert_groups",
  "Watchman::Schema::Result::AlertGroup",
  { "foreign.toneid" => "self.id" },
  { cascade_copy => 0, cascade_delete => 0 },
);

=head2 io_devices

Type: has_many

Related object: L<Watchman::Schema::Result::IoDevice>

=cut

__PACKAGE__->has_many(
  "io_devices",
  "Watchman::Schema::Result::IoDevice",
  { "foreign.toneid" => "self.id" },
  { cascade_copy => 0, cascade_delete => 0 },
);

=head2 rf_incidents

Type: has_many

Related object: L<Watchman::Schema::Result::RfIncident>

=cut

__PACKAGE__->has_many(
  "rf_incidents",
  "Watchman::Schema::Result::RfIncident",
  { "foreign.toneid" => "self.id" },
  { cascade_copy => 0, cascade_delete => 0 },
);


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:XXLR9kbC6R4Zvwb3/RYwLQ


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
