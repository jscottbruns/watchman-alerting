use utf8;
package Watchman::Schema::Result::CallType;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::CallType

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<CallTypes>

=cut

__PACKAGE__->table("CallTypes");

=head1 ACCESSORS

=head2 id

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 typecode

  data_type: 'varchar'
  is_nullable: 0
  size: 12

=head2 callgroup

  data_type: 'integer'
  is_foreign_key: 1
  is_nullable: 0

=head2 label

  data_type: 'varchar'
  is_nullable: 0
  size: 64

=head2 priority

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=head2 alertdisplaycolor

  data_type: 'varchar'
  is_nullable: 1
  size: 16

=head2 alertaudioannounce

  data_type: 'varchar'
  is_nullable: 1
  size: 64

=cut

__PACKAGE__->add_columns(
  "id",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "typecode",
  { data_type => "varchar", is_nullable => 0, size => 12 },
  "callgroup",
  { data_type => "integer", is_foreign_key => 1, is_nullable => 0 },
  "label",
  { data_type => "varchar", is_nullable => 0, size => 64 },
  "priority",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
  "alertdisplaycolor",
  { data_type => "varchar", is_nullable => 1, size => 16 },
  "alertaudioannounce",
  { data_type => "varchar", is_nullable => 1, size => 64 },
);

=head1 PRIMARY KEY

=over 4

=item * L</id>

=back

=cut

__PACKAGE__->set_primary_key("id");

=head1 UNIQUE CONSTRAINTS

=head2 C<index_call_types_type_code>

=over 4

=item * L</typecode>

=back

=cut

__PACKAGE__->add_unique_constraint("index_call_types_type_code", ["typecode"]);

=head1 RELATIONS

=head2 callgroup

Type: belongs_to

Related object: L<Watchman::Schema::Result::CallGroup>

=cut

__PACKAGE__->belongs_to(
  "callgroup",
  "Watchman::Schema::Result::CallGroup",
  { id => "callgroup" },
  { is_deferrable => 1, on_delete => "RESTRICT", on_update => "RESTRICT" },
);

=head2 incidents

Type: has_many

Related object: L<Watchman::Schema::Result::Incident>

=cut

__PACKAGE__->has_many(
  "incidents",
  "Watchman::Schema::Result::Incident",
  { "foreign.calltype" => "self.id" },
  { cascade_copy => 0, cascade_delete => 0 },
);


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:55:30
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:XeKQSo7wiipF5Vs1SrQFIQ


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
