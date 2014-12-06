#!perl
package Watchman::HTTP::Content;

use base qw(Watchman::HTTP);

use CGI ':standard';
use Date::Format;
use Net::Pcap;
use Win32::TieRegistry( Delimiter=>"/" );
use HTML::Entities;

my @defaultColors = qw( red dimred orange brown yellow amber green dimgreen rainbow1 rainbow2 colormix autocolor );

my @serial_speeds = qw( 75 110 134 150 300 600 1200 1800 2400 4800 7200 9600 14400 19200 38400 57600 115200 128000 );

my @serial_ports;
my $comKey = $Registry->{ "LMachine/Hardware/Devicemap/Serialcomm" };
foreach my $e ( keys %{ $comKey } ) {
    push( @serial_ports, $comKey->{ $e } );
}

my $serial_xsl = {
	'alphasign.xsl'    =>  "BetaBrite",
	'prolite.xsl'      =>  "TruColor XP"
};

my ($error, %descr);
Net::Pcap::findalldevs(\$error, \%descr);
foreach ( keys %descr ) {
    push(@device_list, $_);
    $device_hash->{ $_ } = $descr{ $_ };
}

my ($date_formats, $time_formats);
$date_formats = {
    '%L-%d-%Y'      =>  Date::Format::time2str('%L-%d-%Y', time),
    '%L-%d-%y'      =>  Date::Format::time2str('%L-%d-%y', time),
    '%L/%d/%Y'      =>  Date::Format::time2str('%L/%d/%Y', time),
    '%L/%d/%y'      =>  Date::Format::time2str('%L/%d/%y', time),
    '%L.%d.%Y'      =>  Date::Format::time2str('%L.%d.%Y', time),
    '%L.%d.%y'      =>  Date::Format::time2str('%L.%d.%y', time),
    '%h %o %Y'      =>  Date::Format::time2str('%h %o %Y', time),
    '%a %h %o %Y'   =>  Date::Format::time2str('%a, %h %o %Y', time),
};

$time_formats  = {
    '%I:%M %p'      =>  Date::Format::time2str('%I:%M %p', time),
    '%I:%M:%S %p'   =>  Date::Format::time2str('%I:%M:%S %p', time),
    '%H:%M'         =>  Date::Format::time2str('%H:%M', time) . ( Date::Format::time2str('%H', time) <= 12 ? " (24 Hour)" : undef ),
    '%H:%M:%S'      =>  Date::Format::time2str('%H:%M:%S', time) . ( Date::Format::time2str('%H', time) <= 12 ? " (24 Hour)" : undef ),
};

