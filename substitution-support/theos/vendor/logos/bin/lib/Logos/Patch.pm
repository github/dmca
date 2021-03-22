package Logos::Patch;
use strict;

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = {};
	$self->{LINE} = -1;
	$self->{RANGE} = [];
	$self->{SOURCE} = undef;
	$self->{SQUASH} = 0;
	bless($self, $class);
	return $self;
}

##################### #
# Setters and Getters #
# #####################
sub line {
	my $self = shift;
	if(@_) { $self->{LINE} = shift; }
	return $self->{LINE};
}

sub range {
	my $self = shift;
	if(@_) { @{$self->{RANGE}} = @_; }
	return $self->{RANGE};
}

sub start {
	my $self = shift;
	if(@_) { $self->{RANGE}[0] = shift; }
	return $self->{RANGE}[0];
}

sub end {
	my $self = shift;
	if(@_) { $self->{RANGE}[1] = shift; }
	return $self->{RANGE}[1];
}

sub source {
	my $self = shift;
	if(@_) { $self->{SOURCE} = shift; }
	return $self->{SOURCE};
}

sub squash {
	my $self = shift;
	if(@_) { $self->{SQUASH} = shift; }
	return $self->{SQUASH};
}

##### #
# END #
# #####

sub evalSource {
	my $self = shift;
	my $source = shift;
	my $sourceref = ref($source);
	my @lines;
	if($sourceref) {
		if($sourceref eq "ARRAY") {
			for(@$source) {
				splice(@lines, scalar @lines, 0, $self->evalSource($_));
			}
		} else {
			push(@lines, $source->eval());
		}
	} else {
		push(@lines, $source);
	}
	return @lines;
}

sub apply {
	my $self = shift;
	my $lref = shift;
	my $line = $self->{LINE};
	my ($start, $end) = @{$self->{RANGE}};
	my $source = $self->{SOURCE};
	my @lines = $self->evalSource($source);
	if(!defined $start) {
		push(@lines, ::generateLineDirectiveForPhysicalLine($line));
		if($self->{SQUASH}) {
			push(@$lref, join('', @lines));
		} else {
			splice(@$lref, $line, 0, @lines);
		}
	} else {
		substr($lref->[$line], $start, $end-$start) = join('', @lines);
	}
	return;
}

1;
