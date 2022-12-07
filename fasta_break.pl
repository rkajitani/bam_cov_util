#!/usr/bin/perl

(@ARGV < 3) and die "usage: $0 ref.fa min_len breakpoint1.tsv [breakpoint2.tsv...]\n";

open($fasta_fh, $ARGV[0]) or die "cannot open $ARGV[0]\n";
$ref{$name} = $seq while (($name, $seq) = fasta_get($fasta_fh));
		
close($fasta_fh);

$min_len = $ARGV[1];

for $tsv_name (@ARGV[2..$#ARGV]) {
	open(IN, $tsv_name) or die "cannot open $tsv_name\n";
	while (chomp($l = <IN>)) {
		($name, $pos) = split(/\t/, $l);
		if ($pos < length($ref{$name})) {
			substr($ref{$name}, $pos, 1) = lc substr($ref{$name}, $pos, 1);
		}
	}
	close IN;
}

for $name (sort{$a cmp $b} keys %ref) {
	$n = 0;
	$ref{$name} =~ s/^([a-z]+)/uc($1)/e;
	$ref{$name} =~ s/([a-z]+)$/uc($1)/e;
	for $seq (grep{length($_) >= $min_len} split(/[a-z]/, $ref{$name})) {
		$seq =~ s/^(N+)//;
		$seq =~ s/N+$//;
		if (length($seq) >= $min_len) {
			print ">$name-$n\n";
			print_seq($seq, 80);
			++$n;
		}
	}
}


sub fasta_get
{
	my $in = shift;
	my($l, $name, $seq);

	while ($l = <$in>) {
		if ($l =~ /^>(\S*)/) {
			$name = $1;
			last;
		}
	}
	if ($l eq '') {
		return ();
	}

	while ($l = <$in>) {
		if (substr($l, 0, 1) eq '>') {
			seek($in, -length($l), 1);
			return ($name, $seq);
		}
		chomp $l;
		$seq .= $l 
	}
	return ($name, $seq);
}

sub print_seq
{
	my $seq = shift;
	my $line_len = shift;
	my $seq_len = length $seq;

	unless ($line_len) {
		$line_len = 80;
	}

	for (my $i = 0; $i < $seq_len; $i += $line_len) {
		print(substr($seq, $i, $line_len) . "\n");
	}
}
