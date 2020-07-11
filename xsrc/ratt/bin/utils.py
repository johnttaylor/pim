""" Utility functions
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import imp
from time import strftime
import serial
import config
import subprocess

#------------------------------------------------------------------------------
def append_current_time( base_name, separator='_' ):
    """ Appends the current time to 'base_name'
    """
    return base_name + separator + strftime("%Y-%m-%d_%H.%M.%S") 


#------------------------------------------------------------------------------
def run_shell( cmd, stdout=False, on_err_msg=None ):
    """ Executes a shell command and returns a tuple of the command return
        code and the output of the command.  When on_err_msg is NOT set to 
        the None AND an error occured while executing 'cmd', the calling script 
        will be exited and display the contents of on_err_msg.  When the stdout 
        is true, the command's output will displayed on the console as it executes 
        and it will NOT be captured as a return value.
    """
        
    if ( stdout ):
        p = subprocess.Popen( cmd, shell=True )
    else:
        p = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )

    r = p.communicate()
    if ( p.returncode != 0 and on_err_msg != None ):
        exit(on_err_msg)

    r0 = r[0].decode().rstrip() if r[0] != None else ""
    r1 = r[1].decode().rstrip() if r[1] != None else ""
            
    return (p.returncode, "{} {}".format(r0,r1) )

#------------------------------------------------------------------------------
def get_available_serial_ports( platform="Windows" ):
    """ Generates list of available/unused Serial ports.
        Current only support enumerating the COM ports under Windoze
    """

    result = []

    # Find all available COM Ports on a Windoze box
    if ( platform == "Windows"):
        ports = ['COM%s' % (i + 1) for i in range(256)]
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
    else:
        result.append( "Feature not supported on platform: {}".format( platform ))

    return result;

#
def string_to_parity_enum( s ):
    if ( s[:1] == 'e' or s[:1] == 'E' ):
        return serial.PARITY_EVEN
    if ( s[:1] == 'o' or s[:1] == 'O' ):
        return serial.PARITY_ODD

    return serial.PARITY_NONE

#
def int_to_stopbits_enum( numbits ):
    if ( numbits == 1):
        return serial.STOPBITS_TWO
    
    return serial.STOPBITS_ONE

#
def int_to_databits_enum( numbits ):
    if ( numbits == 7 ):
        return serial.SEVENBITS
    if ( numbits == 6):
        return serial.SIXBITS

    return serial.EIGHTBITS

#
def open_serial_port(serialPort, baudrate=115200, parity="none", stopbits=1, bytesize=8, timeout=None, platform="Windows"):
    # Open a Windows COM Port
    if ( platform == "Windows"):
        try:
            serialConnection = serial.Serial( port=serialPort, baudrate=baudrate, parity=string_to_parity_enum(parity), stopbits=int_to_stopbits_enum(stopbits), bytesize=int_to_databits_enum(bytesize), timeout=timeout )
        except Exception as e:
            sys.exit("Failed to open serial port ({}). Error={}".format( serialPort, str(e)) )

        return serialConnection

    else:
        return None


#------------------------------------------------------------------------------
def import_code(code, name):
    """ code can be any object containing code -- string, file object, or
        compiled code object. Returns a new module object initialized
        by dynamically importing the given code.  If the module has already
        been imported - then it is returned and not imported a second time.
    """
   
    # Check if 'code' has already been loaded
    if (name in config.g_utils_import_dictionary):
        return config.g_utils_import_dictionary[name]

    # Load the 'code' into the memory
    try:
        module = imp.new_module(name)
        config.g_utils_import_dictionary[name] = module
        exec(code, module.__dict__)
        return module

    except Exception as e:
        print("Error={}".format( str(e) ))
        return None

#
def import_file(filename, search_paths=None):
    """ Wrapper function to dynamically load/import the specified python
        script file.  The file name - sans the file extension - is used
        for the module name.

        'filename' should be without a absolute path.  The contents of 'search_paths'
        is prepended to 'filename' when trying to find/open the specified file.
        The search order is the order of the 'search_paths', i.e. index 0 is
        searched first, etc.  If 'search_parts' is None, then only the CWD
        is searched.

        Returns a tuple: [module|None, fullpath|errorMsg]
    """

    # Housekeeping
    fd       = None
    fullpath = filename
    modname  = os.path.splitext(os.path.split(filename)[1])[0]

    # Check for already loaded
    if (modname in config.g_utils_import_dictionary):
        return config.g_utils_import_dictionary[modname],fullpath

    # No search paths...
    if (search_paths == None or len(search_paths) == 0 ):
        try:
            fd = open(fullpath, "rt")
        except Exception as e:
            return None, "ERROR. Unable to open the file {} (No search paths provided).  Error={}".format(fullpath,str(e))

    # Search the search paths
    else:
        num_paths = len(search_paths)
        for p in search_paths:
            num_paths -= 1
            fullpath   = os.path.join(p,filename)
            try:
                fd = open(fullpath, "rt")
                break
            except Exception as e:
                if (num_paths == 0):
                    return None, "ERROR. Unable to open the file {}.  Error={}".format(filename,str(e))

    # Load the script/module
    code = fd.read()
    m = import_code(code, modname)
    config.g_utils_import_paths[modname] = fullpath
    fd.close()
    return m,fullpath

#
def find_files( file_extension=config.g_ratt_file_extension , search_paths=None):
    """ Generates a dictionary of files using the specified search paths.  Once 
        a file is found at higher priority path - it is not added to the return
        dictionary a second time.  The search order is:
        1) The current working directory 
        2) Then 'search_paths' is searched starting with index 0

        If no files are found, the None is returned
    """

    # Dictionary of found files: key=file name, value=path found at
    d = {}

    # function to add found files to the dictionary
    def search_directory( d, file_extension, path ):
        if ( path != None ):
            for file in os.listdir(path):
                if file.endswith(file_extension):
                    if ( file in d ):
                        pass
                    else:
                        d[file] = path

        return d

    # No search paths...
    if (search_paths == None or len(search_paths) == 0 ):
        d = search_directory( d, file_extension, ".")

    # Search the search paths
    else:
        num_paths = len(search_paths)
        for p in search_paths:
            d = search_directory( d, file_extension, p )


    # Return the list of found scripts
    return d