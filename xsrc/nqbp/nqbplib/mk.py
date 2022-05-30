#!/usr/bin/python3
""" Top level entry point for NQBP (i.e. called by the project-being-built's mk.py) """

#=============================================================================
# ENVIRONMENT VARIABLES
# ---------------------
#   REQUIRED:
#     NQBP_BIN          Path to the directory of where (and/or which version)
#                       of the NQBP Python package to be used
#
#   OPTIONAL:
#     NQBP_XPKG_MODEL   Used to specified what the 'model' for external handling
#                       external and/or 3rd party source code.  The options are:
#
#                       'legacy'  - Assumes there is single package and external
#                                   source code is all under the 'xsrc/' directory
#                                   in the package.  If the environment variable
#                                   is not set - this is the default model.
#                               
#                       'outcast' - Assumes packages are mounted under the
#                                   'xpgks/' and 'xinc/' directories
#
#                       'mixed'   - Mixed mode attempts to support build
#                                   scripts constructed to use the 'outcast'
#                                   model to be auto-magically compatible when
#                                   using the single package + xsrc/ directory
#                                   structure. This mode is not perfect, but
#                                   works for all currently tested use cases
#                                   Rules for Outcast based packages:
#                                   1) The xpackage's source code and 
#                                      header files that are under its 'src/'
#                                      directory are assumed to be mapped' under 
#                                      the 'src/' directory of the single package 
#                                   2) Anything in the xpackage that referenced
#                                      in build scripts using the 'xpkgs/' 
#                                      directory must be mapped under the
#                                      'xsrc'/<package-name>' directory
#
#                                   NOTE: Sometimes content from a xpackage ends
#                                         up in two places - under 'src' and
#                                         'xsrc/'.  Did I mention that mixed
#                                         was not perfect.
#
#                                   NOTE: When using MIXED mode - the developer
#                                         should ONLY EDIT files that are part
#                                         of the single package's file set, i.e.
#                                         do NOT EDIT 'external' files                       
#=============================================================================

#
import sys   
import os
import logging
import time
from multiprocessing import Process

#
from .docopt.docopt import docopt
from .base import ToolChain
from .output import Printer
from . import base
from . import utils
import multiprocessing

    
# Globals
from .my_globals import NQBP_WORK_ROOT
from .my_globals import NQBP_PKG_ROOT
from .my_globals import NQBP_TEMP_EXT
from .my_globals import NQBP_VERSION
from .my_globals import NQBP_PRJ_DIR
from .my_globals import NQBP_WRKPKGS_DIRNAME
from .my_globals import NQBP_NAME_SOURCES
from .my_globals import NQBP_PRE_PROCESS_SCRIPT
from .my_globals import NQBP_PRE_PROCESS_SCRIPT_ARGS
from .my_globals import NQBP_NAME_LIBDIRS
from .my_globals import NQBP_XPKG_MODEL
from .my_globals import NQBP_XPKG_MODEL_OUTCAST
from .my_globals import NQBP_XPKG_MODEL_MIXED



