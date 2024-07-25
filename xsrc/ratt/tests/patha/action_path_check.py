""" Test Action: 

    This file is the 'expected' file to be loaded
"""

from rattlib import *
import config

def run():
    """ Entry point for the Test Action
    """

    output.write_entry(__name__)
    passcode = config.g_passed

    output.writeline_verbose( "{} in patha/ executated".format( __name__ ))

    output.write_exit(__name__)
    return passcode
