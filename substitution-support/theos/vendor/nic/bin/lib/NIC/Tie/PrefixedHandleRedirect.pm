package NIC::Tie::PrefixedHandleRedirect;
use strict;
use parent qw(Tie::Handle);
sub TIEHANDLE {
	my $proto = shift;
	my $fh = shift;
	my $prefix = shift;
	return bless [$fh, $prefix], $proto;
}

sub _token {
	return "[".$_[0]->[1]."] ";
}

sub PRINT {
	my $t = $_[0]->_token;
	my $fh = $_[0]->[0];
	shift;
	my $str = $t.join('', @_);
	$str =~ s#$/+$##g;
	$str =~ s#$/#$/$t#g;
	print $fh $str,$/;
}

sub PRINTF {
	my $t = $_[0]->_token;
	my $fh = $_[0]->[0];
	shift;
	my $str = $t.sprintf(shift, @_);
	$str =~ s#$/+$##g;
	$str =~ s#$/#$/$t#g;
	print $fh $str,$/;
}
1;
