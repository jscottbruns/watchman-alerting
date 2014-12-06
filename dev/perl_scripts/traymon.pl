#!/usr/bin/perl
#use warnings;
use strict;

use Getopt::Long qw(GetOptions :config no_ignore_case no_auto_abbrev);
use Win32;
use Win32::GUI;
use Win32::GUI::Carp 'fatalsToDialog', 'windie';
use Win32::Event;
use Win32::Process;
use Config::IniFiles;

use constant ICON_NUM => 1;

# Program to monitor
our $command;
our $directory = '.';
our $child;

# Icon and animation
our $event;
our $tip = '';
our $icon;
our @frames;
our $anim_delay = 125;
our $rewind;

# Menu
our @menu = ('E&xit' => '\&terminate');

# GUI Controls
our $win;
our $menu;
our $niTray;

# Help
our $usage = <<"EOU";
Usage: $0 [ -e [PROGRAM] | -p [PID] ] -i [ICON FILE] [OPTIONS]
   or: $0 INIFILE
EOU
our $details = <<'EOD';
Monitor another program by showing a system tray icon while it is running.
  -e, --execute COMMAND  Specify a command to execute and monitor
  -E, --event NAME       Specify the name of the event to watch for
  -f, --frame FILE       Include a frame for animating the icon
  -h, --help             Show this information screen
  -i, --icon FILE        Specify the .ico file to display
  -p, --pid PID          Specify the PID to monitor
  -r, --rewind           Play the icon animation backwards to loop
  -t, --tip TIP          Tip to display when hovering over icon

You must specify either a command or a PID to monitor, as well as an
icon file.

You may also specify a named event to listen for and one or more other
icon files as frames for the animation to play while the named event
is set.  Each frame of animation should be a single .ico file.  These
files are shown in the order they are specified.  If the --rewind
option is given, the animation will play backwards when it reaches the
last frame, instead of simply starting over from the first frame.

Alternatively, you may specify an INI file containing the above
information, and possibly a menu.  The format of the INI file should
be as follows:

  [ Program ]
  Command = String (may use $^X variable)
  Directory = String                             [optional]

  [ Icon ]
  Icon = Icon file
  Tip = String                                   [optional]
  Event = String                                 [optional]
  Rewind = True|False                            [optional]
  Delay = Number (ms)                            [optional]
  Frame = Icon file                              [optional]
  Frame = Icon file                              [optional]
  ...

  [ Menu ]                                       [optional]
  Label = Perl code
  - = undef # Separator
  Label = Perl code
  > Nested Label = Perl code
  ...

The value of each menu entry can be any Perl code that returns a
reference to a subroutine to be executed when the menu item is
selected, or undef.  There are two special functions that may be used
from the menu code: terminate(), and winexec().

terminate() forces the monitored program to quit (via signal 9), and
removes the tray icon.  You can use it as:

  Exit = \&terminate

winexec() launches another program in the background, without causing
the tray icon to freeze.  You can use it as:

  View Log = sub { winexec('notepad.exe logfile.log') }

The default menu consists of a single Exit item which calls terminate().
EOD


GetOptions(
  'h|help'      => sub { print $usage, $details; exit; },
  'e|execute=s' => \$command,
  'p|pid=i'     => \$child,
  'i|icon=s'    => \$icon,
  'f|frame=s'   => \@frames,
  'd|delay=i'   => \$anim_delay,
  'E|event=s'   => \$event,
  'r|rewind!'   => \$rewind,
  't|tip=s'     => \$tip,
) or windie $usage;

read_ini(shift) if @ARGV;

windie $usage unless $child || $command and $icon;


$child = winexec($command) if $command;
start_gui();


# Startup Functions

