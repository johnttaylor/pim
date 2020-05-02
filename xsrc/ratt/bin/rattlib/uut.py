""" Functions to Access the UUT
"""

from rattlib import output
import time
import config
import pexpect

#------------------------------------------------------------------------------
#
def cli( cli_command, prompt=None, max_wait_sec=2, regex_match=False, append_newline=True ):
    """ Sends the specified cli_command/content to the UUT.  If 'prompt' is 
        None then the call does not wait for a response; else it will wait up to
        'max_wait_sec'. When append_newline is true a 'newline' is append to 
        the 'cli_command'.  The method returns None when 'prompt' is None; else
        it returns the results of the wait-for-prompt action.
    """
    # When waiting for a trailing prompt - clear the input buffer so as to NOT
    # match on a stale/previous prompt
    before_cmd = ""
    if ( prompt != None ):
        before_cmd = clear()

    # Send the command to the UUT
    cmd = cli_command + config.g_newline if append_newline else cli_command
    config.g_uut.sendline( cmd  )
    config.g_uut.flush()
    
    # Optionally wait for the prompt
    if ( prompt != None ):
        result = waitfor( max_wait_sec, prompt, regex_match )
        if ( result != None ):
            result = before_cmd + result
        return result
    else:
        return None

#
def clear():
    """ This method clears and returns the pexpect buffer.  The less content
        in the pexpect buffer, the faster the 'waitfor()' performance.  
    """

    # Flush the UUT buffer
    max_retries = 1024
    flushed_stuff = ''
    while( max_retries ):
        d = config.g_uut.read_nonblocking(size=1024)
        max_retries -= 1
        if ( d != None and d != ''):
            flushed_stuff += str(d,'utf-8')
        else:
            output.writeline_verbose( flushed_stuff )
            return flushed_stuff

    # Make sure we return the 'flushed stuff' if the pexpect buffer is
    # constantly being filled up
    output.writeline_verbose( flushed_stuff )
    return flushed_stuff


#
def waitfor( timeout_sec, needle, regex_match=False ):
    """ Waits for the UUT to respond with 'needle'.  When 'regex' is true, then
        'needle' is assumed to be regular expression match; else a simple string
        match is used. The 'timeout_sec' is the maximum time in seconds to wait 
        for the matching response from the UUT.  If 'needle' was found, then the 
        contents of the pexpect buffer (up to and including 'needle') is
        returned; else if 'needle' is not found then None is returned
    """
    
    # Housekeeping
    idx    = -1
    result = ""

    # String match
    if ( regex_match == False ):
        output.writeline_verbose( "Waiting up to {} seconds for the string: [{}]".format( timeout_sec, needle ) )
        idx = config.g_uut.expect_str( [needle, pexpect.EOF, pexpect.TIMEOUT], timeout_sec )

    # Regex Match
    elif ( tokens[1] == 'REGEX' ):
        output.writeline_verbose( "Waiting up to {} seconds for the regex: [{}]".format( timeout_sec, needle ) )
        idx = config.g_uut.expect( [needle, pexpect.EOF, pexpect.TIMEOUT], timeout_sec )

    result = str(config.g_uut.get_before())+str(config.g_uut.get_after()) 
    output.writeline_verbose( result )
    return result if idx == 0 else None
