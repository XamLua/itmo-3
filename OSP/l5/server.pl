#!/usr/bin/perl -T

use strict; 
use warnings qw(FATAL all);

use IO::Socket::UNIX;

$ENV{"PATH"} = "/usr/bin/";
$ENV{"ENV"} = "/usr/bin/";

my $socket_path = './perlsocket_313';
unlink($socket_path);

my $start_run = time();

my $server = IO::Socket::UNIX->new(
   Type   => SOCK_STREAM,
   Local  => $socket_path,
   Listen => 313,
)
   or die("Can't create server socket: $!\n");

my ($name, $pass, $uid, $gid, $quota, $comment, $gcos, $dir, $shell, $expire) = getpwuid($<);

while (my $conn = $server->accept()) {
	my $work_time = time() - $start_run;
	my $data = `uptime`;
	my ($load1, $load5, $load15) = $data =~ /:\s+([\d,]+),\s+([\d,]+),\s+([\d,]+)/;
    $conn->print("Server pid: $$ \nServer uid: $uid \nServer gid: $gid\n" .
        		 "Seconds from start: $work_time\n" .
        		 "Average Load for 1 minute: $load1\n" .
        		 "Average Load for 5 minute: $load5\n" .
        		 "Average Load for 15 minute: $load15\n");
}