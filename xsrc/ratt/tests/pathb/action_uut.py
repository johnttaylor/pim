""" Test Action: 

    Configures the Mock UUT to generate two spam messsages and
    waits for a specific message.
"""

from rattlib import *
import config

def run( timeout ):
    """ Entry function.

        The 'timeout' arugment specifies how long to wait, in seconds, for the 
        UUT to respond with the desired output.

        Returns g_passed if succesful; else g_failed is returned
    """ 

    output.write_entry(__name__)
    passcode = config.g_passed

    uut.cli( '.spam 2 200 "Spam message 2";', max_wait_sec=timeout)
    uut.cli( '.spam 1 1000 "Spam message 1";', max_wait_sec=timeout )
    r = uut.waitfor( timeout, "message 1")
    if r == None:
        output.writeline( "uut.waitfor(): FAILED to find 'message 1'")
        passcode = config.g_failed

    elif "spam 1" in r:
        output.writeline( f"uut.waitfor(): MATCHED the WRONG 'message1', from: [{r}] " )
        passcode = config.g_failed

    else:    
        output.writeline_verbose( f"uut.waitfor(): MATCHED 'message1', from: [{r}] " )

    # Disable the spam messages, i.e. leave the UUT in the same state as found it
    uut.cli( '.spam 1 0 "turnoff";', wait=False );
    uut.cli( '.spam 2 0 "turnoff";', wait=False );

    output.write_exit(__name__)
    return passcode
