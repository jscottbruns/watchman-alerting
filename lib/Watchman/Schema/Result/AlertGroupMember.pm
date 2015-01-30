use utf8;
package Watchman::Schema::Result::AlertGroupMember;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::AlertGroupMember

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<AlertGroupMembers>

=cut

__PACKAGE__->table("AlertGroupMembers");

=head1 ACCESSORS

=head2 memberid

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 timestamp

  data_type: 'timestamp'
  datetime_undef_if_invalid: 1
  default_value: current_timestamp
  is_nullable: 0

=head2 groupid

  data_type: 'integer'
  is_foreign_key: 1
  is_nullable: 0

=head2 zoneid

  data_type: 'integer'
  is_foreign_key: 1
  is_nullable: 0

=cut

__PACKAGE__->add_columns(
  "memberid",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "timestamp",
  {
    data_type => "timestamp",
    datetime_undef_if_invalid => 1,
    default_value => \"current_timestamp",
    is_nullable => 0,
  },
  "groupid",
  { data_type => "integer", is_foreign_key => 1, is_nullable => 0 },
  "zoneid",
  { data_type => "integer", is_foreign_key => 1, is_nullable => 0 },
);

=head1 PRIMARY KEY

=over 4

=item * L</memberid>

=back

=cut

__PACKAGE__->set_primary_key("memberid");

=head1 RELATIONS

=head2 groupid

Type: belongs_to

Related object: L<Watchman::Schema::Result::AlertGroup>

=cut

__PACKAGE__->belongs_to(
  "groupid",
  "Watchman::Schema::Result::AlertGroup",
  { groupid => "groupid" },
  { is_deferrable => 1, on_delete => "CASCADE", on_update => "RESTRICT" },
);

=head2 zoneid

Type: belongs_to

Related object: L<Watchman::Schema::Result::AlertZone>

=cut

__PACKAGE__->belongs_to(
  "zoneid",
  "Watchman::Schema::Result::AlertZone",
  { zoneid => "zoneid" },
  { is_deferrable => 1, on_delete => "CASCADE", on_update => "RESTRICT" },
);


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:dcUZhPh+Ix7+ZK7kgrm9gg


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
