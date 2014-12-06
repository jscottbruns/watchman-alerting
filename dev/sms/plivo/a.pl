#!/usr/bin/perl
use POSIX;
use DBIx::Connector;
use Exception::Class::DBI;
use Data::Dumper;

my $_dbh = DBIx::Connector->new(
    'dbi:mysql:ICAD_MASTER', 
    'root', 
    '',
    {
        PrintError => 0,
        RaiseError => 0,
        HandleError => Exception::Class::DBI->handler
    }
) or die "Can't connect";

my $lvn_pool = [];

eval {
    $lvn_pool = $_dbh->run( sub {
        return $_->selectall_arrayref(
            qq{
                SELECT Number
                FROM SmsNumberPool
                WHERE Provider = 'plivo' AND Inactive = 0
            }
        );
    } )
};      

if ( my $ex = $@ ) { print "Database exception received during URL forward map statement prepare " . $ex->error; exit; }

my $dest = [];
my $t = $ARGV[0] || 10;
for ( 1..$t )
{
    push @{ $dest }, "+1443-$_";
}

my $total_lvn = scalar @{ $lvn_pool };
my $total_dst = scalar @{ $dest };
my $lvn_groups = [];

foreach ( @{ $lvn_pool } )
{
    push @{ $lvn_groups }, 
        {
            'lvn'   => $_->[0],
            'dst'   => []
        };
}

$groups = ceil( $total_dst / $total_lvn );
$groups = 1 if $groups < 1;

print "Total Groups: [$groups]\n";

my $k = 0;
my $j = 0;

foreach my $_recip ( @{ $dest } )
{
    $j++;
    push @{ $lvn_groups->[ $k ]->{'dst'} }, $_recip;

    if ( $j >= $groups )
    {
        $j = 0;
        $k++;
    }
}

foreach my $_lvn ( @{ $lvn_groups } )
{
    print "Delivery to LVN [$_lvn->{'lvn'}]\n";
    print "Recipient List (" . scalar( @{ $_lvn->{'dst'} } ) . "): " . join( '<', @{ $_lvn->{'dst'} } ) . "\n\n";
}
print Dumper( $lvn_groups );

sub ex
{
    my $ex = shift;
    my $err = $ex->error;
    my $state = $ex->state;
    
    $err =~ s/\n//g;
    $err =~ s/\s{3,}/ /g;
    
    return ( $ex->can('error') ? "($state) $err" : $ex );
}
