package Logos::Generator;
use strict;
use Logos::Generator::Thunk;
use Scalar::Util qw(blessed);
use Module::Load::Conditional qw(can_load);
$Module::Load::Conditional::VERBOSE = 1;
our $GeneratorPackage = "";

my %cache;

sub for {
	my $object = shift;
	my $dequalified = undef;
	my $cachekey;
	if(defined $object) {
		$cachekey = $object;
		my $class = blessed($object);
		($dequalified = $class) =~ s/.*::// if defined $class
	}
	$cachekey = "-" if !$cachekey;
	$dequalified .= "Generator" if !defined $dequalified;
	return $cache{$cachekey} if $cache{$cachekey};

	my $qualified = $GeneratorPackage."::".$dequalified;
	my $fallback = "Logos::Generator::Base::".$dequalified;

	my $shouldFallBack = 0;
	can_load(modules=>{$qualified=>undef},verbose=>0) || ($shouldFallBack = 1);
	can_load(modules=>{$fallback=>undef},verbose=>1) if $shouldFallBack;

	my $thunk = Logos::Generator::Thunk->for(($shouldFallBack ? $fallback : $qualified), $object);
	$cache{$cachekey} = $thunk;
	return $thunk;
}

sub use {
	my $generatorName = shift;
	if($generatorName =~ /^(\w+)@(.+)$/) {
		$generatorName = $1;
		unshift @INC, $2;
	}
	$GeneratorPackage = "Logos::Generator::".$generatorName;
	::fileError(-1, "I can't find the $generatorName Generator!") if(!can_load(modules => {
				$GeneratorPackage."::Generator" => undef
			}));
}

1;
