"""
 
Generates Cpl::Itc Request (and Response) Messages
===============================================================================
usage: xcode [common-opts] itcrequest [options] <class> MSGS...


Arguments:
    <class>         Class/File name with NO file extension.  This argument IS
                    case sensistive.
    
    MSGS            Base name for each message in the ITC server.
    
                         
Options:
    -r RSPCLASS     Enables generating companion ITC Response class for the
                    ITC Service. The Response class will be name 'RSPCLASS'
                        
    -h, --help          Display help for this command

Common Options:
    See 'xcode --help'


Examples:
    xcode itrequest AppleRequest Foo Bar  
    xcode itrequest -r AppleResponse AppleRequest Foo Bar
        
"""
from nqbplib.docopt.docopt import docopt
from nqbplib import utils
from other import utils2
import os, sys

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print(("{:<13}{}".format( 'itcrequest', 'Generates Cpl::Itc Request (and Response) Messages' )))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    # Parse command line
    args = docopt(__doc__, argv=cmd_argv)
    
    # Get MSGS as comma seperated string
    msgs = ','.join( args['MSGS'] )
    
    # Get response class name
    rclass  = 'n/a'
    incResp = '0'
    if ( args['-r'] ):
        rclass  = args['-r']
        incResp = '1'
        
    # Create header file
    ofname     = args['<class>']+".h"
    cmd        = 'f4 itc_request.f4t -z --global //colony.core/resources/f4 -v {} -o {} map {} {} {} {} {}'.format( common_args['-g'], ofname, args['<class>'], rclass, common_args['-n'], incResp, msgs )
    rc, output = utils2.run_shell( cmd )
    utils2.verbose( cmd, output, common_args )
    if ( rc != 0 ):
        sys.exit( "ERROR: Failed to create output: {}.  Rerun using the '-v' for additional error information".format(ofname) )
        
    

#class               // Class name
#namespaces          // Namespaces (list)
#incResponse         // Flag/option to include/not include response
#messages            // Base name of each message (list)
