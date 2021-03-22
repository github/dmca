#!/usr/bin/perl
use warnings;
use strict;
if(@ARGV < 3) {
	exit 1;
}
my @afirst = split(/\./, $ARGV[0]);
my $op = $ARGV[1];
my @asecond = split(/\./, $ARGV[2]);

push(@afirst, 0) while(@afirst < @asecond);
push(@asecond, 0) while(@asecond < @afirst);

my $v1 = 0;
my $v2 = 0;
map { ($v1 *= 100) += $_ } (@afirst);
map { ($v2 *= 100) += $_ } (@asecond);

print "1" if $v1 > $v2 && $op eq "gt";
print "1" if $v1 < $v2 && $op eq "lt";
print "1" if $v1 >= $v2 && $op eq "ge";
print "1" if $v1 <= $v2 && $op eq "le";
print "1" if $v1 == $v2 && $op eq "eq";
