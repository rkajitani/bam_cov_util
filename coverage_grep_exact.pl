#!/usr/bin/perl

(@ARGV != 2) and die "usage: $0 seq_name in.cov\n";
$seq_name = shift;
$file_name = shift;

$seq_name = quotemeta $seq_name;

open IN, $file_name;
while ($l = <IN>) {
	if ($l =~ /^>/) {
		chomp $l;
		if (substr($l, 1) eq $seq_name) {
			$l = <IN>;
			while ($l !~ /^>/ and $l ne '') {
				print $l;
				$l = <IN>;
			}
			exit;
		}
	}
}
