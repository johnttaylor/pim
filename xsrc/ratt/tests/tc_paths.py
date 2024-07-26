""" Test Case: 
    
    Verifies the '--pathN' search order

    Pre-Conditions:  None

    Post-Conditions: n/a
"""

from rattlib import *
import config

def run():
    """ Entry point for the Test Case
    """

    output.write_entry(__name__)
    passcode = config.g_passed

    # Run Test action -->should be the test action script from patha/
    passcode = std.run("action_path_check", passcode)

    output.write_exit(__name__)
    return passcode