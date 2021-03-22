#!/usr/bin/env perl
use strict;
use warnings;
use File::Find;
use File::Spec;
use Cwd;
use Getopt::Long;
use Pod::Usage;
use Archive::Tar;
use IPC::Open2;
use POSIX;

package NIC::Archive::Tar::File;
use parent "Archive::Tar::File";
sub new {
	my $class = shift;
	my $self = Archive::Tar::File->new(@_);
	bless($self, $class);
	return $self;
}

sub full_path {
	my $self = shift;
	my $full_path = $self->SUPER::full_path(); $full_path = '' unless defined $full_path;
	$full_path =~ s#^#./# if $full_path ne "" && $full_path ne "." && $full_path !~ m#^\./#;
	return $full_path;
}
1;
package main;

our $VERSION = '2.0';

our $_PROGNAME = "dm.pl";

my $ADMINARCHIVENAME = "control.tar.gz";
my $DATAARCHIVENAME = "data.tar";
my $ARCHIVEVERSION = "2.0";

our $compression = "gzip";
our $compresslevel = -1;
Getopt::Long::Configure("bundling", "auto_version");
GetOptions('compression|Z=s' => \$compression,
	'compress-level|z=i' => \$compresslevel,
	'build|b' => sub { },
	'help|?' => sub { pod2usage(1); },
	'man' => sub { pod2usage(-exitstatus => 0, -verbose => 2); })
	or pod2usage(2);

pod2usage(1) if(@ARGV < 2);

if($compresslevel < 0 || $compresslevel > 9) {
	$compresslevel = 6;
	$compresslevel = 9 if $compression eq "bzip2";
}

if($compresslevel eq 0) {
	$compresslevel = 1;
}

my $pwd = Cwd::cwd();
my $indir = File::Spec->rel2abs($ARGV[0]);
my $outfile = $ARGV[1];

die "ERROR: '$indir' is not a directory or does not exist.\n" unless -d $indir;

my $controldir = File::Spec->catpath("", $indir, "DEBIAN");

die "ERROR: control directory '$controldir' is not a directory or does not exist.\n" unless -d $controldir;
my $mode = (lstat($controldir))[2];
die sprintf("ERROR: control directory has bad permissions %03lo (must be >=0755 and <=0775)\n", $mode & 07777) if(($mode & 07757) != 0755);

my $controlfile = File::Spec->catfile($controldir, "control");
die "ERROR: control file '$controlfile' is not a plain file\n" unless -f $controlfile;
my %control_data = read_control_file($controlfile);

die "ERROR: control file '$controlfile' is missing a Package field" unless defined $control_data{"package"};
die "ERROR: control file '$controlfile' is missing a Version field" unless defined $control_data{"version"};
die "ERROR: control file '$controlfile' is missing an Architecture field" unless defined $control_data{"architecture"};

die "ERROR: package name has characters that aren't lowercase alphanums or '-+.'.\n" if($control_data{"package"} =~ m/[^a-z0-9+-.]/);
die "ERROR: package version ".$control_data{"version"}." doesn't contain any digits.\n" if($control_data{"version"} !~ m/[0-9]/);

foreach my $m ("preinst", "postinst", "prerm", "postrm", "extrainst_") {
	$_ = File::Spec->catfile($controldir, $m);
	next unless -e $_;
	die "ERROR: maintainer script '$m' is not a plain file or symlink\n" unless(-f $_ || -l $_);
	$mode = (lstat)[2];
	die sprintf("ERROR: maintainer script '$m' has bad permissions %03lo (must be >=0555 and <=0775)\n", $mode & 07777) if(($mode & 07557) != 0555)
}

if (-d "$outfile") {
	$outfile = sprintf('%s/%s_%s_%s.deb', $outfile, $control_data{"package"}, $control_data{"version"}, $control_data{"architecture"});
}

print "$_PROGNAME: building package `".$control_data{"package"}.":".$control_data{"architecture"}."' in `$outfile'\n";

open(my $ar, '>', $outfile) or die $!;

print $ar "!<arch>\n";
print_ar_record($ar, "debian-binary", time, 0, 0, 0100644, 4);
print_ar_file($ar, "$ARCHIVEVERSION\n", 4);

