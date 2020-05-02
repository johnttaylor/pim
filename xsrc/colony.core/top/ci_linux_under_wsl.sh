#!/bin/bash
# This script is used by the CI/Build machine to build the Linux test
# project on a Windows machine by running the WSL (Window Subsystem for Linux)
#
# usage: ci_linux_under_wsl <windows_path_to_workspace> <outcast_pkg_root> <outcast_pkg_uverse> <nqbp_bin>
#
# Example: wsl ./ci_linux_under_wsl Jenkins/workspace/CI.Colony.Core _devel/Packages/ _devel/Outcast-Universe/ Jenkins/workspace/CI.Colony.Core/xpkgs/nqbp

export _root_=/mnt/c

# Go to the Test directory
cd $_root_
cd $1
cd colony.core/tests

# setup the environment
export OUTCAST_PKGS_ROOT=$_root_/$2
export OUTCAST_PKGS_UVERSE=$_root_/$3
export NQBP_BIN=$_root_/$4
export PATH=$PATH:./

# Build all linux projects
bob.py build -v linux  --bld-all -t
