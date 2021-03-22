package Logos::Generator::Base::StaticClassGroup;
use strict;

sub _methodForClassWithScope {
	my $self = shift;
	my $class = shift;
	my $scope = shift;
	my $return = "";
	my $methodname = Logos::sigil($scope eq "+" ? "static_metaclass_lookup" : "static_class_lookup").$class;
	my $lookupMethod = $scope eq "+" ? "objc_getMetaClass" : "objc_getClass";

	# This is a dirty assumption - we believe that we will always be using a compiler that defines __GNUC__ and respects GNU C attributes.
	return "static __inline__ __attribute__((always_inline)) __attribute__((unused)) Class ".$methodname."(void) { static Class _klass; if(!_klass) { _klass = ".$lookupMethod."(\"".$class."\"); } return _klass; }";
}

sub declarations {
	my $self = shift;
	my $group = shift;
	my $return = "";
	return "" if scalar(keys %{$group->usedMetaClasses}) + scalar(keys %{$group->usedClasses}) + scalar(keys %{$group->declaredOnlyClasses}) == 0;
	foreach(keys %{$group->usedMetaClasses}) {
		$return .= $self->_methodForClassWithScope($_, "+");
	}
	foreach(keys %{$group->usedClasses}) {
		$return .= $self->_methodForClassWithScope($_, "-");
	}
	return $return;
}

sub initializers {
	return "";
}

1;
