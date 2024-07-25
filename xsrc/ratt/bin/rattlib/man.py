""" Provides interactive help for test scripts
"""

import config
import utils
from rattlib import output
import os
import pydoc
from inspect import getmembers, isfunction, signature

#------------------------------------------------------------------------------
def man( script_name, file_extension=config.g_ratt_file_extension, list_all_funcs=False ):
    """ Displays help for the specified script name and its 'run()' method 
        (which is assumed to be the script's primary/entry function).

        If 'list_all_funcs' is True, the all of the methods of script are listed
        out (instead of the assumed 'run()' method).
    """
    
    # Look-up/load the script
    filename = script_name+file_extension
    m,e = utils.import_file( filename, config.g_script_paths )
    if ( m == None ):
        output.writeline( "ERROR: {} was not found. [{}]".format( script_name+file_extension, e ) )
        return

    # get the module's location
    modname = os.path.splitext(os.path.split(filename)[1])[0]
    modpath = config.g_utils_import_paths[modname]

    # get module help
    indent  = "    "
    modhelp = indent + pydoc.getdoc(m).replace(config.g_newline, config.g_newline+indent)
    try:
        runhelp = indent + pydoc.getdoc(m.run).replace(config.g_newline, config.g_newline+indent)
    except:
        runhelp = None

    # Display the help text
    output.writeline( "" )
    output.writeline( "MODULE NAME:" )
    output.writeline( indent + modname ) 
    output.writeline( "" )
    output.writeline( "LOCATION:" )
    output.writeline( indent + modpath ) 
    output.writeline( "" )
    output.writeline( "DESCRIPTION:" )
    output.writeline( modhelp )
    output.writeline( "" )
    if ( runhelp != None and list_all_funcs == False):
        output.writeline( "RUN() FUNCTION:")
        output.writeline( runhelp )
        output.writeline( "" )
    else:
        output.writeline( "FUNCTIONS:" )
        funcs = getmembers(m,isfunction)
        for f in funcs:
            output.writeline(f'{indent}{f[0]}{signature(f[1])}')
            runhelp = indent + indent + pydoc.getdoc(f[1]).replace(config.g_newline, config.g_newline+indent+indent)
            output.writeline( runhelp )
            output.writeline( "" )


#


#
def list( verbose = False, file_extension=config.g_ratt_file_extension ):
    """ List all of the available test scripts and if 'verbose' is set to 
        True, the path location of each test script
    """    

    # Get the list of files
    files = utils.find_files( file_extension, config.g_script_paths )

    # Display the files (in sorted order)
    if ( len(files) == 0 ):
        output.writeline( "No test scripts available.")
    else:
        for key, value in sorted(files.items()):
            if ( verbose ):
                output.writeline( "{:<40}  {}".format( key, value ))
            else:
                output.writeline( "{}".format( key ) )
