use utf8;
package Watchman::Schema::Result::AlertGroup;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::AlertGroup

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<AlertGroups>

=cut

__PACKAGE__->table("AlertGroups");

=head1 ACCESSORS

=head2 groupid

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 groupaddr

  data_type: 'varchar'
  is_nullable: 0
  size: 16

=head2 timestamp

  data_type: 'timestamp'
  datetime_undef_if_invalid: 1
  default_value: current_timestamp
  is_nullable: 0

=head2 groupname

  data_type: 'varchar'
  is_nullable: 0
  size: 24

=head2 grouptype

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=head2 unitid

  data_type: 'integer'
  is_foreign_key: 1
  is_nullable: 1

=head2 alertqueue_prialert

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=head2 alertqueue_localalert

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=head2 alertqueue_filter

  data_type: 'varchar'
  is_nullable: 1
  size: 255

=head2 audiopreamble

  data_type: 'varchar'
  is_nullable: 1
  size: 255

=head2 toneid

  data_type: 'integer'
  is_foreign_key: 1
  is_nullable: 1

=cut

__PACKAGE__->add_columns(
  "groupid",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "groupaddr",
  { data_type => "varchar", is_nullable => 0, size => 16 },
  "timestamp",
  {
    data_type => "timestamp",
    datetime_undef_if_invalid => 1,
    default_value => \"current_timestamp",
    is_nullable => 0,
  },
  "groupname",
  { data_type => "varchar", is_nullable => 0, size => 24 },
  "grouptype",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
  "unitid",
  { data_type => "integer", is_foreign_key => 1, is_nullable => 1 },
  "alertqueue_prialert",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
  "alertqueue_localalert",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
  "alertqueue_filter",
  { data_type => "varchar", is_nullable => 1, size => 255 },
  "audiopreamble",
  { data_type => "varchar", is_nullable => 1, size => 255 },
  "toneid",
  { data_type => "integer", is_foreign_key => 1, is_nullable => 1 },
);

=head1 PRIMARY KEY

=over 4

=item * L</groupid>

=back

=cut

__PACKAGE__->set_primary_key("groupid");

=head1 UNIQUE CONSTRAINTS

=head2 C<index_group_addr>

=over 4

=item * L</groupaddr>

=back

=cut

__PACKAGE__->add_unique_constraint("index_group_addr", ["groupaddr"]);

=head1 RELATIONS

=head2 alert_group_members

Type: has_many

Related object: L<Watchman::Schema::Result::AlertGroupMember>

=cut

__PACKAGE__->has_many(
  "alert_group_members",
  "Watchman::Schema::Result::AlertGroupMember",
  { "foreign.groupid" => "self.groupid" },
  { cascade_copy => 0, cascade_delete => 0 },
);

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
    on_delete     => "CASCADE",
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
    on_delete     => "CASCADE",
    on_update     => "RESTRICT",
  },
);


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:ORvddAhSQiyHeXHKy+pOfg


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
