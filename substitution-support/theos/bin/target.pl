#!/usr/bin/perl
use File::Temp;

my @o;
for(reverse @ARGV) {
	my $i = 0;
	for my $a (split /:/) {
		if(length $a > 0) {
			@o = () if($i == 0 && $o[$i] && $o[$i] ne $a);
			$o[$i] = $a;
		}
		$i++;
	}
}
#print join(':', map { $_ eq "" ? "-" : $_ } @o);
my $i = 0;
my ($fh, $tempfile) = File::Temp::tempfile();
binmode($fh, ":utf8");

for(@o) {
	print $fh "export __THEOS_TARGET_ARG_$i := $_\n";
	++$i;
}

close($fh);
print $tempfile,$/;
