""" Test Action: 

    This file is SHOULD be loaded (for the test case to pass)
"""

from rattlib import *
import config

def run():
    """ Entry point for the Test Action
    """

    output.write_entry(__name__)
    passcode = config.g_failed

    output.writeline( "ERROR: {} in pathb/ executated".format( __name__ ))

    output.write_exit(__name__)
    return passcode
