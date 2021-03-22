package NIC::NICBase::Symlink;
use strict;
use warnings;
use parent qw(NIC::NICType);
use Scalar::Util qw(refaddr);

use overload '""' => sub {
	my $self = shift;
	my $ref = ref($self);
	$ref =~ s/^.*::(\w+)$/$1/g;
	my $target = (ref($self->target) && refaddr($self) == refaddr($self->target)) ? "itself" : "\"".$self->target."\"";
	return '"'.$self->name."\" ($ref to $target)";
};

sub _take_init {
	my $self = shift;
	$self->{TARGET} = shift // undef;
}

sub type {
	my $self = shift;
	return NIC::NICType::TYPE_SYMLINK;
}

sub target {
	my $self = shift;
	if(@_) { $self->{TARGET} = shift; }
	return $self->{TARGET};
}

sub target_type {
	my $self = shift;
	my $t = $self->{TARGET};
	return ref($t) ? $t->type : NIC::NICType::TYPE_UNKNOWN;
}

sub target_name {
	my $self = shift;
	my $t = $self->{TARGET};
	return ref($t) ? $t->name : $t;
}

sub create {
	my $self = shift;
	my $name = $self->{OWNER}->substituteVariables($self->{NAME});
	my $dest = $self->{OWNER}->substituteVariables($self->target_name);
	symlink($dest, $name) or return 0;
	return 1;
}

1;

