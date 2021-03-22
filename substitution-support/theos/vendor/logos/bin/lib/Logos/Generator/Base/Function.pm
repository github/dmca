package Logos::Generator::Base::Function;
use strict;
use Logos::Generator;
use Logos::Util;

sub getLiteralOrLookupFunctionName {
	my $self = shift;
	my $name = shift;
	return "lookup\$".substr($name, 1, -1) if (substr($name, 0, 1) eq "\"" && substr($name, -1, 1) eq "\"");
	return $name;
}

sub originalFunctionName {
	my $self = shift;
	my $function = shift;
	return Logos::sigil("orig").$function->group->name."\$".$self->getLiteralOrLookupFunctionName($function->name);
}

sub newFunctionName {
	my $self = shift;
	my $function = shift;
	return Logos::sigil("function").$function->group->name."\$".$self->getLiteralOrLookupFunctionName($function->name);
}

sub originalFunctionCall {
	my $self = shift;
	my $function = shift;
	my $args = shift;

	my $return = $self->originalFunctionName($function)."(";
	if (!$args) {
		$args = [];
		for(@{$function->args}) {
			push(@$args, $self->getArgumentName($_));
		}
		$return .= join(", ", @$args);
	} else {
		$return .= $args;
	}
	$return .= ")";

	return $return;
}

sub declaration {
	my $self = shift;
	my $function = shift;
	my $return = "";
	$return .= "__unused static ".$function->retval." (*".$self->originalFunctionName($function).")(".join(", ", @{$function->args})."); ";
	$return .= "__unused static ".$function->retval." ".$self->newFunctionName($function)."(".join(", ", @{$function->args}).")";
	return $return;
}

sub initializers {
	::fileError(-1, "Base::Function does not implement initializers");
}

##########
# extras #
##########

sub getArgumentName {
	my $self = shift;
	my $arg = shift;

	# Split the argument string by spaces
	my $argArray = $self->notSoSmartSplit($arg, " ");
	# Try to get the name of the last element of the array if it is a function pointer
	my $return = $self->escapeFunctionPointer(@$argArray[-1]);
	# Separate the name from trailing vector size
	$return =~ s/\[/ \[/g;
	# Trimm spaces or asterisks from the start
	while (substr($return, 0, 1) eq " " || substr($return, 0, 1) eq "*") {
		$return = substr($return, 1, length($return));
	}
	# Split the resulting string by spaces
	$argArray = $self->notSoSmartSplit($return, " ");
	# Get the first element
	my $return = @$argArray[0];

	return $return;
}

sub escapeFunctionPointer {
	my $self = shift;
	my $arg = shift;
	if($arg !~ /\(\s*[*^]/) {
		return $arg;
	}
	my $substring = $arg;
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
	return substr($arg, $offset, $closing-$opening-1);
}

#for a lack of Logos::Util::matchedDelimiterSet()
sub notSoSmartSplit {
	my $self = shift;
	my $argumentString = shift;
	my $delimiter = shift;

	# Default to commas
	if (!$delimiter) {
		$delimiter = ",";
	}
	$argumentString .= $delimiter; #uber hax

# curved  brackets or parens ()
	my $parensLevel = 0;
# squared brackets or crotchets []
	my $crotchetsLevel = 0;
# curly   brackets or braces {}
	my $bracesLevel = 0;
# angled  brackets or chevrons <>
	my $chevronsLevel = 0;

	my $token = "";
	my $pc = "";

	my $args = [];

	foreach my $c (split //, $argumentString) {
		if ($c eq $delimiter) {
			# If at root level, end token, push to array and start again
			if ($parensLevel == 0 && $crotchetsLevel == 0 && $bracesLevel == 0 && $chevronsLevel == 0) {
				push(@$args, $token);
				$pc = $c;
				$token = "";
				next;
			}
		}

		if ($c eq "(") {
			$parensLevel++;
		}
		if ($c eq ")") {
			$parensLevel--;
		}

		if ($c eq "[") {
			$crotchetsLevel++;
		}
		if ($c eq "]") {
			$crotchetsLevel--;
		}

		if ($c eq "{") {
			$bracesLevel++;
		}
		if ($c eq "}") {
			$bracesLevel--;
		}

		if ($c eq "<") {
			$chevronsLevel++;
		}
		if ($c eq ">") {
			$chevronsLevel--;
		}

		#skip redundant empty spaces
		if (($pc eq $delimiter && $c eq " ") || ($pc eq " " && $c eq " ")) {
			next;
		}

		# Concatenate char to token
		$token .= $c;
		#save previous char
		$pc = $c;
	} # foreach

	return $args;
}

1;
