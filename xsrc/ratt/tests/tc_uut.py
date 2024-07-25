""" Test Case: 

    Verifies functionally in the rattlib.uut module

    Pre-Conditions:  The Mocked UUT must be running.

    Post-Conditions: Same as the Pre-condition
"""

from rattlib import *
import config

def run():
    """ Entry point for the Test Case
    """

    output.write_entry(__name__)

    passcode = config.g_passed

    # Send a newline to the UUT and wait for a response form UUT, i.e. is the UUT alive?
    r = uut.cli( "" )
    if r != None:
        # execute the test action - with a 2 second timeout
        passcode = std.run("action_uut", script_kwargs={'timeout':2} )
    
    else:
        output.writeline("ERROR: The UUT is not responding")
        passcode = config.g_failed

    output.write_exit(__name__)
    return passcode