# 
usage = """ 
(N)ot (Q)uite (B)env/(P)ython Build Script
===============================================================================
Usage: nqbp [options] [-b variant] 
       nqbp [options] [-b variant] -d DIR
       nqbp [options] [-b variant] -f FILE
       nqbp [options] [-b variant] -s DIR [-e DIR]
       nqbp [options] [-b variant] -m [-d DIR | -f FILE]

Arguments:
  -b variant       Builds the specified configuration/variant instead of the 
                   release build. What 'variants' are available is Toolchain 
                   specific. The default is variant is determined by the
                   mytoolchain.py script.
  --try variant    Same as the '-b' option, except that if the variant does not
                   exist - the script does not report a failure on exit.
  --bld-all        Builds all variants.  Does NOT build variants that start 
                   with a leading '_'.
  -d DIR           Compile ONLY the specified directory relative to the pkg 
                   root. If 'DIR' starts with a directory seperator ('\\' or 
                   '/') then the directory is relative to the package root. If
                   'DIR' starts with a double directory seperator ('\\\\' or
                   '//') then the directory is relative to the workspace root.
  -f FILE          Compile ONLY the specified file relative to the pkg root.
                   If 'FILE' starts with a directory seperator ('\\' or '/'
                   then the file is relative to the package root. If 'FILE' 
                   starts with a double directory seperator ('\\\\' or
                   '//') then the file is relative to the workspace root. A 
                   'FILE' in the project directory can be compile by prefixing 
                   'FILE' with './' or '.\\' (i.e. current directory).
  -s DIR           Skips all entries in libdirs.b BEFORE 'DIR', i.e. start with 
                   'DIR'.  Note: 'DIR' is with respect to the expanded list of 
                   directories, i.e. after any included libdirs.b files are 
                   expanded.
  -e DIR           Skip all entries in libdirs.b AFTER 'DIR', i.e. stop 
                   building directories once 'DIR' has been built. Note: 'DIR' 
                   is with respect to the expanded list of directories, i.e. 
                   after any included libdirs.b files are expanded.
  -q STR           Only builds directories that contain 'STR' in their name/path
  -Q STRX          Same as '-q', except 'STRX' is a regular expression.
  -c STR           Builds all directories except those that contain 'STR' in 
                   their name/path.
  -C STRX          Same as '-c', except 'STRX' is a regular expression.
  -p               Skips all external directories and/or libdirs.b files.
  -x               Skips all the package's directories and/or libdirs.b files
  -a, --noabs      Skips (and does not clean) all absolute directories
  -m               Compiles all of the project directory.
  -g               Debug build (default is release build.
  -v               Display Compiler/linker options.
  -l               Link ONLY (can be combinded with '-mpxdfseqQ' options).
  -k               Cleans only the package's objects/files (use with '-p')
  -j               Cleans only the external objects/files (use with '-x').
  -1               Suppresses the use of multiple processes when building.
  --bldnum M       Passes 'M' as build number information for the build. 
                   [Default: 0].          
  --def1 SYM1      Defines (as a compiler option) the preprocessor 'SYM1'.
  --def2 SYM2      Defines (as a compiler option) the preprocessor 'SYM2'.
  --def3 SYM3      Defines (as a compiler option) the preprocessor 'SYM3'.
  --def4 SYM4      Defines (as a compiler option) the preprocessor 'SYM4'.
  --def5 SYM5      Defines (as a compiler option) the preprocessor 'SYM5'.
  -t, --turbo      Uses multiple process to build directories in parallel.
  -z, --clean-all  Cleans ALL files for ALL build configurations and then exits
  --debug          Enables debug info internally to NQBP.
  --qry            Outputs the current project directory (does nothing else)
  --qry-and-clean  Combines '--qry' and '--clean-all' options into a single 
                   operation.
  --qry-blds       Displays the build 'variants' supported by the toolchain 
                   (no build is performed).
  --qry-dirs       Displays the list of directories (in order, for the selected
                   build variant) referenced in the libdirs.b file.
  --qry-dirs2      Same as --qry-dirs with the addition of the any source file
                   include/exclude info
  --qry-model      Returns the Outcast vs Legacy model for external packages.
  -h,--help        Display help.
  --version        Display version number.

Notes:
    Default operation is to do an implicit BUILD ALL and CLEAN ALL on each 
    build.  The exception to this rule is when one of the following options are  
    specified: -d, -f, -s, -e, -p, -x, -m, -l, -k, -j -q -Q -c -C
  
    By default, NQBP will attempt to build all files in a single directory in
    parallel. However, not all compilers deal well with parallel building (i.e
    the crash). The '-1' option will suppress all parallel building.  In addition 
    the environment variable NQBP_CMD_OPTIONS (when set to '-1') can be used to
    apply the '-1' option to every build.

    When the environment variable NQBP_XPKG_MODEL is set to 'outcast' then 
    NQBP assumes that the Outcast paradigm (of xpkgs/ & xinc/) for external 
    packages. Values of 'mixed' or 'legacy' NQPB will uses the xsrc/ directory
    (under the current package) paradigm.
       
Examples:

"""


