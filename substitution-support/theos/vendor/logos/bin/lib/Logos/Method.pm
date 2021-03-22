package Logos::Method;
use strict;
use Logos::Util qw(matchedParenthesisSet);

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = {};
	$self->{CLASS} = undef;
	$self->{SCOPE} = undef;
	$self->{RETURN} = undef;
	$self->{SELECTOR_PARTS} = [];
	$self->{ARGNAMES} = [];
	$self->{ARGTYPES} = [];
	$self->{NEW} = 0;
	$self->{TYPE} = "";
	bless($self, $class);
	return $self;
}

##################### #
# Setters and Getters #
# #####################
sub class {
	my $self = shift;
	if(@_) { $self->{CLASS} = shift; }
	return $self->{CLASS};
}

sub scope {
	my $self = shift;
	if(@_) { $self->{SCOPE} = shift; }
	return $self->{SCOPE};
}

sub return {
	my $self = shift;
	if(@_) { $self->{RETURN} = shift; }
	return $self->{RETURN};
}

sub groupIdentifier {
	my $self = shift;
	return $self->class->group->identifier;
}

sub selectorParts {
	my $self = shift;
	if(@_) { @{$self->{SELECTOR_PARTS}} = @_; }
	return $self->{SELECTOR_PARTS};
}

sub setNew {
	my $self = shift;
	if(@_) { $self->{NEW} = shift; }
	return $self->{NEW};
}

sub isNew {
	my $self = shift;
	return $self->{NEW};
}

sub type {
	my $self = shift;
	if(@_) { $self->{TYPE} = shift; }
	return $self->{TYPE};
}

sub argnames {
	my $self = shift;
	return $self->{ARGNAMES};
}

sub argtypes {
	my $self = shift;
	return $self->{ARGTYPES};
}
##### #
# END #
# #####

sub numArgs {
	my $self = shift;
	return scalar @{$self->{ARGTYPES}};
}

sub addArgument {
	my $self = shift;
	my ($type, $name) = @_;
	push(@{$self->{ARGTYPES}}, $type);
	push(@{$self->{ARGNAMES}}, $name);
}

sub selector {
	my $self = shift;
	if($self->numArgs == 0) {
		return $self->{SELECTOR_PARTS}[0];
	} else {
		return join(":", @{$self->{SELECTOR_PARTS}}).":";
	}
}

sub _new_selector {
	my $self = shift;
	if($self->numArgs == 0) {
		return $self->{SELECTOR_PARTS}[0];
	} else {
		return join("\$", @{$self->{SELECTOR_PARTS}})."\$";
	}
}

sub methodFamily {
	my $self = shift;
	my $selector = $self->selector;
	if ($self->scope eq "+") {
		if ($selector =~ /^alloc($|[A-Z,:])/) {
			return "alloc" if $self->return eq "id" || $self->return eq "instancetype";
		}
		if ($selector eq "new") {
			return "new" if $self->return eq "id" || $self->return eq "instancetype";
		}
	} else {
		if ($selector =~ /^init($|[A-Z,:])/) {
			return "init" if $self->return eq "id" || $self->return eq "instancetype";
		}
		if (($selector eq "copy") || ($selector eq "copyWithZone:")) {
			return "copy";
		}
		if (($selector eq "mutableCopy") || ($selector eq "mutableCopyWithZone:")) {
			return "mutableCopy";
		}
	}
	return "";
}

sub printArgForArgType {
	my $argtype = shift;
	my $argname = shift;

	my ($formatchar, $fallthrough) = formatCharForArgType($argtype);
	return undef if $formatchar eq "--";

	$argtype =~ s/^\s+//g;
	$argtype =~ s/\s+$//g;

	return "NSStringFromSelector($argname)" if $argtype =~ /^SEL$/;
	return "$argname" if $argtype =~ /^Class$/;
	return "$argname.location, $argname.length" if $argtype =~ /^NSRange$/;
	return "$argname.origin.x, $argname.origin.y, $argname.size.width, $argname.size.height" if $argtype =~ /^(CG|NS)Rect$/;
	return "$argname.x, $argname.y" if $argtype =~ /^(CG|NS)Point$/;
	return "$argname.width, $argname.height" if $argtype =~ /^(CG|NS)Size$/;
	return "(long)$argname" if $argtype =~ /^NS(Integer|SocketNativeHandle|StringEncoding|SortOptions|ComparisonResult|EnumerationOptions|(Hash|Map)TableOptions|SearchPath(Directory|DomainMask))$/i;
	return "(unsigned long)$argname" if $argtype =~ /^NSUInteger$/i;

	return ($fallthrough ? "(unsigned int)" : "").$argname;
}

