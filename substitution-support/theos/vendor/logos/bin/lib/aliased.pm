package aliased; # git description: v0.33-3-g0a61221
# ABSTRACT: Use shorter versions of class names.
$aliased::VERSION = '0.34';
require Exporter;
@EXPORT = qw(alias prefix);

use strict;
use warnings;

sub _croak {
    require Carp;
    Carp::croak(@_);
}

sub import {
    # Without args, just export @EXPORT
    goto &Exporter::import if @_ <= 1;

    my ( $class, $package, $alias, @import ) = @_;

    my $callpack = caller(0);
    _load_alias( $package, $callpack, @import );
    _make_alias( $package, $callpack, $alias );
}

sub _get_alias {
    my $package = shift;
    $package =~ s/.*(?:::|')//;
    return $package;
}

sub _make_alias {
    my ( $package, $callpack, $alias ) = @_;

    $alias ||= _get_alias($package);

    my $destination = $alias =~ /::/
      ? $alias
      : "$callpack\::$alias";

    # need a scalar not referenced elsewhere to make the sub inlinable
    my $pack2 = $package;

    no strict 'refs';
    *{ $destination } = sub () { $pack2 };
}

sub _load_alias {
    my ( $package, $callpack, @import ) = @_;

    # We don't localize $SIG{__DIE__} here because we need to be careful about
    # restoring its value if there is a failure.  Very, very tricky.
    my $sigdie = $SIG{__DIE__};
    {
        my $code =
          @import == 0
          ? "package $callpack; use $package;"
          : "package $callpack; use $package (\@import)";
        eval $code;
        if ( my $error = $@ ) {
            $SIG{__DIE__} = $sigdie;
            _croak($error);
        }
        $sigdie = $SIG{__DIE__}
          if defined $SIG{__DIE__};
    }

    # Make sure a global $SIG{__DIE__} makes it out of the localization.
    $SIG{__DIE__} = $sigdie if defined $sigdie;
    return $package;
}

sub alias {
    my ( $package, @import ) = @_;

    my $callpack = scalar caller(0);
    return _load_alias( $package, $callpack, @import );
}

sub prefix {
    my ($class) = @_;
    return sub {
        my ($name) = @_;
        my $callpack = caller(0);
        if ( not @_ ) {
            return _load_alias( $class, $callpack );
        }
        elsif ( @_ == 1 && defined $name ) {
            return _load_alias( "${class}::$name", $callpack );
        }
        else {
            _croak("Too many arguments to prefix('$class')");
        }
    };
}

1;

__END__

=pod

=encoding UTF-8

=head1 NAME

aliased - Use shorter versions of class names.

=head1 VERSION

version 0.34

=head1 SYNOPSIS

  # Class name interface
  use aliased 'My::Company::Namespace::Customer';
  my $cust = Customer->new;

  use aliased 'My::Company::Namespace::Preferred::Customer' => 'Preferred';
  my $pref = Preferred->new;


  # Variable interface
  use aliased;
  my $Customer  = alias "My::Other::Namespace::Customer";
  my $cust      = $Customer->new;

  my $Preferred = alias "My::Other::Namespace::Preferred::Customer";
  my $pref      = $Preferred->new;

=head1 DESCRIPTION

C<aliased> is simple in concept but is a rather handy module.  It loads the
class you specify and exports into your namespace a subroutine that returns
the class name.  You can explicitly alias the class to another name or, if you
prefer, you can do so implicitly.  In the latter case, the name of the
subroutine is the last part of the class name.  Thus, it does something
similar to the following:

  #use aliased 'Some::Annoyingly::Long::Module::Name::Customer';

  use Some::Annoyingly::Long::Module::Name::Customer;
  sub Customer {
    return 'Some::Annoyingly::Long::Module::Name::Customer';
  }
  my $cust = Customer->new;

This module is useful if you prefer a shorter name for a class.  It's also
handy if a class has been renamed.

(Some may object to the term "aliasing" because we're not aliasing one
namespace to another, but it's a handy term.  Just keep in mind that this is
done with a subroutine and not with typeglobs and weird namespace munging.)

Note that this is B<only> for C<use>ing OO modules.  You cannot use this to
load procedural modules.  See the L<Why OO Only?> section.  Also, don't let
the version number fool you.  This code is ridiculously simple and is just
fine for most use.

=head2 Implicit Aliasing

The most common use of this module is:

  use aliased 'Some::Module::name';

C<aliased> will  allow you to reference the class by the last part of the
class name.  Thus, C<Really::Long::Name> becomes C<Name>.  It does this by
exporting a subroutine into your namespace with the same name as the aliased
name.  This subroutine returns the original class name.

For example:

  use aliased "Acme::Company::Customer";
  my $cust = Customer->find($id);

Note that any class method can be called on the shorter version of the class
name, not just the constructor.

=head2 Explicit Aliasing

Sometimes two class names can cause a conflict (they both end with C<Customer>
for example), or you already have a subroutine with the same name as the
aliased name.  In that case, you can make an explicit alias by stating the
name you wish to alias to:

  use aliased 'Original::Module::Name' => 'NewName';

Here's how we use C<aliased> to avoid conflicts:

  use aliased "Really::Long::Name";
  use aliased "Another::Really::Long::Name" => "Aname";
  my $name  = Name->new;
  my $aname = Aname->new;

You can even alias to a different package:

  use aliased "Another::Really::Long::Name" => "Another::Name";
  my $aname = Another::Name->new;

Messing around with different namespaces is a really bad idea and you probably
don't want to do this.  However, it might prove handy if the module you are
using has been renamed.  If the interface has not changed, this allows you to
use the new module by only changing one line of code.

  use aliased "New::Module::Name" => "Old::Module::Name";
  my $thing = Old::Module::Name->new;

=head2 Import Lists

Sometimes, even with an OO module, you need to specify extra arguments when
using the module.  When this happens, simply use L<Explicit Aliasing> followed
by the import list:

Snippet 1:

  use Some::Module::Name qw/foo bar/;
  my $o = Some::Module::Name->some_class_method;

Snippet 2 (equivalent to snippet 1):

  use aliased 'Some::Module::Name' => 'Name', qw/foo bar/;
  my $o = Name->some_class_method;

B<Note>:  remember, you cannot use import lists with L<Implicit Aliasing>.  As
a result, you may simply prefer to only use L<Explicit Aliasing> as a matter
of style.

=head2 alias()

This function is only exported if you specify C<use aliased> with no import
list.

    use aliased;
    my $alias = alias($class);
    my $alias = alias($class, @imports);

C<alias()> is an alternative to C<use aliased ...> which uses less magic and
avoids some of the ambiguities.

Like C<use aliased> it C<use>s the C<$class> (pass in C<@imports>, if given)
but instead of providing an C<Alias> constant it simply returns a scalar set
to the C<$class> name.

    my $thing = alias("Some::Thing::With::A::Long::Name");

    # Just like Some::Thing::With::A::Long::Name->method
    $thing->method;

The use of a scalar instead of a constant avoids any possible ambiguity
when aliasing two similar names:

    # No ambiguity despite the fact that they both end with "Name"
    my $thing = alias("Some::Thing::With::A::Long::Name");
    my $other = alias("Some::Other::Thing::With::A::Long::Name");

and there is no magic constant exported into your namespace.

The only caveat is loading of the $class happens at run time.  If C<$class>
exports anything you might want to ensure it is loaded at compile time with:

    my $thing;
    BEGIN { $thing = alias("Some::Thing"); }

However, since OO classes rarely export this should not be necessary.

=head2 prefix() (experimental)

This function is only exported if you specify C<use aliased> with no import
list.

    use aliased;

Sometimes you find you have a ton of packages in the same top-level namespace
and you want to alias them, but only use them on demand.  For example:

    # instead of:
    MailVerwaltung::Client::Exception::REST::Response->throw()

    my $error = prefix('MailVerwaltung::Client::Exception');
    $error->('REST::Response')->throw();   # same as above
    $error->()->throw; # same as MailVerwaltung::Client::Exception->throw

=head2 Why OO Only?

Some people have asked why this code only support object-oriented modules
(OO).  If I were to support normal subroutines, I would have to allow the
following syntax:

  use aliased 'Some::Really::Long::Module::Name';
  my $data = Name::data();

=for stopwords nilly

That causes a serious problem.  The only (reasonable) way it can be done is to
handle the aliasing via typeglobs.  Thus, instead of a subroutine that
provides the class name, we alias one package to another (as the
L<namespace|namespace> module does.)  However, we really don't want to simply
alias one package to another and wipe out namespaces willy-nilly.  By merely
exporting a single subroutine to a namespace, we minimize the issue.

Fortunately, this doesn't seem to be that much of a problem.  Non-OO modules
generally support exporting of the functions you need and this eliminates the
need for a module such as this.

=head1 EXPORT

This modules exports a subroutine with the same name as the "aliased" name.

=head1 SEE ALSO

The L<namespace> module.

=head1 THANKS

=for stopwords Rentrak

Many thanks to Rentrak, Inc. (http://www.rentrak.com/) for graciously allowing
me to replicate the functionality of some of their internal code.

=head1 AUTHOR

Curtis "Ovid" Poe <ovid@cpan.org>

=head1 COPYRIGHT AND LICENSE

This software is copyright (c) 2005 by Curtis "Ovid" Poe.

This is free software; you can redistribute it and/or modify it under
the same terms as the Perl 5 programming language system itself.

=head1 CONTRIBUTORS

=for stopwords Karen Etheridge Curtis Poe Ovid Florian Ragwitz Grzegorz Rożniecki Father Chrysostomos Belden Lyman Olivier Mengué

=over 4

=item *

Karen Etheridge <ether@cpan.org>

=item *

Curtis Poe <ovid@cpan.org>

=item *

Ovid <curtis_ovid_poe@yahoo.com>

=item *

Florian Ragwitz <rafl@debian.org>

=item *

Grzegorz Rożniecki <xaerxess@gmail.com>

=item *

Father Chrysostomos <sprout@cpan.org>

=item *

Belden Lyman <belden@shutterstock.com>

=item *

Olivier Mengué <dolmen@cpan.org>

=back

=cut