#-----------------------------------------------------------------------------
def build( argv, toolchain ):

    # ensure that I am executing in the project directory
    os.chdir( NQBP_PRJ_DIR() )

    # Append options from optional environment variable
    rawinput = sys.argv[1:]
    NQBP_CMD_OPTIONS = os.environ.get('NQBP_CMD_OPTIONS')
    if ( NQBP_CMD_OPTIONS != None ):
        rawinput.extend( NQBP_CMD_OPTIONS.split(' '))

    # Process command line args...
    arguments = docopt(usage, argv=rawinput, version=NQBP_VERSION() )

    # Allow the '--turbo' option to override the '-1' option
    if ( arguments['--turbo'] ):
        arguments['-1'] = False

    # Create printer (and tell the toolchain about it)
    logfile = os.path.join( os.getcwd(), 'make.log' )
    printer = Printer( multiprocessing.Lock(), logfile, start_new_file=True );
    toolchain.set_printer( printer )

    # Query External Package handling mode
    if ( arguments['--qry-model'] ):
        printer.output( "External Package Model: " + NQBP_XPKG_MODEL() )
        sys.exit()
        
    # Does the specified variant exist
    if ( arguments['--try'] != None ):
        if ( not arguments['--try'] in toolchain.get_variants() ):
            printer.output( "Tried (and failed) to build a non-existent variant: {}".format(arguments['--try'] ))  
            sys.exit()

        # If the variant exists--> set build variant option
        else:
            arguments['-b'] =  arguments['--try']

    # Set default build variant 
    if ( not arguments['-b'] ):
        arguments['-b'] = toolchain.get_default_variant()
    
    
    # Pre-build steps
    pre_build_steps( printer, toolchain, arguments )
    printer.debug( str(arguments) )
    
    # Process 'non-build' options
    if ( arguments['--qry-blds'] ):
        toolchain.list_variants()
        sys.exit()
    
    if ( arguments['--qry'] ):
        printer.output( NQBP_PRJ_DIR() )
        sys.exit()
       
    if ( arguments['--qry-and-clean'] ):
        printer.output( NQBP_PRJ_DIR() )
        toolchain.clean_all( arguments, silent=True )
        printer.remove_log_file()
        sys.exit()
    
    if ( arguments['--clean-all'] ):
        toolchain.clean_all( arguments )
        printer.remove_log_file()
        sys.exit()
        
    # Validate Compiler toolchain is set properly (ONLY after non-build options have been processed, i.e. don't have to have an 'active' toolchain for non-build options to work)
    toolchain.validate_cc()
        
    # Start the selected build(s)
    if ( arguments['--bld-all'] ):
        for b in toolchain.get_variants():
            if ( not b.startswith("_") ):
                do_build( printer, toolchain, arguments, b )
    else: 
        do_build( printer, toolchain, arguments, arguments['-b'] )        
            
           

