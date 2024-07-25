""" Utilities, common functions, etc.

"""

from rattlib import *
import config
import json
import time
import sys


    
#------------------------------------------------------
    
def read_and_check_state( model_point, expected_state, database='dm', silent=True ):
    """ Reads the specified model point and checks if it matches the 
        'expected_state'. This method only works with 'enum'
        valued model points. 

        Returns True if actual and expected state matches; else
        False is returned.

        The default behavior is to output an warning message when
        the method return False.  The 'silent' flag can be used
        to silence the warning message.      
    """
    # Get the current state of the model point
    r = uut.cli( f"{database} read {model_point}" )
    if ( r == None ):
        output.writeline("ERROR: The UUT is not responding")
        return False
    
    try:
        startObj = r.index('{')
        r = r[startObj:]
        o = json.loads(r)
    except Exception as e:
        output.writeline( f"ERROR: bad json object [{e}]" )
        return False
        
    try:
        state = o["val"]
    except Exception as e:
        output.writeline( f"ERROR: missing 'val' keyword [{e}]" )
        return False
        
    if ( state != expected_state ):
        if ( not silent ):
            output.writeline(f"WARNING: The {database}.{model_point} state - {state} - is NOT in the expected - {expected_state} - state")
        return False
    return True

#------------------------------------------------------
def read_and_compare( model_point, expected_value, compare='==', database='dm', silent=True ):
    """ Reads the specified model point and compares it against
        the 'expected_value'.  This method only works with numeric
        valued model points. 
        
        Compare options:
        '==', '<=', '<', '>', '>=', '!='
        
        Returns True if actual and expected value 'compare'; else
        False is returned.

        The default behavior is to output an warning message when
        the method return False.  The 'silent' flag can be used
        to silence the warning message.    
    """

    # Get the current state of the Stroke component
    r = uut.cli( f"{database} read {model_point}" )
    if ( r == None ):
        output.writeline("ERROR: The UUT is not responding")
        return False
    
    try:
        startObj = r.index('{')
        r = r[startObj:]
        o = json.loads(r)
    except Exception as e:
        output.writeline( f"ERROR: bad json object [{e}] [r={r}]" )
        return False
        
    try:
        val = o["val"]
    except Exception as e:
        output.writeline( f"ERROR: missing 'val' keyword [{e}]" )
        return False
        
    if ( compare == '==' and val == expected_value ):
        pass
    elif ( compare == '<=' and val <= expected_value ):
        pass
    elif ( compare == '<' and val < expected_value ):
        pass
    elif ( compare == '>' and val > expected_value ):
        pass
    elif ( compare == '>=' and val >= expected_value ):
        pass
    elif ( compare == '!=' and val != expected_value ):
        pass
    else:
        if ( not silent ):
            output.writeline(f"WARNING: The {database}.{model_point} value - {val} - is NOT: {compare} {expected_value}")
        return False
    return True

#------------------------------------------------------
def read_and_compare_range( model_point, expected_value, upper_tolerance, lower_tolerance, database='dm', silent=True ):
    """ Reads the specified model point and compares it against
        the 'expected_value' WITH an upper and lower tolerance.  
        This method only works with numeric valued model points. 
        
        Returns true if actual and expected value 'compare'; else
        false is returned.

        The default behavior is to output an warning message when
        the method return False.  The 'silent' flag can be used
        to silence the warning message.    
    """    

    # Get the current state of the Stroke component
    r = uut.cli( f"{database} read {model_point}" )
    if ( r == None ):
        output.writeline("ERROR: The UUT is not responding")
        return False
    
    try:
        startObj = r.index('{')
        r = r[startObj:]
        o = json.loads(r)
    except Exception as e:
        output.writeline( f"ERROR: bad json object [{e}]" )
        return False
        
    try:
        val = o["val"]
    except Exception as e:
        output.writeline( f"ERROR: missing 'val' keyword [{e}]" )
        return False

    if ( val > expected_value + upper_tolerance ):
        if ( not silent ):
            output.writeline(f"WARNING: The {database}.{model_point} value: {val} is greater than upper: {expected_value+upper_tolerance}")
        return False
    if ( val < expected_value - lower_tolerance ):
        if ( not silent ):
            output.writeline(f"WARNING: The {database}.{model_point} value: {val} is less than lower: {expected_value-lower_tolerance}")
        return False
    
    return True
    