sub system_settings
{
	my $cgi = shift;

	my $settings = $_[0]->SUPER::settings;
    my $_GET = $_[0]->SUPER::__GET__;
    my $_POST = $_[0]->SUPER::__POST__;
    my $dbh = $_[0]->SUPER::DBH;

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

            $dbh->prepare("UPDATE settings
                           SET setting = ?
                           WHERE name = ? AND category = ?")->execute($post->{ $_i }, $_i, 'system') or die "DBH Update Error: $DBI::errstr\n";
	    }
    }

    my $callTypeLabel = $_[0]->SUPER::callTypeLabel;
    my $callType = $_[0]->SUPER::callType;

    my $html =
    div(
        {
            -id    =>  "system",
            -class =>  "tabcontent",
        },
        div( { -class =>  "section b left" }, "System Settings"),
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "settings left",
            },
            Tr(
                td( { -class => "qu" }, "System Mode:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'system|live',
                        -values     =>  ["1", "0"],
                        -default    =>  $settings->{'system'}->{'live'},
                        -size       =>  -1,
                        -labels     =>  {
                            "1" => "System Live",
                            "0" => "System Offline/Testing",
                        }
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Station Number\:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'system|station',
                        -value  =>  $settings->{'system'}->{'station'},
                        -size   =>  2,
                        -class  =>  "required"
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "License Number:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'system|license',
                        -value  =>  $settings->{'system'}->{'license'},
                        -class  =>  "required"),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Default Signboard:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'system|signboardDisplay',
                        -values     =>  ["msg", "curdate", "curtime", "curdt"],
                        -default    =>  $settings->{'system'}->{'defaultSignboard'},
                        -size       =>  -1,
                        -labels     =>  {
                            "msg"       => "Custom Message",
                            "curdate"   => "Current Date",
                            "curtime"   => "Current Time",
                            "curdt"     => "Current Date/Time",
                        }
                    ),
                    div(
                        { -style => 'margin-top:10px;' },
                        div( { -style => "margin-bottom:3px;" }, "Signboard Message:" ),
                        textfield('system|defaultSignboard', $settings->{'system'}->{'defaultSignboard'})
                    ),
                    div(
                        { -style => 'margin-top:10px;' },
                        div( { -style => "margin-bottom:3px;" }, "Default Date Format:" ),
                        scrolling_list(
                            -name       =>  'system|dateFormat',
                            -values     =>  [ keys %{ $date_formats } ],
                            -default    =>  $settings->{'system'}->{'dateFormat'},
                            -size       =>  -1,
                            -labels     =>  $date_formats,
                        ),
                    ),
                    div(
                        { -style => 'margin-top:10px;' },
                        div( { -style => "margin-bottom:3px;" }, "Default Time Format:" ),
                        scrolling_list(
                            -name       =>  'system|timeFormat',
                            -values     =>  [ keys %{ $time_formats } ],
                            -default    =>  $settings->{'system'}->{'timeFormat'},
                            -size       =>  -1,
                            -labels     =>  $time_formats,
                        ),
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Pending Incident Alerting System: " ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'system|preIncident',
                        -values     =>  ["on", "off"],
                        -default    =>  $settings->{'system'}->{'preIncident'},
                        -size       =>  -1,
                        -labels     =>  {
                            "on" => "Enabled",
                            "off" => "Disabled",
                        }
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Printer Capture Alerting System: " ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'system|netIncident',
                        -values     =>  ["on", "off"],
                        -default    =>  $settings->{'system'}->{'netIncident'},
                        -size       =>  -1,
                        -labels     =>  {
                            "on" => "Enabled",
                            "off" => "Disabled",
                        }
                    ),
                ),
            ),
           # Tr(
           #     td( { -class => "qu" }, "CAD Integrated Alerting System: " ),
           #     td(
           #         { -class => "an" },
           #         scrolling_list(
           #             -name       =>  'system|cadIncident',
           #             -values     =>  ["on", "off"],
           #             -default    =>  $settings->{'system'}->{'cadIncident'},
           #             -size       =>  -1,
           #             -labels     =>  {
           #                 "on"  => "Enabled",
           #                 "off" => "Disabled",
           #             }
           #         ),
           #     ),
           # ),
        ),
        div( { -class =>  "section b left" }, "System Testing"),
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "settings left",
            },
            Tr(
                td( { -class => "qu" }, "Debug Logging:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'system|debug',
                        -values     =>  ["1", "0"],
                        -default    =>  $settings->{'system'}->{'debug'},
                        -size       =>  -1,
                        -labels     =>  {
                            "1" => "Enabled",
                            "0" => "Disabled",
                        }
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Test Incident File (.txt):" ),
                td(
                    { -class => "an" },
                    textfield('system|captureFile', $settings->{'system'}->{'captureFile'}),
                ),
            ),
        ),
        div( { -class =>  "section b left" }, "Signboard Settings"),
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "settings left",
            },
            Tr(
                td( { -class => "qu" }, "Signboard Type:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'serial|stylesheet',
                        -values     =>  [ sort keys %{ $serial_xsl } ],
                        -default    =>  $settings->{'serial'}->{'stylesheet'},
                        -size       =>  -1,
                        -labels     =>  $serial_xsl,
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Signboard Serial Port:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'serial|port',
                        -values     =>  [ @serial_ports ],
                        -default    =>  $settings->{'serial'}->{'port'},
                        -size       =>  -1,
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Baud Rate: " ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'serial|baudrate',
                        -values     =>  [ @serial_speeds ],
                        -default    =>  $settings->{'serial'}->{'baudrate'},
                        -size       =>  -1,
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Parity:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'serial|parity',
                        -values     =>  ["Even", "Odd", "None", "Mark", "Space"],
                        -default    =>  $settings->{'serial'}->{'parity'},
                        -size       =>  -1
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Databits:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'serial|databits',
                        -values     =>  [ 4, 5, 6, 7, 8 ],
                        -default    =>  $settings->{'serial'}->{'databits'},
                        -size       =>  -1,
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Stopbits:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'serial|stopbits',
                        -values     =>  [ 1, 1.5, 2 ],
                        -default    =>  $settings->{'serial'}->{'stopbits'},
                        -size       =>  -1,
                    ),
                ),
            ),
        ),
        div( { -class =>  "section b left" }, "Network Settings"),
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "settings left",
            },
            Tr(
                td( { -class => "qu" }, "Network Interface:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'network|interface',
                        -values     =>  [ @device_list ],
                        -default    =>  $settings->{'network'}->{'interface'},
                        -size       =>  -1,
                        -labels     =>  $device_hash,
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Protocol: " ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'network|protocol',
                        -values     =>  [ "TCP", "UDP" ],
                        -default    =>  $settings->{'network'}->{'protocol'},
                        -size       =>  -1,
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "IP Address:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  "network|ip_addr",
                        -value  =>  $settings->{'network'}->{'ip_addr'},
                        -class  =>  "required validate-ip"
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Destination Port:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name       =>  "network|dst_port",
                        -value      =>  $settings->{'network'}->{'dst_port'},
                        -size       =>  2,
                        -maxlength  =>  6,
                        -class      =>  'required validate-digits'),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Promiscuous Mode:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'network|promisc',
                        -values     =>  ["1", "0"],
                        -default    =>  $settings->{'network'}->{'promisc'},
                        -size       =>  -1,
                        -labels     =>  {
                            "1" => "Enabled",
                            "0" => "Disabled",
                        }
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "MTU:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  "network|mtu",
                        -value  =>  $settings->{'network'}->{'mtu'},
                        -size   =>  4,
                        -class  =>  "validate-mtu"),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Capture Timeout:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  "network|to_ms",
                        -value  =>  $settings->{'network'}->{'to_ms'},
                        -size   =>  1,
                        -class  =>  "validate-ms"
                    ),
                    "<small><i>-1 for continuous capture</i></small>",
                ),
            ),
            Tr(
                td( { -class => "qu" }, "OPT:" ),
                td(
                    { -class => "an" },
                    checkbox(
                        -name    => "network|opt",
                        -checked => ( $settings->{'network'}->{'opt'} ? 1 : 0 ),
                        -value   => 1,
                        -label   => '',
                    ),
                ),
            ),
        ),
    );

    return $html;
}

sub call_types
{
	my $settings = $_[0]->SUPER::settings;

    my $callGroup = $_[0]->SUPER::callGroup;
    my $callGrouping = $_[0]->SUPER::callGrouping;
    my $callTypeLabel = $_[0]->SUPER::callTypeLabel;
    my $callType = $_[0]->SUPER::callType;

    my ($inner, $inner_types, $type);
    foreach ( keys %{ $callGrouping } ) {

        $type = $_;
        $type =~ s/^(type)(.*)$/\2/;

        $inner_groups .=
        Tr(
            td( { -class => "rw" },
                a(
                    {
                        -href   =>  "?ct=$_i"
                    },
                    uc $type
                ),
             ),
            td( { -class => "rw" }, $callGroup->{ $_ }->{'alertColor'} ),
            td( { -class => "rw" }, ( $callGroup->{ $_ }->{'audioAnnounce'} ? "audio\\" . $callGroup->{ $_ }->{'audioAnnounce'} : undef ) ),
            td( { -class => "rw" }, ( $callGroup->{ $_ }->{'audioAlert'} ? "audio\\" . $callGroup->{ $_ }->{'audioAlert'} : undef ) ),
        );


        $inner .=
        div(
            { -class =>  "section b left" },
            "$type Call Types",
            span(
                { -style => "padding-left:10px;font-size:90%;font-weight:normal;" },
                "[",
                a(
                    { -href => "?add=$_" },
                    "New Call Type"
                ),
                "]",
            ),
        );

        undef $inner_types;
        foreach my $_i ( sort @{ $callGrouping->{ $_ } } ) {

            $inner_types .=
            Tr(
                td( { -class => "rw" },
                    a(
                        {
                            -href   =>  "?ct=$_i"
                        },
                        $_i
                    ),
                 ),
                td( { -class => "rw" }, $callType->{ $_i }->{'name'} ),
                td( { -class => "rw" }, $callType->{ $_i }->{'alertColor'} ),
                td( { -class => "rw" }, ( $callType->{ $_i }->{'audioAnnounce'} ? "audio\\" . $callType->{ $_i }->{'audioAnnounce'} : undef ) ),
            );
        }

        $inner .=
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "calltype",
            },
            Tr(
                td( { -class => "hd", -style => "width:15%;" }, "Call Type Code" ),
                td( { -class => "hd", -style => "width:35%;" }, "Call Type Label" ),
                td( { -class => "hd", -style => "width:25%;" }, "Display Color" ),
                td( { -class => "hd", -style => "width:25%;" }, "Audible Announcement" ),
            ),
            $inner_types,
        );
    }


	return
    div(
        {
            -id    =>  "calltypes",
            -class =>  "tabcontent",
        },
	    div(
	        { -class =>  "section b left" },
	        "Call Groups",
	    ),
	    table(
	        {
	            -cellspacing   =>  1,
	            -cellpadding   =>  4,
	            -class         =>  "calltype",
	        },
	        Tr(
	            td( { -class => "hd", -style => "width:15%;" }, "Call Group" ),
	            td( { -class => "hd", -style => "width:35%;" }, "Display Color" ),
	            td( { -class => "hd", -style => "width:25%;" }, "Audible Announcement" ),
	            td( { -class => "hd", -style => "width:25%;" }, "Audible Alert" ),
	        ),
	        $inner_groups,
	    ),
        $inner
    );
}

