This is JSIM, a Josephson Junction aware spice simulator
originally written by Emerson S. Fang as part of his work
in Ted Van Duzer's lab at the University of California, Berkeley

== Quick Start ==

If you're new to JSIM, then welcome.  JSIM is meant for superconducting
simluations of Jospehson Junction based technologies, and does not
have the full features that you may be used to in other spice variants.
If you're not looking to simulate superconducting circuits, it's very
unlikely that you want to use JSIM.

This repository has been updated to use CMake to build, and should work
on a wide variety of different architectures.  It's known to compile on
Mac OSX Sierra and Ubuntu, but should compile on most any unix-like
operating system.  I recommend that you install and build using CMake,
though the original makefiles have been preserved in the various branches
and might still work.

Here is the super quick guide to get up and running:

Install CMake, and an appropriate build environment on your system.

git clone https://github.com/coldlogix/jsim.git
cd jsim/vendor/jsim-2.0e/
mkdir build
cd build
cmake ..
make

This should produce both jsim and jsim_n executables.  The jsim_n
version includes stochastic noise simulation added by J. Satchell in 1995,
and probably will eventually just become part of jsim in this repository
in the future.

== Branches ==

This repository attempts to collect and order close to 30 years of
variants of the JSIM project.  There is an attempt to provide branches
for all of the versions of JSIM encountered, and to try to parent them
from other branches in an appropriate way such that if you have changes
that you've made, you can easily integrate those with the repository
and contribute your changes back to the community.

These following branches are available, and their origin and histories are
annotated in the log messages for the commits for each of the branches.

  vendor_ortlepp_jsim_n
  vendor_stellenbosch_jsim_n
  vendor_whiteley_jsim_2.0d
  vendor_whiteley_jsim_2.0e

== History ==

This git repository is an attempt to collect and consolidate all
of the various revisions to jsim that have occured over the years
and provide one central manageable point for future work on JSIM.

The original thesis from Emerson S. Fang was written in 1989, so
there is a very deep and old histroy for JSIM, with many many people
touching and modifying the source over many many years.  I have
attempted as best as possible to organize and properly parent the 
various versions I've found in this repository, using branches for
each new version I've found.

The various collection formats (tar, zip, raw directories and others)
have diverged over the many years, and so the directory structure in
the original sources is not always the same across versions of JSIM
that I've found "in the wild". I've attempted to checkin the raw
versions as close to the formats I've found them in as possible, and
then to normalize the versions, hoping that this will make it easier
for others to merge any changes that they've made into the repository.


