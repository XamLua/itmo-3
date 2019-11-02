#!/usr/bin/perl -T

use strict; 
use warnings qw(FATAL all);

use Socket;

use IO::Socket::UNIX;

my $socket_path = './perlsocket_313';

my $socket = IO::Socket::UNIX->new(
   Type => SOCK_STREAM,
   Peer => $socket_path,
)
   or die("Can't connect to server: $!\n");

while (my $line = <$socket>){
	print $line;
}