#-----------------------------------------------------------------------------
def do_build( printer, toolchain, arguments, variant ):
    # Set the build variant (Note: The method constructs the libdirs.b directory list)
    toolchain.pre_build( variant, arguments )

    # Output start banner
    if ( arguments['--qry-dirs'] == False ):
        start_banner(printer, toolchain)
     
    # Spit out handy-dandy debug info
    printer.debug( '# NQBP version   = ' + NQBP_VERSION() )
    printer.debug( '# NQBP_BIN       = ' + os.path.dirname(os.path.abspath(__file__)) )
    printer.debug( '# NQBP_WORK_ROOT = ' + NQBP_WORK_ROOT() )
    printer.debug( '# NQBP_PKG_ROOT  = ' + NQBP_PKG_ROOT() )
    printer.debug( '# Project Dir    = ' + NQBP_PRJ_DIR() )
    
    # Display my full set of directories
    if ( arguments['--qry-dirs'] ):
        for dir, flag in toolchain.libdirs:
            d,s,sl = dir
            printer.output( "{:<5}  {}".format( str(flag), d)  )
        return

    # Display my full set of directories with Source include/exclude info
    if ( arguments['--qry-dirs2'] ):
        for dir, flag in toolchain.libdirs:
            d,s,sl = dir
            if ( s == None or sl == None ):
                printer.output( "{:<5}  {}".format( str(flag), d)  )
            else:
                printer.output( "{:<5}  {}  {}{}{} {} ".format( str(flag), d, s,s,s, sl)  )
        return
    
    # Set default operations
    clean_pkg = True
    clean_ext = True
    clean_abs = True
    bld_prj   = True
    do_link   = True
    bld_libs  = True
    stop      = False
    
    # Skip cleaning when selective building of libdirs.b
    if ( arguments['-p'] or arguments['-x'] or arguments['-s']  or arguments['-e'] or arguments['--noabs'] or arguments['-q'] or arguments['-Q'] or arguments['-c'] or arguments['-C']):
        clean_pkg = clean_ext = clean_abs = False
        
    # Compile only a single file    
    if ( arguments['-f'] ):
        clean_pkg = clean_ext = clean_abs = bld_prj = do_link = bld_libs = False
        build_single_file( printer, arguments, toolchain )
        
    # Don't automatically build the project directory OR link when using the -s -e -q -Q options
    if ( arguments['-s']  or arguments['-e'] or arguments['-q'] or arguments['-Q'] or arguments['-c'] or arguments['-C']):
        bld_prj = do_link = False

    # Compile only a single directory    
    if ( arguments['-d'] ):
        clean_pkg = clean_ext = clean_abs = bld_prj = do_link = bld_libs = False
        dir_path  = utils.standardize_dir_sep( arguments['-d'] )
        entry     = 'local'

        # Trap directory is relative to the workspace root
        if ( dir_path.startswith(os.sep+os.sep) ):
            dir_path = dir_path[2:]
            entry    = 'xpkg'
                                              
        # Trap directory is relative to the package root
        elif ( dir_path.startswith(os.sep) ):
            dir_path = dir_path[1:]
            entry    = 'pkg'
            
        # special case of relative to the workspace
        elif ( dir_path.startswith(NQBP_WRKPKGS_DIRNAME()) ):
            dir_path = dir_path[len(NQBP_WRKPKGS_DIRNAME())+1:]
            entry    = 'xpkg'
        
       # special case of absolute directory that was already built, e.g. nqbp -d __abs\c\my\absolute\path
        elif ( dir_path.startswith("__abs") ):
            entry    = 'absolute'
            dir_path = dir_path[len("__abs")+1:]
            if ( dir_path[1] == os.sep ):
                dir_path = dir_path[0:1] + ':' + dir_path[1:]

        # Trap absolute/environment variable directory
        elif ( dir_path.startswith('$') ):
            dir_path = utils.replace_environ_variable( printer, dir_path )
            entry    = "absolute"

        # Attempt to find the specified entry in the libdirs list to GET the 'source list' for the specified directory
        found,dir,entry = utils.find_libdir_entry( toolchain.libdirs, dir_path, entry_type=entry )
        build_single_directory( printer, arguments, toolchain, dir, entry, NQBP_PKG_ROOT(), NQBP_WORK_ROOT(), NQBP_WRKPKGS_DIRNAME(), NQBP_PRJ_DIR(), NQBP_PRE_PROCESS_SCRIPT(), NQBP_PRE_PROCESS_SCRIPT_ARGS() )

    # Trap compile just the project directory
    if ( arguments['-m'] ):
        clean_pkg = clean_ext = clean_abs = do_link = bld_libs = False
        bld_prj   = True
    
    # Trap link only flag
    if ( arguments['-l'] ):
        do_link   = True
        clean_pkg = clean_ext = clean_abs = bld_libs = bld_prj = False

        # fix race condition between the -l and -m options
        if ( arguments['-m'] or arguments['-x'] or arguments['-p'] ):
            bld_prj = True
            
        # fix race condition between the -l and -px|-s|-e options
        if ( arguments['-x'] or arguments['-p'] or arguments['-s'] or arguments['-e'] or arguments['-q'] or arguments['-Q'] or arguments['-c'] or arguments['-C']):
            bld_libs = True

    # Trap the clean options 
    if ( arguments['--noabs'] ):
        clean_abs = False
    if ( arguments['-k'] ):
        clean_pkg = True
        if ( not arguments['-j'] ):
            clean_ext = False
    if ( arguments['-j'] ):
        clean_ext = True
        if ( not arguments['-k'] ):
            clean_pkg = False
        
            
                        
    # Clean before the build starts
    toolchain.clean( clean_pkg, clean_ext, clean_abs )
    
    # Build libdirs.b
    stopped = False
    if ( bld_libs ):
        
        # Filter directories when '-s' option is used
        startdir = None
        if ( arguments['-s'] ):
            startdir = utils.standardize_dir_sep( arguments['-s'] )
            
            # Trap special case of using 'xpkgs' directory
            if ( startdir.startswith(NQBP_WRKPKGS_DIRNAME()) ):
                startdir = os.sep + startdir[len(NQBP_WRKPKGS_DIRNAME())+1:]
        
           # special case of absolute directory that was already built, e.g. nqbp -s __abs\c\my\absolute\path
            elif ( startdir.startswith("__abs") ):
                startdir = startdir[len("__abs")+1:]
                if ( startdir[1] == os.sep ):
                    startdir = startdir[0:1] + ':' + startdir[1:]

            # expand any environment variables
            startdir = utils.replace_environ_variable(printer, startdir)

            # Print out debug info...
            printer.debug( '# startdir   = ' + startdir )

        # Filter directories when '-e' option is used
        stopdir = None
        if ( arguments['-e'] ):
            stopdir = utils.standardize_dir_sep( arguments['-e'] )
            stopped = True

            # Trap special case of using 'xpkgs' directory
            if ( stopdir.startswith(NQBP_WRKPKGS_DIRNAME()) ):
                stopdir = os.sep + stopdir[len(NQBP_WRKPKGS_DIRNAME())+1:]
            
            # special case of absolute directory that was already built, e.g. nqbp -e __abs\c\my\absolute\path
            elif ( stopdir.startswith("__abs") ):
                stopdir = stopdir[len("__abs")+1:]
                if ( stopdir[1] == os.sep ):
                    stopdir = stopdir[0:1] + ':' + stopdir[1:]     
   
            # expand any environment variables
            stopdir = utils.replace_environ_variable(printer, stopdir)

            # Print out debug info...
            printer.debug( '# stopdir    = ' + stopdir )

        # Apply the start/end filters
        build,skip = filter_dir_list( printer, toolchain.libdirs, startdir, stopdir )

        # Display directories being skipped
        if ( skip != None ): 
            for d in skip:
                printer.output( "= Skippping directory: " + d )

        # Build directories    
        if ( build != None ):
            # Build one directory at time
            if ( arguments['-1'] or not arguments['--turbo'] ):
                for d in build:
                    process_entry_build_directory( printer, arguments, toolchain, d[0], d[1], NQBP_PKG_ROOT(), NQBP_WORK_ROOT(), NQBP_WRKPKGS_DIRNAME(), NQBP_PRJ_DIR(), NQBP_PRE_PROCESS_SCRIPT(), NQBP_PRE_PROCESS_SCRIPT_ARGS() )

            # Build multiple directories at the same time (limited to building at most 2 directories at a time)
            else:
                # Housekeeping
                max     = len(build)
                index   = 0
                busy    = 0
                cpus    = multiprocessing.cpu_count() // 2 + 1 
                handles = []
                for h in range(0,cpus):
                    handles.append( None )

                # Process 'n' directories at a time
                while( index < max or busy > 0 ):
                    # Start multiple processes
                    for i in range(0, cpus):
                        if ( handles[i] == None and index < max ):
                            d,e        = build[index]
                            index     += 1
                            busy      += 1
                            handles[i] = Process(target=process_entry_build_directory, args=(printer, arguments, toolchain, d, e, NQBP_PKG_ROOT(), NQBP_WORK_ROOT(), NQBP_WRKPKGS_DIRNAME(), NQBP_PRJ_DIR(), NQBP_PRE_PROCESS_SCRIPT(), NQBP_PRE_PROCESS_SCRIPT_ARGS() ))
                            handles[i].start()

                    # Poll for processes being done
                    for i in range(0, cpus):
                        if ( handles[i] != None and not handles[i].is_alive() ):
                            if ( handles[i].exitcode != 0 ):
                                exit( handles[i].exitcode )
                            handles[i] = None
                            busy      -= 1

                    # sleep for 10ms before polling to see if a process has completed
                    if ( busy >= cpus ):
                        time.sleep( 0.010 )

    
    # Build project dir
    if ( bld_prj and not stopped ):
        # Banner 
        printer.output( "=====================" )
        printer.output( "= Building Project Directory:" )
    
        # Check/run the PreProcessing script
        utils.run_pre_processing_script( printer, NQBP_PRJ_DIR(), NQBP_WORK_ROOT(), NQBP_PKG_ROOT(), NQBP_PRJ_DIR(), NQBP_PRE_PROCESS_SCRIPT(), NQBP_PRE_PROCESS_SCRIPT_ARGS(),  verbose=arguments['-v'] )
        
        # check for existing 'sources.b' file 
        files = utils.get_files_to_build( printer, toolchain, '.', NQBP_NAME_SOURCES() )

        # compile files
        for f in files:
            toolchain.cc( arguments, '.' + os.sep + f )
    
    # Peform link
    if ( do_link ):
       inf = open( NQBP_NAME_LIBDIRS(), 'r' )
       link_libdirs = toolchain.pre_link( arguments, inf, 'local', variant )
       inf.close()
       
       toolchain.link( arguments, link_libdirs, 'local', variant )
                            
    # Output end banner
    end_banner(printer, toolchain)
     
           
