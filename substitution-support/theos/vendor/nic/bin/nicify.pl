#!/usr/bin/perl

use strict;
use warnings;
use File::Find;
use File::Spec;
use Archive::Tar;
use Cwd qw(abs_path getcwd);

use FindBin;
use lib "$FindBin::RealBin/lib";
use NIC::Formats::NICTar;

package NIC::Archive::Tar::File;
use parent "Archive::Tar::File";
sub new {
	my $class = shift;
	my $self = Archive::Tar::File->new(@_);
	bless($self, $class);
	return $self;
}

sub _prefix_and_file {
	my $self = shift;
	my $path = shift;
	my ($prefix, $file) = $self->SUPER::_prefix_and_file($path);
	$prefix =~ s/^/\.\// if $prefix ne "" && $prefix ne "." && $prefix !~ /^\.\//;
	return ($prefix, $file);
}
1;
package main;

if(@ARGV == 0) {
	exitWithError("Syntax: $FindBin::Script <directory>");
}

my $cwd = abs_path(getcwd());
my $tar = Archive::Tar->new();
my $controlfile = undef;
our @tarfiles = (
	NIC::Archive::Tar::File->new(data=>"./", "", {type=>Archive::Tar::Constant::DIR, uid=>0, gid=>0, mode=>0755}),
	NIC::Archive::Tar::File->new(data=>"./NIC/", "", {type=>Archive::Tar::Constant::DIR, uid=>0, gid=>0, mode=>0777})
);

chdir $ARGV[0];

my $control_in = undef;

if(-f "pre.NIC") {
	warning("Using legacy pre.NIC as ./NIC/control.");
	$control_in = "./pre.NIC";
} elsif(-f "NIC/control") {
	$control_in = "./NIC/control";
}

if(!$control_in) {
	exitWithError("No control file found at NIC/control.");
	exit 1;
}

$controlfile = NIC::Archive::Tar::File->new(file=>$control_in);
$controlfile->prefix("./NIC");
$controlfile->name("control");
push(@tarfiles, $controlfile);

find({wanted => \&wanted, preprocess => \&preprocess, follow => 0, no_chdir => 1}, ".");

$tar->add_files(@tarfiles);

chdir($cwd);
my $newnic = NIC::Formats::NICTar->new($tar);
if(!defined $newnic->name) {
	exitWithError("Template has no name. Please insert a `name \"<name>\"` directive into $control_in.");
}

{ my $contents = scalar @{$newnic->{CONTENTS}}; info("$contents entr".($contents==1?"y.":"ies.")); }
{ my $prompts = scalar @{$newnic->{PROMPTS}}; info("$prompts prompt".($prompts==1?".":"s.")); }
{
	my %constrainthash = ();
	for(@{$newnic->{CONTENTS}}) {
		for my $c ($_->constraints) {
			$constrainthash{$c}++;
		}
	}
	my $constraints = scalar keys %constrainthash;
	info("$constraints constraint".($constraints==1?".":"s."));
}

my $fixedfn = join("_", File::Spec->splitdir($newnic->name));
my $filename = $fixedfn.".nic.tar";
$tar->write($filename) and info("Archived template \"".$newnic->name."\" to $filename.");

sub preprocess {
	my @list = @_;
	if($File::Find::dir eq "./NIC") {
		@list = grep !/^control$/, @list;
	}
	@list = grep !/^pre.NIC$/ && !/^\.svn$/ && !/^\.git$/ && !/^_MTN$/ && !/\.nic\.tar$/ && !/^\.DS_Store$/ && !/^\._/, @list;
	return @list;
}

sub wanted {
	local $_ = $File::Find::name;
	my $mode = (stat)[2];

	my $tarfile = undef;
	if(-d) {
		s/$/\// if !/\/$/;
		return if /^\.\/$/;
		return if /^\.\/NIC\/?$/;
		$tarfile = NIC::Archive::Tar::File->new(data=>$_, "", {mode=>$mode, uid=>0, gid=>0, type=>Archive::Tar::Constant::DIR});
	} elsif(-f && ! -l) {
		$tarfile = NIC::Archive::Tar::File->new(file=>$_);
		$tarfile->mode($mode);
		$tarfile->uid(0);
		$tarfile->gid(0);
	} elsif(-l) {
		$tarfile = NIC::Archive::Tar::File->new(data=>$_, "", {linkname=>readlink($_), uid=>0, gid=>0, type=>Archive::Tar::Constant::SYMLINK});
	}
	push(@tarfiles, $tarfile) if $tarfile;
}

sub slurp {
	my $fn = shift;
	open(my($fh), "<", $fn);
	local $/ = undef;
	my $d = <$fh>;
	return $d;
}

sub info {
	my $text = shift;
	print STDERR "[info] ", $text, $/;
}

sub warning {
	my $text = shift;
	print STDERR "[warning] ", $text, $/;
}

sub exitWithError {
	my $error = shift;
	print STDERR "[error] ", $error, $/;
	exit 1;
}

