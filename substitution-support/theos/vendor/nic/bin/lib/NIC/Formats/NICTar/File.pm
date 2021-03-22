package NIC::Formats::NICTar::File;
use parent qw(NIC::Formats::NICTar::_TarMixin NIC::NICBase::File);
use strict;

sub _take_init {
	my $self = shift;
	$self->NIC::NICBase::File::_take_init(@_);
	$self->NIC::Formats::NICTar::_TarMixin::_take_init(@_);
}

sub data {
	my $self = shift;
	return $self->SUPER::data(@_) // $self->tarfile->get_content;
}

1;
