use utf8;
package Watchman::Schema::Result::Setting;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::Setting

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<Settings>

=cut

__PACKAGE__->table("Settings");

=head1 ACCESSORS

=head2 name

  data_type: 'varchar'
  is_nullable: 0
  size: 45

=head2 descr

  data_type: 'varchar'
  is_nullable: 0
  size: 128

=head2 category

  data_type: 'varchar'
  is_nullable: 0
  size: 16

=head2 setting

  data_type: 'varchar'
  is_nullable: 1
  size: 255

=cut

__PACKAGE__->add_columns(
  "name",
  { data_type => "varchar", is_nullable => 0, size => 45 },
  "descr",
  { data_type => "varchar", is_nullable => 0, size => 128 },
  "category",
  { data_type => "varchar", is_nullable => 0, size => 16 },
  "setting",
  { data_type => "varchar", is_nullable => 1, size => 255 },
);

=head1 PRIMARY KEY

=over 4

=item * L</name>

=back

=cut

__PACKAGE__->set_primary_key("name");


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:SNK3nq2XrJUcId9KvDEZbw


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
