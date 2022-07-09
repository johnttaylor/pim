#!/usr/bin/python3
"""
 
avr-dude is a python wrapper to invoke the AVRDUDE utility
===============================================================================
usage: avr-dude [options] <hexfile>
       avr-dude [options]

Arguments:
  hexfile           Path/name of the .HEX file to program into the target AVR.
                    If no hexfile is specified, it assumes the file: 
                    _arduino\*.hex
  -p COM            Specifies which serial/COM port to use. Overrides the 
                    default setting in the global config file.
  -b BAUD           Specifies the baud rate of the serial ports.  Overrides
                    the default setting in the global config file.
  -c CONFIG         Specifies which config file to use.  The default is to 
                    use the global config file in under the %ARDUINO_TOOLS%
                    directory.
  -m CPU            Specifies the AVR microcontroller type [default: atmega328p]
  --nohex           Does not attempt to program a HEX file.
  --extra ARGS      When specified, the 'ARGS' token is included in the argument
                    list when invoking avrdude.  Note: use quotes around ARGS
  -v                Be verbose. 
  -h, --help        Display help for common options/usage.
    


NOTE:
  1. Assume that environment variable ARDUINO_TOOLS is set to the installed
     Arduino development system.
  2. AVRDUDE is a utility to download/upload/manipulate the ROM and EEPROM 
     contents of AVR microcontrollers using the in-system programming technique 
     (ISP). See http://www.nongnu.org/avrdude/

"""

from __future__ import absolute_import
from __future__ import print_function
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
def get_default_hexfile_name( default_dir='_arduino'):
    hexfile = None

    # Make sure the default directory exists...
    if ( os.path.isdir(default_dir) ):
        files = utils.dir_list_filter_by_ext(default_dir, ['hex'])

        # set the default HEX file is ONE and ONLY ONE Hex file exists in the default directory
        if ( len(files) == 1 ):
            hexfile = os.path.join(default_dir, files[0] )

    return hexfile

#------------------------------------------------------------------------------
def run( argv ):

    # Parse command line
    args = docopt(__doc__, version="0.1")

    # Get environment variable for where the arduino tools are located
    ARDUINO_TOOLS = os.environ.get( 'ARDUINO_TOOLS' )
    if ( ARDUINO_TOOLS == None ):
        print("The environment variable - ARDUINO_TOOLS - is NOT set.")
        sys.exit(1)

    # Default tool stuff
    avrdude    = os.path.join(ARDUINO_TOOLS, 'hardware', 'tools', 'avr', 'bin', 'avrdude')
    dudeconfig = os.path.join(ARDUINO_TOOLS, 'hardware', 'tools', 'avr', 'etc', 'avrdude.conf')
            
    # Get hex file to program
    hexfile = get_default_hexfile_name();
    if ( args['<hexfile>'] ):
        hexfile = args['<hexfile>'] 
    if ( hexfile == None ):
        print("No HEX file was specified OR multiple HEX files exist in the default directory")
        sys.exit(1)

    # build options....
    verbose = ' -v' if args['-v'] else ''
    comport = '' if not args['-p'] else ' -P '+args['-p']
    baud    = '' if not args['-b'] else ' -b '+args['-b']
    config  = ' -C ' + dudeconfig if not args['-c'] else ' -C "'+args['-c'] + '"'
    mcu     = ' -p ' + args['-m']
    target  = '' if args['--nohex'] else '-U flash:w:{}:i'.format(hexfile)
    extra   = '' if not args['--extra'] else ' ' + args['--extra']
    options = config + comport + verbose + mcu + ' -c arduino -D ' + target + extra
    
    # Run AVRDUDE
    printer = Printer()
    cmd = avrdude + ' ' + options
    if ( args['-v'] ):
        print(cmd)
    if (utils.run_shell(printer, cmd, False) ):
        print()
        print('** A FAILURE occurred while attempting to run avrdude')
        print()
        sys.exit(1)

    

