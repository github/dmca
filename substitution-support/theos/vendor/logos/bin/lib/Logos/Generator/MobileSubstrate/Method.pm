package Logos::Generator::MobileSubstrate::Method;
use strict;
use parent qw(Logos::Generator::Base::Method);

sub _originalMethodPointerDeclaration {
	my $self = shift;
	my $method = shift;
	if(!$method->isNew) {
		my $build = "static ";
		my $classargtype = $self->selfTypeForMethod($method);
		my $name = "(*".$self->originalFunctionName($method).")(".$classargtype.", SEL";
		my $argtypelist = join(", ", @{$method->argtypes});
		$name .= ", ".$argtypelist if $argtypelist;

		$name .= ")";
		$build .= Logos::Method::declarationForTypeWithName($self->returnTypeForMethod($method), $name);
		$build .= $self->functionAttributesForMethod($method);
		return $build;
	}
	return undef;
}

sub _methodPrototype {
	my $self = shift;
	my $method = shift;
	my $includeArgNames = 0 || shift;
	my $build = "static ";
	my $classargtype = $self->selfTypeForMethod($method);
	my $arglist = "";
	if($includeArgNames == 1) {
		map $arglist .= ", ".Logos::Method::declarationForTypeWithName($method->argtypes->[$_], $method->argnames->[$_]), (0..$method->numArgs - 1);
	} else {
		my $typelist = join(", ", @{$method->argtypes});
		$arglist = ", ".$typelist if $typelist;
	}

	my $name = $self->newFunctionName($method)."(".$classargtype.($includeArgNames?" __unused self":"").", SEL".($includeArgNames?" __unused _cmd":"").$arglist.")";
	$build .= Logos::Method::declarationForTypeWithName($self->returnTypeForMethod($method), $name);
	$build .= $self->functionAttributesForMethod($method);
	return $build;
}

sub definition {
	my $self = shift;
	my $method = shift;
	my $build = "";
	$build .= $self->_methodPrototype($method, 1);
	return $build;
}

sub originalCall {
	my $self = shift;
	my $method = shift;
	my $customargs = shift;
	return "" if $method->isNew;

	my $build = $self->originalFunctionName($method)."(self, _cmd";
	if(defined $customargs && $customargs ne "") {
		$build .= ", ".$customargs;
	} elsif($method->numArgs > 0) {
		$build .= ", ".join(", ",@{$method->argnames});
	}
	$build .= ")";
	return $build;
}

sub declarations {
	my $self = shift;
	my $method = shift;
	my $build = "";
	my $orig = $self->_originalMethodPointerDeclaration($method);
	$build .= $orig."; " if $orig;
	$build .= $self->_methodPrototype($method)."; ";
	return $build;
}

sub initializers {
	my $self = shift;
	my $method = shift;
	my $cgen = Logos::Generator::for($method->class);
	my $classvar = ($method->scope eq "+" ? $cgen->metaVariable : $cgen->variable);
	if(!$method->isNew) {
		my $r = "MSHookMessageEx(".$classvar.", ".$self->selectorRef($method->selector).", (IMP)&".$self->newFunctionName($method).", (IMP*)&".$self->originalFunctionName($method).");";
	} else {
		my $r = "";
		$r .= "{ ";
		if(!$method->type) {
			$r .= "char _typeEncoding[1024]; unsigned int i = 0; ";
			for ($method->return, "id", "SEL", @{$method->argtypes}) {
				my $typeEncoding = Logos::Method::typeEncodingForArgType($_);
				if(defined $typeEncoding) {
					my @typeEncodingBits = split(//, $typeEncoding);
					my $i = 0;
					for my $char (@typeEncodingBits) {
						$r .= "_typeEncoding[i".($i > 0 ? " + $i" : "")."] = '$char'; ";
						$i++;
					}
					$r .= "i += ".(scalar @typeEncodingBits)."; ";
				} else {
					$r .= "memcpy(_typeEncoding + i, \@encode($_), strlen(\@encode($_))); i += strlen(\@encode($_)); ";
				}
			}
			$r .= "_typeEncoding[i] = '\\0'; ";
		} else {
			$r .= "const char *_typeEncoding = \"".$method->type."\"; ";
		}
		$r .= "class_addMethod(".$classvar.", ".$self->selectorRef($method->selector).", (IMP)&".$self->newFunctionName($method).", _typeEncoding); ";
		$r .= "}";
		return $r;
	}
}

1;
