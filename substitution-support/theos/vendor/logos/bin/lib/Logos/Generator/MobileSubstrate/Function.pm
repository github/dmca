package Logos::Generator::MobileSubstrate::Function;
use strict;
use parent qw(Logos::Generator::Base::Function);

sub initializers {
	my $self = shift;
	my $function = shift;

	my $return = "";
	$return .= " MSHookFunction((void *)";
	if (substr($function->name, 0, 1) eq "\"") {
		$return .= "MSFindSymbol(NULL, ".$function->name.")";
	} else {
		$return .= $function->name;
	}
	$return .= ", (void *)&".$self->newFunctionName($function);
	$return .= ", (void **)&".$self->originalFunctionName($function);
	$return .= ");";

	return $return;
}

1;
