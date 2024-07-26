#!/usr/bin/env bash
# This script sets up the PIM environment to use NQBP and the GCC compiler
# 
# This script MUST BE 'sourced' since it sets environment variables
#
# usage: env.sh [<gcc-bin-path>|default]
#
# <gcc-bin-path>   Optional explicit path to the GCC bin/ directory. The 
#                  default behavior is to use the GCC toolchain in the command
#                  path.  If 'default' is specified than the environment
#                  reverts to the native GCC toolchain
#



# Get the root directory
HERE="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Set optional GCC path
if [ -n "${1}" ]; then
    if [ ${1} = "default" ]; then
        unset NQBP_GCC_BIN
    else
        export NQBP_GCC_BIN=${1}
    fi
fi

# Put the current directory into the command path (simplifies invoking the nqbp.py scripts)
export PATH=$PATH:./:$NQBP_BIN/ninja

# Configure NQBP
export NQBP_PKG_ROOT=$HERE
export NQBP_WORK_ROOT=$HERE/..
export NQBP_XPKGS_ROOT=$HERE/xsrc
export NQBP_BIN=$NQBP_XPKGS_ROOT/nqbp2

# Add ninja to the command path
export PATH=$PATH:$NQBP_BIN/ninja


alias t="cd $HERE"
alias bob="$NQBP_BIN/other/bob.py"
alias chuck="$NQBP_BIN/other/chuck.py"

# Display which compiler is being used
if [ -z "${NQBP_GCC_BIN}" ]; then
    echo "Environment set (using native GCC compiler)"
    gcc --version
else
    echo "Environment set (using GCC compiler @ ${NQBP_GCC_BIN})"
    ${NQBP_GCC_BIN}/gcc --version
fi

