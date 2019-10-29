#!/usr/bin/perl -T

use strict; 
use warnings qw(FATAL all);

use Scalar::Util qw(looks_like_number);

my $lines_counter = 10;
my $counter = 1;
my @files;
my $omit = 0;

head();

sub head {

	if(parse_args() == -1){
		return -1;
	}

	if($#files == -1){
		push @files, "-";
	}

	foreach my $file(@files){
		if($#files > 0 and $omit == 0){
			print "==> $file <==\n";
		}
		print_lines($file);
		if($#files > 0 and $omit == 0){
			print "\n";
		}
	}

}

sub parse_args {

	my $i = 0;

	while ($i <= $#ARGV){

		if ($ARGV[$i] eq "-n"){
			if (looks_like_number($ARGV[++$i])){ 
				$lines_counter = $ARGV[$i];
			}
			else{
				print "invalid number of lines: '$ARGV[$i]'";
				return -1;
			}
		}
		elsif($ARGV[$i] eq "-q"){
			$omit = 1;
		}
		elsif($ARGV[$i] =~ /-.+/){
			print "invalid option -- '$ARGV[$i]'";
			return -1;
		}
		else{
			last;
		}
		$i++;

	}

	while ($i <= $#ARGV){
		push @files, $ARGV[$i++];

	}

}

sub print_lines {

	my $file = $_[0];
	my $FH;

	if (-d $file and $file ne "-"){
		print "error reading '$file': Is a directory\n";
		return -1;
	}

	unless (-r $file || $file eq "-"){
		print "cannot open '$file' for reading: Permission denied\n";
		return -1;
	}

	my $count = 1;

	if ($file eq "-"){
		$FH = *STDIN;
	}
	else{
		open($FH, '<', $file);
	}

	while(<$FH>){

		print $_;
		last if ($count >= $lines_counter);
		$count++;

	}

	close($FH);
}