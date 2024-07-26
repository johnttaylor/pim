""" Test Case: 

    Exercises the happy paths for Cpl::MApp namespace

    Pre-Conditions:  The UUT must be running with no MApp instances running.

    Post-Conditions: Same as the Pre-condition
"""

from rattlib import *
import config

def run():
    """ Entry point for the Test Case
    """

    output.write_entry(__name__)

    passcode = config.g_passed

	# Verify No instances running
    r = uut.cli("mapp")
    if r == None:
        output.writeline("ERROR: The UUT is not responding")
        return config.g_failed
    if ( not "NO MApps are running" in r ):
        return config.g_failed

    # Verify Available list
    r = uut.cli("mapp ls")
    if r == None:
        output.writeline("ERROR: The UUT is not responding")
        return config.g_failed
    if not ("t1" in r and "t2" in r and "t3" in r ):
        return config.g_failed

    # Start the t3 instance
    r = uut.cli("mapp start t3")
    if r == None:
        output.writeline("ERROR: The UUT is not responding")
        return config.g_failed
    if not ("Starting: t3" in r and "t3: <data invalid>" in r):
        return config.g_failed

    # Start the t1 instance
    r = uut.cli("mapp start t1")
    if r == None:
        output.writeline("ERROR: The UUT is not responding")
        return config.g_failed
    if not ("Starting: t1" in r and "avg:" in r and "max:" in r):
        return config.g_failed

    output.write_exit(__name__)
    return passcode