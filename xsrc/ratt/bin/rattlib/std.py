""" Basic/standard functions 
"""

import config
import utils
from rattlib import output


#------------------------------------------------------------------------------
def load( script_name, file_extension=config.g_ratt_file_extension ):
    """ Dynamically loads the specifies script. 'script_name' is a string of 
        script's file name WITHOUT a file extension. 'file_extension' appended
        to 'script_name' to complete the script's file name.
        
        The method returns the 'module object' for the loaded script.
    """
    m,e = utils.import_file( script_name+file_extension, config.g_script_paths )
    if ( m == None ):
        output.writeline( "Error loading script: (). [{}]".format( e ) )
        return None

    return m


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