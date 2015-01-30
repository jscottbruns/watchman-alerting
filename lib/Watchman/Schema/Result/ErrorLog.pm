use utf8;
package Watchman::Schema::Result::ErrorLog;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::ErrorLog

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<ErrorLogs>

=cut

__PACKAGE__->table("ErrorLogs");

=head1 ACCESSORS

=head2 rowid

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 timestamp

  data_type: 'timestamp'
  datetime_undef_if_invalid: 1
  default_value: current_timestamp
  is_nullable: 0

=head2 logfile

  data_type: 'varchar'
  is_nullable: 0
  size: 128

=head2 level

  data_type: 'varchar'
  default_value: 'ERROR'
  is_nullable: 0
  size: 16

=head2 errortext

  data_type: 'varchar'
  is_nullable: 0
  size: 255

=cut

__PACKAGE__->add_columns(
  "rowid",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "timestamp",
  {
    data_type => "timestamp",
    datetime_undef_if_invalid => 1,
    default_value => \"current_timestamp",
    is_nullable => 0,
  },
  "logfile",
  { data_type => "varchar", is_nullable => 0, size => 128 },
  "level",
  {
    data_type => "varchar",
    default_value => "ERROR",
    is_nullable => 0,
    size => 16,
  },
  "errortext",
  { data_type => "varchar", is_nullable => 0, size => 255 },
);

=head1 PRIMARY KEY

=over 4

=item * L</rowid>

=back

=cut

__PACKAGE__->set_primary_key("rowid");


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:MVj8+WoQrCww1njGbVJLbw


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