sub formatCharForArgType {
	local $_ = shift;
	s/^\s+//g;
	s/\s+$//g;

	# Integral Types
	# Straight characters get %c. Signed/Unsigned characters get %hhu/%hhd.
	return "'%c'" if /^char$/;
	if(/^((signed|unsigned)\s+)?(unsigned|signed|int|long|long\s+long|bool|BOOL|_Bool|char|short)$/) {
		my $conversion = "d";
		$conversion = "u" if /\bunsigned\b/;

		my $length;
		$length = "" if /\bint\b/;
		$length = "l" if /\blong\b/;
		$length = "ll" if /\blong long\b/;
		$length = "h" if /\bshort\b/;
		$length = "hh" if /\bchar\b/;

		return "%".$length.$conversion;
	}
	return "%ld" if /^NS(Integer|SocketNativeHandle|StringEncoding|SortOptions|ComparisonResult|EnumerationOptions|(Hash|Map)TableOptions|SearchPath(Directory|DomainMask))$/i;
	return "%lu" if /^NSUInteger$/i;
	return "%d" if /^GS(FontTraitMask)$/i;

	# Pointer Types
	return "%s" if /^char\s*\*$/;
	return "%p" if /^void\s*\*$/; # void *
	return "%p" if /^id\s*\*$/; # id *
	return "%p" if /^((unsigned|signed)\s+)?(unsigned|signed|int|long|long\s+long|bool|BOOL|_Bool|char|short|float|double)\s*\*+$/;
	return "%p" if /^NS.*?(Pointer|Array)$/;
	return "%p" if /^NSZone\s*\*$/;
	return "%p" if /^struct.*\*$/; # struct pointer
	return "%p" if /\*\*+$/; # anything with more than one pointer indirection
	return "%p" if /\[.*\]$/; # any array

	# Objects
	return "%@" if /^id$/; # id is an objc_object.
	return "%@" if /^\w+\s*\*$/; # try to treat *any* other pointer as an objc_object.
	return "%@" if /^\w+Ref$/; # *Ref can be printed with %@.

	# Floating-Point Types
	return "%f" if /^(double|float|CGFloat|CGDouble|NSTimeInterval)$/;

	# Special Types (should also have an entry in printArgForArgType
	return "%@" if /^SEL$/;
	return "%@" if /^Class$/;

	# Even-more-special expanded types
	return "(%d:%d)" if /^NSRange$/;
	return "{{%g, %g}, {%g, %g}}" if /^(CG|NS)Rect$/;
	return "{%g, %g}" if /^(CG|NS)Point$/;
	return "{%g, %g}" if /^(CG|NS)Size$/;

	# Discarded Types
	return "--" if /^(CG\w*|CF\w*|void)$/;
	return "--" if /^NS(HashTable(Callbacks)?|Map(Table((Key|Value)Callbacks)?|Enumerator))$/;
	return "--" if /^struct/; # structs that aren't covered by 'struct ... *'

	# Fallthrough - Treat everything we don't understand as POD.
	return ("0x%x", 1) if wantarray; # The 1 is the fallthrough flag - used to signal to argName(...) that we should be casting.
	return "0x%x";
}

sub typeEncodingForArgType {
	local $_ = shift;
	s/^\s+//g;
	s/\s+$//g;

	return "c" if /^char$/;
	return "i" if /^int$/;
	return "s" if /^short$/;
	return "l" if /^long$/;
	return "q" if /^long long$/;

	return "C" if /^unsigned\s+char$/;
	return "I" if /^unsigned\s+int$/;
	return "S" if /^unsigned\s+short$/;
	return "L" if /^unsigned\s+long$/;
	return "Q" if /^unsigned\s+long long$/;

	return "f" if /^float$/;
	return "d" if /^double$/;
	return "B" if /^(bool|_Bool)$/;

	return "v" if /^void$/;

	return "*" if /^char\s*\*$/;

	return "@" if /^id$/;
	return "@" if /^instancetype$/;
	return "#" if /^Class$/;
	return ":" if /^SEL$/;

	if(/^([^*\s]+)\s*\*$/) {
		my $subEncoding = typeEncodingForArgType($1);
		return undef if(!defined $subEncoding);
		return "^".$subEncoding;
	}

	return undef;
}

sub declarationForTypeWithName {
	my $argtype = shift;
	my $argname = shift;
	if($argtype !~ /\(\s*[*^]/) {
		return $argtype." ".$argname;
	}
	my $substring = $argtype;
	my ($opening, $closing) = matchedParenthesisSet($substring, 0);
	my $offset = 0;
	while(1) {
		# We want to put the parameter name right before the closing ) in the deepest nested set if we found a (^ or (*.
		$substring = substr($substring, $opening, $closing - $opening - 1);
		$offset += $opening;
		my ($newopening, $newclosing) = matchedParenthesisSet($substring, 0);
		last if !defined $newopening;
		$opening = $newopening;
		$closing = $newclosing;
	}
	my $out = $argtype;
	substr($out, $offset-$opening+$closing-1, 0, $argname);
	return $out;
}

1;
