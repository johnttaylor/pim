#!/usr/bin/python3
"""Collection of helper functions"""

import os
import logging
import sys
import subprocess
import pathlib
import platform
import collections
import fnmatch
import re

# Globals
from .my_globals import NQBP_WORK_ROOT
from .my_globals import NQBP_PKG_ROOT
from .my_globals import NQBP_TEMP_EXT
from .my_globals import NQBP_VERSION
from .my_globals import NQBP_PRJ_DIR
from .my_globals import NQBP_NAME_LIBDIRS
from .my_globals import NQBP_PRJ_DIR_MARKER1
from .my_globals import NQBP_PRJ_DIR_MARKER2
from .my_globals import NQBP_PKG_TOP
from .my_globals import NQBP_WRKPKGS_DIRNAME
from .my_globals import NQBP_PRE_PROCESS_SCRIPT
from .my_globals import NQBP_PRE_PROCESS_SCRIPT_ARGS
from .my_globals import OUT
from .my_globals import NQBP_XPKG_MODEL
from .my_globals import NQBP_XPKG_MODEL_MIXED
from .my_globals import NQBP_XPKG_MODEL_OUTCAST

# Module globals
_dirstack = []
verbose_mode = False



#-----------------------------------------------------------------------------
def dir_list_filter_by_ext(dir, exts, derivedDir=False): 
    """Returns a list of files filter by the passed file extensions
    
    Accepts one or more file extensions (with no '.') 
    """

    results = []
    try:
        for name in os.listdir(dir):
            for e in exts:
                if ( name.endswith("." + e) ):
                    results.append(name)
    except:
        if ( derivedDir ):
            sys.exit("ERROR: Derived/Built directory '{}' does not exist".format( dir ))
        else:
            sys.exit("ERROR: Source directory '{}' does not exist".format( dir ))

    return results


def get_objects_list( objext, objpath, new_root ):
    """ Returns a list (as a single string) of object files for specified object 
        path with each object having the 'new_root' appended to it path.
    """

    files = dir_list_filter_by_ext( objpath, objext.split(), derivedDir=True )
    objs  = '';
    for f in files:
        basename = os.path.splitext(f)[0]
        basename = os.path.join(new_root, objpath, basename)
        objs     = objs + ' ' + basename + '.' + objext

    return objs

def walk_file_list( pattern, pkgpath, skipdir=None ):
    """ Generates a list of files in a directory and with the ability to skip a specified directory tree """

    list = []
    for root, dirs, files in os.walk(pkgpath):
        if ( skipdir == None or root != skipdir ):
            for f in fnmatch.filter(files,pattern):
                list.append( os.path.join(root,f) )
            
    return list  

#-----------------------------------------------------------------------------
def run_shell( printer, cmd, capture_output=True ):
    p = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE ) if capture_output else subprocess.Popen( cmd, shell=True ) 
    r = p.communicate()

    if ( r[0] != None ):
        line =  r[0].decode().rstrip()
        if ( line != "" ):
            printer.output( line )
            
    if ( r[1] != None ):
        line =  r[1].decode().rstrip()
        if ( line != "" ):
            printer.output( line )

    return p.returncode

#
def run_shell2( cmd, stdout=False, on_err_msg=None ):
    """ Alternate semantics use internal 'verbose' flag instead of a printer
    """

    print_verbose( cmd )
    if ( stdout ):
        p = subprocess.Popen( cmd, shell=True )
    else:
        p = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )

    r  = p.communicate()
    r0 = '' if r[0] == None else r[0].decode()
    r1 = '' if r[1] == None else r[1].decode()

    if ( p.returncode != 0 and on_err_msg != None ):
        print_verbose( "{} {}".format(r0,r1) )
        sys.exit(on_err_msg)

    return (p.returncode, "{} {}".format(r0,r1))
    
#-----------------------------------------------------------------------------
def del_files_by_ext(dir, *exts): 
    """Delete file(s) from 'dir' based the passed file extensions
    
    Accepts one or more file extensions (with no '.') 
    """

    files_to_delete = dir_list_filter_by_ext( dir, *exts, derivedDir=True )
    for f in files_to_delete:
        delete_file(f)

    
