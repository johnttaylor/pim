#!/usr/bin/python3
"""
 
nrf-util is a python wrapper to invoke the nrfutil utility
===============================================================================
usage: nrf-util [options] <zipfile>
       nrf-util [options]

Arguments:
  zipfile       Path/name of the .ZIP file to program into the target NRF52.
                If no zipfile is specified, it assumes the file: _arduino\*.zip
  -p COM        Specifies which serial/COM port to use. [Default: COM4]
  -b BAUD       Specifies the baud rate of the serial ports.  [Default: 115200]
                the default setting in the global config file.
  --nozip       Does not attempt to program a ZIP file.
  --extra ARGS  When specified, the 'ARGS' token is included in the argument
                list when invoking nrf-util.  Note: use quotes around ARGS
  -v            Be verbose. 
  -h, --help    Display help for common options/usage.
    


NOTE:
  1. Assume that environment variable ARDUINO_TOOLS, ARDUINO_BSP_VER and
     ARDUINO_NRF_UTIL_VER are set to the installed Arduino development system.
  2. nrfutil is a utility provided by Nordic Semiconductor

"""

import sys
import os

from nqbplib.docopt.docopt import docopt
from nqbplib import utils



#------------------------------------------------------------------------------
# Create a dumb downed printer class to be compatible with utils.run_shell() method
class Printer:
    def __init__(self):
        pass

    def output(self,line):
        print(line)


#------------------------------------------------------------------------------
def get_default_zipfile_name( default_dir='_arduino'):
    zipfile = None

    # Make sure the default directory exists...
    if ( os.path.isdir(default_dir) ):
        files = utils.dir_list_filter_by_ext(default_dir, ['zip'])

        # set the default HEX file is ONE and ONLY ONE Hex file exists in the default directory
        if ( len(files) == 1 ):
            zipfile = os.path.join(default_dir, files[0] )

    return zipfile

#------------------------------------------------------------------------------
def run( argv ):

    # Parse command line
    args = docopt(__doc__, version="0.1")

    # Get environment variable for where the arduino tools are located
    ARDUINO_TOOLS = os.environ.get( 'ARDUINO_TOOLS' )
    if ( ARDUINO_TOOLS == None ):
        print("The environment variable - ARDUINO_TOOLS - is NOT set.")
        sys.exit(1)
    ARDUINO_BSP_VER = os.environ.get( 'ARDUINO_BSP_VER' )
    if ( ARDUINO_BSP_VER == None ):
        print("The environment variable - ARDUINO_BSP_VER - is NOT set.")
        sys.exit(1)
    ARDUINO_NRF_UTIL_VER = os.environ.get( 'ARDUINO_NRF_UTIL_VER' )
    if ( ARDUINO_NRF_UTIL_VER == None ):
        print("The environment variable - ARDUINO_NRF_UTIL_VER - is NOT set.")
        sys.exit(1)

    # Default tool stuff
    nrfutil    = os.path.join(ARDUINO_TOOLS, 'hardware', 'nrf52', ARDUINO_BSP_VER, 'tools', ARDUINO_NRF_UTIL_VER, 'binaries', 'win32', 'nrfutil' )
            
    # Get hex file to program
    zipfile = get_default_zipfile_name();
    if ( args['<zipfile>'] ):
        zipfile = args['<zipfile>'] 
    if ( zipfile == None ):
        print("No ZIP file was specified OR multiple ZIP files exist in the default directory")
        sys.exit(1)

    # build options....
    verbose = ' --verbose ' if args['-v'] else ''
    comport = ' --port '+args['-p']
    baud    = ' -b '+args['-b']
    command = ''
    if (  not args['--nozip'] ):
        target  = ' dfu serial -pkg {}'.format(zipfile)
        command = target + comport + baud
    extra   = '' if not args['--extra'] else ' ' + args['--extra']
    options = verbose + command + extra
    
    # Run NRFUTIL
    printer = Printer()
    cmd = nrfutil + ' ' + options
    if ( args['-v'] ):
        print(cmd)
    if (utils.run_shell(printer, cmd, False) ):
        print()
        print('** A FAILURE occurred while attempting to run nrfutil')
        print()
        sys.exit(1)

    

