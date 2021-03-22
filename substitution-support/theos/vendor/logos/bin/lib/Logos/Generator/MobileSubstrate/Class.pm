package Logos::Generator::MobileSubstrate::Class;
use strict;
use parent qw(Logos::Generator::Base::Class);

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
	$return .= $self->SUPER::initializers($class);
	return $return;
}

1;
