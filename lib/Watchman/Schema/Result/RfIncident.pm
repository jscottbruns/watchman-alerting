use utf8;
package Watchman::Schema::Result::RfIncident;

# Created by DBIx::Class::Schema::Loader
# DO NOT MODIFY THE FIRST PART OF THIS FILE

=head1 NAME

Watchman::Schema::Result::RfIncident

=cut

use strict;
use warnings;

use base 'DBIx::Class::Core';

=head1 TABLE: C<RF_Incidents>

=cut

__PACKAGE__->table("RF_Incidents");

=head1 ACCESSORS

=head2 id

  data_type: 'integer'
  is_auto_increment: 1
  is_nullable: 0

=head2 timestamp

  data_type: 'timestamp'
  datetime_undef_if_invalid: 1
  default_value: current_timestamp
  is_nullable: 0

=head2 toneid

  data_type: 'integer'
  is_foreign_key: 1
  is_nullable: 0

=cut

__PACKAGE__->add_columns(
  "id",
  { data_type => "integer", is_auto_increment => 1, is_nullable => 0 },
  "timestamp",
  {
    data_type => "timestamp",
    datetime_undef_if_invalid => 1,
    default_value => \"current_timestamp",
    is_nullable => 0,
  },
  "toneid",
  { data_type => "integer", is_foreign_key => 1, is_nullable => 0 },
);

=head1 PRIMARY KEY

=over 4

=item * L</id>

=back

=cut

__PACKAGE__->set_primary_key("id");

=head1 RELATIONS

=head2 toneid

Type: belongs_to

Related object: L<Watchman::Schema::Result::RfAlert>

=cut

__PACKAGE__->belongs_to(
  "toneid",
  "Watchman::Schema::Result::RfAlert",
  { id => "toneid" },
  { is_deferrable => 1, on_delete => "CASCADE", on_update => "RESTRICT" },
);


# Created by DBIx::Class::Schema::Loader v0.07042 @ 2015-01-30 10:38:40
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:UITLSQDwR022zNpohwCaFQ


# You can replace this text with custom code or comments, and it will be preserved on regeneration
1;