sub read_ini {
  my $file = shift;
  my $cfg = Config::IniFiles->new(-file => $file);
#    or windie("Unable to load $file: $@ \n");

if ( ! $cfg ) {
	
	foreach ( @Config::IniFiles::errors ) {
		Win32::MsgBox( $_ );
	}
	exit;
}
	
  $command    = $cfg->val('Program', 'Command'  ) || $command;
  $directory  = $cfg->val('Program', 'Directory') || $directory;
  $event      = $cfg->val('Icon',    'Event'    ) || $event;
  $tip        = $cfg->val('Icon',    'Tip'      ) || $tip;
  $icon       = $cfg->val('Icon',    'Icon'     ) || $icon;
  @frames     = $cfg->val('Icon',    'Frame'    );
  $anim_delay = $cfg->val('Icon',    'Delay'    ) || $anim_delay;
  $rewind     = $cfg->val('Icon',    'Rewind'   ) || $rewind;

  $rewind = lc($rewind) ne 'false' || $rewind;

  # Load the menu
  if($cfg->SectionExists('Menu')) {
    @menu = map { $_ => scalar $cfg->val('Menu', $_) } $cfg->Parameters('Menu');
  }
}

sub start_gui {
  $win  = Win32::GUI::Window->new();
  $icon = Win32::GUI::Icon->new($icon);

  if($event) {
    $event = Win32::Event->new(1, 0, $event);
    @frames = map { new Win32::GUI::Icon($_) } sort map { glob $_ } @frames;
    push @frames, reverse @frames if $rewind;
  }

  $win->Text($tip);
  $win->AddTimer('Poll', 500);

  set_icon($icon);

  # Translate the menu
  for(my $i=0; $i<$#menu; $i+=2) {
    my $ref   = eval $menu[$i+1];
    windie("Error in menu item $menu[$i]: $@\n") if $@;

    no strict 'refs';
    *{"MenuItem$i\_Click"} = $ref if defined $ref;

    $menu[$i] = " > $menu[$i]";
    $menu[$i+1] = "MenuItem$i";
  }
  unshift @menu, ('Tray' => 'Tray');

  $menu = Win32::GUI::MakeMenu(@menu);

  Win32::GUI::Dialog();
}


# GUI Functions

sub set_icon {
  my $img = shift;

  unless($niTray) {
    $niTray = $win->AddNotifyIcon(
      -id   => ICON_NUM,
      -name => 'Tray',
      -icon => $img,
      -tip  => $tip,
    );
  } else {
    Win32::GUI::NotifyIcon::Modify(
      $win,
      -id   => ICON_NUM,
      -name => 'Tray',
      -icon => $img,
      -tip  => $tip,
    );
  }
}

sub clear_icon {
  if($niTray) {
    $win->AddNotifyIcon(
      -id   => ICON_NUM,
      -name => 'Tray',
    );
    undef $niTray;
  }
}

{
  my $frame;
  my $atimer;

  sub start_animation {
    return if defined $frame;
    $atimer = $win->AddTimer('Animate', $anim_delay);
    $frame = 0;
  }

  sub stop_animation {
    return unless defined $frame;
    undef $frame;
    $atimer->Kill();
    set_icon($icon);
  }

  sub Animate_Timer {
    $frame = ($frame + 1) % @frames;
    set_icon($frames[$frame]);
    return 1;
  }
}

sub Poll_Timer {
  if(not kill 0, $child) {
    clear_icon();
    return -1;
  }

  if($event and $event->wait(0) == 1) {
    start_animation();
  } else {
    stop_animation();
  }

  return 1;
}

sub Tray_RightClick {
  $win->TrackPopupMenu($menu->{Tray}, Win32::GUI::GetCursorPos());
  return 1;
}


# User Functions

sub terminate {
  clear_icon();
  kill 9, $child;
  return -1;
}

sub winexec {
  my $cmd = join ' ', @_;
  my $pgm = $#_ > 1 ? shift : (split /(?<!\\) /, $cmd)[0];

  $pgm =~ s/(\$\^X)/$1/eeg;

  Win32::Process::Create(
    my $obj,
    $pgm,
    $cmd,
    0,
    DETACHED_PROCESS,
    $directory,
  ) or windie "Unable to start `$pgm': " . Win32::FormatMessage(
    Win32::GetLastError()
  );

  return $obj->GetProcessID();
}

sub restart {
	Win32::MsgBox("Restart");
}

sub sys_settings {
	Win32::MsgBox("System Settings");
}