package NIC::Bridge::Context;
use strict;
use warnings;
use subs qw(warn exit);
use Module::Load::Conditional qw(can_load);
use NIC::Tie::PrefixedHandleRedirect;
use NIC::Bridge::NICBase;

our %handlers = (
	PROMPT => sub { },
);

sub import {
	my $package = shift;
	my %arg = @_;
	for(keys %arg) {
		$handlers{$_} = $arg{$_};
	}
}

our $bridge = undef;
our $global_ret = undef;
our $errored_out = undef;

sub NIC {
	return $bridge;
}

sub warn(@) {
	print STDERR "[".$bridge->{FOR}->name."/warning] ",@_,$/;
}

sub error(@) {
	print STDERR "[".$bridge->{FOR}->name."/error] ",@_,$/;
	$errored_out = 1;
	die;
}

sub exit {
	$global_ret = shift;
	die;
}

sub prompt {
	__PACKAGE__->_prompt($bridge->{FOR}, undef, @_);
}

sub _prompt {
	my $self = shift;
	my $nic = shift;

	my $n = scalar @_;
	my $opts = $_[$n-1];
	if(ref $opts eq "HASH") {
		$n--;
	} else {
		$opts = {};
	}

	my $variable;
	$variable = shift unless $n == 1;
	my $promptstring = shift;

	$handlers{PROMPT}->($nic, $variable, $promptstring, $opts->{default});
}

sub _wrap {
	my $self = shift;
	my @r = map {
		my $wrap = $_;
		my $_wrapType = $wrap ? (ref $wrap) : "_Undefined";
		if(!$_wrapType || (ref($wrap) && $wrap->isa("NIC::Bridge::_BridgedObject"))) {
			return $wrap;
		} else {
			$_wrapType =~ s/.*:://;
			my $wrappingClass = "NIC::Bridge::$_wrapType";
			can_load(modules=>{$wrappingClass=>undef}, verbose=>0) or return undef;
			my $wrapper = $wrappingClass->new($self, $wrap);
			return $wrapper;
		}
	} (@_);
	return @r if wantarray;
	return (@r > 0 ? $r[0] : undef);
}

sub _unwrap {
	my $self = shift;
	my @r = map { (ref($_) && $_->isa("NIC::Bridge::_BridgedObject")) ? $_->{FOR} : $_; } (@_);
	return @r if wantarray;
	return (@r > 0 ? $r[0] : undef);
}

sub _execute {
	my $self = shift;
	my $nic = shift;
	my $script = shift;
	my $ret = 1;
	{
		local $global_ret;
		local $errored_out;
		local $bridge = NIC::Bridge::NICBase->new($self, $nic);
		local $SIG{__DIE__} = sub { };
		tie *OVERRIDE, "NIC::Tie::PrefixedHandleRedirect", *STDERR, $nic->name;
		my $stdout = select(*OVERRIDE);
		eval("#line 1 ".$nic->name."/control.pl\n".$script);
		select($stdout);
		if(defined $errored_out) {
			$ret = 0;
		} elsif(defined $global_ret) {
			$ret = $global_ret;
			print STDERR "[".$nic->name."/error] Control script exited with status $ret.",$/;
		} elsif($@) {
			$ret = 0;
			print STDERR "[".$nic->name."/error] Control script exited due to an error: $@";
		}
	}
	return $ret;
}

1;
