""" Test Case: 

    TBD

    Pre-Conditions:  The Mocked UUT must be running and was previously running 
                     in the cooling mode.
                     System Configured for 3 stage Furnace (with an AC unit)

    Post-Conditions: Running in ID Heating mode
"""

from rattlib import *
import config

def run():
    """ Entry point for the Test Case
    """

    output.write_entry(__name__)

    passcode = config.g_passed

    # Set up for heating
    if ( passcode == config.g_passed ):
        uut.cli( 'dm write {name:"userMode", val:"eHEATING" }', '$$$ ', 2)
        uut.cli( 'dm write {name:"setpoints", val:{heat:68.15,cool:79.0} }', '$$$ ', 2)
        uut.cli( 'house disable 70.0', '$$$ ', 2)
        uut.cli( 'house enable 20.0', '$$$ ', 2)
       
        # First On Cycle       
        uut.cli( 'tick +20000', '$$$ ', 120)  # Advance 0:20
        r = uut.cli( 'state', '$$$ ', 4)
        if ( r == None ):
            output.writeline("ERROR (1st ON cycle): UUT timed out")
            passcode = config.g_failed
        else:
            if ( (not "g=off" in r) or ( not "w1=ON" in r) or ("w2=ON" in r) or ("y1=ON" in r)):
                output.writeline("ERROR (1st cycle): Expected G OFF, W1 on")
                passcode = config.g_failed
            if ( not "Cycle=eON_CYCLE" in r ):
                output.writeline("ERROR (1st cycle): Expected to be in an ON cycle to be on")
                passcode = config.g_failed
            if ( not "SystemOn=ON" in r ):
                output.writeline("ERROR (1st cycle): Expected the System to be ON")
                passcode = config.g_failed

        # First Off cycle
        if ( passcode == config.g_passed ):
            uut.cli( 'tick +2569000', '$$$ ', 240)  
            r = uut.cli( 'state', '$$$ ', 4)
            if ( r == None ):
                output.writeline("ERROR (1st OFF cycle): UUT timed out")
                passcode = config.g_failed
            else:
                if ( (not "g=off" in r) or ( not "y1=off" in r) or ( not "w1=ON" in r) or ( not 'w2=off' in r) ):
                    output.writeline("ERROR (1st cycle): Expected ALL relays to be OFF")
                    passcode = config.g_failed
                if ( not "Cycle=eOFF_CYCLE" in r ):
                    output.writeline("ERROR (1st cycle): Expected to be in an OFF cycle to be on")
                    passcode = config.g_failed
                if ( not "SystemOn=ON" in r ):
                    output.writeline("ERROR (1st cycle): Expected the System to be ON")
                    passcode = config.g_failed
       
        # Second On Cycle       
        uut.cli( 'tick +783000', '$$$ ', 180)  
        r = uut.cli( 'state', '$$$ ', 4)
        if ( r == None ):
            output.writeline("ERROR (2nd ON cycle): UUT timed out")
            passcode = config.g_failed
        else:
            if ( (not "g=off" in r) or ( not "w1=ON" in r) or (not "w2=ON" in r) or ("y1=ON" in r)):
                output.writeline("ERROR (2nd cycle): Expected G OFF, W1 on, W2 on")
                passcode = config.g_failed
            if ( not "Cycle=eON_CYCLE" in r ):
                output.writeline("ERROR (2nd cycle): Expected to be in an ON cycle to be on")
                passcode = config.g_failed
            if ( not "SystemOn=ON" in r ):
                output.writeline("ERROR (2nd cycle): Expected the System to be ON")
                passcode = config.g_failed

    output.write_exit(__name__)
    return passcode