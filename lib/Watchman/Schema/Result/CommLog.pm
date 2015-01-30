use utf8;
package Watchman::Schema::Result::CommLog;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::CommLog

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<CommLogs>

=cut

__PACKAGE__->table("CommLogs");

=head1 ACCESSORS

=head2 objid

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 timestamp

  data_type: 'timestamp'
  datetime_undef_if_invalid: 1
  default_value: current_timestamp
  is_nullable: 0

=head2 type

  data_type: 'integer'
  default_value: 1
  is_nullable: 0

=head2 activatezone

  data_type: 'tinyint'
  default_value: 1
  is_nullable: 0

=head2 ack

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=head2 cleared

  data_type: 'tinyint'
  default_value: 0
  is_nullable: 0

=cut

__PACKAGE__->add_columns(
  "objid",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "timestamp",
  {
    data_type => "timestamp",
    datetime_undef_if_invalid => 1,
    default_value => \"current_timestamp",
    is_nullable => 0,
  },
  "type",
  { data_type => "integer", default_value => 1, is_nullable => 0 },
  "activatezone",
  { data_type => "tinyint", default_value => 1, is_nullable => 0 },
  "ack",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
  "cleared",
  { data_type => "tinyint", default_value => 0, is_nullable => 0 },
);

=head1 PRIMARY KEY

=over 4

=item * L</objid>

=back

=cut

__PACKAGE__->set_primary_key("objid");


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:eAoMlk7Zaj96GJkLJaoUZg


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
