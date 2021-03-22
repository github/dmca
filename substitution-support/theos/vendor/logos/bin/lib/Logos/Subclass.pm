package Logos::Subclass;
use Logos::Class;
our @ISA = ('Logos::Class');

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = $class->SUPER::new();
	$self->{SUPERCLASS} = undef;
	$self->{PROTOCOLS} = {};
	$self->{IVARS} = [];
	$self->{OVERRIDDEN} = 1;
	bless($self, $class);
	return $self;
}

##################### #
# Setters and Getters #
# #####################
sub superclass {
	my $self = shift;
	if(@_) { $self->{SUPERCLASS} = shift; }
	return $self->{SUPERCLASS};
}

sub ivars {
	my $self = shift;
	return $self->{IVARS};
}

sub protocols {
	my $self = shift;
	return $self->{PROTOCOLS};
}

sub initRequired {
	my $self = shift;
	return 1; # Subclasses must always be initialized.
}

##### #
# END #
# #####

sub addProtocol {
	my $self = shift;
	my $protocol = shift;
	$self->{PROTOCOLS}{$protocol}++;
}

sub addIvar {
	my $self = shift;
	my $ivar = shift;
	$ivar->class($self);
	push(@{$self->{IVARS}}, $ivar);
}

sub getIvarNamed {
	my $self = shift;
	my $name = shift;
	foreach(@{$self->{IVARS}}) {
		return $_ if $_->name eq $name;
	}
	return undef;
}

1;
