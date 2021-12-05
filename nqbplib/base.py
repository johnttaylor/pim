#!/usr/bin/python3
"""Base toolchain functions and classes

Magic Symbols
--------------------
ME_CC_BASE_FILENAME - A toolchain can use the text symbol: ME_CC_BASE_FILENAME
                      as a place holder for the file currently being built.  For
                      Example the following compiler option will generate a list
                      file with the base name the same as the C file being
                      compiled.

                      self._base_release.cflags = self._base_release.cflags + '-Wa,-alhs=ME_CC_BASE_FILENAME.lst '

_BUILT_DIR_         - The build fields .firstobjs and .lastobjs, can be set (in
                      mytoolchain.py file) to the text symbol - _BUILT_DIR.aaaa
                      where 'aaaa' is a directory (as specified in libdirs.b).  
                      The symbol will be replaced at link time to the list of 
                      objects files in from the 'aaaaa' directory.  The list of 
                      objects files will be 'correct' with respect to any 'source list' 
                      specified in the libdirs.b for the 'aaaa' directory.  For 
                      example the line below will expand to the list of all of 
                      the object files in the src\Cpl\Container\_0test directory

                      base_release.firstobjs = _BUILD_DIR_.src\Cpl\Container\_0test 
                      
                      Note: This functionality is required because the WINDOZE 
                            command shell does NOT process wildcards like a 
                            *nix shell.  

"""

#
import logging
import time
import os
import shutil
import sys
import subprocess

#
from . import utils

# Globals
from .my_globals import NQBP_WORK_ROOT
from .my_globals import NQBP_PKG_ROOT
from .my_globals import NQBP_TEMP_EXT
from .my_globals import NQBP_VERSION
from .my_globals import NQBP_PRJ_DIR
from .my_globals import NQBP_WRKPKGS_DIRNAME
from .my_globals import NQBP_PUBLICAPI_DIRNAME
from .my_globals import NQBP_NAME_LIBDIRS
from .my_globals import NQBP_XPKG_MODEL
from .my_globals import NQBP_XPKG_MODEL_OUTCAST
from .my_globals import NQBP_XPKG_MODEL_MIXED


# Structure for holding build-variant specific options
class BuildValues:
    def __init__(self):
        self.inc          = ''
        self.asminc       = ''
        self.cflags       = ''
        self.c_only_flags = ''
        self.cppflags     = ''
        self.asmflags     = ''
        self.linkflags    = ''
        self.linklibs     = ''
        self.linkscript   = ''
        self.firstobjs    = ''
        self.lastobjs     = ''

    def append(self,src):
        self.inc          += ' ' + src.inc        
        self.asminc       += ' ' + src.asminc           
        self.cflags       += ' ' + src.cflags           
        self.c_only_flags += ' ' + src.c_only_flags
        self.cppflags     += ' ' + src.cppflags         
        self.asmflags     += ' ' + src.asmflags         
        self.linkflags    += ' ' + src.linkflags        
        self.linklibs     += ' ' + src.linklibs 
        self.linkscript   += ' ' + src.linkscript 
        self.firstobjs    += ' ' + src.firstobjs 
        self.lastobjs     += ' ' + src.lastobjs 
  
        
    def copy(self):
        new = BuildValues()
        new.inc          = self.inc        
        new.asminc       = self.asminc           
        new.cflags       = self.cflags           
        new.c_only_flags = self.c_only_flags
        new.cppflags     = self.cppflags         
        new.asmflags     = self.asmflags         
        new.linkflags    = self.linkflags        
        new.linklibs     = self.linklibs 
        new.linkscript   = self.linkscript 
        new.firstobjs    = self.firstobjs 
        new.lastobjs     = self.lastobjs 
       
        return new
            

