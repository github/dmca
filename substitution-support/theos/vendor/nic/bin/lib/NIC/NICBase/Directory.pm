package NIC::NICBase::Directory;
use strict;
use warnings;
use parent qw(NIC::NICType);
use File::Path qw(mkpath);

sub type {
	my $self = shift;
	return NIC::NICType::TYPE_DIRECTORY;
}

sub _mode {
	return 0755;
}

sub create {
	my $self = shift;
	mkpath($self->{OWNER}->substituteVariables($self->{NAME}), { mode => $self->mode }) or return 0;
	return 1;
}


1;

