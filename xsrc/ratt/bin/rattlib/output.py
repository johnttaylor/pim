""" Utility functions for outputting messages/information that get sent to 
    stdout as well as the log file
"""
import config
from rattlib import std

g_verbose = False
g_debug = False
g_fdout = None
g_logout = None
g_ident_string = ""
g_ident_timestamp = " "* 16

#00 00:00:00.002:
#------------------------------------------------------------------------------
def write(string, log_only=False, console_only=False ):
    """ Write/appends 'string' to the output stream.  See the truth table
        below for output behaivor.

                log_only    console_only    OUTPUT
                --------    ------------  | ------
                 false        false       | fdout and logout
                 true         false       | logout
                 false        true        | fdout
                 true         true        | NO output
    """
    # Do nothing if the incoming string is empty
    if ( string == None or len(string) == 0 ):
        return

    if (g_fdout != None and log_only == False ):
        g_fdout.write(string)
        g_fdout.flush()
    if (g_logout != None and console_only == False ):
        g_logout.write(bytes(string,'utf-8'))
        g_logout.flush()

#
def writeline(string, log_only=False, console_only=False, prefix_timestamp=False):
    """ Write/appends 'string' to the output stream AND adds a trailine
        newline. Note: 'log_only' must be false to output the string to the 
        console.
    """
    # internal helper function to output a line
    def write_string( string, log_only=False, console_only=False ):
        if (g_fdout != None and log_only == False):
            g_fdout.write(string)
        if (g_logout != None and console_only == False):
            g_logout.write(bytes(string, 'utf-8'))

    # Do nothing if the incoming string is empty
    if ( string == None or len(string) == 0 ):
        return

    string = string.rstrip()        
    if ( prefix_timestamp ):
        ts = std.get_elapsed_timestamp_ms(std.get_elapsed_time_ns())
        lines = string.split(config.g_newline)
        for l in lines:
            write_string( f"{ts}: {l}{config.g_newline}", log_only, console_only )
        write_string( config.g_newline, log_only, console_only )

    else:
        write_string( string+config.g_newline, log_only, console_only )

#
def write_verbose(string, log_only=False, console_only=False):
    """ Same as write(), except the output is only 'enabled' when verbose
        output has been enabled.

        Always writes to the Log file
    """

    if (g_verbose):
        write(string,log_only, console_only)
    else:
        write(string,log_only=True, console_only=False)
#
def writeline_verbose(string, log_only=False, console_only=False, prefix_timestamp=False):
    """ Same as writeline(), except the output is only 'enabled' when verbose
        output has been enabled.

        Always writes to the Log file
    """
    if (g_verbose ):
        writeline(string,log_only, console_only, prefix_timestamp)
    else:
        writeline(string,log_only=True, console_only=False, prefix_timestamp=prefix_timestamp)

#
def write_debug(string, log_only=False, console_only=False):
    """ Same as write(), except the output is only 'enabled' when debug
        output has been enabled.

        Always writes to the Log file
    """

    if (g_debug):
        write(string,log_only, console_only)
    else:
        write(string,log_only=True, console_only=False)

#
def writeline_debug(string, log_only=False, console_only=False, prefix_timestamp=False):
    """ Same as writeline(), except the output is only 'enabled' when debug
        output has been enabled.

        Always writes to the Log file
    """

    if (g_debug):
        writeline(string,log_only, console_only, prefix_timestamp)
    else:
        writeline(string,log_only=True, console_only=False, prefix_timestamp=prefix_timestamp)


#------------------------------------------------------------------------------
def write_entry(module_name, format_string=config.g_entry_banner, indent_prefix=config.indent_prefix):
    """ Writes the formated string - with the specified module/function name.
        The format string should assume that indent string is the first argument,
        the module_name is the second argument

        The write_entry() and write_exit() calls MUST always been done pairs!
    """

    global g_ident_string
    entrystring = format_string.format(g_ident_string, module_name)
    writeline(entrystring)
    g_ident_string += indent_prefix

def write_exit(module_name, format_string=config.g_exit_banner, indent_prefix=config.indent_prefix):
    """ Writes the formated string - with the specified module/funciton name.
        The format string should assume that indent string is the first argument,
        the module_name is the second argument

        The write_entry() and write_exit() calls MUST always been done pairs!
    """

    global g_ident_string
    g_ident_string = g_ident_string[:-len(indent_prefix)]
    exitstring = format_string.format(g_ident_string, module_name)
    writeline(exitstring)


#------------------------------------------------------------------------------
def set_verbose_mode(enabled):
    """ Enables/Disabled verbose output
    """

    global g_verbose
    g_verbose = enabled

#
def set_debug_mode(enabled):
    """ Enables/Disabled debug output
    """

    global g_debug
    g_debug = enabled

#
def set_output_fds(out_fd, log_fd):
    """ Sets/defines the opened file objects for console and log output
    """

    global g_fdout
    global g_logout
    g_fdout  = out_fd
    g_logout = log_fd


