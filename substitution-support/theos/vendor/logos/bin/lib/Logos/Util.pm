package Logos::Util;
use 5.006;
use strict;
our @ISA = ('Exporter');
our @EXPORT = qw(quotes fallsBetween sanitize matchedParenthesisSet nestedParenString smartSplit);
our $errorhandler = \&_defaultErrorHandler;

sub _defaultErrorHandler {
	die shift;
}

sub quotes {
	my ($line) = @_;
	my @quotes = ();
	while($line =~ /(?<!\\)\"/g) {
		push(@quotes, $-[0]);
	}
	return @quotes;
}

sub fallsBetween {
	my $idx = shift;
	while(@_ > 0) {
		my $start = shift;
		my $end = shift;
		return 1 if ($start < $idx && (!defined($end) || $end > $idx))
	}
	return 0;
}

sub sanitize {
	my $input = shift;
	my $output = $input;
	$output =~ s/[^\w]//g;
	return $output;
}

sub matchedParenthesisSet {
	my $in = shift;
	my $atstart = shift;
	$atstart = 1 if !defined $atstart;
	my $untilend = shift;
	$untilend = 0 if !defined $untilend;

	my @parens;
	if(!$atstart || $in =~ /^\s*\(/) {
		# If we encounter a ) that puts us back at zero, we found a (
		# and have reached its closing ).
		my $parenmatch = $in;
		my $pdepth = 0;
		my @pquotes = quotes($parenmatch);
		while($parenmatch =~ /[;()]/g) {
			next if fallsBetween($-[0], @pquotes);

			if($& eq "(") {
				if($pdepth == 0) { push(@parens, $+[0]); }
				$pdepth++;
			} elsif($& eq ")") {
				$pdepth--;
				if($pdepth == 0) { push(@parens, $+[0]); last if(!$untilend); }
			}
		}
	}

	return undef if scalar @parens == 0;
	# Odd number of parens means a closing paren was left off!
	&$errorhandler("missing closing parenthesis") if scalar @parens % 2 == 1;
	return @parens;
}

sub nestedParenString {
	my $in = shift;
	my ($opening, $closing) = matchedParenthesisSet($in);

	my @ret;
	if(defined $opening) {
		$ret[0] = substr($in, $opening, $closing - $opening - 1);
		$in = substr($in, $closing);
	}
	$ret[1] = $in;
	return @ret;
}

sub smartSplit {
	my $re = shift;
	my $in = shift;
	return () if !$in || $in eq "";

	my $limit = shift;
	$limit = 0 if !defined $limit;

	my @quotes = quotes($in);
	# We pass 1 for arg 3 to catch all matching parentheses until the end of the string
	# as smartSplit only operates on a substring.
	my @parens = matchedParenthesisSet($in, 0, 1);

	my $lstart = 0;
	my @pieces = ();
	my $piece = "";
	while($in =~ /$re/g) {
		next if (defined $parens[0] && fallsBetween($-[0], @parens)) || fallsBetween($-[0], @quotes);
		$piece = substr($in, $lstart, $-[0]-$lstart);
		push(@pieces, $piece);
		$lstart = $+[0];
		$limit--;
		last if($limit == 1); # One item left? Bail out and throw the rest of the string into it!
	}
	$piece = substr($in, $lstart);
	push(@pieces, $piece);
	return @pieces;
}

1;
