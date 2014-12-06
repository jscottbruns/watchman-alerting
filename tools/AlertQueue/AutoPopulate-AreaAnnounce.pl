#!/usr/bin/perl

use Data::Dumper;
use DBI;
use Exception::Class::DBI;
use File::Spec;

my $path = '/usr/local/watchman-alerting/';
my $audio_dir = $ARGV[0] || die "Failed to specify directory for generated audio files\n";
die "Cannot stat audio directory [" . $path . $audio_dir . "] \n" unless -d File::Spec->catfile($path, $audio_dir);

my $DBH = &init_db;

my $regex_ref = $DBH->selectall_arrayref("SELECT t1.SearchKey, t1.ReplaceKey, t1.Category FROM FormattingRules t1", { Slice=>{} });

my $regex_rules = {};
foreach my $_reg ( @$regex_ref )
{               
    $regex_rules->{ $_reg->{'Category'} } = [] unless defined @{ $regex_rules->{ $_reg->{'Category'} } };
                
    push @{ $regex_rules->{ $_reg->{Category} } }, {
        'search'    => $_reg->{'SearchKey'},
        'replace'   => $_reg->{'ReplaceKey'}
    };                          
}

my $sth2 = $DBH->prepare("UPDATE AlertQueueFilter SET AreaAnnounce = ? WHERE ObjID = ?");
my $sth = $DBH->prepare("SELECT ObjID, BoxArea FROM AlertQueueFilter");
$sth->execute;

my $_dir = File::Spec->catfile($path, $audio_dir);

while ( my $ref = $sth->fetchrow_hashref )
{
	my $boxArea = $ref->{'BoxArea'};
    $box = &formatTextString( $boxArea, 'box' );
    if ( $box )
    {
    	my $fname = "FZ_" . $boxArea . ".wav";
    	my $ssml = "'$box'";
    	`perl /usr/local/watchman-alerting/tools/audio/generate_TTS_utility.pl --ssml=$ssml --dir=$_dir --fname=$fname`;
    	
    	if ( $? == 0 )
    	{
    		print "Updating AlertQueueFilter [$ref->{ObjID}] Area Announcement => [" . File::Spec->catfile($audio_dir, $fname) . "]\n";
    		
    		unless ( $sth2->execute( File::Spec->catfile($audio_dir, $fname), $ref->{'ObjID'}) )
    		{
    			print "Error updating AlertQueueFilter row w/audio result: $DBI::errstr\n";
    		}
    	}
    }
}

sub rtrim($)
{
    my $string = shift;
    $string =~ s/\s+$//;
    return $string;
}

sub ltrim($)
{
    my $string = shift;
    $string =~ s/^\s+//;
    return $string;
}

sub trim($)
{
    my $string = shift;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    return $string;
}

sub formatTextString
{
    my $string = shift;
    my $category = shift;
    
    $string = &trim( $string ); 
    return unless $string;
    
    &log("Applying formatting rules to string [$string] against category [$category]");
    
    foreach my $_reg ( @{ $regex_rules->{ $category } } )
    {   
        my $regex_s = qr/$_reg->{search}/;                      
        if ( $string =~ m/$regex_s/ )
        {           
            my $regex_r = eval $_reg->{replace};
            $string =~ s/$regex_s/$regex_r->()/ei;
        }
    }                       

    return $string;
}

sub log
{
    my $msg = shift;
    print "$msg \n";
}

sub init_db
{
    my $dsn = "dbi:mysql:WatchmanAlerting;socket=/var/lib/mysql/mysql.sock";

    &log("Opening database connection => $dsn");

    my $conn;
    unless (
        $conn = DBI->connect(
            $dsn,
            'root',
            '',
            {
                PrintError  => 0,
                RaiseError  => 0,
                HandleError => Exception::Class::DBI->handler,
                AutoCommit  => 1,
            }
        )
    ) {

        &log("Database connection error: " . $DBI::errstr, E_ERROR);
        die "Database connection error: " . $DBI::errstr;
    }

    return $conn;
}

