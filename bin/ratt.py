#!/usr/bin/env python3
"""
Test Tool that automates issuing actions to a Under-Under-Test (UUT)
===============================================================================
usage: ratt [options] --win <executable>...
       ratt [options] --linux <executable>...
       ratt [options] --comport <comnum>
       ratt [options] --serialports
       ratt [options] --nouut
       
Arguments:
   <executable>         UUT is an executable and 'connecting' via stdio
   --comport            Connect to a Window's serial port attached to the UUT 
   <socketnum>          The UUT's TCP socket number to connect to
   <comnum>             Windows serial port number
   --serialports        List available serial ports
   --nouut              Does NOT connect to any UUT

Options:
   --input TSFILE       Launchs ratt in script mode and begins executing the
                        TSFILE script.  TSFILE s assumed to be Test Suite file.
                        A Test Suite file must be a valid python file that 
                        contains a method named main() that takes no arguments 
                        and returns a positive integer.  A return value of zero 
                        indicates that the test suited 'passed' all of its
                        test cases; a value > zero indicates one or more test
                        suite's test cases failed.
   --path1 PATH         Optional directory/search path used when opening
                        script files.  This path is used if the file being
                        opening can not be found in/relative to the CWD
   --path2 PATH         Optional directory/search path used when opening
                        script files.  This path is used if the file being
                        opening can not be found in/relative to --path1
   --path3 PATH         Optional directory/search path used when opening
                        script files.  This path is used if the file being
                        opening can not be found in/relative to --path2

   --baud RATE          Baud rate of the serial port [Default: 115200]
   --parity PARITY      Parity bits, values={even,odd,none} [Default: none]
   --databits DBIT      Number of data bits of the serial port [Default: 8]
   --stopbits SBIT      Number of Stop bits of the serial port [Default: 1]

   --crlf               Sets the UUT newline to '\\r\\n' (instead of '\\n')
   --logfile BASE       Defines the base log file name [Default: ratt.log]
   --log                Enables logging
   -v                   Be verbose
   --vlog               Be verbose in the log (and terse on console ouput)
   -d, --debug          Enables additional output for debugging RATT scripts
   --dlog               Enables debug output in the log file ONLY
   -h, --help           Display help for common options/usage

Examples:
    ; UUT is the executable
    ratt.py --win mypath\\my_utt.exe

    ; UUT is a physical device connected to a Windows PC on COM4
    ratt.py --comport 4

    ; Run mysuite.py script with a UUT connected via TCP (on the same PC on 
    ; port 5002) using Putty's plink 
    ratt.py --input mysuite.ratt --win "E:\\Program Files (x86)\\PuTTY\\plink.exe" -telnet localhost -P 5002

"""

import os
import sys
import time
import utils
import rexpect
import pexpect
import config
from rattlib import output
from rattlib import std
from rattlib import uut
from rattlib import man
from docopt.docopt import docopt
from collections import deque
from time import time, localtime, strftime

VERSION = "0.2"


# ------------------------------------------------------------------------------
def main():

    # Parse command line
    args = docopt(__doc__, version=VERSION, options_first=True)

    # Add the ratt directory to the system path (so module can access the
    # 'utils' package
    sys.path.append(__file__)

    # Enumrate Windoze COM Ports
    if (args['--serialports'] == True):
        ports = utils.get_available_serial_ports(platform="Windows")
        for p in ports:
            print(p)
        sys.exit()

    # Get Newline option
    config.newline = '\r\n' if args['--crlf'] else '\n'
    
    # Open log file (when not disabled)
    logfile = None
    if (args['--log'] == True or args['--vlog'] or args['--dlog']):
        if (args['--vlog']):
            config.g_verbose_logs = True
        if (args['--dlog']):
            config.g_debug_logs = True
        logfile = open(utils.append_current_time(args['--logfile']), "wb")

    ## Created 'Expected' object for a: Windoze executable UUT
    if (args['--win']):
        config.g_uut = rexpect.ExpectWindowsConsole(" ".join(args['<executable>']), logfile)
   
    # Created 'Expected' object for a: Linux/Wsl executable UUT
    elif (args['--linux']):
        config.g_uut = rexpect.ExpectLinuxConsole(" ".join(args['<executable>']), logfile)

    # Created 'Expected' object for a: UUT via a Windoze COM Port
    elif (args['--comport']):
        serial = utils.open_serial_port('com' + args['<comnum>'], timeout=0, baudrate=int(args['--baud']), parity=args['--parity'], stopbits=int(args['--stopbits']), bytesize=int(args['--databits']))
        config.g_uut = rexpect.ExpectSerial(serial, logfile)

    # Create 'Expected' object for a: NO UUT
    elif (args['--nouut']):
        config.g_uut = rexpect.ExpectNullConsole(logfile)

    # Enable output
    output.set_verbose_mode(args['-v'])
    output.set_debug_mode(args['--debug'])
    output.set_output_fd(sys.stdout, logfile)

    # Get script paths
    config.g_script_paths.append('.')
    if (args['--path1'] != None):
        config.g_script_paths.append(args['--path1'])
    if (args['--path2'] != None):
        config.g_script_paths.append(args['--path2'])
    if (args['--path3'] != None):
        config.g_script_paths.append(args['--path3'])

    # Check for batch mode
    if (args['--input'] != None):
        input,result = utils.import_file(args['--input'], config.g_script_paths)
        if (input == None):
            sys.exit(result)

        start_time = time()
        output.writeline("------------ START: Ratt, ver={}. Start time={}".format(VERSION, strftime("%Y-%m-%d_%H.%M.%S",localtime(start_time))))
        output.writeline("------------ RUNNING SUITE CASE: {}".format(result))
        passcode = input.main()
        end_time = time()
        if (passcode != 0):
            output.writeline("------------ TEST SUITE FAILED ({}).".format(passcode))
        else:
            output.writeline("------------ TEST SUITE PASSED.")
        output.writeline("------------ END: End time={}, delta={:.2f} mins".format(strftime("%Y-%m-%d_%H.%M.%S",localtime(end_time)), (end_time - start_time) / 60.0))

        sys.exit(passcode)

    # interactive mode
    else:
        output.writeline("")
        output.writeline("------------ Welcome to Ratt, this is my Kung-Fu and it is strong! ------------")
        output.writeline("                   ver={}. Start time={}".format(VERSION,  utils.append_current_time("", "")))
        output.writeline("")
        exec('from rattlib import *')
        exec('import config')
        
        while(True):
            output.write(">")
            line = sys.stdin.readline().rstrip("\r\n")
            output.writeline(line, log_only=True)
            try:
                exec(line)
            except Exception as e:
                output.writeline(str(e))


# BEGIN
if __name__ == '__main__':
    main()