#-----------------------------------------------------------------------------
def expand_environ_var_dir_path( printer, dir_path, marker='$' ):
    envvar, subpath = dir_path[1:].split(marker, 1)
    rootpath = os.environ.get(envvar)
    if ( rootpath == None ):
        printer.output( "ERROR: Non-existent environment variable - {} - reference in line ({})".format(envvar,dir_path))
        sys.exit(1)
    subpath = subpath.lstrip(os.sep)
    return os.path.join(rootpath, subpath)
    

#-----------------------------------------------------------------------------
def replace_environ_variable( printer, line, marker='$' ):
    # Find the next environment variable
    start_idx = line.find(marker)
    if ( start_idx < 0 ):
        return line
    end_idx = line.find(marker,start_idx+1)
    if ( end_idx < 0 ):
        printer.output( "ERROR: Invalid variable syntax - missing trailing {} - in line:({})".format(marker,line) )
        sys.exit(1)

    # Expand the environment variable's content
    var   = line[start_idx+1:end_idx]
    value = os.environ.get(var)
    if ( value == None ):
        printer.output( "ERROR: Non-existent environment variable - {} - reference in line ({})".format(var,line))
        sys.exit(1)
    line = line[:start_idx] + value + line[end_idx+1:]

    # Keeping search for other environment variables....
    return replace_environ_variable(printer,line,marker)

