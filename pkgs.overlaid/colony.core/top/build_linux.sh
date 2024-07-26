#!/usr/bin/env bash
set -e

# This script is used by the CI/Build machine to build the Linux projects
#
# The script ASSUMES that the working directory is the package root
#
# usage: build_linux.sh <bldnum>
# 

# setup the environment
source ./env.sh default

# Build all test linux projects (Note: Only building 64bit builds for now) 
pushd tests
$NQBP_BIN/other/bob.py -v4 linux  -b posix64 --bldtime --bldnum $1

# Run unit tests 
$NQBP_BIN/other/chuck.py -v --match a.out --dir _posix64
$NQBP_BIN/other/chuck.py -v --match aa.out --dir _posix64
$NQBP_BIN/other/chuck.py -v --match a.py --dir _posix64
$NQBP_BIN/other/chuck.py -v --match aa.py --dir _posix64

$NQBP_BIN/other/chuck.py -v --match a.out --dir _posix32
$NQBP_BIN/other/chuck.py -v --match aa.out --dir _posix32
$NQBP_BIN/other/chuck.py -v --match a.py --dir _posix32
$NQBP_BIN/other/chuck.py -v --match aa.py --dir _posix32


