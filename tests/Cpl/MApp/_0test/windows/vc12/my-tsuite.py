""" Test Suite: 

    Example: Blah....

    Pre-Conditions:  Blah...

    Post-Conditions: Blah...
"""

from rattlib import *
import config
import time

def main():
    """ Entry point for the Test Suite
    """
    output.write_entry( __name__ )
    passcode = config.g_passed

    # Test cases
    passcode = std.run('tc_uut_alive', passcode)
    passcode = std.run('tc_uut', passcode)
    
    # Exit the UUT
    uut.cli("bye app")
    time.sleep( 2 )
    
    output.write_exit( __name__ )
    return passcode