#-----------------------------------------------------------------------------
def create_working_libdirs( printer, inf, arguments, libdirs, libnames, local_external_flag, variant, parent=None ):

    # process all entries in the file        
    for line in inf:
        # 'normalize' the file entries
        line      = standardize_dir_sep( line.strip() )
        entry     = local_external_flag
        newparent = parent
        path      = ''

        # drop comments and blank lines
        if ( line.startswith('#') ):
            continue
        if ( line == '' ):
            continue
        
        # Filter by variant
        if ( line.startswith('[') ):
            tokens = line[1:].split(']')
            if ( len(tokens) == 1 ):
                printer.output( "ERROR: invalid [<variant>] prefix qualifier ({})".format( line ) )
                sys.exit(1)
            if ( not _matches_variant(tokens[0], variant) ):
                continue

            # Remove the filter prefix
            line = line.split(']')[1].strip()
            
            
        # Attempt cross compatibility between the Legacy and Outcast model with respect to external packages
        if ( NQBP_XPKG_MODEL() == NQBP_XPKG_MODEL_MIXED() ):
            if ( line.startswith(os.sep + os.sep) ):
                path  = NQBP_PKG_ROOT() + os.sep
                parts = path_split_all(line[2:])
                if ( len(parts) > 1 ):
                    if ( parts[1] == 'src' ):
                        line = os.sep.join( parts[1:] )
                    else:
                        line = NQBP_WRKPKGS_DIRNAME() + os.sep + parts[0] + os.sep + os.sep.join( parts[1:] )


        # Calc root/leading path    
        if ( line.startswith(os.sep+os.sep) ):
            path      = os.path.join(NQBP_WORK_ROOT(), NQBP_WRKPKGS_DIRNAME() ) + os.sep
            line      = line [2:]
            entry     = 'xpkg'
            newparent = line.split(os.sep)[0]
     
        # Absolute root path via an environment variable
        elif ( line.startswith('$')):
            line = replace_environ_variable( printer, line )
            entry = 'absolute'

        # relative path (i.e. an '#include' statement)
        elif ( line.startswith('.') ):
            
            # all relative includes must be libdirs.b includes
            if ( not line.endswith( NQBP_NAME_LIBDIRS() ) ):
                printer.output( "ERROR: using a relative include to a non-libdirs.b file ({})".format( line ) )
                sys.exit(1)
                
            path  = NQBP_PRJ_DIR() + os.sep
            
            
        # within my package...
        else:
            if ( line.startswith(os.sep) ):
                path    = NQBP_PKG_ROOT() + os.sep
                line    = line [1:]
                entry   = 'pkg'
               
            # append 'parent' when there is one, i.e. when I have been include from xpkg libdirs.b    
            else:
                # Find external packages when NOT using the Outcast model
                if ( NQBP_XPKG_MODEL() != NQBP_XPKG_MODEL_OUTCAST() and line.startswith(NQBP_WRKPKGS_DIRNAME()) ):
                    entry     = 'xpkg'

                if ( newparent != None ):
                    line = os.path.join(newparent,line)
            
            
        # Expand any/all embedded environments variables (that did NOT start the directory entry)
        line = replace_environ_variable(printer, line)

        # Split off include/exclude source files list
        srctype = None
        srclist = None
        orgline = line
        tokens  = line.split(' ', 1);
        if ( len(tokens) == 2 ):
            line    = tokens[0].strip()
            srclist = tokens[1].strip()
            tokens  = srclist.split( ' ' )

            if ( len(tokens) == 1 or (tokens[0] != '<' and tokens[0] != '>') ):
                printer.output( "ERROR: Invalid include/exclude syntax specified in libdirs.b. line='{}'".format(orgline) )
                sys.exit(1)

            srctype = tokens[0];
            srclist = tokens[1:]

        # trap nested 'libdirs.b' files
        if ( line.endswith( NQBP_NAME_LIBDIRS() ) ):
            fname = path+line
            if ( not os.path.isfile(fname) ):
                printer.output( "ERROR: Missing/invalid nest '{}': {}".format(NQBP_NAME_LIBDIRS(),line) )
                sys.exit(1)
                
            printer.debug( "# Nested libdirs file: " + path+line )
            f = open( path+line, 'r' )
            create_working_libdirs( printer, f, arguments, libdirs, libnames, entry, variant, newparent )
            f.close()
            continue               

        # output the line
        if ( arguments['-p'] and entry == 'xpkg' ):
            pass
        elif ( arguments['-x'] and entry != 'xpkg' ):
            pass
        elif ( arguments['--noabs'] and entry == 'absolute' ):
            pass
        elif ( arguments['-q'] != None and not arguments['-q'] in line ):
            pass
        elif ( arguments['-Q'] != None and re.search( arguments['-Q'], line ) == None ):
            pass
        elif ( arguments['-c'] != None and arguments['-c'] in line ):
            pass
        elif ( arguments['-C'] != None and re.search( arguments['-C'], line ) != None ):
            pass
        else:
            libdirs.append( ((line, srctype, srclist), entry) )
            libnames.append( line )


    # Check for duplicates (Note: Duplicates can/will fail a build when performing parallel builds)
    duplicates = False
    d          = collections.Counter(libnames)
    for k,v in d.items():
        if ( v > 1 ):
            duplicates = True
            print( "Duplicate libdirs: ", k )
    if ( duplicates and not arguments['--debug'] ):
        sys.exit( "ERROR Duplicate entries in libdirs.b" )

        
# 
def find_libdir_entry( libdirs, dir_path, entry_type=None ):
    dir_path = standardize_dir_sep(dir_path)
    for d,e in libdirs:
        line,srctype,srclist = d
        if ( entry_type == None ):
            if ( dir_path == line ):
                return (True, (dir_path, srctype, srclist), e )
        elif ( e == entry_type and dir_path in line ):
                return (True, (dir_path, srctype, srclist), e )

    return (False, (dir_path, None, None), entry_type)

#-----------------------------------------------------------------------------
def run_pre_processing_script( printer, current_dir, work_root, pkg_root, prj_dirname, preprocess_script, preprocess_args, build_clean="build", verbose=False ):
    # Do nothing if feature not enabled
    if ( preprocess_script != None ):
        script = os.path.join( current_dir, preprocess_script )
        
        # Do nothing if no pre-process script is present
        if ( os.path.isfile( script) ):
            verbose_opt = "verbose" if verbose else "terse"
            printer.output( "= Running Pre-Process script: " + preprocess_script )
            cmd = "{} {} {} {} {} {} {} {}".format( script, build_clean, verbose_opt, work_root, pkg_root, prj_dirname, current_dir, preprocess_args)
            printer.debug( "# PreProcessing cmd = " + cmd )
            run_shell2( cmd, stdout=True, on_err_msg="Running PreProcess Script Failed!")


