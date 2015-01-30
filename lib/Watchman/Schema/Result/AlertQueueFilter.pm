use utf8;
package Watchman::Schema::Result::AlertQueueFilter;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::AlertQueueFilter

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<AlertQueueFilters>

=cut

__PACKAGE__->table("AlertQueueFilters");

=head1 ACCESSORS

=head2 id

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 agency

  data_type: 'varchar'
  is_nullable: 1
  size: 20

=head2 boxarea

  data_type: 'varchar'
  is_nullable: 0
  size: 12

One box area per row, regex matching permitted

=head2 calltype

  data_type: 'text'
  is_nullable: 0

Formatting options: (1) Pipe delimited types (i.e. TYPE1|TYPE2|TYPE3 -- regex/wildcard matching: TYPE1.*|TYPE2[0-3] -- prefix optional: type:TYPE1.* ) (2) Pipe delimited groups (i.e. group:LOCAL|group:FIRE -- wildcard matching: group:*)

=head2 areaannounce

  data_type: 'varchar'
  is_nullable: 1
  size: 255

=cut

__PACKAGE__->add_columns(
  "id",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "agency",
  { data_type => "varchar", is_nullable => 1, size => 20 },
  "boxarea",
  { data_type => "varchar", is_nullable => 0, size => 12 },
  "calltype",
  { data_type => "text", is_nullable => 0 },
  "areaannounce",
  { data_type => "varchar", is_nullable => 1, size => 255 },
);

=head1 PRIMARY KEY

=over 4

=item * L</id>

=back

=cut

__PACKAGE__->set_primary_key("id");


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:Qm21bEr2kzjeusA0cxSGoQ


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
