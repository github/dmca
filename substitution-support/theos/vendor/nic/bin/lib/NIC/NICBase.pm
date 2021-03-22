package NIC::NICBase;
use strict;
use warnings;

use NIC::NICBase::File;
use NIC::NICBase::Directory;
use NIC::NICBase::Symlink;

use List::Util qw(first);

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = {};
	$self->{NAME} = shift;
	$self->{CONTENTS} = [];
	$self->{VARIABLES} = {};
	$self->{CONSTRAINTS} = {};
	$self->{PROMPTS} = [];
	$self->{IGNORED_VARS} = {};
	bless($self, $class);

	return $self;
}

sub _fileClass { "NIC::NICBase::File"; }
sub _directoryClass { "NIC::NICBase::Directory"; }
sub _symlinkClass { "NIC::NICBase::Symlink"; }

sub _getContentWithoutCreate {
	my $self = shift;
	my $name = shift;
	return first { $_->name eq $name } @{$self->{CONTENTS}};
}

sub _getContent {
	my $self = shift;
	my $ref = $self->_getContentWithoutCreate(@_);
	return $ref if $ref;
	$ref = NIC::NICType->new($self, @_);
	push(@{$self->{CONTENTS}}, $ref);
	return $ref;
}

sub _generate {
	my $self = shift;
	my $class = shift;
	my $name = shift;
	my $ref = $self->_getContent($name, @_);
	if($ref->type == NIC::NICType::TYPE_UNKNOWN) {
		$class->take($ref, @_);
	}
	return $ref;
}

sub registerDirectory {
	my $self = shift;
	my $dir = $self->_generate($self->_directoryClass, @_);
	return $dir;
}

sub registerFile {
	my $self = shift;
	my $file = $self->_generate($self->_fileClass, @_);
	return $file;
}

sub registerSymlink {
	my $self = shift;
	my $symlink = $self->_generate($self->_symlinkClass, @_);
	return $symlink;
}

sub registerPrompt {
	my($self, $key, $prompt, $default) = @_;
	push(@{$self->{PROMPTS}}, {
			name => $key,
			prompt => $prompt,
			default => $default
		});
}

sub registerFileConstraint {
	my $self = shift;
	my $filename = shift;
	my $constraint = shift;
	$self->_getContent($filename)->addConstraint($constraint);
}

sub resolveSymlinks {
	my $self = shift;
	for(@{$self->{CONTENTS}}) {
		next unless $_->type == NIC::NICType::TYPE_SYMLINK;
		next if $_->target_type != NIC::NICType::TYPE_UNKNOWN;
		my $ref = $self->_getContentWithoutCreate($_->target);
		$_->target($ref) if $ref;
	}
}

sub variable: lvalue {
	my $self = shift;
	my $key = shift;
	$self->{VARIABLES}->{$key};
}

sub name {
	my $self = shift;
	if(@_) { $self->{NAME} = shift; }
	return $self->{NAME} // "(unnamed template)";
}

sub prompts {
	my $self = shift;
	return @{$self->{PROMPTS}};
}

sub addConstraint {
	my $self = shift;
	my $constraint = shift;
	$self->{CONSTRAINTS}->{$constraint} = 1;
}

sub removeConstraint {
	my $self = shift;
	my $constraint = shift;
	delete $self->{CONSTRAINTS}->{$constraint};
}

sub _constraintMatch {
	my $self = shift;
	my $constraint = shift;
	my $negated = 0;
	if(substr($constraint, 0, 1) eq "!") {
		$negated = 1;
		substr($constraint, 0, 1, "");
	}
	return 0 if(!$negated && (!defined $self->{CONSTRAINTS}->{$constraint} || $self->{CONSTRAINTS}->{$constraint} != 1));
	return 0 if($negated && (defined $self->{CONSTRAINTS}->{$constraint} || $self->{CONSTRAINTS}->{$constraint} != 0));
	return 1;
}

sub _meetsConstraints {
	my $self = shift;
	my $content = shift;
	foreach ($content->constraints) {
		return 0 if !$self->_constraintMatch($_);
	}
	return 1;
}

sub substituteVariables {
	my $self = shift;
	my $line = shift;
	foreach my $key (keys %{$self->{VARIABLES}}) {
		my $value = $self->{VARIABLES}->{$key};
		$line =~ s/\@\@$key\@\@/$value/g;
	}
	return $line;
}

sub ignoreVariable {
	my $self = shift;
	my $var = shift;
	$self->{IGNORED_VARS}->{$var}++;
}

sub variableIgnored {
	my $self = shift;
	my $var = shift;
	return defined $self->{IGNORED_VARS}->{$var};
}

sub prebuild {

}

sub postbuild {

}

sub exec {
	return 1;
}

sub build {
	my $self = shift;
	my $dir = shift;
	mkdir($dir) or die "Failed to create the directory '$dir': $!\n";
	chdir($dir) or die $!;
	$self->prebuild();
	foreach my $content (sort { $a->type <=> $b->type } (@{$self->{CONTENTS}})) {
		next if $content->type == NIC::NICType::TYPE_UNKNOWN;
		next if !$self->_meetsConstraints($content);
		$content->create();
	}
	$self->postbuild();
}

sub dumpPreamble {
	my $self = shift;
	my $preamblefn = shift;
	open(my $pfh, ">", $preamblefn);
	print $pfh "name \"".$self->{NAME}."\"",$/;
	foreach my $prompt (@{$self->{PROMPTS}}) {
		print $pfh "prompt ".$prompt->{name}." \"".$prompt->{prompt}."\"";
		print $pfh " \"".$prompt->{default}."\"" if defined $prompt->{default};
		print $pfh $/;
	}
	foreach my $filename (keys %{$self->{FILES}}) {
		my $file = $self->{FILES}->{$filename};
		if(!defined $file->{constraints}) {
			next;
		}
		foreach (@{$file->{constraints}}) {
			print $pfh "constrain file \"".$filename."\" to ".$_,$/
		}
	}
	close($pfh);
}

1;
