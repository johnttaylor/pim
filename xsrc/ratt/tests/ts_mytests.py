""" Test Suite: 

    Verifies the feature content of the Ratt tool and its rattlib package.

    Pre-Conditions:  The Mocked UUT must be running.

    Post-Conditions: The Mocked UUT will be exited
"""

from rattlib import *
import config

def main():
    """ Entry point for the Test Suite
    """
    output.write_entry( __name__ )
    passcode = config.g_passed
    uut.setprompt("$>")

    # Test rattlib.uut
    passcode = std.run("tc_uut", passcode )
    passcode = std.run("tc_paths", passcode )
    
    # Cleanly Exit the UUT
    uut.cli(".exit;")

    output.write_exit( __name__ )
    return passcode