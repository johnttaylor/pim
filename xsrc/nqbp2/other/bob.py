#!/usr/bin/python3
r""" 
Script to build projects using NQBP
===============================================================================
usage: bob [options] here [<build-opts>...]
       bob [options] PATTERN [<build-opts>...]
       bob [options] --file BLDLIST

Options:
    here                 Builds all NQBP projects starting from the current 
                         working directory.
    <build-opts>         Option(s) to be passed directly to build script
    PATTERN              If a subdir under PRJDIR matches PATTERN, then that
                         directory is built.  Standard Python wildcards can
                         be used in the PATTERN.
    --p2 FILTER          An second filter that is AND'd with PATTERN to
                         filter which sub-directories get built. Standard Python 
                         wildcards can be used in the FILTER.
    --p3 FILTER          An third filter that is AND'd with PATTERN to
                         filter which sub-directories get built. Standard Python 
                         wildcards can be used in the FILTER.
    --path PRJDIR        The full path to the project directory of the 
                         projects to build.  If no path is specified, the
                         current working directory is used for the project
                         path.
    --exclude DIR        Excludes the specified sub directory. Standard Python 
                         wildcards can be used in the DIR
    --e2 DIR             Excludes an additional sub directory. Standard Python 
                         wildcards can be used in the DIR
    --e3 DIR             Excludes an additional sub directory. Standard Python 
                         wildcards can be used in the DIR
    --file BLDLIST       A text file containing a list of projects to build.
                         The format of file is a list of 'build' commands. 
                         Blank lines and line starting with '#' are skipped.
    --xconfig SCRIPT     Name and full path to the compiler config script.  If 
                         no script is provided then it is assume no additional 
                         config/setup is required.
    --config SCRIPT      Same as the '--xconfig' option, but the name and path 
                         are relative to the package root directory
    -x SCRIPT            Build using SCRIPT [Default: nqbp.py]
    -2                   Run two builds at the same time
    -4                   Run four builds at the same time
    -v                   Be verbose 
    -h, --help           Display help for common options/usage
    
Examples:
    ; Builds all NQBP projects (and all 'variants') under the projects/ 
    ; directory that contain 'mingw' in their path.  The '--bld-all' option 
    ; is NQBP option.
    bob.py --path \mywrkspace\mypkg\projects mingw --bld-all
    
    ; Builds the projects listed in the file 'mybuild.lst'
    bob.py --file mybuild.lst

    ; Builds all 'mybuild' projects (with the '-s' option) under the current 
    ; working directory
    bob.py -x mybuild.bat here -s
    
"""

import sys
import os
import subprocess
import fnmatch
from multiprocessing import Process
import multiprocessing
import time

sys.path.append( os.path.dirname(__file__) + os.sep + ".." )
from nqbplib.docopt.docopt import docopt
from nqbplib import utils
from nqbplib.my_globals import NQBP_PKG_ROOT

BOB_VERSION = '1.0'

#------------------------------------------------------------------------------
def _filter_prj_list( all_prj, pattern, pkgroot, exclude=None, exclude2=None, exclude3=None, p2=None, p3=None ):
    list = []
    for p in all_prj:
        relpath = p.replace(pkgroot,'')
        dirs    = relpath.split(os.sep)
        if ( len(fnmatch.filter(dirs,pattern))> 0 ):
            keep1 = True if exclude == None or len(fnmatch.filter(dirs,exclude)) == 0 else False
            keep2 = True if exclude2 == None or len(fnmatch.filter(dirs,exclude2)) == 0 else False
            keep3 = True if exclude3 == None or len(fnmatch.filter(dirs,exclude3)) == 0 else False
            keep4 = True if p2 == None or len(fnmatch.filter(dirs,p2)) > 0 else False
            keep5 = True if p3 == None or len(fnmatch.filter(dirs,p3)) > 0 else False
            if ( keep1 and keep2 and keep3 and keep4 and keep5 ):
                list.append( p )
            

    return list
    
