package Logos::Generator::Base::Method;
use strict;
use Logos::Util;

sub originalFunctionName {
	my $self = shift;
	my $method = shift;
	return Logos::sigil(($method->scope eq "+" ? "meta_" : "")."orig").$method->groupIdentifier."\$".$method->class->name."\$".$method->_new_selector;
}

sub newFunctionName {
	my $self = shift;
	my $method = shift;
	return Logos::sigil(($method->scope eq "+" ? "meta_" : "")."method").$method->groupIdentifier."\$".$method->class->name."\$".$method->_new_selector;
}

sub definition {
	::fileError(-1, "generator does not implement Method::definition");
}

sub originalCall {
	::fileError(-1, "generator does not implement Method::originalCall");
}

sub selectorRef {
	my $self = shift;
	my $selector = shift;
	if ($selector eq "dealloc") {
		return "sel_registerName(\"".$selector."\")";
	}
	return "\@selector(".$selector.")";
}

sub selfTypeForMethod {
	my $self = shift;
	my $method = shift;
	if($method->scope eq "+") {
		return "_LOGOS_SELF_TYPE_NORMAL Class _LOGOS_SELF_CONST";
	}
	if($method->methodFamily eq "init") {
		return "_LOGOS_SELF_TYPE_INIT ".$method->class->type;
	}
	return "_LOGOS_SELF_TYPE_NORMAL ".$method->class->type." _LOGOS_SELF_CONST";
}

sub returnTypeForMethod {
	my $self = shift;
	my $method = shift;
	if($method->methodFamily ne "") {
		return $method->class->type;
	}
	my $result = $method->return;
	if ($result eq "instancetype") {
		return $method->class->type;
	}
	return $result;
}

sub functionAttributesForMethod {
	my $self = shift;
	my $method = shift;
	if($method->methodFamily ne "") {
		return " _LOGOS_RETURN_RETAINED";
	}
	return "";
}

sub buildLogCall {
	my $self = shift;
	my $method = shift;
	my $args = shift;
	# Log preamble
	my $build = "HBLogDebug(\@\"".$method->scope."[<".$method->class->name.": %p>";
	my $argnamelist = "";
	if($method->numArgs > 0) {
		# For each argument, add its keyword and a format char to the log string.
		map $build .= " ".$method->selectorParts->[$_].":".Logos::Method::formatCharForArgType($method->argtypes->[$_]), (0..$method->numArgs - 1);
		# This builds a list of args by making sure the format char isn't -- (or, what we're using for non-operational types)
		# Map (in list context) "format char == -- ? nothing : arg name" over the indices of the arg list.
		my @newarglist = map(Logos::Method::printArgForArgType($method->argtypes->[$_], $method->argnames->[$_]), (0..$method->numArgs - 1));
		my @existingargs = grep(defined($_), @newarglist);
		if(scalar(@existingargs) > 0) {
			$argnamelist = ", ".join(", ", grep(defined($_), @existingargs));
		}
	} else {
		# Space and then the only keyword in the selector.
		$build .= " ".$method->selector;
	}

	my @extraFormatSpecifiers;
	my @extraArguments;
	for(Logos::Util::smartSplit(qr/\s*,\s*/, $args)) {
		my ($popen, $pclose) = matchedParenthesisSet($_);
		my $type = "id";
		if(defined $popen) {
			$type = substr($_, $popen, $pclose-$popen-1);
		}
		push(@extraFormatSpecifiers, Logos::Method::formatCharForArgType($type));
		my $n = Logos::Method::printArgForArgType($type, "($_)");
		push(@extraArguments, $n) if $n;
	}

	# Log postamble
	$build .= "]";
	$build .= ": ".join(", ", @extraFormatSpecifiers) if @extraFormatSpecifiers > 0;
	$build .= "\", self".$argnamelist;
	$build .= ", ".join(", ", @extraArguments) if @extraArguments > 0;
	$build .= ")";
}

sub declarations {
	::fileError(-1, "generator does not implement Method::declarations");
}

sub initializers {
	::fileError(-1, "generator does not implement Method::initializers");
}

1;
