use utf8;
package Watchman::Schema::Result::AlertQueueRule;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::AlertQueueRule

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<AlertQueue_Rules>

=cut

__PACKAGE__->table("AlertQueue_Rules");

=head1 ACCESSORS

=head2 ruleid

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 agency

  data_type: 'varchar'
  is_nullable: 1
  size: 20

Optional agency filter, one per row

=head2 boxarea

  data_type: 'varchar'
  is_nullable: 0
  size: 100

Regex formatting box area, one per row

=head2 ruleset

  data_type: 'text'
  is_nullable: 0

Regex formatted call type and/or call group rules to apply to corresponding box area and agency (if not null)

=head2 areaannounce

  data_type: 'varchar'
  is_nullable: 1
  size: 255

=cut

__PACKAGE__->add_columns(
  "ruleid",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "agency",
  { data_type => "varchar", is_nullable => 1, size => 20 },
  "boxarea",
  { data_type => "varchar", is_nullable => 0, size => 100 },
  "ruleset",
  { data_type => "text", is_nullable => 0 },
  "areaannounce",
  { data_type => "varchar", is_nullable => 1, size => 255 },
);

=head1 PRIMARY KEY

=over 4

=item * L</ruleid>

=back

=cut

__PACKAGE__->set_primary_key("ruleid");


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:N6u0F/kYccP5RPbtnnp/kQ


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
