#!perl
package Watchman::HTTP;

$| = 1;

use Watchman::HTTP::Daemon;
use Watchman::HTTP::Content;

use Date::Format;
use CGI qw( :standard );
use Win32::GUI();

$CR = "\015";
$LF = "\012";
$CRLF = "\015\012";
$SEP = '/';

sub new {
    my $this = shift;
    my %params = @_;

    my $class = ref($this) || $this;
    my $self = {};

    $self->{'version'}          =   $::VERSION;
    $self->{'path'}             =   $::PATH;
    $self->{'debug'}            =   $::DEBUG;
    $self->{'temp_dir'}         =   $::TEMP_DIR;
    $self->{'doc_root'}         =   "$self->{path}\\httpd\\www";
    $self->{'port'}             =   $params{'port'} || '7609';
    $self->{'host'}             =   $params{'host'} || 'localhost';

    $self->{'doc_root'} =~ s/\\/\//g; # Format the path for web standard

    mkdir( $self->{'temp_dir'} ) if ! -d $self->{'temp_dir'};

    $self->{'db_file'}          =   "$::PATH\\$::DB_FILE";
    $self->{'dbh'} = DBI->connect(
        "dbi:SQLite:dbname=$self->{db_file}",
        "",
        "",
        {
           ShowErrorStatement => 1,
           PrintError => 1
        }
    );
    unless ( $self->{'dbh'} ) {

        &main::write_log("Error connecting to database: " . $DBI::errStr, 1 );
        return undef;
    }

    my ( %row );

    # Load the system settings
    my $sth = $self->{'dbh'}->prepare("SELECT *
                                       FROM settings") or die "DBH Query Error: " . $DBI::errStr;
    if ( $sth->execute ) {

        $sth->bind_columns( \( @row{ @{$sth->{NAME} } } ) );
        while ( $sth->fetch ) {
            $self->{'settings'}->{ $row{'category'} }->{ $row{'name'} } = $row{'setting'};
        }
    }

    $self->{'dispatch'} = {
        'index.htm'    =>  \&resp_index,
        'index.cgi'    =>  \&doit_index
    };

	$self->{'mime'} = {
        'js'    =>  'application/x-javascript',
        'gz'    =>  'application/x-gzip',
        'zip'   =>  'application/zip',
        'bin'   =>  'application/octet-stream',
        'dms'   =>  'application/octet-stream',
        'lha'   =>  'application/octet-stream',
        'lzh'   =>  'application/octet-stream',
        'exe'   =>  'application/octet-stream',
        'class' =>  'application/octet-stream',
        'mpga'  =>  'audio/mpeg',
        'mp2'   =>  'audio/mpeg',
        'mp3'   =>  'audio/mpeg',
        'aif'   =>  'audio/x-aiff',
        'aiff'  =>  'audio/x-aiff',
        'aifc'  =>  'audio/x-aiff',
        'ram'   =>  'audio/x-pn-realaudio',
        'rm'    =>  'audio/x-pn-realaudio',
        'ra'    =>  'audio/x-realaudio',
        'wav'   =>  'audio/x-wav',
        'bmp'   =>  'image/bmp',
        'gif'   =>  'image/gif',
        'jpeg'  =>  'image/jpeg',
        'jpg'   =>  'image/jpeg',
        'jpe'   =>  'image/jpeg',
        'png'   =>  'image/png',
        'css'   =>  'text/css',
        'html'  =>  'text/html',
        'htm'   =>  'text/html',
        'asc'   =>  'text/plain',
        'txt'   =>  'text/plain',
        'rtf'   =>  'text/rtf',
        'qt'    =>  'video/quicktime',
        'mov'   =>  'video/quicktime',
        'avi'   =>  'video/x-msvideo',
	};

	$self->{'_methods'} = {
		'GET'     =>  1,
		'POST'    =>  1,
		'HEAD'    =>  1,
	};

    bless $self, $class;

    my $sth = $self->{'dbh'}->prepare("SELECT *
                                       FROM callGroup") or die "DBH Query Error: " . $DBI::errStr;
    if ( $sth->execute ) {

        $sth->bind_columns( \( @row{ @{$sth->{NAME} } } ) );
        while ( $sth->fetch ) {
            $self->{'callGroup'}->{ $row{'callGroup'} } = {
                'alertColor'       =>  $row{'alertColor'},
                'audioAnnounce'    =>  $row{'audioAnnounce'},
                'audioAlert'       =>  $row{'audioAlert'},
            };
            $self->{'callGrouping'}{ $row{'callGroup'} } = [];
        }
    }

    my $sth = $self->{'dbh'}->prepare("SELECT *
                                       FROM callType t1
                                       ORDER BY category, name ASC") or die "DBH Query Error: " . $DBI::errStr;
    if ( $sth->execute ) {

        $sth->bind_columns( \( @row{ @{$sth->{NAME} } } ) );
        while ( $sth->fetch ) {
            $self->{'callTypeLabel'}->{ $row{'code'} } = $row{'name'};
            $self->{'callType'}->{ $row{'code'} } = {
                'category'      =>  $row{'category'},
                'name'          =>  $row{'name'},
                'priority'      =>  $row{'priority'},
                'alertColor'    =>  $row{'alertColor'},
                'audioAnnounce' =>  $row{'audioAnnounce'}
            };
            push( @{ $self->{'callGrouping'}{ $row{'category'} } }, $row{'code'});
        }
    }

    sub callType { $self->{'callType'} };
    sub callGroup { $self->{'callGroup'} };
    sub callGrouping { $self->{'callGrouping'} };
    sub callTypeLabel { $self->{'callTypeLabel'} };
    sub settings { $self->{'settings'} };
    sub DBH { $self->{'dbh'} };
    sub __GET__ { $self->{'__GET__'} };
    sub __POST__ { $self->{'__POST__'} };

    $self->init_http;
}

sub init_http
{
    my $self = shift;

    &main::write_log("Initializing HTTP daemon\n");

    $self->{'http'} = new Watchman::HTTP::Daemon(
        LocalPort   =>  $self->{'port'},
        LocalAddr   =>  'localhost',
        Listen      =>  5,
        Reuse       =>  1
    );
    print "Listening on localhost:" . $self->{'port'} . "\n";

    while ( my $new_sock = $self->{'http'}->accept() ) {

        $new_sock->timeout( 5 );
        while ( my $request_obj = $new_sock->get_request() ) {

            &main::write_log("New request connection initiated\n") if $self->{'debug'};
            &main::write_log("Connection timeout=" . $new_sock->timeout()) if $self->{'debug'};
            &main::write_log("Request Headers:\n" . $request_obj->method . " " . $request_obj->as_string()) if $self->{'debug'};

            undef $self->{'__GET__'};
            if ( uc $request_obj->method eq 'GET' && $request_obj->uri =~ /\?(.*)$/ ) {

                $self->{'__GET__'} = {};

            	my @q_list = split /&/, $1;
            	foreach my $pair ( @q_list ) {

                    my ($key, $value) = split /=/, $pair;
                    $value =~ tr/+/ /;
                    $value =~ s/%([\dA-Fa-f][\dA-Fa-f])/pack ("C", hex ($1))/eg;

                    $self->{'__GET__'}->{ Watchman::HTTP::Content::URLDecode( $key ) } = Watchman::HTTP::Content::URLDecode( $value );
            	}
            }

            undef $self->{'__POST__'};
		    if ( uc $request_obj->method eq 'POST' ) {

		        $self->{'__POST__'} = {};
		        my @q_list = split /&/, $request_obj->content;
		        foreach my $pair ( @q_list ) {

		            my ($key, $value) = split /=/, $pair;
		            $value =~ tr/+/ /;
		            $value =~ s/%([\dA-Fa-f][\dA-Fa-f])/pack ("C", hex ($1))/eg;

		            $self->{'__POST__'}->{ Watchman::HTTP::Content::URLDecode( $key ) } = Watchman::HTTP::Content::URLDecode( $value );
		        }
		    }

            my $connection = 'close';  # Default to 'close'
            if ( defined $self->{ '_methods' }{ $request_obj->method } ) {

                my $resp_headers = $self->dispatch($request_obj, $new_sock);

                # Send the response
                $resp_headers->header('Location', 'http://www.google.com/') if ( $request_obj->url->path eq 'index.cgi' );
                $new_sock->send_response( $resp_headers );
                $connection = $resp_headers->header('Connection') || $request_obj->header('Connection') || 'close';

                $resp_headers->content('');
                &main::write_log("Headers sent: " . $request_obj->url->path . "\n" . $resp_headers->as_string) if $self->{'debug'};

            } else {

                log_req( "[" . Date::Format::time2str("%a %b %d %Y %T", time) . "] \"" . uc( $request_obj->method ) . " " . $request_obj->url->path . "\" 405 Method Not Allowed" );
                $new_sock->send_error(405, "<br/>$CRLF Method Not Allowed <br/>$CRLF" . $self->page_footer);
            }

            &main::write_log("Connection=$connection, req proto=". $request_obj->protocol) if $self->{'debug'};

            last if ( $connection eq 'close' );
            &main::write_log("End request - waiting for next request on connection=$connection") if $self->{'debug'};
        }

        &main::write_log("Connection has been closed") if $self->{'debug'};

        close( $new_sock ) if ( defined $new_sock );
        undef $new_sock;
    }

    close( $server );
}

sub dispatch
{
    my ($self, $req_head, $sock) = @_;

    my $meth = $req_head->method;
    return undef unless ( defined $self->{'_methods'}{ $meth } );

    my ( $host, $port ) = split(':', ( $req_head->header('Host') || '' ), 2);
    $host ||= $self->{'host'};

    my $resp_head = $self->new_response( $req_head->url->path );

    my $peer = $sock->peername();
    my ( $peer_port, $peer_iaddr ) = unpack_sockaddr_in( $peer );
    $req_head->header('Peer_addr', inet_ntoa( $peer_iaddr ));

    log_req( inet_ntoa( $peer_iaddr ) . " - [" . Date::Format::time2str("%a %b %d %Y %T", time) . "] \"" . uc( $meth ) . " " . $req_head->url->path . "\" " );

    my $text = $self->load_page($req_head, $resp_head, $peer);

    $resp_head->content_length( length( $text ) );
    $resp_head->content( $text ) if $text;

    if ( $meth eq 'HEAD' ) {

        $resp_head->header('Connection', 'close');
        $resp_head->content( undef );
    }

    return $resp_head;
}

sub load_page
{
    my ($self, $req_head, $resp_head, $peer) = @_;
    local (*IN);

    my $query_string = $req_head->uri->equery || '';
    my $page = $req_head->url->path;

    $page =~ s!^/!!;
    $page =~ s!/!$SEP!g;
    $page = 'index.htm' if ! $page;

    my $req_page_ext = scalar reverse substr( reverse( $req_page ), 0, 3 );

    my $content = "";
    my $handler = $self->{'dispatch'}->{ $page };

    my $cgi = CGI->new();
    if ( defined $handler && ref($handler) eq "CODE" ) {

        $content = $handler->( $self, $cgi, $req_head);

        $resp_head->code('200');
        $resp_head->message('OK');
        $resp_head->content_type( $self->{'mime'}->{ $req_page_ext } );

        log_req( "200 OK\n" );

    } elsif ( -f "$self->{doc_root}/$page" ) {

        open(IN, "< $self->{doc_root}/$page");
        my $is_text = ( -T "$self->{doc_root}/$page" );
        if ( ! $is_text ) {
            binmode IN ;
            binmode STDOUT;
        }

        if ( $is_text ) {
            $content = join('', <IN>);
        } else {
            my $buf;
            while ( sysread(IN, $buf, 1024) ) {
                $content .= $buf;
            }
        }
        close( IN );

        if ( $content ) {

	        $content = $self->filter_headers($content, $resp_head) if defined $handler;

	        $resp_head->code('200');
	        $resp_head->message('OK');
	        $resp_head->content_type( $self->{'mime'}->{ $req_page_ext } );

	        log_req( "200 OK\n" );

        } else {

            $resp_head->code('500');
            $resp_head->message('Internal Server Error');

            log_req( "500 Internal Server Error\n" );

        }

    } else {

        $resp_head->code('404');
        $resp_head->message('Not Found');
        $resp_head->content_type( $self->{'mime'}->{ 'htm' } );

        $content .= start_html('Firehouse Watchman :: 404 - Page not found');
        $content .= h1( { -style => "margin-top:5px;" }, 'Page not found');
        $content .= div( { -style => "margin-bottom:15px;" }, "Unable to process request for $page. Please try again");
        $content .= $self->page_footer,
        $content .= end_html;

        log_req( "400 Not Found\n" );
    }

    return $content;
}

sub new_response
{
    my ($self, $req_page) = @_;

    require HTTP::Response;

    my $resp_head = new HTTP::Response;

    $resp_head->protocol('HTTP/1.1');
    $resp_head->date(time);
    $resp_head->server( &Watchman::HTTP::Daemon::product_tokens( $self->{'version'} ) );
    $resp_head->header('Connection', 'close');

    return $resp_head;
}

sub filter_headers
{
    my ($self, $text, $headers) = @_;

    $text =~ s/^\s+//s;
    my ($top, @lines) = split(/$CR?$LF$CR?$LF/, $text, 2);
    return $text unless ( defined $top );
    $top =~ s/\s+$//s;

    &main::write_log("Filtering headers: top=**$top**") if $self->{'debug'};
    while ( $top =~ s/^([A-Za-z][\w\-]+):(.*)$//om ) {
        my ($key, $val) = ($1, $2);
        $val =~ s/$CR?$LF//o;
        $val =~ s/^\s+//o;

        # Cookies are concatted together
        if ( $key =~ /^set.cookie/i && $headers->header($key) ) {
            $headers->header($key, [$headers->header($key), $val]);
        } else {
            $headers->remove_header($key) if ( defined $headers->header($key) );
            $headers->header($key, $val);
        }
    }

    # Check if we are given a status code
    if ( $headers->header('Status') ) {
        my ($code, $mess) = split(/\s+/, $headers->header('Status'), 2);
        $headers->code( $code );
        $headers->message( $mess );
    }
    &main::write_log("Filtered headers: headers=**" . $headers->as_string . "**") if $self->{'debug'};

    $text = join("$CRLF", @lines) . "$CRLF";
    $text =~ s/^\s+//s;
    $text = $top . "$CRLF" . $text if $top;

    &main::write_log("Filtered headers: after filter, text=**$text**") if $self->{'debug'};
    return $text;
}

sub resp_index
{
    my ($self, $cgi, $req_head) = @_;

    my (%callTypeLabels, %device_hash);
    my (@callTypes, @device_list);

    my ($tbl_f, $tbl_l, $tbl_r, $tbl_e);
    my (%row, %callGroup);
    my $optGroups;

    return
    $self->index_head( $cgi ).
	start_form(
	    -name      =>  'watchman_form',
	    -id        =>  'watchman_form',
	    -method    =>  'post',
	    -action    =>  'index.cgi',
	    -enctype   =>  'application/x-www-form-urlencoded'
	).
    div(
        { -style =>  "margin:15px;" },
        table(
            {
                -cellspacing   =>  2,
                -cellpadding   =>  0,
                -width         =>  '100%',
                -border        =>  0
            },
            Tr(
                td(
                    {
                        -style  =>  "vertical-align:top",
                        -width  =>  '5%'
                    },
                    h3(
                        { -style => "margin-bottom:5px; color:#000;margin-top:0px;" },
                        "Firehouse Watchman"
                    ),
                ),
                td( "&nbsp;" ),
                td(
                    { -style => "white-space: nowrap" },
                    table(
                        { -class => "shaded-header" },
                        Tr(
                            td(
                                { -class => "header-element b expand" },
                                "System Settings"
                            ),
                            td(
                                {
                                    -class => "header-element s",
                                    -style => "white-space: nowrap; padding-top:8px;",
                                },
                                submit(
	                                -name => 'savebtn',
	                                -value => 'Save Settings',
	                                -class => 'cl_button'
	                            ),
                            ),
                        ),
                    ),
                ),
            ),
        ),
    ).
    ul(
        {
        	-id    =>  "mainnav",
        	-class =>  "shadetabs",
        },
        li( a( { -href => "#", -rel => "system", -class => "selected"}, "System Settings" ) ),
        li( a( { -href => "#", -rel => "calltypes" }, "Call Groups/Types" ) ),
        li( a( { -href => "#", -rel => "alerting" }, "Alert Settings" ) ),
        li( a( { -href => "#", -rel => "notification" }, "Text & Pager Notifications" ) ),
        li( a( { -href => "#", -rel => "preinc" }, "Pending Incident Settings" ) ),
        li( a( { -href => "#", -rel => "rss" }, "RSS Incident Ticker" ) ),
    ).
    div(
        {
        	-style =>  "border:1px solid gray; width:95%; margin-bottom: 1em; padding: 10px; margin-left:15px;"
        },
        Watchman::HTTP::Content->system_settings( $cgi ),
        Watchman::HTTP::Content->call_types( $cgi ),
        Watchman::HTTP::Content->alert_settings( $cgi ),
        Watchman::HTTP::Content->notification( $cgi ),
        Watchman::HTTP::Content->preinc_settings( $cgi ),
        Watchman::HTTP::Content->rss( $cgi ),
    ) .
    $self->page_footer .
    end_html;
}

sub index_head
{
	my $self = shift;
	my $cgi = shift;

	return start_html (
        -title  =>  'Firehouse Watchman :: Control Center',
        -script =>  $self->index_jscript,
        -style  =>  $self->index_css,
        -onLoad =>  "init_tabcontent();init_validator('watchman_form');",
    );
}

sub index_jscript
{
    return [
        {
            -type  =>  'text/javascript',
            -src   =>  'js/jquery-1.4.2.min.js',
        },
        {
        	-type  =>  'text/javascript',
        	-src   =>  'js/jquery-ui-1.8.6.custom.min.js',
        },
        {
        	-type  =>  'text/javascript',
        	-src   =>  'js/tabcontent.js',
        },
        {
            -type  =>  'text/javascript',
            -src   =>  'js/watchman.js',
        },
        {
            -type  =>  'text/javascript',
            -src   =>  'js/slider.js',
        },
        {
            -type  =>  'text/javascript',
            -src   =>  'js/validation.js',
        }
    ];
}

sub index_css
{
	my $css = <<CSS;
body {
  margin-left: 2em; margin-right: 2em;
  font-family: arial,sans-serif;
  color:#000; background-color:#fff;
}
a:active { color:#000 }
a:visited { color:#000 }
a:link { color:#000 }
.shaded-header { background-image: url(images/shadeactive.gif); margin: 0px; padding: 0px }
.shaded-subheader { background-color: #CCE5ED; margin: 12px 0px 0px 0px; padding: 0px }
.plain-subheader { background-color: #fff; margin: 12px 0px 0px 0px;padding: 0px }
.header-element { margin: 0px; padding: 2px}
.s { font-size: smaller }
.b { font-weight: bold }
.i { font-style: italic }
.expand { width: 98% }
.left { margin-left:25px; }
.bt { margin-bottom:5px; }
.tp { margin-top:5px; }
.section { background-image: url(images/shadeinvert.gif); padding-bottom:4px; width:95%; margin-bottom:10px; margin-top:10px; }
.settings { width:95%; background-color:#cccccc; }
.settings td { background-color:#ffffff; vertical-align:top; }
.settings td.qu { width:25%; text-align:right; padding:8px 3px 5px 5px; }
.settings td.an { text-align:left;  padding:5px; }
.calltype { background-color:#cccccc; width:95%; margin-bottom:10px; margin-left:25px; font-size:95%; }
.calltype td.hd { background-color:#efefef; font-weight:bold; }
.calltype td.rw { background-color:#ffffff; }
.calltype td.inactive { background-color:#D9D9D9; font-style: italic; color: #8f8f8f }
.rt { text-align:right }
.lft { text-align:left }
fieldset { padding:15px 10px; width:70%; }
.cl_button { font: 11px helvetica, tahoma, verdana, sans serif; border: solid 1px #666699; font-weight:bold; cursor:pointer;padding:5px;}
CSS

    return {
    	-src   =>  [ 'css/ui-lightness/jquery-ui-1.8.6.custom.css', 'css/tabcontent.css', 'css/validation.css' ],
    	-code  =>  $css
    };
}

sub page_footer
{
    my $self = shift;

    return "<hr>$CRLF<div style=\"font-weight:bold;font-size:12px;\">" . &Watchman::HTTP::Daemon::product_tokens( $self->{'version'} ) . "</div>$CRLF";
}

sub log_req
{
	my $msg = shift;

	open(REQ, ">>http.log");
	print REQ "$msg";
	close REQ;
}

sub select_file
{
	my $self = shift;
    my %p = @_;
    my ( @file, $file, $title );

    my $multi = $p{'multi'} || 0;
    my $ftype = $p{'ftype'};
    my $lastfile = $p{'lastfile'};
    my $directory = $p{'directory'} || $self->{'path'};

    my @filter;
    if ( $ftype eq 'img' ) {
        @filter = [
            'BMP - Windows Bitmap', '*.bmp',
            'GIF - Graphics Interchange Format', '*.gif',
            'JPG - Joint Photographics Experts Group', '*.jpg',
            'JPG - Joint Photographics Experts Group', '*.jpeg'
        ];
        $title = 'Select Image File';
    } elsif ( $ftype eq 'aud' ) {
        @filter = [
            'WAV - Windows Audio Files', '*.wav',
        ];
        $title = 'Select Audio File';
    } else {
        @filter = [
            'All Files - *', '*'
        ];
        $title = 'Select a File';
    }

    my ( @parms );
    push @parms,
        -multisel => $multi,
        -filter => @filter,
        -directory => $directory,
        -title => $title;
    push @parms, -file => $lastfile  if $lastfile;

    my $DOS = Win32::GUI::GetPerlWindow();
    Win32::GUI::Hide($DOS);

    @file = Win32::GUI::GetOpenFileName ( @parms );

    return @file;
}

sub doit_index
{
    my ($self, $cgi, $req_head) = @_;

    my $_GET = $self->{'__GET__'};
    my $_POST = $self->{'__POST__'};

    if ( $_POST->{'savebtn'} ) {

        my $post = {};
        for my $_el ( sort keys %{ $_POST } ) {
            if ( $_el =~ /^([a-zA-Z]*)\|(.*)$/g ) {

                my $cat = $1;
                my $key = $2;
                if ( $cat =~ /^system(:edit)?/ ) {
                    $post->{ $key } = $_POST->{ $_el };
                }
            }
        }



        for my $_i ( keys %{ $post } ) {

            #print "UPDATE settings SET setting = '$post->{ $_i }' WHERE name = '$_i' \n";
            #$self->{'dbh'}->do("UPDATE settings SET setting = '$post->{ $_i }' WHERE name = '$_i'") or die "Update error: $DBI::errstr\n";
        }
    }
}

1;