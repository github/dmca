package NIC::Bridge::NICBase;
use strict;
use warnings;
use parent qw(NIC::Bridge::_BridgedObject);
use NIC::NICBase::Directory;
use NIC::NICBase::File;
use NIC::NICBase::Symlink;

sub variables {
	my $self = shift;
	return keys %{$self->{FOR}->{VARIABLES}};
}

sub variable :lvalue {
	my $self = shift;
	$self->{FOR}->variable(@_);
}

sub mkdir {
	my $self = shift;
	my $dirname = shift;
	my $mode = shift;
	my $ref = $self->{FOR}->_getContent($dirname);
	NIC::NICBase::Directory->take($ref);
	$ref->mode($mode) if $mode;
	return $self->{CONTEXT}->_wrap($ref);
}

sub mkfile {
	my $self = shift;
	my $name = shift;
	my $mode = shift;
	my $ref = $self->{FOR}->_getContent($name);
	NIC::NICBase::File->take($ref);
	$ref->mode($mode) if $mode;
	return $self->{CONTEXT}->_wrap($ref);
}

sub symlink {
	my $self = shift;
	my $oldfile = shift;
	my $newfile = shift;
	my $ref = $self->{FOR}->_getContent($newfile);

	my $realtarget = ref($oldfile) ? $self->{CONTEXT}->_unwrap($oldfile) : $self->{FOR}->_getContentWithoutCreate($oldfile);
	$realtarget = $oldfile if !$realtarget;

	NIC::NICBase::Symlink->take($ref, $realtarget);

	return $self->{CONTEXT}->_wrap($ref);
}

sub lookup {
	my $self = shift;
	my $name = shift;
	return $self->{CONTEXT}->_wrap($self->{FOR}->_getContentWithoutCreate($name));
}

sub setConstraint {
	my $self = shift;
	my $constraint = shift;
	$self->{FOR}->addConstraint($constraint);
}

sub clearConstraint {
	my $self = shift;
	my $constraint = shift;
	$self->{FOR}->removeConstraint($constraint);
}

sub prompt {
	my $self = shift;
	$self->{CONTEXT}->_prompt($self->{FOR}, @_);
}

1;
