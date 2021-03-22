package Logos::StaticClassGroup;
use Logos::Group;
our @ISA = ('Logos::Group');

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = Logos::Group->new();
	$self->name("_staticClass");
	$self->explicit(0);
	$self->{DECLAREDONLYCLASSES} = {};
	$self->{USEDCLASSES} = {};
	$self->{USEDMETACLASSES} = {};
	bless($self, $class);
	return $self;
}

sub addUsedClass {
	my $self = shift;
	my $class = shift;
	$self->{USEDCLASSES}{$class}++;
}

sub addUsedMetaClass {
	my $self = shift;
	my $class = shift;
	$self->{USEDMETACLASSES}{$class}++;
}

sub addDeclaredOnlyClass {
	my $self = shift;
	my $class = shift;
	$self->{DECLAREDONLYCLASSES}{$class}++;
}

sub declaredOnlyClasses {
	my $self = shift;
	return $self->{DECLAREDONLYCLASSES};
}

sub usedClasses {
	my $self = shift;
	return $self->{USEDCLASSES};
}

sub usedMetaClasses {
	my $self = shift;
	return $self->{USEDMETACLASSES};
}

1;