#==============================================================================
class ToolChain:
    """ Base ToolChain Class"""

    #--------------------------------------------------------------------------
    def __init__( self, exename, prjdir, build_variants, default_variant ):
        
        # Housekeeping -->set some global vars    
        NQBP_PRJ_DIR( prjdir )
        
        # Public members
        self.libdirs = []
        
        # Private members
        self._bld_variants               = build_variants
        self._build_time_utc             = int(time.time())
        self._cflag_symdef               = '-D'
        self._asmflag_symdef             = '-D'
        self._cflag_symvalue_delimiter   = ''
        self._asmflag_symvalue_delimiter = ''
        self._printer                    = None
        
        # Tools
        self._ccname   = 'Generic GCC'
        self._cc       = 'gcc'  
        self._ld       = 'gcc'  
        self._asm      = 'as'   
        self._ar       = 'ar'   
        self._objcpy   = 'objcpy'
        
        self._obj_ext  = 'o'    
        self._asm_ext  = 's'    
        
        self._echo_cc  = True 
        self._echo_asm = True
        
        self._validate_cc_options = '-v'
        
        self._clean_list     = ['o', 'd', 'lst', 'txt', 'map', 'obj', 'idb', 'pdb', 'out', 'pyc', NQBP_TEMP_EXT(), 'gcda', 'gcov', 'gcno', 'tmp' ]
        self._clean_pkg_dirs = [ 'src' ]
        self._clean_ext_dirs = [ NQBP_WRKPKGS_DIRNAME() ]
        self._clean_abs_dirs = [ '__abs' ]
 
        self._ar_library_name = 'library.a'
        self._ar_options      = 'rc ' + self._ar_library_name
        
        self._link_lib_prefix       = ''
        self._linker_libgroup_start = '-Wl,--start-group'
        self._linker_libgroup_end   = '-Wl,--end-group'
        
        self._final_output_name = exename
        self._link_output       = '-o ' + exename
        


        #
        # Build Config/Variant: "release"
        #
        
        # Common/base options, flags, etc.
        # Notes:
        #   o Order of the header file include path should be in the 
        #     following order (i.e. first directory searched)
        #       - Current directory
        #       - Package source directory
        #       - Project directory                 
        #       - Workspace Public Include directory
        #       - Legacy 'xsrc' Third party source tree
        #
        self._base_release = BuildValues()
        self._base_release.inc       = '-I. -I{}{}src  -I{} -I{}'.format(NQBP_PKG_ROOT(),os.sep, prjdir, NQBP_PUBLICAPI_DIRNAME()  )
        self._base_release.asminc    = self._base_release.inc
        self._base_release.cflags    = '-c -DBUILD_TIME_UTC={:d} '.format(self._build_time_utc)
        self._base_release.asmflags  = self._base_release.cflags
        #self._base_release.linklibs  = '-L lib -Wl,-lstdc++'
        self._base_release.linklibs  = '-Wl,-lstdc++ -Wl,-lm'
        
        # Optimized options, flags, etc.
        self._optimized_release = BuildValues()
        
        # Debug options, flags, etc.
        self._debug_release = BuildValues()
        self._debug_release.cflags = '-g -DDEBUG_BUILD'
        

        # Update dictionary of build variants
        self._bld = default_variant
        self._bld_variants[self._bld]['base']      = self._base_release
        self._bld_variants[self._bld]['optimized'] = self._optimized_release
        self._bld_variants[self._bld]['debug']     = self._debug_release
 
        
    
    #--------------------------------------------------------------------------
    def set_printer(self, printer):
        self._printer = printer
        
    #--------------------------------------------------------------------------
    def get_default_variant(self):
        return self._bld
        
        
    #--------------------------------------------------------------------------
    def list_variants(self):
        self._printer.output( 'Available Build Configurations/Variants:' )
        for key in self._bld_variants.keys():
            marker = ' *' if key == self._bld else ''
            self._printer.output( '  {}{}'.format(key,marker) )
           
        
    
    #--------------------------------------------------------------------------
    def clean(self, pkg, ext, abs, silent=False, blddir=True):
        if ( blddir == True ):
            if ( not silent ):
                self._printer.output( "= Cleaning Built artifacts..." )
            # remove output/build variant directory
            vardir = '_' + self._bld
            self._printer.debug( '# Cleaning directory: {}'.format( vardir ) )
            if ( os.path.exists(vardir) ):
                shutil.rmtree( vardir, True )   

        if ( pkg == True ):
            if ( not silent ):
                self._printer.output( "= Cleaning Project and local Package derived objects..." )
            self._printer.debug( '# Cleaning file extensions: {}'.format( self._clean_list ) )
            utils.run_clean_dir_pre_processing( NQBP_PRJ_DIR(), self._printer )
            utils.run_clean_pre_processing( self._printer, self.libdirs, clean_pkg=True, clean_local=True )
            utils.del_files_by_ext( NQBP_PRJ_DIR(), self._clean_list )

            self._printer.debug( '# Cleaning directories: {}'.format( self._clean_pkg_dirs ) )
            for d in self._clean_pkg_dirs:
                if ( os.path.exists(d) ):
                    shutil.rmtree( d, True )

        if ( ext == True ):
            if ( not silent ):
                self._printer.output( "= Cleaning External Package derived objects..." )
            self._printer.debug( '# Cleaning directories: {}'.format( self._clean_ext_dirs ) )
            utils.run_clean_pre_processing( self._printer, self.libdirs, clean_xpkgs=True )
            for d in self._clean_ext_dirs:
                if ( os.path.exists(d) ):
                    shutil.rmtree( d, True )

        if ( abs == True ):
            if ( not silent ):
                self._printer.output( "= Cleaning Absolute Path derived objects..." )
            self._printer.debug( '# Cleaning directories: {}'.format( self._clean_abs_dirs ) )
            utils.run_clean_pre_processing( self._printer, self.libdirs, clean_absolute=True )
            for d in self._clean_abs_dirs:
                if ( os.path.exists(d) ):
                    shutil.rmtree( d, True )
                
 
    #--------------------------------------------------------------------------
    def clean_all( self, arguments, silent=False ):            
        for b in self.get_variants():
            if ( not silent ):
                self._printer.output( "=====================" )
                self._printer.output( "= Build Variant: " + b )
            self.pre_build( b, arguments )
            self.clean(True,True,True,silent)
    

    #--------------------------------------------------------------------------
    def get_final_output_name(self):
        return self._final_output_name
                                                                             
    #--------------------------------------------------------------------------
    def get_build_time(self):
        return self._build_time_utc
        
    #--------------------------------------------------------------------------
    def get_build_variant(self):
        return self._bld
    
    #--------------------------------------------------------------------------
    def get_variants(self):
        return list(self._bld_variants.keys())
            
    #--------------------------------------------------------------------------
    def get_ccname(self):
        return self._ccname
    
    #--------------------------------------------------------------------------
    def get_asm_extensions(self):
        extlist = [ self._asm_ext ]
        return extlist
        

    def is_asm_file( self, fullname ):
        for e in self.get_asm_extensions():                 
            if ( fullname.endswith(e) ):
                return True
           
        # If I get here there was not match   
        return False
        
    #--------------------------------------------------------------------------
    def pre_build(self, bld_var, arguments ):
        self._printer.debug( '# ENTER: base.ToolChain.pre_build' )
        
        # Select/set build variant
        self._bld = bld_var
        if ( bld_var not in self._bld_variants ):
            self._printer.output( 'ERROR: Invalid variant ({}) selected'.format( bld_var ) )
            sys.exit(1)
            
        self._printer.debug( '# Build Variant "{}" selected'.format( self._bld ) )
        if ( arguments['--debug'] ):
            self._dump_variants( self._bld_variants )
                      
        c_self_defines   = ""
        asm_self_defines = ""
        if ( arguments['--def1'] != None ):
            c_self_defines   += self._format_custom_c_define( arguments['--def1'] )
            asm_self_defines += self._format_custom_asm_define( arguments['--def1'] )
        if ( arguments['--def2'] != None ):
            c_self_defines   += self._format_custom_c_define( arguments['--def2'] )
            asm_self_defines += self._format_custom_asm_define( arguments['--def2'] )
        if ( arguments['--def3'] != None ):
            c_self_defines   += self._format_custom_c_define( arguments['--def3'] )
            asm_self_defines += self._format_custom_asm_define( arguments['--def3'] )
        if ( arguments['--def4'] != None ):
            c_self_defines   += self._format_custom_c_define( arguments['--def3'] )
            asm_self_defines += self._format_custom_asm_define( arguments['--def4'] )
        if ( arguments['--def5'] != None ):
            c_self_defines   += self._format_custom_c_define( arguments['--def5'] )
            asm_self_defines += self._format_custom_asm_define( arguments['--def5'] )

        # Construct build options
        null           = BuildValues()
        bld            = self._bld_variants[self._bld] 
        base           = bld.get('base', self._base_release ).copy() # default to the release 'base' if not defined
        base.cflags   += " {}BUILD_VARIANT_{} {}{}BUILD_NUMBER={}{} {}".format(self._cflag_symdef,  self._bld.upper(), self._cflag_symdef,   self._cflag_symvalue_delimiter,   arguments['--bldnum'],  self._cflag_symvalue_delimiter, c_self_defines );
        base.asmflags += " {}BUILD_VARIANT_{} {}{}BUILD_NUMBER={}{} {}".format(self._asmflag_symdef,self._bld.upper(), self._asmflag_symdef, self._asmflag_symvalue_delimiter, arguments['--bldnum'],  self._asmflag_symvalue_delimiter, asm_self_defines );