def _build_project( full_path_of_build_script, verbose, bldopts, config, xconfig, pkgroot ):
    # reconcile config options
    cfg = None
    if ( config ):
        cfg = os.path.join( pkgroot, config )
    elif ( xconfig ):
        cfg = xconfig
    
    # Build the project
    utils.push_dir( os.path.dirname(full_path_of_build_script) )
    cmd = full_path_of_build_script + ' ' + " ".join(bldopts)
    print( "BUILDING: "+ cmd )
    if ( config ):
        cmd = utils.concatenate_commands( cfg, cmd )
    utils.run_shell2( cmd, verbose, f"ERROR: Build failure ({cmd})" )
    utils.pop_dir()




#------------------------------------------------------------------------------
# BEGIN
if __name__ == '__main__':

    # Parse command line
    args = docopt(__doc__, version=BOB_VERSION, options_first=True )
    
    # Set quite & verbose modes
    utils.set_verbose_mode( args['-v'] )

    # Default the projects/ dir path to the current working directory
    ppath = os.getcwd()
    
    # Project dir path is explicit set
    if ( args['--path'] ):
        ppath = args['--path']

    
    # Set which build engine to use
    build_script = args['-x']


    # Get superset of projects to build
    utils.push_dir( ppath )
    utils.set_pkg_and_wrkspace_roots( ppath )    
    pkgroot = NQBP_PKG_ROOT()
    all_prjs = utils.walk_file_list( build_script, ppath )

    # Get project list from a file
    if ( args['--file'] ):
        try:
            inf = open( args['--file'], 'r' )

            # process all entries in the file        
            for line in inf:
               
                # drop comments and blank lines
                line = line.strip()
                if ( line.startswith('#') ):
                    continue
                if ( line == '' ):
                    continue
           
                # 'normalize' the file entries
                line = utils.standardize_dir_sep( line.strip() )
        
                # Process 'add' entries
                cmd = "bob.py " + line
                utils.run_shell( cmd, args['-v'], "ERROR: Build from File failed." )
    
            inf.close()
        
        except Exception as ex:
            exit( "ERROR: Unable to open build list: {}".format(args['--file']) )
        
    

    # The project list is from the command line
    else:
        # Only build the projects that match the pattern
        pattern = args['PATTERN']
        if ( args['here'] ):
            pattern = '*'
    
        jobs = _filter_prj_list( all_prjs, pattern, pkgroot, args['--exclude'], args['--e2'], args['--e3'], args['--p2'], args['--p3'] )
       
        # Run the Jobs serially
        if ( not args['-2'] and not args['-4'] ):
            for p in jobs:
                _build_project(p, args['-v'], args['<build-opts>'], args['--config'], args['--xconfig'], pkgroot )

        # Run the Jobs in PARALLEL
        else:
            max     = len(jobs)
            index   = 0
            busy    = 0
            cpus    = 2 if args['-2'] else 4
            handles = []
            for h in range(0,cpus):
                handles.append( None )

            # Process 'n' directories at a time
            while( index < max or busy > 0 ):
                # Start multiple processes
                for i in range(0, cpus):
                    if ( handles[i] == None and index < max ):
                        j         = jobs[index]
                        index     += 1
                        busy      += 1
                        handles[i] = Process(target=_build_project, args=(j, args['-v'], args['<build-opts>'], args['--config'], args['--xconfig'], pkgroot) )
                        handles[i].start()

                # Poll for processes being done
                for i in range(0, cpus):
                    if ( handles[i] != None and not handles[i].is_alive() ):
                        if ( handles[i].exitcode != 0 ):
                            exit( handles[i].exitcode )
                        handles[i] = None
                        busy      -= 1

                # sleep for 10ms before polling to see if a process has completed
                if ( busy >= cpus ):
                    time.sleep( 0.010 )

    # restore original cwd
    utils.pop_dir()
    
