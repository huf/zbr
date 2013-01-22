#!/usr/bin/perl
use strict;
use warnings;
use feature qw(say);

use Data::Dumper;

sub framesize {
	my ($w, $h) = (0)x2;

	for my $f (@_) {
		$h = @$f unless $h > @$f;

		for my $l (@$f) {
			$w = length $l unless $w > length $l;
		}
	}
	if ($w%2) {
		$w += 1;
	}
	($w+2, $h);
}

sub fixframes {
	my ($w, $h, $frames) = @_;
	for my $f (@$frames) {
		for my $l (@$f) {
			$l = ' '.$l;
			my $ll = length $l;
			$l .= ' 'x($w-$ll) if $ll < $w;
		}
		if ($h > @$f) {
			unshift @$f, (' 'x$w)x($h-@$f);
		}
	}
}

sub fmtline {
	my $_ = shift;
	s/(["\\])/\\$1/g;
	$_;
}

sub fmtframes {
	my $frames = shift;
	print "  {\n";
	for my $f (@$frames) {
		print "    {\n";
		for my $l (@$f) {
			my $fl = fmtline($l);
			print qq{      "$fl",\n};
		}
		print "    },\n";
	}
	print "  },\n";
}

die "zebra.txt\n" unless @ARGV;
die "real file man\n" unless -f $ARGV[0];

my $raw = do { local(@ARGV, $/) = $ARGV[0]; <> } or die "bad file\n";

my @right = map { [ split /\n/, $_ ] } split /\n{2}/, $raw;

my ($w, $h) = framesize(@right);

fixframes($w, $h, \@right);

die "left and right must be same\n" if @right & 0x1;

my @left = splice @right, 0, @right/2;

say qq{#define NR_DIR 2};
say qq{#define NR_FRAMES }, scalar @right;
say qq{#define NR_W }, $w;
say qq{#define NR_H }, $h;

print "static char frames[NR_DIR][NR_FRAMES][NR_H][NR_W+1] = {\n";
for my $dir (\@left, \@right) {
	fmtframes($dir);
}
print "};\n";
