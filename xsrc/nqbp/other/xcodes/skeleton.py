"""
 
Generates skeleton C++ header/cpp files
===============================================================================
usage: xcode [common-opts] skeleton [options] <class>


Arguments:
    <class>         Class/File name with NO file extension.  This argument IS
                    case sensistive.
                        
Options:
    --header        Only generate a header file
    --cpp           Only generate a CPP file
    -h, --help      Display help for this command

Common Options:
    See 'xcode --help'


Notes:
        
"""
from nqbplib.docopt.docopt import docopt
from nqbplib import utils
from other import utils2
import os, sys


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'skeleton', 'Generate skeleton C++ header/cpp files' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    global WRKSPACE_ROOT
    global PKG_ROOT
    
    # Parse command line
    args = docopt(__doc__, argv=cmd_argv)
    
    # Generate header file
    if ( not args['--cpp'] ):
        ofname     = args['<class>']+".h"
        cmd        = 'f4 header_skeleton.f4t -z --global //colony.core/resources/f4 -v {} -o {} map {} {}'.format( common_args['-g'], ofname, args['<class>'], common_args['-n'] )
        rc, output = utils2.run_shell( cmd )
        utils2.verbose( cmd, output, common_args )
        if ( rc != 0 ):
            sys.exit( "ERROR: Failed to create output: {}.  Rerun using the '-v' for additional error information".format(ofname) )
        
    # Generate CPP file
    if ( not args['--header'] ):
        ofname     = args['<class>']+".cpp"
        cmd        = 'f4 cpp_skeleton.f4t -z --global //colony.core/resources/f4 -v {} -o {} map {} {}'.format( common_args['-g'], ofname, args['<class>'], common_args['-n'] )
        rc, output = utils2.run_shell( cmd )
        utils2.verbose( cmd, output, common_args )
        if ( rc != 0 ):
            sys.exit( "ERROR: Failed to create output: {}.  Rerun using the '-v' for additional error information".format(ofname)  )
    