# Internal helper method to invoke building a single directory
def process_entry_build_directory( printer, arguments, toolchain, dir, entry, pkg_root, work_root, pkgs_dirname, prj_dirname, preprocess_script, preprocess_args ):
    build_single_directory( printer, arguments, toolchain, dir, entry, pkg_root, work_root, pkgs_dirname, prj_dirname, preprocess_script, preprocess_args )


#-----------------------------------------------------------------------------
def pre_build_steps(printer, toolchain, arguments ):

    # Enable verbose logging (if selected)
    if ( arguments['-v'] ):
        printer.enable_verbose()
    
    # Enable debugging logging (if selected) note: order is important here -->must follow enable.verbose()    
    if ( arguments['--debug'] ):
        printer.enable_debug()
        
        
            
#-----------------------------------------------------------------------------
def start_banner(printer, toolchain):

    # 
    start = int( toolchain.get_build_time() ) 
    printer.output('')
    printer.output( '=' * 80 );
    printer.output( '= START of build for:  {}'.format( toolchain.get_final_output_name()) )
    printer.output( '= Project Directory:   {}'.format( NQBP_PRJ_DIR() ))
    printer.output( '= Toolchain:           {}'.format( toolchain.get_ccname()) )
    printer.output( '= Build Configuration: {}'.format( toolchain.get_build_variant()) )
    printer.output( '= Begin (UTC):         {}'.format( time.strftime("%a, %d %b %Y %H:%M:%S", time.gmtime())) )
    printer.output( '= Build Time:          {:d} ({:x})'.format( start, start ) )
    printer.output( '=' * 80 );

        
