use utf8;
package Watchman::Schema::Result::AlertZone;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::AlertZone

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<AlertZones>

=cut

__PACKAGE__->table("AlertZones");

=head1 ACCESSORS

=head2 zoneid

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 zonename

  data_type: 'varchar'
  is_nullable: 0
  size: 64

=head2 timestamp

  data_type: 'timestamp'
  datetime_undef_if_invalid: 1
  default_value: current_timestamp
  is_nullable: 0

=head2 hostaddr

  data_type: 'varchar'
  is_nullable: 0
  size: 16

=head2 ifacehostaddr

  data_type: 'varchar'
  is_nullable: 0
  size: 16

=head2 ifacetcpcontrolport

  data_type: 'varchar'
  is_nullable: 0
  size: 5

=head2 ifaceudpcontrolport

  data_type: 'varchar'
  is_nullable: 0
  size: 5

=head2 ifaceserialport

  data_type: 'varchar'
  is_nullable: 0
  size: 5

=head2 ioresetaddr

  data_type: 'varchar'
  default_value: 1
  is_nullable: 0
  size: 5

=head2 defaultvollevel

  data_type: 'varchar'
  is_nullable: 0
  size: 3

=head2 alertvollevel

  data_type: 'varchar'
  is_nullable: 0
  size: 3

=head2 timeoutmins

  data_type: 'integer'
  default_value: 3
  is_nullable: 0

=head2 autoupdate

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=head2 silentwatch

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=head2 silentstarttime

  data_type: 'varchar'
  is_nullable: 0
  size: 4

=head2 silentendtime

  data_type: 'varchar'
  is_nullable: 0
  size: 4

=head2 silentvollevel

  data_type: 'varchar'
  is_nullable: 0
  size: 3

=head2 troubletimeout

  data_type: 'integer'
  default_value: 3
  is_nullable: 0

=head2 currvollevel

  data_type: 'varchar'
  default_value: 5
  is_nullable: 0
  size: 3

=head2 currstreamno

  data_type: 'varchar'
  is_nullable: 1
  size: 12

=head2 currurl

  data_type: 'varchar'
  is_nullable: 1
  size: 255

=head2 ifacealarm

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=head2 ifaceerror

  data_type: 'integer'
  default_value: 0
  is_nullable: 0

=head2 ifaceuptime

  data_type: 'integer'
  default_value: 0
  is_nullable: 0

=cut

__PACKAGE__->add_columns(
  "zoneid",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "zonename",
  { data_type => "varchar", is_nullable => 0, size => 64 },
  "timestamp",
  {
    data_type => "timestamp",
    datetime_undef_if_invalid => 1,
    default_value => \"current_timestamp",
    is_nullable => 0,
  },
  "hostaddr",
  { data_type => "varchar", is_nullable => 0, size => 16 },
  "ifacehostaddr",
  { data_type => "varchar", is_nullable => 0, size => 16 },
  "ifacetcpcontrolport",
  { data_type => "varchar", is_nullable => 0, size => 5 },
  "ifaceudpcontrolport",
  { data_type => "varchar", is_nullable => 0, size => 5 },
  "ifaceserialport",
  { data_type => "varchar", is_nullable => 0, size => 5 },
  "ioresetaddr",
  { data_type => "varchar", default_value => 1, is_nullable => 0, size => 5 },
  "defaultvollevel",
  { data_type => "varchar", is_nullable => 0, size => 3 },
  "alertvollevel",
  { data_type => "varchar", is_nullable => 0, size => 3 },
  "timeoutmins",
  { data_type => "integer", default_value => 3, is_nullable => 0 },
  "autoupdate",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
  "silentwatch",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
  "silentstarttime",
  { data_type => "varchar", is_nullable => 0, size => 4 },
  "silentendtime",
  { data_type => "varchar", is_nullable => 0, size => 4 },
  "silentvollevel",
  { data_type => "varchar", is_nullable => 0, size => 3 },
  "troubletimeout",
  { data_type => "integer", default_value => 3, is_nullable => 0 },
  "currvollevel",
  { data_type => "varchar", default_value => 5, is_nullable => 0, size => 3 },
  "currstreamno",
  { data_type => "varchar", is_nullable => 1, size => 12 },
  "currurl",
  { data_type => "varchar", is_nullable => 1, size => 255 },
  "ifacealarm",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
  "ifaceerror",
  { data_type => "integer", default_value => 0, is_nullable => 0 },
  "ifaceuptime",
  { data_type => "integer", default_value => 0, is_nullable => 0 },
);

=head1 PRIMARY KEY

=over 4

=item * L</zoneid>

=back

=cut

__PACKAGE__->set_primary_key("zoneid");

=head1 RELATIONS

=head2 alert_group_members

Type: has_many

Related object: L<Watchman::Schema::Result::AlertGroupMember>

=cut

__PACKAGE__->has_many(
  "alert_group_members",
  "Watchman::Schema::Result::AlertGroupMember",
  { "foreign.zoneid" => "self.zoneid" },
  { cascade_copy => 0, cascade_delete => 0 },
);


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:b/dnQVj+NOT/V6PU77zOsg


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
