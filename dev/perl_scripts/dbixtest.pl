#!/usr/bin/perl
use POSIX;
use DBIx::Connector;
use Exception::Class::DBI;
use Data::Dumper;

BEGIN {

    use constant E_ERROR=> 'error';
    use constant E_WARN=> 'warning';
    use constant E_CRIT=> 'critical';
    use constant E_DEBUG=> 'debug';
    use constant E_INFO=> 'info';
}

my $DBH;

unless (
    $DBH = DBIx::Connector->new(
        'dbi:mysql:WatchmanAlerting',
        'root',
        '',
        {
            PrintError=> 0,
            RaiseError=> 0,
            HandleError=> Exception::Class::DBI->handler,
            AutoCommit=> 1,
        }
    )
) {
    die "Database connection error: " . $DBI::errstr;
}

my $units = '231-E17 231-E16 231-E30 231-TK17 231-4032';
my $UnitArray = split ' ', $units;
my $BoxArea = '317';
my $CallType = 'FIRECOM';
my $CallGroup = 'FIRE';

my $prior = {};

eval {
    $prior = $DBH->run( sub {
        my $_ref = $_->selectall_arrayref("Select Name from Settings", { Slice => {} });
        $prior->{ $_->{Name} } = 1 foreach ( @{ $_ref } );
        return $prior;
    } )
    };

    if ( $@ )
    {
        print $@;
    }
print Dumper($prior);
my $i = &indexArray('TTS_Enabled', @incref );
print $i;
sub indexArray
{
    1 while $_[0] ne pop;
    @_-1;
}

sub ex
{
    my $ex = shift;
    my $err = $ex->error;
    my $state = $ex->state;
    
    $err =~ s/\n//g;
    $err =~ s/\s{3,}/ /g;
    
    return ( $ex->can('error') ? "($state) $err" : $ex );
}
