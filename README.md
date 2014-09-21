# shasum

SHA-1 checksum generator

# Building

`shasum` is a simple C++ application that generates SHA-1 checksums for an individual file, or an
entire directory structure that contains many files. Output is in the same format as the Perl
shasum utility for backward compatibility.

## Dependencies

* [CMake](http://www.cmake.org) 2.8.12 or newer
* C compiler. The following have been tested:
  * [llvm](http://llvm.org) 3.4 or newer
  * [gcc](http://gcc.gnu.org) 4.8.3 or newer

## Instructions

I highly recommend building outside of the source tree so that build products do not pollute the
repository. The simplest way to accomplish this is to create a sub-directory named "build" as
follows:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ make test

# Examples

* Generate a checksum for a single file:

    shasum README.md
    5CF456B3195EDBCF12CE6EA02297C7C4164B9E0C README.md

* Generate checksums for all of the files contained the current directory:

    shasum .
    6281AB4081C6180DBBB5D46780EF5E357C3277CE ./CMakeLists.txt
    377A51F1401633D848CA02D645FC8044EB9805A7 ./Doxyfile.in
    5CF456B3195EDBCF12CE6EA02297C7C4164B9E0C ./README.md
    1BDECEB678E5882219E438631D01216AF2EF5739 ./source/main.cpp
    

# TODO List

1.  More robust processing of the command line.
2.  Follow symbolic links.

# Contributing

1.  Fork it
2.  Create a feature branch (`git checkout -b new-feature`)
3.  Commit changes (`git commit -am "Added new feature xyz"`)
4.  Push the branch (`git push origin new-feature`)
5.  Create a new pull request.

# Maintainers

* Paul Howes (http://github.com/PaulHowes/)

# License

Webby copyright 2014 Paul Howes and is licensed under the [Apache License](LICENSE).
