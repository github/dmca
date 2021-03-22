package Logos::Generator::Thunk;
use strict;

our $AUTOLOAD;

my %subrefCache;

sub AUTOLOAD {
	my $self = shift;
	my $method = $AUTOLOAD;
	return if $method eq "DESTROY";

	$method =~ s/.*:://;
	my $fullyQualified = $self->{PACKAGE}."::".$method;
	my $subref = $subrefCache{$fullyQualified};

	$subref = $self->can($method) if !$subref;

	unshift @_, $self->{OBJECT} if $self->{OBJECT};
	goto &$subref;
}

sub can {
	my $self = shift;
	my $method = shift;
	my $subref = $self->SUPER::can($method);
	return $subref if $subref;

	$method =~ s/.*:://;
	my $fullyQualified = $self->{PACKAGE}."::".$method;
	return $subrefCache{$fullyQualified} if $subrefCache{$fullyQualified};

	$subref = sub {unshift @_, $self->{PACKAGE}; goto &{$self->{PACKAGE}->can($method)}};
	$subrefCache{$fullyQualified} = $subref;

	return $subref;
}

sub DESTROY {
	my $self = shift;
	$self->SUPER::destroy();
}

sub for {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = {};
	$self->{PACKAGE} = shift;
	$self->{OBJECT} = shift;
	bless($self, $class);
	return $self;
}

1;
