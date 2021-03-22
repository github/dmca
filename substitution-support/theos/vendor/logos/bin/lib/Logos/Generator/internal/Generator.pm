package Logos::Generator::internal::Generator;
use strict;
use parent qw(Logos::Generator::Base::Generator);

sub findPreamble {
	my $self = shift;
	my $aref = shift;
	my @matches = grep(/\s*#\s*(import|include)\s*[<"]objc\/message\.h[">]/, @$aref);
	return $self->SUPER::findPreamble($aref) && @matches > 0;
}

sub preamble {
	my $self = shift;
	return join("\n", (
		$self->SUPER::preamble(),
		"#include <objc/message.h>"
	));
}

sub staticDeclarations {
	my $self = shift;
	return join("\n", ($self->SUPER::staticDeclarations(),
		"__attribute__((unused)) static void ".Logos::sigil("register_hook")."(Class _class, SEL _cmd, IMP _new, IMP *_old) {",
		"unsigned int _count, _i;",
		"Class _searchedClass = _class;",
		"Method *_methods;",
		"while (_searchedClass) {",
			"_methods = class_copyMethodList(_searchedClass, &_count);",
			"for (_i = 0; _i < _count; _i++) {",
				"if (method_getName(_methods[_i]) == _cmd) {",
					"if (_class == _searchedClass) {",
						"*_old = method_getImplementation(_methods[_i]);",
						"*_old = method_setImplementation(_methods[_i], _new);",
					"} else {",
						"class_addMethod(_class, _cmd, _new, method_getTypeEncoding(_methods[_i]));",
					"}",
					"free(_methods);",
					"return;",
				"}",
			"}",
			"free(_methods);",
			"_searchedClass = class_getSuperclass(_searchedClass);",
		"}",
	"}"));
}

1;
