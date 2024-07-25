""" Test Case: 

    Verifies the UUT is running

    Pre-Conditions:  The UUT must be running.

    Post-Conditions: Same as the Pre-condition
"""

from rattlib import *
import config

def run():
    """ Entry point for the Test Case
    """

    output.write_entry(__name__)

    passcode = config.g_passed

	
    # Send a help command to the UUT and wait for a response form UUT, i.e. is the UUT alive?
    r = uut.cli( "" ) # Consume the initial prompt at start-up
    r = uut.cli( "help" )
    if r != None:
        passcode = config.g_passed if 'help [* | <cmd>]' in r else config.g_failed
    else:
        output.writeline("ERROR: The UUT is not responding")
        passcode = config.g_failed

    output.write_exit(__name__)
    return passcode