#-----------------------------------------------------------------------------
def end_banner(printer, toolchain):

    # Calculate elasped time in hhh mm ss    
    elasped = int(time.time() - toolchain.get_build_time()) 
    mm      = (elasped // 60) % 60
    hhh     = elasped // (60*60)
    ss      = elasped % 60
    
    # 
    printer.output( '=' * 80 );
    printer.output( '= END of build for:    {}'.format( toolchain.get_final_output_name()) )
    printer.output( '= Project Directory:   {}'.format( NQBP_PRJ_DIR() ))
    printer.output( '= Toolchain:           {}'.format( toolchain.get_ccname()) )
    printer.output( '= Build Configuration: {}'.format( toolchain.get_build_variant()) )
    printer.output( '= Elapsed Time (hh mm:ss): {:02d} {:02d}:{:02d}'.format(hhh, mm, ss) )
    printer.output( '=' * 80 );

    
#-----------------------------------------------------------------------------
def build_single_file( printer, arguments, toolchain ):

    # Get file to compile
    is_project_dir = False
    fname          = utils.standardize_dir_sep( arguments['-f'] )
    srcpath        = NQBP_PKG_ROOT() + os.sep
    
    # Trap that file is relative to package root
    if ( fname.startswith(os.sep) ):
        fname   = fname[1:]
        srcpath = NQBP_PKG_ROOT() + os.sep
            
    # Trap that file is relative to workspace root
    elif ( fname.startswith(os.sep+os.sep) ):
        fname   = fname[2:]
        srcpath = NQBP_WORK_ROOT() + os.sep
            
    # Trap that file is relative to the build directory
    elif ( fname.startswith('.') ):
        srcpath        = os.getcwd() + os.sep
        is_project_dir = True
        
    # create object directory 
    if ( is_project_dir ):
        dir = os.getcwd()
    else:
        dir = utils.create_subdirectory_from_file( printer, os.getcwd(), fname )
    
    # call toolchain compile method
    utils.push_dir( dir )
    toolchain.cc( arguments, srcpath + fname )

    # build archive (when not compiling a file in the project directory)
    if ( not is_project_dir ):
        toolchain.ar( arguments )
    utils.pop_dir()
    
#-----------------------------------------------------------------------------
def filter_dir_list( printer, fulllist, startdir, stopdir ):
    # Do nothing if building all libraries
    if ( startdir == None and stopdir == None ):
        return fulllist, None
    
    # Housekeeping
    skipping  = True if startdir != None else False
    stopped   = False
    appending = True
    buildlist = []
    skiplist  = []
    

    # Filter the list...
    for d,e in fulllist:
        # convert dirname from the list to match the format of startdir/stopdir
        thisdir = d[0]
        if ( e != 'local' and e != 'pkg' and e!= 'absolute'):
            thisdir = os.sep + thisdir

        # Match starting directory
        if ( skipping ):
            if ( startdir != thisdir ):
                skiplist.append( thisdir )
                continue;    
            else:
                skipping = False
        
        # add directory to the filtered list
        if ( appending ):
            buildlist.append( (d,e) )

        # Handle the stop option
        if ( stopdir != None ):
            if ( stopped ):
                skiplist.append( thisdir )
                
            if ( stopdir == thisdir ):
                stopped   = True
                appending = False

    # return the filtered and skipped lists
    return buildlist, skiplist

#-----------------------------------------------------------------------------
def build_single_directory( printer, arguments, toolchain, dir, entry, pkg_root, work_root, pkgs_dirname, prj_dirname, preprocess_script, preprocess_args ):
   
    srcpath, display, dir = utils.derive_src_path( pkg_root, work_root, pkgs_dirname, entry, dir )

    # Hack for legacy/Mixed model
    if ( NQBP_XPKG_MODEL() != NQBP_XPKG_MODEL_OUTCAST() ):
        badpath = pkgs_dirname + os.sep + pkgs_dirname
        idx = srcpath.find( badpath )
        if ( idx > 0 ):
            srcpath = pkg_root + os.sep + pkgs_dirname + srcpath[idx+len(badpath):] 
        idx = dir[0].find( badpath )
        if ( idx > 0 ):
            dir[0] = pkgs_dirname + os.sep + dir[0][idx+len(badpath):] 
    
    # Banner 
    printer.output( "=====================" )
    printer.output( "= Building Directory: " + display )

    # Debug info
    printer.debug( "#   entry  = {}".format( entry ) )
    printer.debug( "#   objdir = {}".format( dir[0] ) )
    printer.debug( "#   srcdir = {}".format( srcpath ) )

    
    # verify source directory exists
    if ( not os.path.exists( srcpath ) ):
        printer.output( "")
        printer.output( "ERROR: Build Failed - directory does not exist: " )
        printer.output( "" )
        sys.exit(1)
        
    # create object directory 
    d = utils.create_subdirectory( printer, os.getcwd(), dir[0] )
    utils.push_dir( d )

    # Check/run the PreProcessing script
    utils.run_pre_processing_script( printer, srcpath, work_root, pkg_root, prj_dirname, preprocess_script, preprocess_args, verbose=arguments['-v'] )

    # Get/Construct the source file list and filter it (if needed) for the specified directory
    files = utils.get_and_filter_files_to_build( printer, toolchain, dir, srcpath, NQBP_NAME_SOURCES() )
    
    # compile using a single process
    if ( arguments['-1'] ):
        for f in files:
            process_entry_cc( toolchain, arguments, srcpath + os.sep + f )

    # compile files using multiple processes (one process per file)
    else:
        p = []
        for f in files:
            hdl = Process(target=process_entry_cc, args=(toolchain, arguments, srcpath + os.sep + f))
            p.append( hdl )
            hdl.start()
            
        # Wait for all files to be compiled and exit if there was an error
        for f in p:
            f.join()
            if ( f.exitcode != 0 ):
                exit( f.exitcode )
        
    # build archive
    toolchain.ar( arguments )
    utils.pop_dir()

# Internal helper method to invoke the CC operation on my toolchain
def process_entry_cc( toolchain, arguments, path ):
    toolchain.cc( arguments, path )
        
