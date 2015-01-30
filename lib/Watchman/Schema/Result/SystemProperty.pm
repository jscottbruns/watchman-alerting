use utf8;
package Watchman::Schema::Result::SystemProperty;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::SystemProperty

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<SystemProperties>

=cut

__PACKAGE__->table("SystemProperties");

=head1 ACCESSORS

=head2 propname

  data_type: 'varchar'
  is_nullable: 0
  size: 45

=head2 propvalue

  data_type: 'varchar'
  is_nullable: 1
  size: 64

=cut

__PACKAGE__->add_columns(
  "propname",
  { data_type => "varchar", is_nullable => 0, size => 45 },
  "propvalue",
  { data_type => "varchar", is_nullable => 1, size => 64 },
);

=head1 PRIMARY KEY

=over 4

=item * L</propname>

=back

=cut

__PACKAGE__->set_primary_key("propname");


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:BZINhnfwsMEtsvVHOiJxxw


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
