#!/usr/bin/python3
r"""
 
Chuck is an script for invoking a package's automated unit tests
===============================================================================
usage: chuck [options] --match PATTERN [args <testargs>...]
       chuck [options] --file CMDLIST
       chuck [--help]

Arguments:
    --match PATTERN      Performs a recursive search starting from the 'root'
                         directory.  Any executable that is found that matches
                         'PATTERN' is executed.  The optional '<testargs>' are 
                         passed to each instance executed.
    --file CMDLIST       A text file containing a list of test applications to 
                         to run.  The format of file is a list of 'chuck' 
                         commands. Blank lines and line starting with '#' are 
                         skipped.
    args                 When specifing '<testargs>' you must specify the 
                         literaly 'args' (this is a work around on how 'chuck'
                         parses it command line options.
                         
                         
Options:
    --m2 PATTERN         A seconnd 'PATTERN' that will be OR'd with the --match
                         argument.
    --path TESTPATH      The full path to 'root' directory that will be used
                         when searching for test applications. If no path is 
                         specified, the current working directory is used for 
                         root directory.
    --dir DIR            When specified it restricts the directory search 
                         results during the '--match' operation to directory
                         trees that contain the pattern 'DIR' in the names.  
                         [Default: *]   
    --d2 DIR             A second directory condition that will be AND'd with 
                         with the --dir option when restricting the directory
                         search.                    
    --loop N             Repeat the test 'N' times.  [default: 1] 
    -t,--turbo           Runs the tests in parallel
    -v                   Be verbose 
    -h, --help           Displays this information

Examples:
    ; Runs all 'a.exe' executables found under the current working directory
    ; 5 times in row
    chuck.py --match a.exe --loop 5

    ; Runs all a.exe built with the vc12 compiler
    chuck.py --match a.exe --dir vc12

      
"""

import sys
import os
import fnmatch
from multiprocessing import Process
import multiprocessing
import time

sys.path.append( os.path.dirname(__file__) + os.sep + ".." )

from nqbplib import utils
from nqbplib.docopt.docopt import docopt


CHUCK_VERSION = "1.0"

#------------------------------------------------------------------------------
        
        
#------------------------------------------------------------------------------
#
def execute_test( target, ppath, args, arg_string ):
    utils.push_dir( os.path.split(target)[0] )
    reldir = target.replace(ppath,'')[1:]
    exe    = os.path.basename(target)
    cmd    = ".{}{} {}".format( os.path.sep, exe, arg_string )
    for n in range( int(args['--loop']) ):
        print( "= EXECUTING (#{}): {} {}".format( n+1, reldir, arg_string ) )
        result = utils.run_shell2( cmd, args['-v'], "** ERROR: Test failed (#{} {} {} )**".format(n+1, reldir, arg_string) )
    utils.pop_dir

#------------------------------------------------------------------------------
# BEGIN
if __name__ == '__main__':

    # Parse command line
    args = docopt(__doc__, version=CHUCK_VERSION, options_first=True )
    
    # Set quite & verbose modes
    utils.set_verbose_mode( args['-v'] )
    
    # Default the projects/ dir path to the current working directory
    ppath = os.getcwd()
    if ( args['--path'] ):
        ppath = args['--path']
    
    # Set the current directory
    utils.push_dir( ppath )
    
    
    # Trap --file options
    if ( args['--file'] ):
        # Get project list from a file
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
                cmd = "chuck.py " + line
                run_shell2.run_shell( cmd, args['-v'], "ERROR: Test Command from File failed." )
    
            inf.close()
            exit(0)
            
        except Exception as ex:
            exit( "ERROR: Unable to open Test Command list: {}".format(args['--file']) )
    
    
    
    
    # Get superset of projects to build
    if ( args['--match'] ):
        all = utils.walk_file_list( args['--match'], ppath )
        if ( args['--m2'] ):
            all.extend( utils.walk_file_list( args['--m2'], ppath ) )
             
        # Apply directory filter
        tests = []
        for d in all:
            names    = d.split(os.sep)
            filtered = fnmatch.filter(names, args['--dir'] )
            d2       = True
            if ( args['--d2'] and len(fnmatch.filter(names, args['--d2'] )) == 0 ):
                d2 = False
            if ( len(filtered) > 0 and d2 ):
                tests.append(d)
    
        # Build arg string
        arg_string = ' '.join( args['<testargs>'] )
        
        # Run the tests SEQUENTIALLY
        if ( not args['--turbo'] ):
            for t in tests:
                execute_test( t, ppath, args, arg_string )
        
        # Run the tests in PARALLEL
        else:
            max     = len(tests)
            index   = 0
            busy    = 0
            cpus    = multiprocessing.cpu_count()
            handles = []
            for h in range(0,cpus):
                handles.append( None )

            # Process 'n' directories at a time
            while( index < max or busy > 0 ):
                # Start multiple processes
                for i in range(0, cpus):
                    if ( handles[i] == None and index < max ):
                        d         = tests[index]
                        index     += 1
                        busy      += 1
                        handles[i] = Process(target=execute_test, args=(d, ppath, args, arg_string) )
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


        print( "= ALL Test(s) passed." )
 
