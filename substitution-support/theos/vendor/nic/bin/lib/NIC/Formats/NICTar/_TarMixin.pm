package NIC::Formats::NICTar::_TarMixin;
use strict;

sub _take_init {
	my $self = shift;
	$self->{TARFILE} = undef;
}

sub tarfile {
	my $self = shift;
	if(@_) { $self->{TARFILE} = shift; }
	return $self->{TARFILE};
}

sub _mode {
	my $self = shift;
	return $self->tarfile->mode;
}

1;
