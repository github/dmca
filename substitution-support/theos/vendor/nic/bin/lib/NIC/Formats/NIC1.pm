package NIC::Formats::NIC1;
use parent NIC::NICBase;
use strict;

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;

	my $fh = shift;
	my $self = NIC::NICBase->new(@_);
	bless($self, $class);

	$self->load($fh);
	return $self;
}

sub _processLine {
	my $self = shift;
	my $fh = shift;
	local $_ = shift;
	if(/^name \"(.*)\"$/) {
		$self->name($1);
	} elsif(/^dir (.+)$/) {
		$self->registerDirectory($1);
	} elsif(/^file (\d+) (.+)$/) {
		my $lines = $1;
		my $filename = $2;
		my $fref = $self->registerFile($filename);
		my $filedata = "";
		while($lines > 0) {
			$filedata .= <$fh>;
			$lines--;
		}
		$fref->data($filedata);
	} elsif(/^prompt (\w+) \"(.*?)\"( \"(.*?)\")?$/) {
		my $key = $1;
		my $prompt = $2;
		my $default = $4 || undef;
		$self->registerPrompt($key, $prompt, $default);
	} elsif(/^symlink \"(.+)\" \"(.+)\"$/) {
		my $name = $1;
		my $dest = $2;
		$self->registerSymlink($name, $dest);
	} elsif(/^constrain file \"(.+)\" to (.+)$/) {
		my $constraint = $2;
		my $filename = $1;
		$self->registerFileConstraint($filename, $constraint);
	}
}

sub load {
	my $self = shift;
	my $fh = shift;
	while(<$fh>) {
		$self->_processLine($fh, $_);
	}
	$self->resolveSymlinks;
}

1;
