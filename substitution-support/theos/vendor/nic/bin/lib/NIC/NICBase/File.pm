package NIC::NICBase::File;
use strict;
use warnings;
use parent qw(NIC::NICType);

sub _take_init {
	my $self = shift;
	$self->{DATA} = undef;
}

sub type {
	my $self = shift;
	return NIC::NICType::TYPE_FILE;
}

sub _mode {
	return 0644;
}

sub data {
	my $self = shift;
	$self->{DATA} = shift if @_;
	$self->{DATA};
}

sub create {
	my $self = shift;
	my $filename = $self->{OWNER}->substituteVariables($self->name);
	open(my $nicfile, ">", $filename) or return 0;
	print $nicfile $self->{OWNER}->substituteVariables($self->data);
	close($nicfile);
	chmod($self->mode, $filename);
	return 1;
}

1;
