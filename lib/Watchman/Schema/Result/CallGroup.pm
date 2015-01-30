use utf8;
package Watchman::Schema::Result::CallGroup;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::CallGroup

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<CallGroups>

=cut

__PACKAGE__->table("CallGroups");

=head1 ACCESSORS

=head2 id

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 callgroup

  data_type: 'varchar'
  is_nullable: 0
  size: 24

=head2 alertdisplaycolor

  data_type: 'varchar'
  is_nullable: 1
  size: 16

=head2 alertaudioannounce

  data_type: 'varchar'
  is_nullable: 1
  size: 64

=head2 audiopreamble

  data_type: 'varchar'
  is_nullable: 1
  size: 64

=head2 audiopostamble

  data_type: 'varchar'
  is_nullable: 1
  size: 64

=head2 priority

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=cut

__PACKAGE__->add_columns(
  "id",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "callgroup",
  { data_type => "varchar", is_nullable => 0, size => 24 },
  "alertdisplaycolor",
  { data_type => "varchar", is_nullable => 1, size => 16 },
  "alertaudioannounce",
  { data_type => "varchar", is_nullable => 1, size => 64 },
  "audiopreamble",
  { data_type => "varchar", is_nullable => 1, size => 64 },
  "audiopostamble",
  { data_type => "varchar", is_nullable => 1, size => 64 },
  "priority",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
);

=head1 PRIMARY KEY

=over 4

=item * L</id>

=back

=cut

__PACKAGE__->set_primary_key("id");

=head1 UNIQUE CONSTRAINTS

=head2 C<index_call_groups_call_group>

=over 4

=item * L</callgroup>

=back

=cut

__PACKAGE__->add_unique_constraint("index_call_groups_call_group", ["callgroup"]);

=head1 RELATIONS

=head2 call_types

Type: has_many

Related object: L<Watchman::Schema::Result::CallType>

=cut

__PACKAGE__->has_many(
  "call_types",
  "Watchman::Schema::Result::CallType",
  { "foreign.callgroup" => "self.id" },
  { cascade_copy => 0, cascade_delete => 0 },
);


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:HDPC4Z688OJcLuuPRvkaNg


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
