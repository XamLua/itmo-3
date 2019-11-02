#!/usr/bin/perl -T

use strict; 
use warnings qw(FATAL all);
use FileHandle;

$ENV{"PATH"} = "/usr/bin/";
$ENV{"ENV"} = "/usr/bin/";

pipe(READER, WRITER);

FileHandle::autoflush WRITER 1;

my $pid = fork();
#Child
if ($pid == 0){
	close WRITER;

	open \*STDIN, ">&", \*READER;

	my $result = `wc`;

	close(READER);

	print $result;
}
#Parent
else{
	close READER;

	my $fp = $ARGV[0];

	my ($fh, $char);

	open $fh, "<", $fp;

	my $read;
	my $check = 0;
	while ($read = read $fh, $char, 1) {
		if ($check % 2 == 0){
    		print WRITER "$char";
    	}
    	$check = ($check + 1) % 2;
	}
	close(WRITER);
}