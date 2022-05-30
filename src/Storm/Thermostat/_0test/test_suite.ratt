""" Test Suite: 

    Test basic duty cycling

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

    # Send a newline to the UUT and wait for a response form UUT, i.e. is the UUT alive?
    r = uut.cli( "help", '$$$ ', 2)
    if ( r == None ):
        output.writeline("ERROR: The UUT is not responding")
        passcode = config.g_failed

    # Configure as single state AC, with a 3 stage furnace
    if ( passcode == config.g_passed ):
        uut.cli( 'dm write { name:"equipmentConfig", val:{iduType:"eFURNACE",numIduHeat:3,oduType:"eAC",numCompStages:1}}', '$$$ ', 2)
        uut.cli( 'dm write { name:"comfortConfig", val:{cmpCool:{cph:"e3CPH",minOn:300,minOff:250},indoorHeat:{cph:"e3CPH",minOn:300,minOff:250}}}', '$$$ ', 2)

    # Test Basic Cooling
    if ( passcode == config.g_passed ):
         tc = std.load("tc_cooling_basic_cycling")
         passcode = config.g_failed if tc == None else tc.run()

    # Test Basic Heating
    if ( passcode == config.g_passed ):
         tc = std.load("tc_idheating_basic_cycling")
         passcode = config.g_failed if tc == None else tc.run()

    # Cleanly Exit the UUT
    uut.cli("bye app")

    output.write_exit( __name__ )
    return passcode if passcode != None else config.g_failed