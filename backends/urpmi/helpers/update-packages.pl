#!/usr/bin/perl

use strict;

use lib;
use File::Basename;

BEGIN {
  push @INC, dirname($0);
}

use urpm;
use urpm::media;
use urpm::select;
use urpm::args;
use urpmi_backend::tools;
use urpmi_backend::open_db;
use urpmi_backend::actions;

# This script call only be called with one argument (the package id)
exit if($#ARGV != 0);

my @names;
foreach(split(/\|/, $ARGV[0])) {
  my @pkgid = split(/;/, $_);
  push @names, $pkgid[0];
}

my $urpm = urpm->new_parse_cmdline;
urpm::media::configure($urpm);

my $db = open_rpm_db();
$urpm->compute_installed_flags($db);

my %requested;

my @depslist = @{$urpm->{depslist}};
my $pkg = undef;
foreach my $depslistpkg (@depslist) {
  foreach my $name (@names) {
    if($depslistpkg->name =~ /^$name$/ && $depslistpkg->flag_upgrade) {
      $requested{$depslistpkg->id} = 1;
      goto tonext;
    }
  }
  tonext:
}

perform_installation($urpm, \%requested);
