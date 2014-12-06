#!perl
BEGIN {

    use constant PATH => $ENV{WATCHMAN_HOME};
    use constant TEMP_DIR => "tmp";

    use vars qw( $PATH $TEMP_DIR );
}

if ( ! $ENV{WATCHMAN_HOME} ) {

    open( LOG, ">watchman.log" );
    print LOG "Please make sure that the environment variable %WATCHMAN_HOME% has been defined and is pointing to the installation directory\n\n";
    close LOG;

    Win32::MsgBox("Please make sure that the environment variable %WATCHMAN_HOME% has been defined and is pointing to the installation directory");
    exit;
}

$PATH = PATH;
$TEMP_DIR = "$PATH\\" . TEMP_DIR;
$DB_FILE = 'watchman.s3db';
print $DB_FILE;
use DBI;
use Date::Format;

&main::init_db;

sub init_db
{
    my $db_file = $DB_FILE;
    my $new_db = 1 if ! -f "$PATH\\$db_file";

    &main::write_log("Establishing connection to database ... dbi:SQLite:dbname=\"$PATH\\$db_file\" \n");
    $dbh = DBI->connect(
        "dbi:SQLite:dbname=$PATH\\$db_file",
        "",
        "",
        {
           ShowErrorStatement => 1,
           PrintError => 1
        }
    );
    unless ( $dbh ) {

        &main::write_log( "Error connecting to database: " . $DBI::errStr );
        Win32::MsgBox("The Watchman was unable to connect to the database. This may be due an incomplete installation or incorrect file permissions. Please close the Watchman and try again.");
        exit;
    }

    # If the new db flag was set, import the
    if ( $new_db ) {

        &main::write_log("First run detected, importing database schema\n");
        eval { system("sqlite3 $PATH\\$db_file \".read sqlite3/schema.sql\"") };
        if ( $@ || $DBI::errStr ) {

            &main::write_log("Error creating database tables 'sqlite3 $PATH\\$db_file \".read schema.sql\"': " . ( $DBI::errStr ? $DBI::errStr : $@ ) . "\n", 1);
            Win32::MsgBox("An error was encountered when attempting to import the system database. This may be due an incomplete installation or incorrect file permissions. Please close the Watchman and try again.");
            exit;
        }
    }
}

sub write_log
{
    my $msg = shift;
    my $err = shift;
    my ($package, $file, $line) = caller;

    $msg =~ s/\n$//;
    $msg .= " (" . $$ . ")" if substr($$, 0, 1) eq '-';
    $msg = "[" . Date::Format::time2str('%C', time) . "] [" . $package . ":" . $line . "] " . ($err ? "[error] " : undef) . " $msg\n";

    unless ( $err ) { print STDOUT $msg }
    else { print STDOUT $msg }
}

sub trim
{
    my $string = shift;
    if ( $string ) {
        $string =~ s/^\s+//;
        $string =~ s/\s+$//;
        return $string;
    }
}