sub alert_settings
{
    my $settings = $_[0]->SUPER::settings;

    $settings->{'alerting'}->{'volume'} = -1 if ! $settings->{'alerting'}->{'volume'};

    my $callGrouping = $_[0]->SUPER::callGrouping;
    my $callTypeLabel = $_[0]->SUPER::callTypeLabel;

    my $priCallType = [];
    for my $_i ( keys %{ $_[0]->SUPER::callType } ) {
        push @{ $priCallType }, $_i if ( $_[0]->SUPER::callType->{ $_i }->{'priority'} );
    }

    my $JSslider =
    div(
        {
        	-style => "margin-bottom: 5px;"
        },
        hidden(
            -name  =>  'alerting|volumeLevel',
            -id    =>  'sliderValue1',
            -value =>  $settings->{'alerting'}->{'volume'},
        ) .
        checkbox(
            -name    => "alerting|autoVolume",
            -checked => ( $settings->{'alerting'}->{'volume'} == -1 ? 1 : 0 ),
            -value   => 1,
            -label   => '',
            -onClick => "if(this.checked){\$('slider1_Holder').hide();}else{\$('slider1_Holder').show();}"
        ) .
        "&nbsp;Use Computer Volume Settings"
    ) .
    div(
        {
            -id    =>  "slider1_Holder",
            -style =>  "display:" . ( $settings->{'alerting'}->{'volume'} == -1 ? "none" : "block" ) . "; margin-left:30px;"
        }, "
        <script language=\"JavaScript\">
            var A_TPL4h = {
                'b_vertical' : false,
                'b_watch': true,
                'n_controlWidth': 100,
                'n_controlHeight': 16,
                'n_sliderWidth': 17,
                'n_sliderHeight': 16,
                'n_pathLeft' : 0,
                'n_pathTop' : 0,
                'n_pathLength' : 83,
                's_imgControl': 'images/slider/sldr3h_bg.gif',
                's_imgSlider': 'images/slider/sldr3h_sl.gif',
                'n_zIndex': 1
            }
            var A_INIT4h = {
                's_form' : 0,
                's_name': 'sliderValue1',
                'n_minValue' : 0,
                'n_maxValue' : 100,
                'n_value' : " . ( ! $settings->{'alerting'}->{'volume'} || $settings->{'alerting'}->{'volume'} == -1 ? '85' : $settings->{'alerting'}->{'volume'} ) . ",
                'n_step' : 1
            }

            new slider(A_INIT4h, A_TPL4h);
        </script>"
    );

    return
    div(
        {
            -id    =>  "alerting",
            -class =>  "tabcontent",
        },
        div( { -class =>  "section b left" }, "Signboard Alert Settings"),
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "settings left",
            },
            Tr(
                td( { -class => "qu" }, "Signboard Alerting:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'alerting|serialAlerting',
                        -values     =>  [ 1, 0 ],
                        -default    =>  $settings->{'alerting'}->{'serialAlerting'},
                        -size       =>  -1,
                        -labels     =>  {
                            1   =>  "Enabled",
                            0   =>  "Disabled",
                        }
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Incident Flash Length:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'alerting|flashLength',
                        -values     =>  [ "3 Sec", "4 Secs", "5 Secs", "6 Secs", "7 Secs", "8 Secs", "9 Secs", "10 Secs" ],
                        -default    =>  $settings->{'alerting'}->{'flashLength'},
                        -size       =>  -1
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Signboard Reset Time:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'alerting|alertReset',
                        -value  =>  $settings->{'alerting'}->{'alertReset'},
                        -size   =>  1,
                        -class  =>  'validate-secs'
                    ),
                    "seconds",
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Default Signboard Color:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'alerting|defaultColor',
                        -values     =>  [ @defaultColors ],
                        -default    =>  $settings->{'alerting'}->{'defaultColor'},
                        -size       =>  -1
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Display Incident Comments:" ),
                td(
                    { -class => "an" },
                    checkbox(
                        -name    => "alerting|serialDisplayComment",
                        -checked => ( $settings->{'alerting'}->{'serialDisplayComment'} ? 1 : 0 ),
                        -value   => 1,
                        -label   => '',
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Max Length for Incident Text/Comments:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'alerting|maxCommentLength',
                        -value  =>  $settings->{'alerting'}->{'maxCommentLength'},
                        -size   =>  2,
                        -class  =>  'validate-length'
                    ),
                ),
            ),
        ),
        div( { -class =>  "section b left" }, "Audible Alert Settings"),
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "settings left",
            },
            Tr(
                td( { -class => "qu" }, "Audible Alerting:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'alerting|audioAlerting',
                        -values     =>  [ 1, 0 ],
                        -default    =>  $settings->{'alerting'}->{'audioAlerting'},
                        -size       =>  -1,
                        -labels     =>  {
                            1   =>  "Enabled",
                            0   =>  "Disabled",
                        }
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "New Incident Audible Alert:" ),
                td(
                    { -class => "an", -style => "vertical-align:bottom;" },
                    div(
                        { -style => "float:right;" },
                        a(
                            {
                                -href     =>  "javascript:void(0);",
                                -style    =>  "display:block; font-size:smaller;",
                                -onClick  =>  "file_selector('incidentAlertHolder', 'incidentAlertFile');"
                            },
                            "File Selector"
                        ),
                    ),
                    div(
                        {
                            -style =>  "display:block;",
                            -id    =>  "incidentAlertHolder",
                        },

                        ( $settings->{'alerting'}->{'incidentAlert'} ?
                            "audio\\" . $settings->{'alerting'}->{'incidentAlert'} : "No Audible Alert Defined"
                        ),
                    ),
                    div(
                        {
                            -style =>  "display:none;",
                            -id    =>  "incidentAlertFile",
                        },
                        filefield(
                            {
                                -name  =>  'alerting|incidentAlert',
                            }
                        ),
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Volume Level:" ),
                td(
                    {
                        -class  =>  "an",
                    },
                    div( $JSslider ),
                ),
            ),
        ),
        div( { -class =>  "section b left" }, "Priority Incident Alerting"),
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "settings left",
            },
            Tr(
                td( { -class => "qu" }, "Priority Call Types:<div style=\"font-size:6pt;margin-top:7px;font-style:italic;\">Call types to trigger prioritized<br />alerting when first due</div>" ),
                td(
                    { -class => "an" },
                    popup_menu(
                        -name       =>  'alerting|priority',
                        -values     =>
                        [
                            optgroup(
                                -name   =>  'Fire Call Types',
                                -values =>  [ sort @{ $callGrouping->{ 'typeFire' } } ],
                                -labels =>  $callTypeLabel,
                            ),
                            optgroup(
                                -name   =>  'Local Call Types',
                                -values =>  [ sort @{ $callGrouping->{ 'typeLocal' } } ],
                                -labels =>  $callTypeLabel,
                            ),
                            optgroup(
                                -name   =>  'Rescue Call Types',
                                -values =>  [ sort @{ $callGrouping->{ 'typeRescue' } } ],
                                -labels =>  $callTypeLabel,
                            ),
                            optgroup(
                                -name       =>  'EMS Call Types',
                                -values     =>  [ sort @{ $callGrouping->{ 'typeEms' } }  ],
                                -labels     =>  $callTypeLabel,
                            ),
                        ],
                        -default    =>  [ @{ $priCallType } ],
                        -size       =>  5,
                        -class      =>  "required",
                        -multiple
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Priority Incident Display Color:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'alerting|priorityDisplayColor',
                        -values     =>  [ @defaultColors ],
                        -default    =>  $settings->{'alerting'}->{'priorityDisplayColor'},
                        -size       =>  -1
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Priority Incident Audible Alert:" ),
                td(
                    { -class => "an", -style => "vertical-align:bottom;" },
                    div(
                        { -style => "float:right;" },
                        a(
                            {
                                -href     =>  "javascript:void(0);",
                                -style    =>  "display:block; font-size:smaller;",
                                -onClick  =>  "file_selector('priorityAudioAlertHolder', 'priorityAudioAlertFile');"
                            },
                            "File Selector"
                        ),
                    ),
                    div(
                        {
                            -style =>  "display:block;",
                            -id    =>  "priorityAudioAlertHolder",
                        },

                        ( $settings->{'alerting'}->{'priorityAudioAlert'} ?
                            "audio\\" . $settings->{'alerting'}->{'priorityAudioAlert'} : "No Audible Alert Defined"
                        ),
                    ),
                    div(
                        {
                            -style =>  "display:none;",
                            -id    =>  "priorityAudioAlertFile",
                        },
                        filefield(
                            {
                                -name  =>  'alerting|priorityAudioAlert',
                            }
                        ),
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Priority Incident Flash Message:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'alerting|priorityMessage',
                        -value  =>  $settings->{'alerting'}->{'priorityMessage'},
                        -class  =>  'required'),
                ),
            ),
        ),
    );
}

