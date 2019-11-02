#!/usr/bin/perl -T

use strict; 
use warnings qw(FATAL all);

$ENV{"PATH"} = "/usr/bin/";
$ENV{"ENV"} = "/usr/bin/";

my $start_run = time();
my ($name, $pass, $uid, $gid, $quota, $comment, $gcos, $dir, $shell, $expire) = getpwuid($<);
my $work_time = 0;
my $data = `uptime`;
my ($load1, $load5, $load15) = $data =~ /:\s+([\d,]+),\s+([\d,]+),\s+([\d,]+)/;

$SIG{HUP} = \&hup_handler;
$SIG{INT}  = \&int_handler;
$SIG{TERM}  = \&term_handler;
$SIG{USR1} = \&usr1_handler;
$SIG{USR2}  = \&usr2_handler;

while (1){
	sleep(1);
	my $work_time = time() - $start_run;
	my $data = `uptime`;
	my ($load1, $load5, $load15) = $data =~ /:\s+([\d,]+),\s+([\d,]+),\s+([\d,]+)/;
}

sub hup_handler {
    print "Server pid: $$\n";
}

sub int_handler {
    print "Server uid: $uid\n";
}

sub term_handler {
    print "Server gid: $gid\n";
}

sub usr1_handler {
    print "Seconds from start: $work_time\n";
}

sub usr2_handler {
    print "Average Load for 1 minute: $load1\n";
    print "Average Load for 5 minute: $load5\n";
    print "Average Load for 15 minute: $load15\n";
}