#
def run_clean_pre_processing( printer, libdirs, clean_pkg=False, clean_local=False, clean_xpkgs=False, clean_absolute=False ):
    # Do nothing if no pre-processing script is defined
    if ( NQBP_PRE_PROCESS_SCRIPT() != None and len(libdirs) > 0 ):
        
        # Walk list of possible directories
        for d,e in libdirs:
            line,srctype,srclist = d

            # Clean Local and PKG dirs
            if ( (clean_pkg and e == 'local') or (clean_local and e == 'pkg') ):
                dir = os.path.join( NQBP_PKG_ROOT(), line )
                run_clean_dir_pre_processing( dir, printer )

            # Clean External Packages
            if ( clean_xpkgs and e == 'xpkg' ):
                dir = os.path.join( NQBP_WORK_ROOT(), NQBP_WRKPKGS_DIRNAME(), line )
                run_clean_dir_pre_processing( dir, printer )

            # Clean Absolute directories
            if ( clean_absolute and e == 'absolute' ):
                run_clean_dir_pre_processing( line, printer )

#
def run_clean_dir_pre_processing( dir, printer ):
    # Do nothing if no pre-processing script is defined
    if ( NQBP_PRE_PROCESS_SCRIPT() != None ):
        printer.debug( "# Clean pre_processing dir= " + dir )

        # Set verbose script option
        verbose_opt = "verbose" if printer.verbose_on else "terse"
    
        # Run script if it exists
        script = os.path.join( dir, NQBP_PRE_PROCESS_SCRIPT() )
        if ( os.path.isfile( script) ):
            printer.output( "= Cleaning Pre-Process script: " + NQBP_PRE_PROCESS_SCRIPT() )
            cmd = "{} {} {} {} {} {} {} {}".format( script, "clean", verbose_opt, NQBP_WORK_ROOT(), NQBP_PKG_ROOT(), NQBP_PRJ_DIR (), dir, NQBP_PRE_PROCESS_SCRIPT_ARGS())
            printer.debug( "# Clean PreProcessing cmd = " + cmd )
            run_shell2( cmd, stdout=True, on_err_msg="Cleaning PreProcess Script Failed!")

           
# 
def fix_absolute_root( filepath ):
    print( "fix_root", filepath, len(pathlib.Path( filepath ).parts) )
    if ( platform.system() == 'Windows' ):
        p = pathlib.Path( filepath )
        if ( len(p.parts) > 1 ):
            filepath = os.path.join( os.sep, p.parts[0] + ':' + os.sep)
            if ( len(p.parts) > 2 ):
                filepath = os.path.join( filepath, *p.parts[1:] )

    print("   end: ", filepath )
    return filepath

#-----------------------------------------------------------------------------
def create_subdirectory( printer, pardir, new_subdir ):
    dname  = os.path.abspath( standardize_dir_sep(pardir) + os.sep + new_subdir )
    
    try:
        os.makedirs(dname)
        
    except OSError:
        if ( os.path.exists(dname) ):
            # Directory already exists -->no actual error
            pass
            
        else:
            printer.output( "ERROR: Failed to create directory: {}".format( dname ) )
            sys.exit(1)
            
    return dname
    
    
def create_subdirectory_from_file( printer, pardir, fname ):
    subdir = os.path.dirname( standardize_dir_sep(fname) )
    dname  = os.path.abspath( standardize_dir_sep(pardir) + os.sep + subdir )
    
    try:
        os.makedirs(dname)
        
    except OSError:
        if ( os.path.exists(dname) ):
            # Directory already exists -->no actual error
            pass
            
        else:
            printer.output( "ERROR: Failed to create directory: {}".format( dname ) )
            sys.exit(1)
            
    return dname
    
        
