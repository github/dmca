package Logos::Class;
use strict;

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = {};
	$self->{NAME} = undef;
	$self->{EXPR} = undef;
	$self->{METAEXPR} = undef;
	$self->{TYPE} = undef;
	$self->{META} = 0;
	$self->{INST} = 0;
	$self->{OVERRIDDEN} = 0;
	$self->{REQUIRED} = 0;
	$self->{METHODS} = [];
	$self->{NUM_METHODS} = 0;
	$self->{PROPERTIES} = [];
	$self->{NUM_PROPERTIES} = 0;
	$self->{GROUP} = undef;
	bless($self, $class);
	return $self;
}

##################### #
# Setters and Getters #
# #####################
sub name {
	my $self = shift;
	if(@_) { $self->{NAME} = shift; }
	return $self->{NAME};
}

sub expression {
	my $self = shift;
	if(@_) {
		$self->{EXPR} = shift;
		$self->type("id");
		$self->{OVERRIDDEN} = 1;
	}
	return $self->{EXPR};
}

sub metaexpression {
	my $self = shift;
	if(@_) {
		$self->{METAEXPR} = shift;
		$self->{OVERRIDDEN} = 1;
	}
	return $self->{METAEXPR};
}

sub type {
	my $self = shift;
	if(@_) { $self->{TYPE} = shift; }
	return $self->{TYPE} if $self->{TYPE};
	return $self->{NAME}."*";
}

sub hasmetahooks {
	my $self = shift;
	if(@_) { $self->{META} = shift; }
	return $self->{META};
}

sub hasinstancehooks {
	my $self = shift;
	if(@_) { $self->{INST} = shift; }
	return $self->{INST};
}

sub group {
	my $self = shift;
	if(@_) { $self->{GROUP} = shift; }
	return $self->{GROUP};
}

sub required {
	my $self = shift;
	if(@_) { $self->{REQUIRED} = shift; }
	return $self->{REQUIRED};
}

sub overridden {
	my $self = shift;
	return $self->{OVERRIDDEN};
}

sub methods {
	my $self = shift;
	return $self->{METHODS};
}

sub properties {
	my $self = shift;
	return $self->{PROPERTIES};
}

sub initRequired {
	my $self = shift;
	return $self->required || scalar @{$self->{METHODS}} > 0 || scalar @{$self->{PROPERTIES}} > 0;
}

##### #
# END #
# #####

sub addMethod {
	my $self = shift;
	my $hook = shift;
	push(@{$self->{METHODS}}, $hook);
	$self->{NUM_METHODS}++;
}

sub addProperty {
	my $self = shift;
	my $property = shift;
	push(@{$self->{PROPERTIES}}, $property);
	$self->{NUM_PROPERTIES}++;
}

1;
