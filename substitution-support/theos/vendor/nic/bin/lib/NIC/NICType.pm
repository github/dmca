package NIC::NICType;
use strict;
use warnings;

use constant {
	TYPE_UNKNOWN => 0,
	TYPE_DIRECTORY => 1,
	TYPE_FILE => 2,
	TYPE_SYMLINK => 3
};

use overload '""' => sub {
	my $self = shift;
	my $ref = ref($self);
	$ref =~ s/^.*::(\w+)$/$1/g;
	return '"'.$self->name."\" ($ref, mode ".sprintf("%4.04o", $self->mode).")";
};

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = {};
	$self->{OWNER} = shift // undef;
	$self->{NAME} = shift // undef;
	$self->{MODE} = undef;
	$self->{CONSTRAINTS} = [];
	bless($self, $class);

	return $self;
}

sub _take_init {
}

sub take {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $obj = shift;
	bless($obj, $class);
	$obj->_take_init(@_);
}

sub name {
	my $self = shift;
	if(@_) { $self->{NAME} = shift; }
	return $self->{NAME};
}

sub _mode {
	return 0;
}

sub mode {
	my $self = shift;
	if(@_) { $self->{MODE} = shift; }
	return $self->{MODE} // $self->_mode;
}

sub type {
	my $self = shift;
	return TYPE_UNKNOWN;
}

sub constraints {
	my $self = shift;
	return @{$self->{CONSTRAINTS}};
}

sub addConstraint {
	my $self = shift;
	my $constraint = shift;
	push(@{$self->{CONSTRAINTS}}, $constraint);
}

sub create {
	return 0;
}

1;
