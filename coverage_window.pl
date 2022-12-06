#!/usr/bin/perl

(@ARGV != 3) and die "usage: $0 in.cov window_size step_size\n";
$file_name = shift;
$wsize = shift;
$ssize = shift;

open IN, $file_name;
while ($l = <IN>) {
	push(@cov, $l + 0);
}

for $i (0..($wsize - 1)) {
	$sum += $cov[$i];
}
print($wsize / 2, "\t", $sum / $wsize, "\n");

for $i (1..$#cov) {
	$sum -= $cov[$i - 1];
	$sum += $cov[$i + $wsize - 1];
	if ($i % $ssize == 0) {
		print($i + $wsize / 2, "\t", $sum / $wsize, "\n");
	}
}
