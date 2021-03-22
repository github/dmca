package Logos::Group;
use strict;

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = {};
	$self->{NAME} = undef;
	$self->{EXPLICIT} = 1;
	$self->{INITIALIZED} = 0;
	$self->{INITLINE} = -1;
	$self->{CLASSES} = [];
	$self->{FUNCTIONS} = [];
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

sub explicit {
	my $self = shift;
	if(@_) { $self->{EXPLICIT} = shift; }
	return $self->{EXPLICIT};
}

sub initialized {
	my $self = shift;
	if(@_) { $self->{INITIALIZED} = shift; }
	return $self->{INITIALIZED};
}

sub initRequired {
	my $self = shift;
	for(@{$self->{CLASSES}}) {
		return 1 if $_->initRequired;
	}
	for(@{$self->{FUNCTIONS}}) {
		return 1 if $_->initRequired;
	}
	return 0;
}

sub identifier {
	my $self = shift;
	return main::sanitize($self->{NAME});
}

sub initLine {
	my $self = shift;
	if(@_) { $self->{INITLINE} = shift; }
	return $self->{INITLINE};
}

sub classes {
	my $self = shift;
	return $self->{CLASSES};
}

sub functions {
	my $self = shift;
	return $self->{FUNCTIONS};
}
##### #
# END #
# #####

sub addClass {
	my $self = shift;
	my $class = shift;
	$class->group($self);
	push(@{$self->{CLASSES}}, $class);
}

sub addClassNamed {
	my $self = shift;
	my $name = shift;

	my $class = $self->getClassNamed($name);
	return $class if defined($class);

	$class = ::Class()->new();
	$class->name($name);
	$self->addClass($class);
	return $class;
}

sub getClassNamed {
	my $self = shift;
	my $name = shift;
	foreach(@{$self->{CLASSES}}) {
		return $_ if $_->name eq $name;
	}
	return undef;
}

sub addFunction {
	my $self = shift;
	my $args = shift;

	my $functionRetval = undef;
	my $functionName = undef;
	my $functionArgs = [];

	my $argIdx = 0;
	for (@$args) {
		if ($argIdx == 0) {
			$argIdx++;
			$functionRetval = $_;
		} elsif ($argIdx == 1) {
			$argIdx++;
			$functionName = $_;
		} else {
			push(@$functionArgs, $_);
		}
	}
	
	my $function = ::Function()->new();
	$function->retval($functionRetval);
	$function->name($functionName);
	for(@$functionArgs) {
		$function->addArg($_);
	}
	$function->group($self);
	push(@{$self->{FUNCTIONS}}, $function);

	return $function;
}

1;
