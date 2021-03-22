package Logos::Generator::Base::Property;
use strict;
use Logos::Util;

sub getterName {
	my $self = shift;
	my $property = shift;
	return Logos::sigil("method").$property->group."\$".$property->class."\$".$property->getter;
}

sub setterName {
	my $self = shift;
	my $property = shift;
	return Logos::sigil("method").$property->group."\$".$property->class."\$".$property->setter;
}

sub definition {
	my $self = shift;
	my $property = shift;

	my $propertyType = $property->type;
	my $propertyClass = $property->class;
	my $propertyGetter = $property->getter;
	my $propertyGetterName = $self->getterName($property);
	my $propertySetter = $property->setter;
	my $propertySetterName = $self->setterName($property);
	my $propertyAssociationPolicy = $property->associationPolicy;
	my $wrapValue = !($propertyAssociationPolicy =~ /RETAIN|COPY/);

	# Build getter
	my $getter_func = "__attribute__((used)) "; # If the property is never accessed, clang's optimizer will remove the getter/setter if this attribute isn't specified
	$getter_func .= "static $propertyType $propertyGetterName($propertyClass * __unused self, SEL __unused _cmd) ";
	if($wrapValue) {
		$getter_func .= "{ NSValue * value = objc_getAssociatedObject(self, (void *)$propertyGetterName); $propertyType rawValue; [value getValue:&rawValue]; return rawValue; }";
	} else {
		$getter_func .= "{ return ($propertyType)objc_getAssociatedObject(self, (void *)$propertyGetterName); }";
	}

	# Build setter
	my $setter_func = "__attribute__((used)) "; # If the property is never accessed, clang's optimizer will remove the getter/setter if this attribute isn't specified
	$setter_func .= "static void $propertySetterName($propertyClass * __unused self, SEL __unused _cmd, $propertyType rawValue) ";
	if($wrapValue) {
		$setter_func .= "{ NSValue * value = [NSValue valueWithBytes:&rawValue objCType:\@encode($propertyType)]; objc_setAssociatedObject(self, (void *)$propertyGetterName, value, OBJC_ASSOCIATION_RETAIN_NONATOMIC); }";
	} else {
		$setter_func .= "{ objc_setAssociatedObject(self, (void *)$propertyGetterName, rawValue, $propertyAssociationPolicy); }";
	}

	return "$getter_func; $setter_func";
}

sub initializers {
	my $self = shift;
	my $property = shift;

	my $className = Logos::sigil("class").$property->group."\$".$property->class;
	my $propertyType = $property->type;
	my $propertyGetter = $property->getter;
	my $propertyGetterName = $self->getterName($property);
	my $propertySetter = $property->setter;
	my $propertySetterName = $self->setterName($property);

	my $build = "{ char _typeEncoding[1024];";

	# Getter
	$build .= " sprintf(_typeEncoding, \"%s\@:\", \@encode($propertyType));";
	$build .= " class_addMethod($className, \@selector($propertyGetter), (IMP)&$propertyGetterName, _typeEncoding);";

	# Setter
	$build .= " sprintf(_typeEncoding, \"v\@:%s\", \@encode($propertyType));";
	$build .= " class_addMethod($className, \@selector($propertySetter:), (IMP)&$propertySetterName, _typeEncoding);";

	$build .= " } ";

	return $build;
}

1;