#-----------------------------------------------------------------------------
def set_pkg_and_wrkspace_roots( from_fname):
    from_fname = os.path.abspath(from_fname)
    root = _get_marker_dir( standardize_dir_sep(from_fname), NQBP_PRJ_DIR_MARKER1() )
    if ( root != None ):
        pass
    
    else:
        root = _get_marker_dir( standardize_dir_sep(from_fname), NQBP_PRJ_DIR_MARKER2() )
        if ( root != None ):
            pass
            
        else:
            print( "ERROR: Cannot find the 'Package Root'" )
            sys.exit(1)
                
    NQBP_WORK_ROOT( os.path.dirname(root) )     
    NQBP_PKG_ROOT( root )
    
   
def _get_marker_dir( from_fname, marker ):
    path   = from_fname.split(os.sep)
    result = ''  
    idx    = 0  
    path.reverse()
    
    try:
        idx = path.index(marker)
        idx = len(path) - idx -1
        path.reverse()
        for d in path[1:idx]:
            result += os.sep + d
        
    except:
        result = None
    
    if ( result == None ):
        result = _test_for_top( result, marker )    
    
    return result
    

def _test_for_top( dir, marker ):
    while( dir != None ):
        if ( os.path.isdir(dir + os.sep + NQBP_PKG_TOP()) ) :
            break
        else:
            dir = _get_marker_dir( dir, marker )
    
    return dir
    
    
def _matches_variant( filter, my_variant ):
    tokens = filter.split('|')
    for t in tokens:
        if ( t.strip() == my_variant ):
            return True
    return False    
                    
#--------------------------------------------------------------------------
def delete_file( fname ):
    """ remove file(s) and suppress error if 'fname' does not exist """
    
    result = False
    try:
        os.remove( fname )
        result = True
    except OSError:
        pass
           
    return result
    
           
#-----------------------------------------------------------------------------
def standardize_dir_sep( pathinfo ):
    return pathinfo.replace( '/', os.sep ).replace( '\\', os.sep )

def push_dir( newDir ):
    global _dirstack
    _dirstack.append( os.getcwd() )
    os.chdir( newDir )
    
    
def pop_dir():
    global _dirstack
    os.chdir( _dirstack.pop() )
    
def path_split_all(path):
    allparts = []
    while 1:
        parts = os.path.split(path)
        if parts[0] == path:  # sentinel for absolute paths
            allparts.insert(0, parts[0])
            break
        elif parts[1] == path: # sentinel for relative paths
            allparts.insert(0, parts[1])
            break
        else:
            path = parts[0]
            allparts.insert(0, parts[1])
    return allparts    

#-----------------------------------------------------------------------------
#
def derive_src_path( pkg_root, work_root, pkgs_dirname, entry, dir ):
    srcpath   = ''
    display   = dir[0]
    newlibdir = dir[0]
    
    # directory is local to my package
    if ( entry == 'local' ):
        srcpath = pkg_root + os.sep + dir[0]
        display = dir[0]
    
    elif ( entry == 'pkg' ):
        srcpath = os.path.join( pkg_root, dir[0] )
        display = dir[0]
        
    # directory is an absolute path
    elif ( entry == 'absolute' ):
        srcpath   = dir[0]
        display   = dir[0]
        objpath   = dir[0].replace(":",'',1)
        newlibdir = os.path.join( "__abs", objpath.lstrip(os.sep) )

    # directory is an external package
    else:
        display   = os.sep + dir[0]
        srcpath   = work_root + os.sep + pkgs_dirname + os.sep + dir[0]
        newlibdir = pkgs_dirname + os.sep + dir[0]

    return srcpath, display, (newlibdir, dir[1], dir[2])

def get_files_to_build( printer, toolchain, dir, sources_b ):
    files = []
    
    # get the list of potential file to build (when no 'sources.b' is present)
    src_b = os.path.join( dir, sources_b )
    if ( not os.path.isfile( src_b) ):
        exts = ['c', 'cpp'] + toolchain.get_asm_extensions()
        printer.debug( "# Creating auto.sources.b for dir: {}. Extensions={}".format( dir, exts )  )
        files = dir_list_filter_by_ext(dir, exts )
                
    # get the list of files to build from 'sources.b'
    else:
        inf = open( src_b, 'r' )
        for line in inf:
            # drop comments and blank lines
            line = line.strip() 
            if ( line.startswith('#') ):
                continue
            if ( line == '' ):
                continue
            
            files.append( line )
       
        inf.close()
             
    # return the file list
    printer.debug( "# Default file list to build for dir: {}. {}".format( dir, files )  )
    return files                
    

