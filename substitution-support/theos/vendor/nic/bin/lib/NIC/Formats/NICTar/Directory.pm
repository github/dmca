package NIC::Formats::NICTar::Directory;
use parent qw(NIC::Formats::NICTar::_TarMixin NIC::NICBase::Directory);
use strict;

sub _take_init {
	my $self = shift;
	$self->NIC::NICBase::Directory::_take_init(@_);
	$self->NIC::Formats::NICTar::_TarMixin::_take_init(@_);
}

1;
