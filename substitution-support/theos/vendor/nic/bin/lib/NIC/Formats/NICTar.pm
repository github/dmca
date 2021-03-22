package NIC::Formats::NICTar;
use parent NIC::NICBase;
use strict;
use NIC::Formats::NICTar::File;
use NIC::Formats::NICTar::Directory;
use NIC::Formats::NICTar::Symlink;
use Archive::Tar;
use File::Temp;
use File::Path;
use File::Spec;
use NIC::Bridge::Context;
$Archive::Tar::WARN = 0;

sub new {
	my $proto = shift;
	my $fh_or_tar = shift;
	my $class = ref($proto) || $proto;

	my $tar = ref($fh_or_tar) eq "Archive::Tar" ? $fh_or_tar : Archive::Tar->new($fh_or_tar);
	return undef if(!$tar);

	my $control = _fileFromTar(undef, $tar, "NIC/control");
	return undef if(!$control);

	my $self = NIC::NICBase->new(@_);
	$self->{_TAR} = $tar;
	bless($self, $class);

	$self->_processData($control->get_content);
	$self->load();

	return $self;
}

sub _fileClass { "NIC::Formats::NICTar::File"; }
sub _directoryClass { "NIC::Formats::NICTar::Directory"; }
sub _symlinkClass { "NIC::Formats::NICTar::Symlink"; }

sub _fileFromTar {
	my $self = shift;
	my $tar = $self ? $self->{_TAR} : shift;
	my $filename = shift;
	my @_tarfiles = $tar->get_files("./$filename", $filename);
	return (scalar @_tarfiles > 0) ? $_tarfiles[0] : undef;
}

sub _processData {
	my $self = shift;
	my $data = shift;
	for(split /\n\r?/, $data) {
		$self->_processLine($_);
	}
}

sub _processLine {
	my $self = shift;
	local $_ = shift;
	if(/^name\s+\"(.*)\"$/ || /^name\s+(.*)$/) {
		$self->name($1);
	} elsif(/^prompt (\w+) \"(.*?)\"( \"(.*?)\")?$/) {
		my $key = $1;
		my $prompt = $2;
		my $default = $4 || undef;
		$self->registerPrompt($key, $prompt, $default);
	} elsif(/^constrain (file )?\"(.+)\" to (.+)$/) {
		my $constraint = $3;
		my $filename = $2;
		$self->registerFileConstraint($filename, $constraint);
	} elsif(/^ignore (\w+)$/) {
		$self->ignoreVariable($1);
	}
}

sub load {
	my $self = shift;
	for($self->{_TAR}->get_files()) {
		next if !$_->full_path || $_->full_path =~ /^(\.\/)?NIC(\/|$)/;
		my $n = $_->full_path;
		$n =~ s/^\.\///;
		next if length $n == 0;
		if($_->is_dir) {
			my $ref = $self->registerDirectory($n);
			$ref->tarfile($_);
		} elsif($_->is_symlink) {
			my $target = $_->linkname;
			$target =~ s/^\.\///;

			my $ref = $self->registerSymlink($n, $target);
			$ref->tarfile($_);
		} elsif($_->is_file) {
			my $ref = $self->registerFile($n);
			$ref->tarfile($_);
		}
	}
	$self->resolveSymlinks;
}

sub _execPackageScript {
	my $self = shift;
	my $script = shift;
	my $tarfile = $self->_fileFromTar("NIC/$script");
	return if !$tarfile || $tarfile->mode & 0500 != 0500;
	my $filename = File::Spec->catfile($self->{_TEMPDIR}, $script);
	my $nicfile = NIC::NICType->new($self, $filename);
	$self->_fileClass->take($nicfile);
	$nicfile->tarfile($tarfile);
	$nicfile->create();
	my $ret = system $filename $script;
	return ($ret >> 8) == 0
}

sub prebuild {
	my $self = shift;
	return $self->_execPackageScript("prebuild");
}

sub postbuild {
	my $self = shift;
	return $self->_execPackageScript("postbuild");
}

sub exec {
	my $self = shift;
	my $_controlpl = $self->_fileFromTar("NIC/control.pl");
	if($_controlpl) {
		return NIC::Bridge::Context->_execute($self, $_controlpl->get_content);
	}
	return 1;
}

sub build {
	my $self = shift;

	for(keys %{$self->{VARIABLES}}) {
		$ENV{"NIC_".$_} = $self->variable($_);
	}

	$self->{_TEMPDIR} = File::Temp::tempdir();

	$self->SUPER::build(@_);

	File::Path::rmtree($self->{_TEMPDIR});
	$self->{_TEMPDIR} = undef;
}

1;
