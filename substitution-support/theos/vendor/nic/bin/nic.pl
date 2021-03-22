#!/usr/bin/perl

my $VER = "2.0";

use strict;
use warnings;
use FindBin;
use lib "$FindBin::RealBin/lib";

use Getopt::Long;
use Cwd qw(abs_path getcwd);
use File::Spec;
use File::Find;
use File::Copy;
use User::pwent;
use POSIX qw(getuid);
use Module::Load::Conditional 'can_load';
use Tie::File;

use NIC::Bridge::Context (PROMPT => \&nicPrompt);
use NIC::Formats::NICTar;
use NIC::NICType;

# Workaround to allow Find to function on WSL. Currently on WSL without this line nic.pl will fail to find templates.
if($^O eq "linux") {
	my $fh;
	open($fh, "/proc/version");
	$File::Find::dont_use_nlink = grep { /Microsoft/ } <$fh>;
	close($fh);
}

our $savedStdout = *STDOUT;

my @_dirs = File::Spec->splitdir(abs_path($FindBin::Bin));
$_dirs[$#_dirs]="templates";
our $_templatepath = File::Spec->catdir(@_dirs);
$#_dirs--;
my $_theospath = File::Spec->catdir(@_dirs);

{
	my $_abstheospath = abs_path($_theospath);
	my $_cwd = abs_path(Cwd::getcwd());
	$_abstheospath .= '/' if $_abstheospath !~ /\/$/;
	$_cwd .= '/' if $_cwd !~ /\/$/;
	exitWithError("Cowardly refusing to make a project inside \$THEOS ($_abstheospath)") if($_cwd =~ /^$_abstheospath/);
}

my %CONFIG = (link_theos => 0);
loadConfig();

my $clean_project_name = "";
my $project_name = "";
my $package_prefix = $CONFIG{'package_prefix'};
$package_prefix = "com.yourcompany" if !$package_prefix;
my $package_name = "";
my $username = $CONFIG{'username'};
$username = "" if !$username;

my $template = undef;
my $nicfile = undef;

Getopt::Long::Configure("bundling");

GetOptions(	"packagename|p=s" => \$package_name,
		"name|n=s" => \$project_name,
		"user|u=s" => \$username,
		"nic=s" => \$nicfile,
		"template|t=s" => \$template);

$project_name = $ARGV[0] if($ARGV[0]);

my $_versionstring = "NIC $VER - New Instance Creator";
print $_versionstring,$/;
print "-" x length($_versionstring),$/;

my $NIC;
if($nicfile) {
	$NIC = _loadNIC($nicfile) if $nicfile && -f $nicfile;
} else {
	my @templates = getTemplates();
	if(scalar @templates == 0) {
		exitWithError("No file specified with --nic and no templates found.");
	}

	if($template) {
		my @matched = grep { $_->name eq $template } @templates;
		$NIC = $matched[0] if(scalar @matched > 0);
	}
	if(!$NIC) {
		$NIC = promptList(undef, "Choose a Template (required)", sub { local $_ = shift; return $_->name; }, @templates);
	}
}

exitWithError("No NIC file loaded.") if !$NIC;

promptIfMissing(\$project_name, undef, "Project Name (required)");
exitWithError("I can't live without a project name! Aieeee!") if !$project_name;
$clean_project_name = cleanProjectName($project_name);

$package_name = $package_prefix.".".packageNameIze($project_name) if $CONFIG{'skip_package_name'};
promptIfMissing(\$package_name, $package_prefix.".".packageNameIze($project_name), "Package Name") unless $NIC->variableIgnored("PACKAGENAME");

promptIfMissing(\$username, getUserName(), "Author/Maintainer Name") unless $NIC->variableIgnored("USER");

my $directory = lc($clean_project_name);
if(-d $directory) {
	my $response;
	promptIfMissing(\$response, "N", "There's already something in $directory. Continue");
	exit 1 if(uc($response) eq "N");
}

$NIC->variable("FULLPROJECTNAME") = $project_name;
$NIC->variable("PROJECTNAME") = $clean_project_name;
$NIC->variable("PACKAGENAME") = $package_name;
$NIC->variable("USER") = $username;

if(! -e "control" && ! -e "layout/DEBIAN/control") {
	$NIC->addConstraint("package");
}

foreach my $prompt ($NIC->prompts) {
	nicPrompt($NIC, $prompt->{name}, $prompt->{prompt}, $prompt->{default});
}

my $cwd = abs_path(getcwd());

# Add theos symlink to the template, if necessary
if($CONFIG{'link_theos'} != 0 && !$NIC->variableIgnored("THEOS")) {
	$NIC->addConstraint("link_theos");

	my $template_theos_reference = $NIC->_getContentWithoutCreate("theos");
	if(!$template_theos_reference || $template_theos_reference->type == NIC::NICType::TYPE_UNKNOWN) {
		print STDERR "[warning] Asked to link theos, but template lacks an optional theos link. Creating one! Contact the author of this template about this issue.",$/;
		$NIC->registerSymlink("theos", '@@THEOS_PATH@@');
	}

	my $theosLinkPath = $CONFIG{'theos_path'};
	$theosLinkPath = readlink("$cwd/theos") if !$theosLinkPath && (-l "$cwd/theos") && !$CONFIG{'ignore_parent_theos'};
	$theosLinkPath = "$cwd/theos" if !$theosLinkPath && (-d "$cwd/theos") && !$CONFIG{'ignore_parent_theos'};
	$theosLinkPath = $_theospath if !$theosLinkPath;

	# Set @@THEOS@@ to 'theos', so that the project refers to its linked copy of theos.
	$NIC->variable("THEOS") = "theos";
	$NIC->variable("THEOS_PATH") = $theosLinkPath;
} else {
	# Trust that the user knows what he's doing and set @@THEOS@@ to $(THEOS). (or whatever the user prefers)
	my $theosLocalName = '$(THEOS)';
	$theosLocalName = $CONFIG{"theos_local_name"} if $CONFIG{"theos_local_name"};
	$NIC->variable("THEOS") = $theosLocalName;
}

# Execute control script.
$NIC->exec or exitWithError("Failed to build template '".$NIC->name."'.");

print "Instantiating ".$NIC->name." in ".lc($clean_project_name)."/...",$/;
my $dirname = lc($clean_project_name);
$NIC->build($dirname);
chdir($cwd);

my @makefiles = ("GNUmakefile", "makefile", "Makefile");
my $makefile;
map { $makefile = $_ if -e $_; } @makefiles;
if($makefile) {
	tie(my @lines, 'Tie::File', $makefile);
	my $hasCommon = 0;
	map {$hasCommon++ if /common\.mk/;} @lines;
	if($hasCommon > 0) {
		my $alreadyHas = 0;
		map {$alreadyHas++ if /^\s*SUBPROJECTS.*$dirname/;} @lines;
		if($alreadyHas == 0) {
			print "Adding '$project_name' as an aggregate subproject in Theos makefile '$makefile'.",$/;
			my $newline = "SUBPROJECTS += $dirname";
			my $i = 0;
			my $aggLine = -1;
			map {$aggLine = $i if /aggregate\.mk/; $i++;} @lines;
			if($aggLine == -1) {
				push(@lines, $newline);
				push(@lines, "include \$(THEOS_MAKE_PATH)/aggregate.mk");
			} else {
				splice(@lines, $aggLine, 0, $newline);
			}
		}
	}
	untie(@lines);
}
print "Done.",$/;

sub promptIfMissing {
	my $vref = shift;
	return if(${$vref});

	my $default = shift;
	my $prompt = shift;

	if($default) {
		print $::savedStdout $prompt, " [$default]: ";
	} else {
		print $::savedStdout $prompt, ": ";
	}

	$| = 1; $_ = <STDIN>;
	chomp;

	if($default) {
		${$vref} = $_ ? $_ : $default;
	} else {
		${$vref} = $_;
	}
}

sub promptList {
	my $default = shift;
	my $prompt = shift;
	my $formatsub = shift // sub { shift; };
	my @list = @_;

	$default = -1 if(!defined $default);

	for(0..$#list) { print " ".($_==$default?">":" ")."[".($_+1).".] ",$formatsub->($list[$_]),$/; }
	print $prompt,": ";
	$| = 1;
	my $idx = -1;
	while(<STDIN>) {
		chomp;
		if($default > -1 && $_ eq "") {
			$idx = $default;
			last;
		}
		if($_ < 1 || $_ > $#list+1) {
			print "Invalid value.",$/,$prompt,": ";
			next;	
		}
		$idx = $_-1;
		last;
	}
	return $list[$idx];
}

sub exitWithError {
	my $error = shift;
	print STDERR "[error] ", $error, $/;
	exit 1;
}

sub _loadNIC {
	my $nicfile = shift;
	open(my $nichandle, "<", $nicfile);
	my $line = <$nichandle>;
	seek($nichandle, 0, 0);

	(my $prettyname = $nicfile) =~ s/$::_templatepath\/(.*)\.nic(\.tar)?/$1/g;
	$prettyname .= " (unnamed template)";

	my $nicversion = 1;
	my $NIC = undef;
	if($line =~ /^nic (\w+)$/) {
		$nicversion = $1;
		my $NICPackage = "NIC$nicversion";
		return undef if(!can_load(modules => {"NIC::Formats::$NICPackage" => undef}));
		$NIC = "NIC::Formats::$NICPackage"->new($nichandle, $prettyname);
	} else {
		$NIC = NIC::Formats::NICTar->new($nichandle, $prettyname);
	}

	close($nichandle);
	return $NIC;
}

sub getTemplates {
	our @templates = ();
	find({wanted => \&templateWanted, no_chdir => 1}, $_templatepath);
	sub templateWanted {
		if(-f && (/\.nic$/ || /\.nic\.tar$/)) {
			my $nic = _loadNIC($_);
			push(@templates, $nic) if $nic;
		}
	}
	return sort { $a->name cmp $b->name } @templates;
}

sub packageNameIze {
	my $name = shift;
	$name =~ s/ //g;
	$name =~ s/[^\w\+-.]//g;
	return lc($name);
}

sub cleanProjectName {
	my $name = shift;
	$name =~ s/ //g;
	$name =~ s/\W//g;
	return $name;
}

sub getUserName {
	my $pw = getpw(getuid());
	my ($fullname) = split(/\s*,\s*/, $pw->gecos);
	return $fullname && $fullname ne "\"\"" ? $fullname : $pw->name;
}

sub getHomeDir {
	my $pw = getpw(getuid());
	return $pw->dir;
}

sub loadConfig {
	open(my $cfh, "<", getHomeDir()."/.nicrc") or return;
	while(<$cfh>) {
		if(/^(.+?)\s*=\s*\"(.*)\"$/) {
			my $key = $1;
			my $value = $2;
			$CONFIG{$key} = $value;
		}
	}
}

sub nicPrompt {
	# Do we want to import these variables into the NIC automatically? In the format name.VARIABLE?
	# If so, this could become awesome. We could $NIC->get($prompt->{name})
	# and have loaded the variables in a loop beforehand.
	# This would also allow the user to set certain variables (package prefix, username) for different templates.
	my ($nic, $variable, $prompt, $default) = @_;
	my $response = undef;
	$response = $CONFIG{$nic->name().".".$variable} if($variable);

	promptIfMissing(\$response, $default, "[".$nic->name."] ".$prompt);

	$NIC->variable($variable) = $response if $variable;

	# Return the response for anybody who's interested.
	$response;
}