#        self._all_opts = base
#        self._all_opts.append( bld.get('user_base', null ) )
        self._all_opts = bld.get('user_base', null )
        self._all_opts.append( base )
       
        
        if ( arguments['-g'] ):
            self._all_opts.append( bld.get('debug', self._debug_release ) )
            self._all_opts.append( bld.get('user_debug', null ) )
        else:
            self._all_opts.append( bld.get('optimized', self._optimized_release ) )
            self._all_opts.append( bld.get('user_optimized', null ) )
            
        if ( arguments['--debug'] ):
            self._printer.debug( "# Final 'all_opts'" )
            self._dump_options(  self._all_opts, True )

        # Replace the xpkgs/ directory with xsrc/ when NOT using the Outcast model
        if ( NQBP_XPKG_MODEL() == NQBP_XPKG_MODEL_MIXED() ):
            xsrc                    = NQBP_PKG_ROOT() + os.sep + NQBP_WRKPKGS_DIRNAME() + os.sep
            self._all_opts.inc    = self._all_opts.inc.replace( NQBP_WORK_ROOT() + '/xpkgs/', xsrc )
            self._all_opts.inc    = self._all_opts.inc.replace( NQBP_WORK_ROOT() +'\\xpkgs\\', xsrc )
            self._all_opts.asminc = self._all_opts.asminc.replace( NQBP_WORK_ROOT() + '/xpkgs/', xsrc )
            self._all_opts.asminc = self._all_opts.asminc.replace( NQBP_WORK_ROOT() +'\\xpkgs\\', xsrc )
            
        # Create the list of directories from libdirs.b file to run the pre-processing clean script against
        self.libdirs  = []
        self.libnames = []
        if ( not arguments['--clean-all'] ):        # Skip if doing a --clean-all
            inf = open( NQBP_NAME_LIBDIRS(), 'r' )
            utils.create_working_libdirs( self._printer, inf, arguments, self.libdirs, self.libnames, 'local', bld_var )  
            inf.close()

    #--------------------------------------------------------------------------
    def ar( self, arguments ):
        # NOTE: Assumes archive is to be built in the current working dir
        self._printer.output("=" )
        self._printer.output("= Archiving: {}".format( self._ar_library_name) )
        
        # remove existing archive
        utils.delete_file( self._ar_library_name )
        
        # Get all object files
        objs = utils.dir_list_filter_by_ext( ".", [self._obj_ext], derivedDir=True )
       
        # build archive string
        cmd = self._ar + ' ' + self._ar_options
        for o in objs:
            cmd = cmd + ' ' + o
            
        # run command            
        if ( arguments['-v'] ):
            self._printer.output( cmd )
        if (utils.run_shell(self._printer, cmd) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: archiver/librarian error")
            self._printer.output("=")
            sys.exit(1)
        
                   
    #--------------------------------------------------------------------------
    def validate_cc( self ):
        cc = self._cc + ' ' + self._validate_cc_options
        self._printer.debug( '# Validating the Compiler using:: {}'.format( cc ) )
        p  = subprocess.Popen( cc, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
        r  = p.communicate()
        if ( p.returncode ):
            self._printer.output( "ERROR: Cannot validate toolchain ({}) - check your path/environment variable(s)".format( self._ccname ) )
            sys.exit(1)
      
        return r

    #--------------------------------------------------------------------------
    def cc( self, arguments, fullname ):
    
        # parse incoming name into its base
        basename = os.path.splitext( os.path.basename( fullname ) )[0]
        
        # construct compiler/assembler command
        cc_base = ' {} {} {} '.format( self._cc,
                                     self._all_opts.cflags,
                                     self._all_opts.inc
                                   )
        cc = cc_base + self._all_opts.c_only_flags

        if ( fullname.endswith('.c') ):
            pass
        elif ( fullname.endswith('.cpp') ):
            cc = ' {} {} '.format(cc_base, self._all_opts.cppflags)
        elif ( self.is_asm_file(fullname) ):
            cc = ' {} {} {} '.format( self._asm,
                                    self._all_opts.asmflags,
                                    self._all_opts.asminc
                                  )
        else:
            self._printer.output( "ERROR: No rule to compile the file: {}".format( os.path.basename(fullname) ) )
            sys.exit(1)

        # Do any substitution of the 'ME_xxx' values
        cc = cc.replace( 'ME_CC_BASE_FILENAME', basename )
        

        # ensure correct directory separator                                
        full_fname = utils.standardize_dir_sep( fullname )

        cc += ' ' + full_fname
        
        # Output Progress...
        if ( self._echo_cc ):
            self._printer.output("= Compiling: " + os.path.basename(fullname) )
        if ( arguments['-v'] ):
            self._printer.output( cc )
        
        # do the compile
        if ( utils.run_shell(self._printer, cc) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: compiler error")
            self._printer.output("=")
            sys.exit(1)



    #--------------------------------------------------------------------------
    #
    def pre_link(self, arguments, inf, local_external_setting, variant ):
        self._printer.debug( '# ENTER: base.ToolChain.pre_link' )
        
        # Set my command options to construct an 'all' libdirs list
        libdirs  = []
        libnames = []
        myargs   = { '-p':False, '-x':False, '-b':arguments['-b'], '--noabs':False, '-q':None, '-Q':None, '-c':None, '-C':None }
        utils.create_working_libdirs( self._printer, inf, myargs, libdirs, libnames, local_external_setting, variant )  
        
        # Expand any _BUILD_DIR.aaaa symbols for .firstobjs and .lastobjs
        self._all_opts.firstobjs = utils.replace_build_dir_symbols(self,  self._all_opts.firstobjs, libdirs, ".." )
        self._all_opts.lastobjs  = utils.replace_build_dir_symbols(self,  self._all_opts.lastobjs, libdirs, ".." )
        
        # Replace the xpkgs/ directory with xsrc/ when NOT using the Outcast model
        if ( NQBP_XPKG_MODEL() == NQBP_XPKG_MODEL_MIXED() ):
            xsrc                    = NQBP_PKG_ROOT() + os.sep + NQBP_WRKPKGS_DIRNAME() + os.sep
            self._all_opts.linklibs  = self._all_opts.linklibs.replace( NQBP_WORK_ROOT() + '/xpkgs/', xsrc )
            self._all_opts.linklibs  = self._all_opts.linklibs.replace( NQBP_WORK_ROOT() +'\\xpkgs\\', xsrc )
            self._all_opts.linkflags = self._all_opts.linkflags.replace( NQBP_WORK_ROOT() + '/xpkgs/', xsrc )
            self._all_opts.linkflags = self._all_opts.linkflags.replace( NQBP_WORK_ROOT() +'\\xpkgs\\', xsrc )
        

        # Return the 'all' libdirs list
        return libdirs

    #
    def link( self, arguments, libdirs, local_external_setting, variant ):

        # Output Progress...
        self._printer.output( "=====================" )
        self._printer.output("= Linking..." )

        # create build variant output
        vardir = '_' + self._bld
        utils.create_subdirectory( self._printer, '.', vardir )
        utils.push_dir( vardir )
        
        # construct link command
        libs = self._build_library_list( libdirs )
        startgroup = self._linker_libgroup_start if libs != '' else ''
        endgroup   = self._linker_libgroup_end   if libs != '' else ''
        ld = '{} {} {} {} {} {} {} {} {} {} {}'.format( 
                                            self._ld,
                                            self._link_output,
                                            self._all_opts.firstobjs,
                                            self._build_prjobjs_list(),
                                            self._all_opts.linkflags,
                                            self._all_opts.linkscript,
                                            startgroup,
                                            libs,
                                            endgroup,
                                            self._all_opts.linklibs,
                                            self._all_opts.lastobjs
                                            )
                                          
        # do the compile
        if ( arguments['-v'] ):
            self._printer.output( ld )
        if ( utils.run_shell(self._printer, ld) ):
            self._printer.output("=")
            self._printer.output("= Build Failed: linker error")
            self._printer.output("=")
            sys.exit(1)

        # Return to project dir
        utils.pop_dir()
        
        
    #==========================================================================
    # Private Methods
    #==========================================================================
    
    #--------------------------------------------------------------------------
    def _format_custom_c_define( self, sym ):
        return self._cflag_symdef   + self._cflag_symvalue_delimiter   + sym + self._cflag_symvalue_delimiter + ' '

    def _format_custom_asm_define( self, sym ):
        return self._asmflag_symdef + self._asmflag_symvalue_delimiter + sym + self._asmflag_symvalue_delimiter + ' '
    

    #--------------------------------------------------------------------------
    def _build_prjobjs_list( self ):
        list = utils.dir_list_filter_by_ext( '..' + os.sep, [self._obj_ext], derivedDir=True )
        path = ''
        for i in list:
            path += ' ..' + os.sep + i
        
        return path
        
        
    #--------------------------------------------------------------------------
    def _build_library_list( self, libs ):
        result = ''
        for pair in libs:
            l,f  = pair
            dirname = l[0]

            path = '..' + os.sep
            if ( f == 'xpkg' ):
                path += NQBP_WRKPKGS_DIRNAME() + os.sep
            elif ( f == 'absolute' ):
                path += "__abs" + os.sep
                
            lname   = path + dirname + os.sep + self._ar_library_name    
            lname   = lname.replace(':','',1)
            result += self._link_lib_prefix + lname + ' '
            
        return result
                  
        
    #--------------------------------------------------------------------------
    def _dump_variants( self, bld_variants ):
        self._printer.debug( '# Build Variants Options' )
        for k,v in bld_variants.items():
            self._printer.debug( '#  VARIANT: ' + k )
            for sk,sv in v.items():
                if ( sk != 'nop' ):
                    self._printer.debug( '#    Options: '      + sk )
                    self._dump_options( sv )
                    
    def _dump_options( self, sv, extraSpace=False ):
        self._printer.debug( '#      inc:        ' + sv.inc        + ("\n" if extraSpace else " "))
        self._printer.debug( '#      asminc:     ' + sv.asminc     + ("\n" if extraSpace else " "))
        self._printer.debug( '#      cflags:     ' + sv.cflags     + ("\n" if extraSpace else " "))
        self._printer.debug( '#      cppflags:   ' + sv.cppflags   + ("\n" if extraSpace else " "))
        self._printer.debug( '#      asmflags:   ' + sv.asmflags   + ("\n" if extraSpace else " "))
        self._printer.debug( '#      linkflags:  ' + sv.linkflags  + ("\n" if extraSpace else " "))
        self._printer.debug( '#      linklibs:   ' + sv.linklibs   + ("\n" if extraSpace else " "))
        self._printer.debug( '#      firstobjs:  ' + sv.firstobjs  + ("\n" if extraSpace else " "))
        self._printer.debug( '#      linkscript: ' + sv.linkscript + ("\n" if extraSpace else " "))
  
                                                                         