#
def filter_files( printer, dir, file_list, filter_type, filter_files ):
    files = file_list
    printer.debug( "# For dir: {}. Filter type: {}, list: {}".format( dir, filter_type, filter_files )  )

    # Include only filter
    if ( filter_type == '<'):
        files = filter_files
    
    # Exclude filter
    elif ( filter_type == '>'):
        files = [n for n in file_list if n not in filter_files]


    printer.debug( "# Filtered file list to build for dir: {}. {}".format( dir, files )  )
    return files;

#
def get_and_filter_files_to_build( printer, toolchain, dir, srcpath, sources_b ):
    # check for existing 'sources.b' file 
    files = get_files_to_build( printer, toolchain, srcpath, sources_b )

    # Filter the source file by the include/exclude list (if there is one)
    if ( dir[1] != None and dir[2] != None ):
        files = filter_files( printer, srcpath, files, dir[1], dir[2] )

    return files

#-----------------------------------------------------------------------------
def replace_build_dir_symbols( toolchain, objects_string, libdirs, new_root ):
    objects_string = standardize_dir_sep(objects_string)

    # Do nothing if no symbol(s) to expand
    if ( not '_BUILT_DIR_.' in objects_string ):
        return objects_string

    final  = ''
    source = objects_string.strip()
    while( True ):
        # Strip out special token/symbol
        tokens = source.split( "_BUILT_DIR_.", 1 )
        if ( len(tokens) != 2 ):
            print( "ERROR: Missing libdir reference for _BUILT_DIR_ symbol in string: {}".format( objects_string ) )
            sys.exit(1)

        final += ' ' + tokens[0]
        #print( "First split", tokens, "source=", source, "final=", final )

        # Get libdir reference string
        tokens = tokens[1].split(' ', 1)
        #print( " Second split", tokens)
        
        # Get corresponding entry from the libdir.b
        result, dir, entry = find_libdir_entry( libdirs, tokens[0] )
        #print( result, dir, entry)
        if ( result == False ):
            print( "ERROR: Cannot find directory entry (in libdirs.b) for {}".format( objects_string ) )
            sys.exit(1)

        # convert source list to an object list
        final += ' ' + get_objects_list(toolchain._obj_ext, dir[0], new_root )

        # Nothing left to parse
        if ( len(tokens) == 1 ):
            return final 

        # Are the any remaining special token/symbols
        source = tokens[1]
        if ( not '_BUILT_DIR_.' in source ):
            return final + ' ' + source

#-----------------------------------------------------------------------------
def list_libdirs( printer, libs ):
    printer.debug( "# Expanded libdirs.b: (localFlag, srcdir [srctype, srclist])" )
    for l in libs:
        dir,flag = l
        d,s,sl = dir
        if ( s == None or sl == None ):
            printer.debug( "#   {:<5}:  {}".format( str(flag), d)  )
        else:
            printer.debug( "#   {:<5}:  {} -- {} {}".format( str(flag), d, s, sl)  )

#-----------------------------------------------------------------------------
def print_verbose( msg, new_line=True ):
    """ Outputs the content of 'msg' based on the global verbose setting. When
        the 'new_line' flag is set to true and newline character sequence is
        appended to the output. Use set_verbose_mode to enable/disable this 
        functions output. 
    """
    if ( verbose_mode ):
        if ( new_line ):
            print( msg )
        else:
            print( msg, end='' )
        
#
def set_verbose_mode( newstate ):
    """ When 'newstate' is true, then the print_verbose method will generate
        output; else the print_verbose method will suppress its output.
    """

    global verbose_mode
    verbose_mode = newstate