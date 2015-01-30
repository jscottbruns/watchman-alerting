use utf8;
package Watchman::Schema::Result::ICadalertTran;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::ICadalertTran

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<iCADAlertTrans>

=cut

__PACKAGE__->table("iCADAlertTrans");

=head1 ACCESSORS

=head2 localtransid

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 icadtransid

  data_type: 'integer'
  is_nullable: 0

=head2 receivedtime

  data_type: 'timestamp'
  datetime_undef_if_invalid: 1
  default_value: current_timestamp
  is_nullable: 0

=head2 senttime

  data_type: 'datetime'
  datetime_undef_if_invalid: 1
  is_nullable: 0

=head2 streamdata

  data_type: 'text'
  is_nullable: 1

=cut

__PACKAGE__->add_columns(
  "localtransid",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "icadtransid",
  { data_type => "integer", is_nullable => 0 },
  "receivedtime",
  {
    data_type => "timestamp",
    datetime_undef_if_invalid => 1,
    default_value => \"current_timestamp",
    is_nullable => 0,
  },
  "senttime",
  {
    data_type => "datetime",
    datetime_undef_if_invalid => 1,
    is_nullable => 0,
  },
  "streamdata",
  { data_type => "text", is_nullable => 1 },
);

=head1 PRIMARY KEY

=over 4

=item * L</localtransid>

=back

=cut

__PACKAGE__->set_primary_key("localtransid");


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:im9d7PbJAqfJRS22axJhZQ


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
