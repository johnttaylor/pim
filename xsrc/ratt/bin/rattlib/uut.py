""" Functions to Access the UUT
"""

from rattlib import output
import sys
import config
import pexpect

prompt_string = "$"
#------------------------------------------------------------------------------
#
def setprompt( new_prompt ):
    """ This method sets the prompt/marker used when waiting for a response to 
        cli() command.  The method returns the previous prompt value.
    """
    global prompt_string;
    prev          = prompt_string
    prompt_string = new_prompt
    return prev


def cli( cli_command, wait=True, max_wait_sec=10, regex_match=False, append_newline=True, strip_prompt=True ):
    """ Sends the specified cli_command/content to the UUT.  If 'wait' is 
        False then the call does not wait for a response; else it will wait up to
        'max_wait_sec'. When append_newline is true a 'newline' is append to 
        the 'cli_command'.  The method returns None when 'wait' is false; else
        it returns the results of the wait-for-prompt action.

        The prompt string/value to wait for is set by the setprompt() method.
    """
    # When waiting for a trailing prompt - clear the input buffer so as to NOT
    # match on a stale/previous prompt
    before_cmd = ""
    if ( wait ):
        before_cmd = clear()

    # Send the command to the UUT
    cmd = cli_command + config.g_newline if append_newline else cli_command
    output.writeline_verbose( cli_command, prefix_timestamp=True ) 
    config.g_uut.sendline( cmd  )
    config.g_uut.flush()
    
    # Optionally wait for the prompt
    if ( wait ):
        result = waitfor( max_wait_sec, prompt_string, regex_match )
        if ( result != None ):
            result = before_cmd + result

            # Remove the prompt from the returned result
            if ( strip_prompt ):
                result = result.rstrip(prompt_string)

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
        try:
            d = config.g_uut.read_nonblocking(size=1024,timeout=0)
            max_retries -= 1
            if ( d != None and d != ''):
                flushed_stuff += str(d,'utf-8')
            else:
                output.writeline_verbose( flushed_stuff, prefix_timestamp=True )
                return flushed_stuff
        except pexpect.exceptions.TIMEOUT:
            # no data -->so exit loop
            break

        except pexpect.exceptions.EOF:
            # Indication that the UUT is no longer running
            sys.exit("ERROR: UUT is no longer executing")

    # Make sure we return the 'flushed stuff' if the pexpect buffer is
    # constantly being filled up
    output.writeline_verbose( flushed_stuff, prefix_timestamp=True )
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
        output.writeline_verbose( "Waiting up to {} seconds for the string: [{}]".format( timeout_sec, needle ), prefix_timestamp=True )
        idx = config.g_uut.expect_str( [needle, pexpect.EOF, pexpect.TIMEOUT], timeout_sec )

    # Regex Match
    elif ( tokens[1] == 'REGEX' ):
        output.writeline_verbose( "Waiting up to {} seconds for the regex: [{}]".format( timeout_sec, needle ), prefix_timestamp=True )
        idx = config.g_uut.expect( [needle, pexpect.EOF, pexpect.TIMEOUT], timeout_sec )

    before = config.g_uut.get_before()
    if ( not isinstance(before, str) ):
        before = ' '

    after  = config.g_uut.get_after()
    if ( not isinstance(after,str) ):
        after = ''

    # For some reason - the 'before' string always has leading space
    if ( len(before) > 0 and before[0] == ' ' ):
        before = before[1:]

    result = before+after
    output.writeline_verbose( result, prefix_timestamp=True ) 
    return result if idx == 0 else None
