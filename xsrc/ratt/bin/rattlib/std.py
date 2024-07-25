""" Basic/standard functions 
"""

import config
import utils
import time
from rattlib import output

start_time_ns = int(time.time_ns())

#------------------------------------------------------------------------------
def load( script_name, file_extension=config.g_ratt_file_extension ):
    """ Dynamically loads the specifies script. 'script_name' is a string of 
        script's file name WITHOUT a file extension. 'file_extension' appended
        to 'script_name' to complete the script's file name.
        
        The same script can be 'loaded' multiple times, however only one 
        instance of the module is imported into python.

        The method returns the 'module object' for the loaded script.
    """
    m,e = utils.import_file( script_name+file_extension, config.g_script_paths )
    if ( m == None ):
        output.writeline( "Error loading script: (). [{}]".format( e ) )
        return None
    return m

def run( script_name, current_passcode = config.g_passed, file_extension=config.g_ratt_file_extension, script_args=(), script_kwargs={} ):
    """ Dynamically loads and then runs the script.  The method returns the
        pass/fail result of the script. 
        
        If 'current_passcode' is config.g_failed, nothing is done and 
        config.g_failed is returned.
    """
    if ( current_passcode != config.g_passed ):
        output.writeline_verbose( f"INFO: Skipping running {script_name} because passcode is 'failed'" )
        return config.g_failed

    tc = load(script_name, file_extension)
    if ( tc == None ):
        return config.g_failed 
    
    r = config.g_failed
    try:
        output.writeline_verbose( f"INFO: running {script_name}" )
        r = tc.run( *script_args, **script_kwargs )

    except Exception as e:
        output.writeline(f'Error: Script {script_name} failed to run. {e}')
        
    return r

#
def shell( cmd ):
    """ Executes the specified command. 'cmd' is string of that contains the 
        command with any optional arguments.
        
        The method returns a tuple with the exit code and output from the 
        command.
    """    
    exitcode, result = utils.run_shell( cmd )
    output.writeline_verbose( result )
    return exitcode, result

#------------------------------------------------------------------------------
def get_start_time_ns():
    """ Returns and caches the starting time in nano seconds
    """
    global start_time_ns
    start_time_ns = int(time.time_ns())
    return start_time_ns

def get_elapsed_time_ns():
    """ Returns the elapsed time in nano seconds
    """
    end_time = time.time_ns() 
    return int(end_time - start_time_ns) 

def get_elapsed_timestamp_ms( elasped_time_ns ):
    """ Returns a string that represents the elapsed host time in milliseconds

        Format: dd hh:mm:ss.nnn
    """
    hh, t   = divmod( elasped_time_ns, (60*60) * 1000000000 )
    mm, t   = divmod( t, 60 * 1000000000 )
    ss, t   = divmod( t, 1000000000 )
    msec, t = divmod( t, 1000000 )
    dd, hh  = divmod( hh, 24 )
    return f"{dd:02d} {hh:02d}:{mm:02d}:{ss:02d}.{msec:03d}"
