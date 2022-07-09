""" Test Case: 

    TBD

    Pre-Conditions:  The Mocked UUT must be just started
                     System Configured for a single stage AC unit
                    
    Post-Conditions: Running in Cooling mode
"""

from rattlib import *
import config

def run():
    """ Entry point for the Test Case
    """

    output.write_entry(__name__)

    passcode = config.g_passed

    # Set up for cooling
    if ( passcode == config.g_passed ):
        uut.cli( 'dm write {name:"userMode", val:"eCOOLING" }', '$$$ ', 2)
        uut.cli( 'dm write {name:"setpoints", val:{cool:69.5} }', '$$$ ', 2)
        uut.cli( 'house enable 97.0', '$$$ ', 2)
       
        # First On Cycle       
        uut.cli( 'tick +300000', '$$$ ', 120)  # Advance 5 mins
        r = uut.cli( 'state', '$$$ ', 4)
        if ( r == None ):
            output.writeline("ERROR (1st ON cycle): UUT timed out")
            passcode = config.g_failed
        else:
            if ( (not "g=ON" in r) or ( not "y1=ON" in r) or ("y2=ON" in r) ):
                output.writeline("ERROR (1st cycle): Expected G/Y1 to be on")
                passcode = config.g_failed
            if ( not "Cycle=eON_CYCLE" in r ):
                output.writeline("ERROR (1st cycle): Expected to be in an ON cycle, but is in OFF cycle")
                passcode = config.g_failed
            if ( not "SystemOn=ON" in r ):
                output.writeline("ERROR (1st cycle): Expected the System to be ON")
                passcode = config.g_failed

        # First Off cycle
        if ( passcode == config.g_passed ):
            uut.cli( 'tick +502000', '$$$ ', 120)  # Advance 8:22 
            r = uut.cli( 'state', '$$$ ', 4)
            if ( r == None ):
                output.writeline("ERROR (1st OFF cycle): UUT timed out")
                passcode = config.g_failed
            else:
                if ( (not "g=off" in r) or ( not "y1=off" in r) ):
                    output.writeline("ERROR (1st cycle): Expected G/Y1 to be OFF")
                    passcode = config.g_failed
                if ( not "Cycle=eOFF_CYCLE" in r ):
                    output.writeline("ERROR (1st cycle): Expected to be in an OFF cycle, but is in ON cycle.")
                    passcode = config.g_failed
                if ( not "SystemOn=ON" in r ):
                    output.writeline("ERROR (1st cycle): Expected the System to be ON")
                    passcode = config.g_failed
       
        # Second On Cycle       
        uut.cli( 'tick +728000', '$$$ ', 120)  # Advance 12:10
        r = uut.cli( 'state', '$$$ ', 4)
        if ( r == None ):
            output.writeline("ERROR (2nd ON cycle): UUT timed out")
            passcode = config.g_failed
        else:
            if ( (not "g=ON" in r) or ( not "y1=ON" in r) or ("y2=ON" in r) ):
                output.writeline("ERROR (2nd cycle): Expected G/Y1 to be on")
                passcode = config.g_failed
            if ( not "Cycle=eON_CYCLE" in r ):
                output.writeline("ERROR (2nd cycle): Expected to be in an ON cycle, but is in OFF cycle")
                passcode = config.g_failed
            if ( not "SystemOn=ON" in r ):
                output.writeline("ERROR (2nd cycle): Expected the System to be ON")
                passcode = config.g_failed

    output.write_exit(__name__)
    return passcode