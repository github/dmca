package Logos::Patch::Source::Generator;
use strict;

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = {};
	$self->{OBJECT} = shift;
	$self->{METHOD} = shift;
	my @args = @_;
	$self->{ARGS} = \@args;
	bless($self, $class);
	return $self;
}

sub eval {
	#no strict 'refs';
	my $self = shift;
	my @args = @{$self->{ARGS}};
	splice(@args, 0, 0, $self->{OBJECT}) if $self->{OBJECT};
	return Logos::Generator::for($self->{OBJECT})->can($self->{METHOD})->(@args);
	#my $thunk = Logos::Generator::for($self->{OBJECT})->can($self->{METHOD})-(>${$self->{ARGS}});;
	#my $mname = $self->{METHOD};
	#return $thunk->$mname(@{$self->{ARGS}});
}

1;
