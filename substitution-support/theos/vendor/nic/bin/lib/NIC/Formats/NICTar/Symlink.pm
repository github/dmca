package NIC::Formats::NICTar::Symlink;
use parent NIC::NICBase::Symlink;
use strict;

sub tarfile {
	my $self = shift;
	if(@_) { $self->{TARFILE} = shift; }
	return $self->{TARFILE};
}

1;
