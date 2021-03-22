package Logos::Generator::internal::Class;
use strict;
use parent qw(Logos::Generator::Base::Class);

sub superVariable {
	my $self = shift;
	my $class = shift;
	return Logos::sigil("superclass").$class->group->name."\$".$class->name;
}

sub superMetaVariable {
	my $self = shift;
	my $class = shift;
	return Logos::sigil("supermetaclass").$class->group->name."\$".$class->name;
}

sub declarations {
	my $self = shift;
	my $class = shift;
	my $return = "";
	if($class->hasinstancehooks) {
		$return .= "static Class ".$self->superVariable($class)."; ";
	}
	if($class->hasmetahooks) {
		$return .= "static Class ".$self->superMetaVariable($class)."; ";
	}
	$return .= $self->SUPER::declarations($class);
	return $return;
}

sub initializers {
	my $self = shift;
	my $class = shift;
	my $return = "";
	if($class->required || $class->overridden || $class->hasinstancehooks || $class->hasmetahooks) {
		$return .= "Class ".$self->variable($class)." = ".$self->_initExpression($class)."; ";
	}
	if($class->hasmetahooks) {
		$return .= "Class ".$self->metaVariable($class)." = ".$self->_metaInitExpression($class)."; ";
	}
	if ($class->hasinstancehooks) {
		$return .= $self->superVariable($class)." = class_getSuperclass(".$self->variable($class)."); ";
	}
	if ($class->hasmetahooks) {
		$return .= $self->superMetaVariable($class)." = class_getSuperclass(".$self->metaVariable($class)."); ";
	}
	$return .= $self->SUPER::initializers($class);
	return $return;
}

1;
