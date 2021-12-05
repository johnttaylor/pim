#!/usr/bin/env bash
# This script sets up the PIM environment to use NQBP and the GCC compiler
#
# This script MUST BE 'sourced' since it sets environment variables
#


# Get the root directory
HERE="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Put the current directory into the command path (simplifies invoking the nqbp.py scripts)
export PATH=$PATH:./

# Configure NQBP
export NQBP_PKG_ROOT=$HERE
export NQBP_WORK_ROOT=$HERE/..
export NQBP_XPKGS_ROOT=$HERE/xsrc
export NQBP_BIN=$NQBP_XPKGS_ROOT/nqbp


alias t="cd $HERE"
alias bob="$HERE/xsrc/nqbp/other/bob.py"
alias chuck="$HERE/xsrc/nqbp/other/chuck.py"

# Assumes the GCC is already in the command path
echo "Environment set (using native GCC compiler)"