{
	my $tar = Archive::Tar->new();
	$tar->add_files(tar_filelist($controldir));
	my $comp;
	my $zFd = IO::Compress::Gzip->new(\$comp, -Level => 9);
	$tar->write($zFd);
	$zFd->close();
	print_ar_record($ar, $ADMINARCHIVENAME, time, 0, 0, 0100644, length($comp));
	print_ar_file($ar, $comp, length($comp));
} {
	my $tar = Archive::Tar->new();
	$tar->add_files(tar_filelist($indir));
	my ($fh_out, $fh_in);
	my $pid = open2($fh_out, $fh_in, compression_cmd()) or die "ERROR: open2 failed to create pipes for '$::compression'\n";
	fcntl($fh_out, F_SETFL, O_NONBLOCK);
	my $tmp_data = $tar->write();
	my $tmp_size = length($tmp_data);

	my ($off_in, $off_out) = (0, 0);
	my ($archivedata, $archivesize);
	while($off_in < $tmp_size) {
		# Write 8KB of data
		$off_in += syswrite $fh_in, $tmp_data, 8192, $off_in;
		# Get the compressed result if possible
	    my $o = sysread $fh_out, $archivedata, 8192, $off_out;
		if (defined($o)) {
			$off_out += $o;
		}
	}
	$fh_in->close();

	while (1) {
		# Get the remaining data
		my $o = sysread $fh_out, $archivedata, 8192, $off_out;
		if (defined($o) && $o > 0) {
			$off_out += $o;
		} elsif ($! != EAGAIN) {
			last;
		}
	}
	$archivesize = $off_out;
	$fh_out->close();
	waitpid($pid, 0);
	print_ar_record($ar, compressed_filename($DATAARCHIVENAME), time, 0, 0, 0100644, $archivesize);
	print_ar_file($ar, $archivedata, $archivesize);
}

close $ar;

sub print_ar_record {
	my ($fh, $filename, $timestamp, $uid, $gid, $mode, $size) = @_;
	printf $fh "%-16s%-12lu%-6lu%-6lu%-8lo%-10ld`\n", $filename, $timestamp, $uid, $gid, $mode, $size;
	$fh->flush();
}

sub print_ar_file {
	my ($fh, $data, $size) = @_;
	syswrite $fh, $data;
	print $fh "\n" if($size % 2 == 1);
	$fh->flush();
}

sub tar_filelist {
	my $dir = getcwd;
	chdir(shift);
	my @filelist;
	my @symlinks;

	find({wanted => sub {
		return if m#^./DEBIAN#;
		my $tf = NIC::Archive::Tar::File->new(file=>$_);
		my $mode = (lstat($_))[2] & 07777;
		$tf->mode($mode);
		$tf->chown("root", "wheel");
		push @symlinks, $tf if -l;
		push @filelist, $tf if ! -l;
	}, no_chdir => 1}, ".");
	chdir($dir);
	return (@filelist, @symlinks);
}

sub read_control_file {
	my $filename = shift;
	open(my $fh, '<', $filename) or die "ERROR: can't open control file '$filename'\n";
	my %data;
	while(<$fh>) {
		die "ERROR: control file contains Windows/Macintosh line endings - please use a text editor or dos2unix to change to Unix line endings\n" if(m/\r/);
		if(m/^(.*?): (.*)/) {
			$data{lc($1)} = $2;
		}
	}
	close $fh;
	return %data;
}

sub compression_cmd {
	return "gzip -c".$compresslevel if $::compression eq "gzip";
	return "bzip2 -c".$compresslevel if $::compression eq "bzip2";
	return "lzma -c".$compresslevel if $::compression eq "lzma";
	return "xz -c".$compresslevel if $::compression eq "xz";
	if($::compression ne "cat") {
		print "WARNING: compressor '$::compression' is unknown, falling back to cat.\n";
	}
	return "cat";
}

sub compressed_filename {
	my $fn = shift;
	my $suffix = "";
	$suffix = ".gz" if $::compression eq "gzip";
	$suffix = ".bz2" if $::compression eq "bzip2";
	$suffix = ".lzma" if $::compression eq "lzma";
	$suffix = ".xz" if $::compression eq "xz";
	return $fn.$suffix;
}

__END__

=head1 NAME

dm.pl

=head1 SYNOPSIS

dm.pl [options] <directory> <package>

=head1 OPTIONS

=over 8

=item B<-b>

This option exists solely for compatibility with dpkg-deb.

=item B<-ZE<lt>compressionE<gt>>

Specify the package compression type. Valid values are gzip (default), bzip2, lzma, xz and cat (no compression.)

=item B<-zE<lt>compress-levelE<gt>>

Specify the package compression level. Valid values are between 1 and 9. Default is 9 for bzip2, 6 for others. 0 is identical to 1. Refer to B<gzip(1)>, B<bzip2(1)>, B<xz(1)> for explanations of what effect each compression level has.

=item B<--help>, B<-?>

Print a brief help message and exit.

=item B<--man>

Print a manual page and exit.

=back

=head1 DESCRIPTION

B<This program> creates Debian software packages (.deb files) and is a drop-in replacement for dpkg-deb.

=cut