sub notification
{
	my $cgi = shift;
    my $settings = $_[0]->SUPER::settings;
    my $_GET = $_[0]->SUPER::__GET__;
    my $_POST = $_[0]->SUPER::__POST__;

    my $callGrouping = $_[0]->SUPER::callGrouping;
    my $callTypeLabel = $_[0]->SUPER::callTypeLabel;
    my $callType = $_[0]->SUPER::callType;

    my $dbh = $_[0]->SUPER::DBH;

    my $html;

    my ($order, $order_dir, $edit_html);
    my @valid_orders = qw( memberName phoneNo carrier );
    my %is_valid;
    for ( @valid_orders ) { $is_valid{$_} = 1; }

    if ( $_GET->{'section'} eq 'notification' ) {

	    $order = $_GET->{'order'} if ( $_GET->{'order'} );
	    $order_dir = $_GET->{'order_dir'} if ( $_GET->{'order_dir'} );

	    if ( defined $_GET->{'edit'} ) {

            my (%row, %gateways);
            my $sth = $dbh->prepare("SELECT *
                                     FROM smsGateways");
            if ( $sth->execute ) {

                $sth->bind_columns( \( @row{ @{$sth->{NAME} } } ) );
                while ( $sth->fetch ) {
                    $gateways->{ $row{'carrier'} } = $row{'gateway'};
                }
            }


            my %row;
            if ( $_GET->{'edit'} ) {

			    my $sth = $dbh->prepare("SELECT *
			                             FROM smsAlerts
			                             WHERE memberEid = ?");
			    if ( $sth->execute( $_GET->{'edit'} ) ) {

			        $sth->bind_columns( \( @row{ @{$sth->{NAME} } } ) );
			        if ( $sth->fetch ) {

                        $row{'phoneNo'} =~ s/^([0-9]{3})([0-9]{3})([0-9]{4})$/\1-\2-\3/;
			        }
			    }
            }

            $edit_html =
	        div(
	            {
	            	-style =>  "margin-left:50px;margin-top:10px;margin-bottom:15px;",
	            },
	            hidden('notification:edit_editMember', $row{'memberEid'}),
	            fieldset(
		            legend( { -class  =>  "s b" }, "Add New Member Notification"),
			        table(
			            {
			                -cellspacing   =>  1,
			                -cellpadding   =>  4,
			                -class         =>  "settings left",
			                -style         =>  "width:65%;",
			            },
			            Tr(
			                td( { -class => "qu" }, "Member Name:" ),
			                td(
			                    { -class => "an" },
			                    textfield('notification:edit_memberName', $row{'memberName'} ),
			                ),
			            ),
                        Tr(
                            td( { -class => "qu" }, "Member ID:" ),
                            td(
                                { -class => "an" },
                                textfield('notification:edit_memberEid', $row{'memberEid'}, 7),
                            ),
                        ),
                        Tr(
                            td( { -class => "qu" }, "Notification Pref:" ),
                            td(
                                { -class => "an" },
			                    scrolling_list(
			                        -name       =>  'notification:edit_alertPref',
			                        -values     =>  [ 'calltype', 'unit' ],
			                        -default    =>  $row{'alertPref'},
			                        -size       =>  -1,
			                        -labels     =>  {
			                            'calltype'   =>  "Alert by Call Type",
			                            'unit'       =>  "Alert by Unit",
			                        }
			                    ),
                            ),
                        ),
                        Tr(
                            td( { -class => "qu" }, "Phone No:" ),
                            td(
                                { -class => "an" },
                                textfield('notification:edit_phoneNo', $row{'phoneNo'}, 10 ),
                            ),
                        ),
                        Tr(
                            td( { -class => "qu" }, "Carrier: " ),
                            td(
                                { -class => "an" },
                            ),
                                scrolling_list(
                                    -name       =>  'notification:edit_carrier',
                                    -values     =>  [ keys %{ $gateways } ],
                                    -default    =>  $row{'carrier'},
                                    -size       =>  -1,
                                ),
                        ),
                        Tr(
                            td( { -class => "qu" }, "Pending Incidents:" ),
                            td(
                                { -class => "an" },
                                scrolling_list(
                                    -name       =>  'notification:edit_preincAlert',
                                    -values     =>  [ qw( 1 0 ) ],
                                    -default    =>  $row{'preincAlert'},
                                    -size       =>  -1,
                                    -labels     =>  {
                                    	1  =>  'Notifications Enabled',
                                    	0  =>  'Notifications Disabled',
                                    },
                                ),
                            ),
                        ),
                        Tr(
                            td( { -class => "qu" }, "Active:" ),
                            td(
                                { -class => "an" },
                                scrolling_list(
                                    -name       =>  'notification:edit_active',
                                    -values     =>  [ qw( 1 0 ) ],
                                    -default    =>  $row{'active'},
                                    -size       =>  -1,
                                    -labels     =>  {
                                        1  =>  'Notifications Active',
                                        0  =>  'Notifications Disabled',
                                    },
                                ),
                            ),
                        ),
                        Tr(
                            td( { -class => "qu" }, "Alert Schedule:" ),
                            td(
                                { -class => "an" },
                                div(
                                    {
                                        -style  =>  "margin:5px;"
                                    },
                                    'Filter by Day of Week',
                                    div(
                                        { -style =>  "margin-top:5px;" },
					                    popup_menu(
					                        -name       =>  'notification:edit_timeScheduleDay',
					                        -values     =>  [ qw( 0 1 2 3 4 5 6 ) ],
					                        -size       =>  5,
					                        -default    =>  [ split(',', $row{'timeScheduleDay'}) ],
                                            -labels     =>  {
                                                0   =>  'SUNDAY',
                                                1   =>  'MONDAY',
                                                2   =>  'TUESDAY',
                                                3   =>  'WEDNESDAY',
                                                4   =>  'THURSDAY',
                                                5   =>  'FRIDAY',
                                                6   =>  'SATURDAY',
                                            },
					                        -multiple,
					                    ),
                                    )
                                ),
                                div(
                                    {
                                        -style  =>  "margin:5px;"
                                    },
                                    'Filter by Time of Day',
                                    div(
                                        { -style =>  "margin-top:5px;" },
                                        textfield('notification:edit_timeSchedule', $row{'timeSchedule'}, 20),
                                        span( { -class => 's i' }, "I.E. 0600-1500"),
                                    )
                                ),
                            ),
                        ),
			        ),
			        div(
    			        { -style  =>  "margin:10px 0px 5px 25px;" },
                        submit('notification|saveBtn', 'Save'),
                        ( defined $row{'memberEid'} ?
                            "&nbsp;&nbsp;&nbsp;".
                            submit('notification|deleteBtn', 'Delete') : undef
                        ),
			        ),
			    ),
	        );
	    }
    }

    $order_dir = 'ASC' if ( $order_dir ne 'ASC' && $order_dir ne 'DESC' );

    my $sth = $dbh->prepare("SELECT *
                             FROM smsAlerts
                             ORDER BY [" .
                             ( $is_valid{ $order } ?
                                 $order : 'memberName'
                             ) . "]
                             $order_dir");
    if ( $sth->execute ) {

        $sth->bind_columns( \( @row{ @{$sth->{NAME} } } ) );
        while ( $sth->fetch ) {

            $row{'phoneNo'} =~ s/([0-9]{3})([0-9]{3})([0-9]{4})/\1-\2-\3/;

            $html .=
            Tr( { -title => ( ! $row{'active'} ? "Text message notifications are inactive on this member" : undef ) },
                td(
	                {
	                	-class => ( ! $row{'active'} ? "inactive" : "rw" ),
	                	-style => "vertical-align:bottom;",
	                },
                    span(
                        { -style => "padding-right:7px;" },
                        a(
                            {
                            	-href      =>  "?notification&rm=$row{memberEid}",
                            	-onclick   =>  "javascript: return confirm('Are you sure you want to remove this user from receiving all text message notifications? This action CANNOT be undone!');",
                            },
                            img {
                                -src    =>  'images/kill.gif',
                                -border =>  0,
                            }
                        ),
                    ),
                    a(
                        { -href   =>  "?section=notification&edit=$row{memberEid}" },
                        $row{'memberEid'},
                    ),
                 ),
                td( { -class => ( ! $row{'active'} ? "inactive" : "rw" ) }, $row{'memberName'} ),
                td(
                    { -class => ( ! $row{'active'} ? "inactive" : "rw" ) },
                    ( $row{'alertPref'} eq 'calltype' ?
                        "Text Alerts by Call Type/Group" : "Text Alerts by Unit(s)"
                     )
                 ),
                td( { -class => ( ! $row{'active'} ? "inactive rt" : "rw rt" ) }, $row{'phoneNo'} ),
                td( { -class => ( ! $row{'active'} ? "inactive rt" : "rw rt" ) }, $row{'carrier'} ),
                td( { -class => ( ! $row{'active'} ? "inactive rt" : "rw rt" ) }, ( $row{'preincAlert'} ? "Y" : "N" ) ),
                td( { -class => ( ! $row{'active'} ? "inactive rt" : "rw rt" ) }, ( $row{'active'} ? "Y" : "N" ) ),
            );
        }
    }

    return
    div(
        {
            -id    =>  "notification",
            -class =>  "tabcontent",
        },
        div( { -class =>  "section b left" }, "Notification Settings"),
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "settings left",
            },
            Tr(
                td( { -class => "qu" }, "Incident Notifications:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'system|smsEnabled',
                        -values     =>  [ 1, 0 ],
                        -default    =>  $settings->{'system'}->{'smsEnabled'},
                        -size       =>  -1,
                        -labels     =>  {
                            1   =>  "Enabled",
                            0   =>  "Disabled",
                        }
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "SMS Relay Proxy Server:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'notification|soap_proxy',
                        -value  =>  $settings->{'notification'}->{'soap_proxy'},
                        -size   =>  65,
                        -class  =>  "required validate-url"
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "SMS Relay Proxy URI:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'notification|soap_uri',
                        -value  =>  $settings->{'notification'}->{'soap_uri'},
                        -size   =>  65,
                        -class  =>  "required validate-url"
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Max Length for Incident Text/Comments:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'notification|commentTextLength',
                        -value  =>  $settings->{'notification'}->{'commentTextLength'},
                        -size   =>  2
                        -class  =>  "required validate-length"
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Server Connection Timeout:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'notification|sms_timeout',
                        -value  =>  $settings->{'notification'}->{'sms_timeout'},
                        -size   =>  2,
                        -class  =>  "required validate-digits"
                    ),
                ),
            ),
        ),
	    div(
	        { -class =>  "section b left" },
	        "Member Notification List",
	        span(
	            { -style => "padding-left:10px;font-size:90%;font-weight:normal;" },
	            "[",
	            a(
	                { -href => "?section=notification&edit=" },
	                "New Member Notification"
	            ),
	            "]",
	        ),
	    ),
	    $edit_html,
	    table(
	        {
	            -cellspacing   =>  1,
	            -cellpadding   =>  4,
	            -class         =>  "calltype",
	        },
	        Tr(
	            td( { -class => "hd", -style => "width:15%;" }, "Member ID"),
	            td( { -class => "hd", -style => "width:20%;" },
                    a(
                        {
                            -href => "?section=notification&order=memberName&order_dir=" . ( $order eq "memberName" && $order_dir eq 'ASC' ? 'DESC' : 'ASC' )
                        },
                        "Member Name",
                    ),
                    ( $order eq 'memberName' ?
                        img {
                            -src => "images/" . ( $order_dir eq 'ASC' ? "s_asc.png" : "s_desc.png" ),
                        } : undef
                    ),
                ),
	            td( { -class => "hd", -style => "width:25%;" }, "Notification Preference" ),
	            td( { -class => "hd rt", -style => "width:10%; padding-right:5px;" },
                    a(
                        {
                            -href => "?section=notification&order=phoneNo&order_dir=" . ( $order eq "phoneNo" && $order_dir eq 'ASC' ? 'DESC' : 'ASC' )
                        },
                        "Phone No",
                    ),
                    ( $order eq 'phoneNo' ?
                        img {
                            -src => "images/" . ( $order_dir eq 'ASC' ? "s_asc.png" : "s_desc.png" ),
                        } : undef
                    ),
                ),
	            td( { -class => "hd rt", -style => "width:10%; padding-right:5px;" },
                    a(
                        {
                            -href => "?section=notification&order=carrier&order_dir=" . ( $order eq "carrier" && $order_dir eq 'ASC' ? 'DESC' : 'ASC' )
                        },
                        "Carrier",
                    ),
                    ( $order eq 'carrier' ?
                        img {
                            -src => "images/" . ( $order_dir eq 'ASC' ? "s_asc.png" : "s_desc.png" ),
                        } : undef
                    ),
                ),
	            td( { -class => "hd rt", -style => "width:15%; padding-right:5px;" }, "Pending Incident Alerts" ),
	            td( { -class => "hd rt", -style => "width:5%; padding-right:5px;" }, "Active" ),
	        ),
	        $html,
	    ),
    );
}

sub preinc_settings
{
    my $settings = $_[0]->SUPER::settings;

    my $callTypeLabel = $_[0]->SUPER::callTypeLabel;
    my $callType = $_[0]->SUPER::callType;

    my ($html_area, $html_type);
    my $preinc_area = {};
    my $preinc_type = {};
    my $preinc_audio = {};

    my $dbh = $_[0]->SUPER::DBH;
    my $sth = $dbh->prepare("SELECT t1.*, t2.alertAudio, t3.alertAudio AS typeAudio, t4.name
                             FROM preIncidentAlert t1
                             LEFT JOIN preIncidentArea t2 ON t2.area = t1.area
                             LEFT JOIN preIncidentType t3 ON t3.code = t1.callType
                             LEFT JOIN callType t4 ON t4.code = t1.callType
                             ORDER BY t1.area ASC") or die "DBH Query Error: " . $DBI::errStr;
    if ( $sth->execute ) {

        my $curloop;
        $sth->bind_columns( \( @row{ @{$sth->{NAME} } } ) );
        while ( $sth->fetch ) {

            $preinc_audio->{ $row{'area'} } = $row{'alertAudio'} if ( ! $preinc_area{ $row{'area'} } );
            $preinc_area->{ $row{'area'} } .= ( $preinc_area->{ $row{'area'} } ? ", " : undef ) . $row{'callType'};
            if ( ! $preinc_type->{ $row{'callType'} } ) {

	            $preinc_type->{ $row{'callType'} } = {
	            	'name'     =>  $row{'name'},
	            	'audio'    =>  $row{'typeAudio'}
	            };
            }
        }
    }

    foreach ( sort keys %{ $preinc_area } ) {

	    $html_area .=
	    Tr(
	        td( { -class => "rw" },
	            a(
	                {
	                    -href   =>  "?preinc&edit=area&area=$_"
	                },
	                $_
	            ),
	         ),
	        td( { -class => ( ! $row{'active'} ? "inactive" : "rw" ) }, $preinc_area->{ $_ } ),
            td( { -class => "rw" }, ( $preinc_audio->{ $_ } ? "audio\\" . $preinc_audio->{ $_ } : undef ) ),
	    );
    }

    foreach ( sort keys %{ $preinc_type } ) {

        $html_type .=
        Tr(
            td( { -class => "rw" },
                a(
                    {
                        -href   =>  "?preinc&edit=type&type=$_"
                    },
                    $preinc_type->{ $_ }->{'name'}
                ),
             ),
            td( { -class => "rw" }, ( $preinc_type->{ $_ }->{'audio'} ? "audio\\" . $preinc_type->{ $_ }->{'audio'} : undef ) ),
        );
    }

    $settings->{'preinc'}->{'host'} =~ s/^http:\/\/(.*)$/\1/;

    return
    div(
        {
            -id    =>  "preinc",
            -class =>  "tabcontent",
        },
        div( { -class =>  "section b left" }, "Pending Incident Settings"),
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "settings left",
            },
            Tr(
                td( { -class => "qu" }, "Remote CAD Host IP Address:" ),
                td(
                    { -class => "an" },
                    "http://",
                    textfield(
                        -name   =>  'preinc|host',
                        -value  =>  $settings->{'preinc'}->{'host'},
                        -size   =>  13,
                        -class  =>  'required validate-ip'
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Host Authentication Cookie Name: " ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|cookie_name',
                        -value  =>  $settings->{'preinc'}->{'cookie_name'},
                        -class  =>  'required'
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Incident Number Prefix:" ),
                td(
                    { -class => "an" },
                    textfield('preinc|incident_prefix', $settings->{'preinc'}->{'incident_prefix'}, 2 ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Call Number Prefix:" ),
                td(
                    { -class => "an" },
                    textfield('preinc|call_prefix', $settings->{'preinc'}->{'call_prefix'}, 2 ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Sleep Between CAD Requests" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|sleep',
                        -value  =>  $settings->{'preinc'}->{'sleep'},
                        -size   =>  2,
                        -class  =>  'validate-sleep-time'
                    ), "seconds"
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Sleep Between HTTP Requst Errors:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|sleep_on_err',
                        -value  =>  $settings->{'preinc'}->{'sleep_on_err'},
                        -size   =>  2,
                        -class  =>  'validate-sleep-time'
                    ), "seconds"
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Watchman HTTP Cookie File:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|cookie_file',
                        -value  =>  $settings->{'preinc'}->{'cookie_file'},
                        -size   =>  20
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Login URL:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|login_url',
                        -value  =>  $settings->{'preinc'}->{'login_url'},
                        -size   =>  35,
                        -class  =>  'required'
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Login Query String:" ),
                td(
                    { -class => "an" },
                    textfield('preinc|login_q', URLDecode( $settings->{'preinc'}->{'login_q'} ), 35 ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Login Failure Message:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|login_failureMsg',
                        -value  =>  $settings->{'preinc'}->{'login_failureMsg'},
                        -size   =>  35,
                        -class  =>  'required'
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Login Username Field Name:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|username_field',
                        -value  =>  $settings->{'preinc'}->{'username_field'},
                        -size   =>  10,
                        -class  =>  'required'
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Username:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|username',
                        -value  =>  $settings->{'preinc'}->{'username'},
                        -size   =>  10,
                        -class  =>  'required'
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Login Password Field Name:" ),
                td(
                    { -class => "an" },
                    textfield('preinc|password_field', $settings->{'preinc'}->{'password_field'}, 10 ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Password:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|password',
                        -value  =>  $settings->{'preinc'}->{'password'},
                        -size   =>  10,
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Lookup Incident URL:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|incident_url',
                        -value  =>  $settings->{'preinc'}->{'incident_url'},
                        -size   =>  35,
                        -class  =>  'required'
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Lookup Incident Query String:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|incident_q',
                        -value  =>  URLDecode( $settings->{'preinc'}->{'incident_q'} ),
                        -size   =>  35,
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Lookup Incident Post Content:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|incident_post_content',
                        -value  =>  URLDecode( $settings->{'preinc'}->{'incident_post_content'} ),
                        -size   =>  35,
                        -class  =>  'required'
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Lookup Incident Number Input Field:" ),
                td(
                    { -class => "an" },
                    textfield('preinc|incident_no_input', $settings->{'preinc'}->{'incident_no_input'}, 10 ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Lookup Incident Hour Input Field:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|incident_no_input',
                        -value  =>  $settings->{'preinc'}->{'incident_no_input'},
                        -size   =>  10,
                        -class  =>  'required'
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Lookup Incident Minute Input Field:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|incident_min_input',
                        -value  =>  $settings->{'preinc'}->{'incident_min_input'},
                        -size   =>  10,
                        -class  =>  'required'
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "HTTP Agent Header:" ),
                td(
                    { -class => "an" },
                    textfield('preinc|ua_agent', $settings->{'preinc'}->{'ua_agent'}, 35 ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "System Sleep Times:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|system_sleep',
                        -value  =>  $settings->{'preinc'}->{'system_sleep'},
                        -size   =>  10,
                        -class  =>  'validate-miltime-range'
                    ),
                    span(
                        { -style => "font-size:90%;font-style:italic;padding-left:10px;" },
                        "I.E. 0600-1500"
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Exceptions to System Sleep Times by Day of Week:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'preinc|sleep_exceptions_weekday',
                        -values     =>  [ qw( 1 2 3 4 5 6 0 ) ],
                        -default    =>  [ split(',', $settings->{'preinc'}->{'sleep_exceptions_weekday'}) ],
                        -multiple   =>  'true',
                        -size       =>  5,
                        -labels     =>  {
                            1   =>  "Monday",
                            2   =>  "Tuesday",
                            3   =>  "Wednesday",
                            4   =>  "Thursday",
                            5   =>  "Friday",
                            6   =>  "Saturday",
                            0   =>  "Sunday",
                        }
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Exceptions to System Sleep Times by Date:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|sleep_exceptions_date',
                        -value  =>  $settings->{'preinc'}->{'sleep_exceptions_date'},
                        -size   =>  20,
                        -class  =>  'validate-short-date'
                    ),
		            span(
		                { -style => "font-size:90%;font-style:italic;padding-left:10px;" },
                        "I.E. 12/25,01/01"
		            ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "CAD Look Back Time: " ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|lookback_time',
                        -value  =>  ( $settings->{'preinc'}->{'lookback_time'} ? ( $settings->{'preinc'}->{'lookback_time'} / 60 ) : 10 ),
                        -size   =>  2,
                        -class  =>  'validate-minutes required'
                    ),
                    "minutes",
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Signboard Reset Time" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|reset_time',
                        -value  =>  $settings->{'preinc'}->{'reset_time'},
                        -size   =>  2,
                        -class  =>  'validate-secs'
                    ),
                    "seconds",
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Pending Incident Audible Alert:" ),
                td(
                    { -class => "an", -style => "vertical-align:bottom;" },
                    div(
                        { -style => "float:right;" },
                        a(
                            {
                                -href     =>  "javascript:void(0);",
                                -style    =>  "display:block; font-size:smaller;",
                                -onClick  =>  "file_selector('alert_beepHolder', 'alert_beepFile');"
                            },
                            "File Selector"
                        ),
                    ),
                    div(
                        {
                            -style =>  "display:block;",
                            -id    =>  "alert_beepHolder",
                        },

                        ( $settings->{'preinc'}->{'alert_beep'} ?
                            "audio\\" . $settings->{'preinc'}->{'alert_beep'} : "No Audible Alert Defined"
                        ),
                    ),
                    div(
                        {
                            -style =>  "display:none;",
                            -id    =>  "alert_beepFile",
                        },
                        filefield(
                            {
                                -name  =>  'alert_beep',
                            }
                        ),
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Pending Incident Update Audible Alert:" ),
                td(
                    { -class => "an", -style => "vertical-align:bottom;" },
                    div(
                        { -style => "float:right;" },
                        a(
                            {
                                -href     =>  "javascript:void(0);",
                                -style    =>  "display:block; font-size:smaller;",
                                -onClick  =>  "file_selector('alert_beepHolder', 'update_beepFile');"
                            },
                            "File Selector"
                        ),
                    ),
                    div(
                        {
                            -style =>  "display:block;",
                            -id    =>  "update_beepHolder",
                        },

                        ( $settings->{'preinc'}->{'update_beep'} ?
                            "audio\\" . $settings->{'preinc'}->{'update_beep'} : "No Audible Alert Defined"
                        ),
                    ),
                    div(
                        {
                            -style =>  "display:none;",
                            -id    =>  "update_beepFile",
                        },
                        filefield(
                            {
                                -name  =>  'preinc|update_beep',
                            }
                        ),
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Pending Incident System Testing - Box Area:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|test_callArea',
                        -value  =>  $settings->{'preinc'}->{'test_callArea'},
                        -size   =>  2,
                        -class  =>  'required'
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Pending Incident System Testing - Call Type:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'preinc|test_callType',
                        -values     =>  [ sort keys %{ $callType } ],
                        -default    =>  $settings->{'preinc'}->{'test_callType'},
                        -size       =>  -1,
                        -labels     =>  $callTypeLabel,
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Pending Incident System Testing - Location:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'preinc|test_callLoc',
                        -value  =>  $settings->{'preinc'}->{'test_callLoc'},
                        -size   =>  50,
                        -class  =>  'required'
                    ),
                ),
            ),

        ),
        div( { -class =>  "section b left" }, "Area & Call Type Alert Settings"),
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "calltype",
            },
            Tr(
                td( { -class => "hd", -style => "width:5%;" }, "Area" ),
                td( { -class => "hd", -style => "width:70%;" }, "Call Types" ),
                td( { -class => "hd", -style => "width:25; padding-right:5px;" }, "Audible Announcement" ),
            ),
            $html_area,
        ),
    );

    preIncidentArea
}

sub rss
{

    my $settings = $_[0]->SUPER::settings;

    return
    div(
        {
            -id    =>  "rss",
            -class =>  "tabcontent",
        },
        div( { -class =>  "section b left" }, "RSS Incident Ticker"),
        table(
            {
                -cellspacing   =>  1,
                -cellpadding   =>  4,
                -class         =>  "settings left",
            },
            Tr(
                td( { -class => "qu" }, "Incident Notifications:" ),
                td(
                    { -class => "an" },
                    scrolling_list(
                        -name       =>  'rss|rssEnabled',
                        -values     =>  [ 1, 0 ],
                        -default    =>  $settings->{'system'}->{'rssEnabled'},
                        -size       =>  -1,
                        -labels     =>  {
                            1   =>  "Enabled",
                            0   =>  "Disabled",
                        }
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "RSS Proxy Server:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'rss|proxy',
                        -value  =>  $settings->{'rss'}->{'proxy'},
                        -size   =>  65,
                        -class  =>  "required validate-url"
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "SMTP Relay Proxy URI:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'rss|uri',
                        -value  =>  $settings->{'rss'}->{'uri'},
                        -size   =>  65,
                        -class  =>  "required validate-url"
                    ),
                ),
            ),
            Tr(
                td( { -class => "qu" }, "Server Connection Timeout:" ),
                td(
                    { -class => "an" },
                    textfield(
                        -name   =>  'rss|timeout',
                        -value  =>  $settings->{'rss'}->{'rss_timeout'},
                        -size   =>  2,
                        -class  =>  'required validate-digits'
                    ),
                ),
            ),
        ),
    );
}

sub URLDecode {
    my $theURL = $_[0];
    $theURL =~ tr/+/ /;
    $theURL =~ s/%([a-fA-F0-9]{2,2})/chr(hex($1))/eg;
    $theURL =~ s/<!--(.|\n)*-->//g;
    return $theURL;
}

sub URLEncode {
   my $theURL = $_[0];
   $theURL =~ s/([\W])/"%" . uc(sprintf("%2.2x",ord($1)))/eg;
   return $theURL;
}

1;