#------------------------------------------------------
def read_and_display( model_point, database='dm' ):
    """ Reads and outputs the specified model point's value
        
        Returns true if the read action was successfully; else
        false is returned (i.e. the MP is invalid).
    """
    # Get the current value of the model point
    r = uut.cli( f"{database} read {model_point}" )
    if ( r == None ):
        output.writeline("ERROR: The UUT is not responding")
        return False
    
    try:
        startObj = r.index('{')
        r = r[startObj:]
        o = json.loads(r)
    except Exception as e:
        output.writeline( f"ERROR: bad json object [{e}]" )
        return False
        
    try:
        val = o["val"]
    except Exception as e:
        output.writeline( f"{database}.{model_point} is INVALID (or bad database/mp-name)" )
        return False
        
    output.writeline(f"VALUE {database}.{model_point}: {val}")
    return True

#------------------------------------------------------
def get_value( model_point, database='dm' ):
    """ Returns the value of the specified model point.  If the MP
        is invalid, then None is returned.
    """
    # Get the current value of the model point
    r = uut.cli( f"{database} read {model_point}" )
    if ( r == None ):
        output.writeline("ERROR: The UUT is not responding")
        return False
    
    try:
        startObj = r.index('{')
        r = r[startObj:]
        o = json.loads(r)
    except Exception as e:
        output.writeline( f"ERROR: bad json object [{e}]" )
        return False
        
    try:
        val = o["val"]
    except Exception as e:
        return None
        
    return val

#------------------------------------------------------
def write_numeric( model_point, new_value, database='dm' ):
    """ Writes the specified numeric 'new_value' to 'model_point'.  
    
        Returns true if the write action was successfully issued; else
        false is returned.
    """
    r = uut.cli( f'{database} write {{name:"{model_point}",val:{new_value}}}' )
    if ( r == None ):
        output.writeline("ERROR: The UUT is not responding")
        return False
    return True

def write_bool( model_point, new_value, database='dm' ):
    """ Writes the specified boolean 'new_value' to 'model_point'.  
         
        Returns true if the write action was successfully issued; else
        false is returned.
    """
    val = 'true' if new_value else 'false'
    r = uut.cli( f'{database} write {{name:"{model_point}",val:{val}}}' )
    if ( r == None ):
        output.writeline("ERROR: The UUT is not responding")
        return False
    return True

def write_string( model_point, new_value, database='dm' ):
    """ Writes the specified string 'new_value' to 'model_point'.  
         
        Returns true if the write action was successfully issued; else
        false is returned.
    """
    r = uut.cli( f'{database} write {{name:"{model_point}",val:"{new_value}"}}' )
    if ( r == None ):
        output.writeline("ERROR: The UUT is not responding")
        return False
    return True

def write_jsonobj( model_point, new_value, database='dm' ):
    """ Writes the specified JSON object 'new_value' to 'model_point'.  
    
        Returns true if the write action was successfully issued; else
        false is returned.
    """
    r = uut.cli( f'{database} write {{name:"{model_point}",val:{{{new_value}}}}}' )
    if ( r == None ):
        output.writeline("ERROR: The UUT is not responding")
        return False
    return True



        
#------------------------------------------------------
def get_uut_elapsed_time( display_time=False ):
    """ Gets the current UUT elapsed time and returns it in milliseconds.
        When 'display_time' is True, the current time will be displayed
        on the console.
        
        If successful the method return elapsed time; else None is returned
    """
    uut.clear()
    r = uut.cli( 'tprint' )
    if ( r == None ):
        output.writeline("ERROR: The UUT is not responding")
        return None

    elapsed_time = r.strip().split(' ')[1]
    h,m,s = elapsed_time.split(':')
    s,ms  = s.split('.')
    tm    = int(ms) + (int(s) * 1000) + (int(m) * 60 * 1000) + (int(h) * 60 * 60 * 1000)

    if ( display_time ):
        output.writeline( f"----- TIME: {elapsed_time} ({tm} msec) -----" )
        
    return tm


#------------------------------------------------------
def scan_for_trace_stmts( cli_output, trace_section, thread_name ):
    """ Scans the 'cli response' data and returns a list of 
        lines that match the specified trace section and thread 
        name
    """
    stmts = []
    lines = cli_output.split(config.g_newline)
    filter = f'({trace_section}) [{thread_name}]'
    for l in lines:
        if ( filter in l ):
            stmts.append(l)
    return stmts
