#!/usr/bin/python3
r"""Base toolchain functions and classes

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
from itertools import filterfalse
import logging
import time
import os
import shutil
import sys
import subprocess
import json

#
from . import utils

# Globals
from .my_globals import NQBP_WORK_ROOT
from .my_globals import NQBP_PKG_ROOT
from .my_globals import NQBP_TEMP_EXT
from .my_globals import NQBP_VERSION
from .my_globals import NQBP_PRJ_DIR
from .my_globals import NQBP_WRKPKGS_DIRNAME
from .my_globals import NQBP_XPKGS_SRC_ROOT
from .my_globals import NQBP_NAME_LIBDIRS


# Structure for holding build-variant specific options
class BuildValues:
    def __init__(self):
        self.inc            = ''
        self.asminc         = ''
        self.cflags         = ''
        self.c_only_flags   = ''
        self.cppflags       = ''
        self.asmflags       = ''
        self.linkflags      = ''
        self.linklibs       = ''
        self.firstobjs      = ''
        self.lastobjs       = ''
        self.exclude_clangd = []
        self.include_clangd = []

    def append(self,src):
        self.inc            += ' ' + src.inc
        self.asminc         += ' ' + src.asminc
        self.cflags         += ' ' + src.cflags
        self.c_only_flags   += ' ' + src.c_only_flags
        self.cppflags       += ' ' + src.cppflags
        self.asmflags       += ' ' + src.asmflags
        self.linkflags      += ' ' + src.linkflags
        self.linklibs       += ' ' + src.linklibs
        self.firstobjs      += ' ' + src.firstobjs
        self.lastobjs       += ' ' + src.lastobjs
        self.exclude_clangd.extend( src.exclude_clangd )
        self.include_clangd.extend( src.include_clangd )
  
        
    def copy(self):
        new                = BuildValues()
        new.inc            = self.inc
        new.asminc         = self.asminc
        new.cflags         = self.cflags
        new.c_only_flags   = self.c_only_flags
        new.cppflags       = self.cppflags
        new.asmflags       = self.asmflags
        new.linkflags      = self.linkflags
        new.linklibs       = self.linklibs
        new.firstobjs      = self.firstobjs
        new.lastobjs       = self.lastobjs
        new.exclude_clangd = self.exclude_clangd.copy()
        new.include_clangd = self.include_clangd.copy()
       
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
        self._compiler_option_delimiter  = '-'
        self._printer                    = None
        self._ninja_writer               = None

        # Tools
        self._ccname    = 'Generic GCC'
        self._cc        = 'gcc'  
        self._ld        = 'gcc'  
        self._asm       = 'as'   
        self._ar        = 'ar'   
        self._objcpy    = 'objcpy'
        self._objdmp    = 'objdump' 
        self._printsz   = 'size'
        self._rm        = 'rm -f'
        self._shell     = ''
        self._os_sep    = os.sep

        self._obj_ext  = 'o'    
        self._asm_ext  = 's'    
        
        self._echo_cc  = True 
        self._echo_asm = True
        
        self._validate_cc_options = '-v'
        
        self._clean_list     = []
        #self._clean_pkg_dirs = []
        #self._clean_ext_dirs = []
        #self._clean_abs_dirs = []
        # Retain cleaning NQBP classic derived directories -->help cleaning up when transition from classic to NQBP2
        self._clean_pkg_dirs = [ 'src', '__pycache__' ]
        self._clean_ext_dirs = [ NQBP_WRKPKGS_DIRNAME() ]
        self._clean_abs_dirs = [ '__abs' ]

 
        self._ar_library_name = 'library.a'
        self._ar_options      = 'crs'
        self._ar_out          = ''

        self._link_lib_prefix       = ''
        self._linker_libgroup_start = '-Wl,--start-group'
        self._linker_libgroup_end   = '-Wl,--end-group'
        
        self._final_output_name = exename
        self._link_output       = '-o '
        


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
        self._base_release                = BuildValues()
        self._base_release.inc            = '-I. -I{}{}src  -I{} -I{}'.format(NQBP_PKG_ROOT(),os.sep, prjdir, NQBP_XPKGS_SRC_ROOT()  )
        self._base_release.asminc         = self._base_release.inc
        self._base_release.cflags         = '-c '
        self._base_release.asmflags       = self._base_release.cflags
        self._base_release.linklibs       = '-Wl,-lstdc++ -Wl,-lm'
        self._base_release.exclude_clangd = ['-x c++', '-c', '-std=c++11', '-std=c++14', '-std=c++17', '-std=gnu++11', '-std=gnu++14', '-std=gnu++17']
        self._base_release.include_clangd = ['-xc++']
        
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
        
    def set_ninja_writer(self, writer):
        self._ninja_writer = writer
        

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
    def clean(self, silent=False):
        if ( not silent ):
            self._printer.output( "= Cleaning Built directory..." )

        # remove output/build variant directory
        vardir = '_' + self._bld
        self._printer.debug( '# Cleaning directory: {}'.format( vardir ) )
        if ( os.path.exists(vardir) ):
            shutil.rmtree( vardir, True )   

        # RETAIN support for cleaning nqbp class derived directories/files (helps when transition to NQBP2)
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

        if ( not silent ):
            self._printer.output( "= Cleaning External Package derived objects..." )
        self._printer.debug( '# Cleaning directories: {}'.format( self._clean_ext_dirs ) )
        utils.run_clean_pre_processing( self._printer, self.libdirs, clean_xpkgs=True )
        for d in self._clean_ext_dirs:
            if ( os.path.exists(d) ):
                shutil.rmtree( d, True )

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
            self._bld = b
            self.clean(silent)
    

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
            
        # Make sure the directory separator is correct for header includes
        self._all_opts.inc = utils.standardize_dir_sep( self._all_opts.inc, self._os_sep )
        self._all_opts.asminc = utils.standardize_dir_sep( self._all_opts.asminc, self._os_sep  )
        self._all_opts.firstobjs = utils.standardize_dir_sep( self._all_opts.firstobjs, self._os_sep  )
        self._all_opts.lastobjs = utils.standardize_dir_sep( self._all_opts.lastobjs, self._os_sep  )

        if ( arguments['--qry-opts'] ):
            self._printer.enable_debug()
            self._dump_options(  self._all_opts, True )
            sys.exit(0)
        if ( arguments['--vs'] ):
            self._create_clangd_file()
            sys.exit(0)
        if ( arguments['--vsgdb'] ):
            self._create_vs_gdbentry()
            sys.exit(0)
        if ( arguments['--debug'] ):
            self._printer.debug( "# Final 'all_opts'" )
            self._dump_options(  self._all_opts, True )

        # Create the list of directories from libdirs.b file to run the pre-processing clean script against
        self.libdirs  = []
        self.libnames = []
        if ( not arguments['--clean-all'] ):        # Skip if doing a --clean-all
            inf = open( os.path.join( "..", NQBP_NAME_LIBDIRS()), 'r' )
            utils.create_working_libdirs( self._printer, inf, arguments, self.libdirs, self.libnames, 'local', bld_var )  
            inf.close()

            # Start the Ninja file
            self._start_ninja_file( bld_var, arguments );


    #--------------------------------------------------------------------------
    def ar( self, arguments, objfiles, relative_objpath ):

        # Create output filename
        outputname = os.path.join( relative_objpath, self._ar_library_name ) 
      
        # normalize the directory separator
        objs = []
        for o in objfiles:
            objs.append( utils.standardize_dir_sep( o, self._os_sep ) )

        # Generate ninja build statement
        self._ninja_writer.build( 
            outputs = outputname,
            rule = 'ar',
            inputs = objs,
            variables = {"aropts":self._ar_options, "arout":self._ar_out} )
        
        self._ninja_writer.newline()
        return outputname

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
    def cc( self, arguments, fullname, relative_objpath ):
    
        # parse incoming name into its base
        basename = os.path.splitext( os.path.basename( fullname ) )[0]
        
        # construct compiler/assembler command
        cc_base = '{} {} '.format( self._all_opts.cflags,
                                     self._all_opts.inc
                                  )
        cc = cc_base + self._all_opts.c_only_flags

        is_cxx = True
        if ( fullname.endswith('.c') ):
            pass
        elif ( fullname.endswith('.cpp') ):
            cc = ' {} {} '.format(cc_base, self._all_opts.cppflags)
        elif ( self.is_asm_file(fullname) ):
            is_cxx = False
            cc = ' {} {} '.format( self._all_opts.asmflags,
                                      self._all_opts.asminc
                                  )
        else:
            self._printer.output( "ERROR: No rule to compile the file: {}".format( os.path.basename(fullname) ) )
            sys.exit(1)

        # Do any substitution of the 'ME_xxx' values
        cc = cc.replace( 'ME_CC_BASE_FILENAME', basename )
        
        # ensure correct directory separator                                
        full_fname = utils.standardize_dir_sep( fullname, self._os_sep )

        # Create output file name
        outputname = utils.standardize_dir_sep( os.path.join(relative_objpath, basename ) + '.' +  self._obj_ext, self._os_sep )

        # Generate ninja build statement
        if ( is_cxx ):
            self._ninja_writer.build( 
                outputs = outputname,
                rule = 'compile',
                inputs = full_fname,
                variables = {"ccopts":cc} )
        else:
            self._ninja_writer.build( 
                outputs = outputname,
                rule = 'assemble',
                inputs = full_fname,
                variables = {"asmopts":cc} )

        self._ninja_writer.newline()

        return outputname

    #--------------------------------------------------------------------------
    #
    def pre_link(self, arguments, inf, local_external_setting, variant, builtlibs ):
        self._printer.debug( '# ENTER: base.ToolChain.pre_link' )
        
        # Expand any _BUILD_DIR.aaaa symbols for .firstobjs and .lastobjs
        self._all_opts.firstobjs = utils.replace_build_dir_symbols(self,  self._all_opts.firstobjs, builtlibs, "." )
        self._all_opts.lastobjs  = utils.replace_build_dir_symbols(self,  self._all_opts.lastobjs, builtlibs, "." )

    #
    def link( self, arguments, builtlibs, objfiles, local_external_setting, extra_inputs_list=None, outname=None):
        if ( outname == None ):
            outname = self._final_output_name

        libs = []
        for item in builtlibs:
            libs.append(item[0])

        startgroup = self._linker_libgroup_start if len(libs) > 0 else ''
        endgroup   = self._linker_libgroup_end   if len(libs) > 0 else ''
        ldopts = '{} {} {} {} {} {} {} {}'.format( 
                                            self._all_opts.firstobjs,
                                            " ".join(objfiles),
                                            self._all_opts.linkflags,
                                            startgroup,
                                            " ".join(libs),
                                            endgroup,
                                            self._all_opts.linklibs,
                                            self._all_opts.lastobjs
                                            )
        link_inputs = objfiles
        if ( extra_inputs_list != None ):
            link_inputs.extend( extra_inputs_list) 

        # Normalize the directory separator
        ldopts = self._link_ldopts_standardize_dir_sep( ldopts )
        final_link_inputs = []
        for i in link_inputs:
            final_link_inputs.append( utils.standardize_dir_sep( i, self._os_sep ) )
        
        # Build implicit list
        impl_list = libs
        impl_list.extend( objfiles) 
        self._ninja_writer.build(
            outputs    = outname,
            rule       = 'link',
            inputs     = final_link_inputs,
            implicit   = impl_list,
            variables  = {"ldopts":ldopts, "ldout":self._link_output} )
        self._ninja_writer.newline()
        
        return None
    
    def finalize( self, arguments, builtlibs, objfiles, local_external_setting, linkout=None ):
        self._ninja_writer.default( self._final_output_name )
        
    def _link_ldopts_standardize_dir_sep( self, ldopts ):
        return utils.standardize_dir_sep( ldopts, self._os_sep )

    #==========================================================================
    # Private Methods
    #==========================================================================
    
    #--------------------------------------------------------------------------
    def _translate_cc_for_clang(self, l):
        return l # Default is GCC compiler -->nothing needed
    
    def _create_clangd_file( self ):
        inclist = self._tokenize_includes( self._all_opts.inc )
        optlist = self._tokenize_copts( self._all_opts.cflags + ' ' + self._all_opts.cppflags + self._all_opts.c_only_flags)
        ofile = os.path.join( NQBP_PKG_ROOT(), "compile_flags.txt" )
        try:
            with open( ofile, "w") as fd:
                for i in inclist:
                    fd.write( i + "\n")
                for o in optlist:
                    fd.write( o + "\n")
                    
        except Exception as e:                    
            self._printer.output( f"ERROR: Failed writing {ofile} VSCode file [{e}].")
            sys.exit(1)
        self._printer.output(f"Succesfuly generated {ofile}")  
          
    def _tokenize_includes( self, inc ):
        inclist = inc.strip().split(' ')
        inclist = list(set(inclist))  # remove duplicates
        exclude = ['', '-I.', '/I.']
        for e in exclude:
            try:
                inclist.remove(e)
            except:
                pass
            
        return inclist

    def _tokenize_copts( self, opts ):
        # brute force tokenize the compiler args
        optslist    = opts.strip().split(' ' + self._compiler_option_delimiter)
        optslist[0] = optslist[0][1:] # Remove the cc delimter from  first item in the list to match the remaining options
        optslist    = list(set(optslist)) # remove duplicates
        
        # Remove whitespace and restores the argument delimiter
        l = []
        for i in optslist[1:]:
            e = self._compiler_option_delimiter + i.strip()
            l.append(e)               

        # Hook to 'convert' non-gcc-like args to something clang understand
        l = self._translate_cc_for_clang(l)
        
        # Add clang specific options
        l.extend(self._all_opts.include_clangd)
        
        # Remove toolchain specific options
        xlist = ['-']
        xlist.extend(self._all_opts.exclude_clangd)
        for e in xlist:
            try:
                l.remove(e)
            except:
                pass
            
        return l
    
    #--------------------------------------------------------------------------
    def _create_vs_gdbentry(self):
        vsdir   = os.path.join( NQBP_PKG_ROOT(), ".vscode" )
        launchf = os.path.join( vsdir, "launch.json" )
        
        # Create .vscode directory if it does not exist
        if ( not os.path.exists(vsdir) ):
            try:
                os.makedirs(vsdir)
            except:
                pass
            
        # Create launch.json file if it does not exist
        if ( not os.path.isfile(launchf) ):
            launchjson = {}
            launchjson['configurations'] = []
            with open(launchf,'w') as fd:
                json.dump(launchjson,fd, indent=2)
            self._printer.output( f"Created empty {launchf} file")
            
        # Load the launch.json file
        try:
            jsondict = self._load_json_file( launchf )
        except Exception as e:                    
            self._printer.output( f"ERROR: Failed Reading {launchf} VSCode file [{e}].")
            sys.exit(1)
            
        jsondict = self._remove_existing_entries( jsondict )
        
        name     = os.path.join( NQBP_PRJ_DIR(), '_' + self.get_build_variant(), self.get_final_output_name() )
        jsondict = self._populate_launch_entry( jsondict, name )
        
        try:
            with open(launchf,'w') as fd:
                json.dump(jsondict,fd, indent=2)
            self._printer.output( f"Update {launchf} with new GDB entry")
            
        except Exception as e:
            self._printer.output( f"ERROR: Failed writing {launchf} VSCode file [{e}].")
            sys.exit(1)
        
    def _load_json_file( self, fname ):
        with open( fname, "r") as fd:
            lines = fd.readlines()
        
        # filter out comments
        newlines = []
        for l in lines:
            if l.strip().startswith("//"):
                continue
            newlines.append(l)
        
        # convert lines to JSON dictionary
        jsondata = ''.join(str(x) for x in newlines) # convert to a single string
        return json.loads(jsondata)
        
    def _remove_existing_entries( self, jsondict ):
        try:
            cfgs    = jsondict["configurations"]
            newcfgs = []
            for c in cfgs:
                if not c["name"].startswith("NQBP2:"):
                    newcfgs.append(c)
            jsondict["configurations"] = newcfgs
            return jsondict
        except Exception as e:
            self._printer.output( f"ERROR: Failed reading the launch.json file [{e}].")
            sys.exit(1)
            
    def _populate_launch_entry( self, jsondict, name ):
        entry = {}
        entry["name"] = f"NQBP2: {name}"
        entry["type"] = "cppdbg"
        entry["request"] = "launch"
        entry["program"] = name
        entry["args"] = []
        entry["environment"] = []
        entry["stopAtEntry"] = False
        entry["cwd"] = NQBP_PRJ_DIR()
        entry["externalConsole"] = False
        entry["MIMode"] ="gdb"
        cmd = {}
        cmd["description"]    = "Enable pretty-printing for gdb"
        cmd["text"]           = "-enable-pretty-printing"
        cmd["ignoreFailures"] = True
        cmds = [ cmd ]
        entry["setupCommands"] = cmds

        jsondict["configurations"].append(entry)
        return jsondict
    
    #--------------------------------------------------------------------------
    def _start_ninja_file( self, bld_variant, arguments ):
        self._ninja_writer.comment( "Project Build" )
        self._ninja_writer.comment( "This file is auto generated by nqbp.py" )
        self._ninja_writer.newline()
        self._ninja_writer.variable( "ninja_required_version", "1.3" )
        self._ninja_writer.newline()
        self._ninja_writer.variable( 'cc', f"{self._cc}" )     
        self._ninja_writer.variable( 'ld', f"{self._ld }" )      
        self._ninja_writer.variable( 'asm', f"{self._asm}" )    
        self._ninja_writer.variable( 'ar',  f"{self._ar}" )      
        self._ninja_writer.variable( 'objcpy', f"{self._objcpy}" )
        self._ninja_writer.variable( 'objdmp', f"{self._objdmp}" )
        self._ninja_writer.variable( 'shell', f"{self._shell}" )
        self._ninja_writer.variable( 'rm', f"{self._rm}" )
        self._ninja_writer.variable( 'buildtime', str(self._build_time_utc) if arguments['--bldtime']  else "0" )
        self._ninja_writer.variable( 'objdmp_redirect', '>' )
        self._ninja_writer.newline()
        self._build_compile_rule()
        self._ninja_writer.newline()
        self._build_assemble_rule()
        self._ninja_writer.newline()
        self._build_ar_rule()
        self._ninja_writer.newline()
        self._build_link_rule()
        self._ninja_writer.newline()
        self._build_objcpy_rule()
        self._ninja_writer.newline()
        self._build_objdmp_rule()
        self._ninja_writer.newline()
        self._build_objdmp_2stage_rule()
        self._ninja_writer.newline()
        self._build_generic_rule()
        self._ninja_writer.newline()
        self._ninja_writer.newline()


    def _build_compile_rule( self ):
        self._ninja_writer.rule( 
            name = 'compile', 
            command = f"$cc -MMD -MT $out -MF $out.d {self._cflag_symdef}BUILD_TIME_UTC=$buildtime $ccopts $in -o $out", 
            description = "Compiling: $in", 
            depfile = "$out.d",
            deps = 'gcc' )

    def _build_withrspfile_compile_rule( self ):
        self._ninja_writer.rule( 
            name = 'compile', 
            command = f"$cc -MMD -MT $out -MF $out.d {self._cflag_symdef}BUILD_TIME_UTC=$buildtime @$out.rsp $in -o $out", 
            rspfile = '$out.rsp',
            rspfile_content = '$ccopts',
            description = "Compiling: $in", 
            depfile = "$out.d",
            deps = 'gcc' )

    def _build_assemble_rule( self ):
        self._ninja_writer.rule( 
            name = 'assemble', 
            command = f"$asm -MMD -MT $out -MF $out.d {self._asmflag_symdef}BUILD_TIME_UTC=$buildtime $asmopts $in -o $out", 
            description = "Assembling: $in", 
            depfile = "$out.d",
            deps = 'gcc' )

    def _build_withrspfile_assemble_rule( self ):
        self._ninja_writer.rule( 
            name = 'assemble', 
            command = f"$asm -MMD -MT $out -MF $out.d {self._asmflag_symdef}BUILD_TIME_UTC=$buildtime @$out.rsp $in -o $out", 
            rspfile = '$out.rsp',
            rspfile_content = '$asmopts',
            description = "Assembling: $in", 
            depfile = "$out.d",
            deps = 'gcc' )

    def _build_ar_rule( self ):
        self._ninja_writer.rule( 
            name = 'ar', 
            command = "$rm $out && $ar $aropts ${arout}${out} $in", 
            description = "Archiving Directory: $out" )
        
    def _build_withrspfile_ar_rule( self ):
        self._ninja_writer.rule( 
            name = 'ar', 
            command = "$rm $out && $ar $aropts ${arout}${out} @$out.rsp", 
            rspfile = '$out.rsp',
            rspfile_content = '$in',
            description = "Archiving Directory: $out" )

    def _build_link_rule( self ):
         self._ninja_writer.rule( 
            name = 'link', 
            command = "$ld ${ldout}${out} $ldopts", 
            description = "Linking: $out" )

    def _build_withrspfile_link_rule( self ):
         self._ninja_writer.rule( 
            name = 'link', 
            command = "$ld ${ldout}${out} @$out.rsp", 
            rspfile = '$out.rsp',
            rspfile_content = '$ldopts',
            description = "Linking: $out" )

    def _build_objcpy_rule( self ):
        self._ninja_writer.rule( 
            name = 'objcpy_rule', 
            command = "$objcpy $objcpy_opts $in $out", 
            description = "Objcpy: $in TO $out" )

    def _build_objdmp_rule( self ):
        self._ninja_writer.rule( 
            name = 'objdmp_rule', 
            command = "$shell $objdmp $objdmp_opts $in > $out", 
            description = "Objdmp: $in TO $out" )

    def _build_generic_rule( self ):
        self._ninja_writer.rule( 
            name = 'generic_cmd', 
            command = "$shell $generic_cmd $generic_cmd_opts $generic_cmd_opts_in $in $generic_cmd_opts_out $out", 
            description = "Generic Command: $cmd" )

    def _build_objdmp_2stage_rule( self ):
        self._ninja_writer.rule( 
            name = 'objdmp_2stage_rule', 
            command = "$shell $objdmp $objdmp_opts1 $in > $out && $objdmp $objdmp_opts2 $in >> $out", 
            description = "Objdmp: $in TO $out" )

    def _create_always_build_statments( self, rule_to_execute, target_basename, impilicit_list=None, order_only_list=None, variables_dict=None ):
        """ Creates a set of build statement that will force 'rule_to_execute' to
            ALWAYS be executed when the final 'target name' is specified as
            dependency to a target.
            
            The returned target name is ALWAYS: target_basename + '_final' 
        """
        name_final_target = target_basename + '_final'
        self._ninja_writer.build(
              outputs    = target_basename + '1',
              rule       = rule_to_execute,
              implicit   = impilicit_list,
              order_only = order_only_list,
              variables  = variables_dict)
        self._ninja_writer.build(
              rule = "phony",
              inputs = target_basename + '1',
              outputs  = target_basename + '2' )
        self._ninja_writer.newline()
        self._ninja_writer.build(
            rule = "phony",
            inputs = target_basename + '2',
            outputs = name_final_target);
        
        return name_final_target
    

    #--------------------------------------------------------------------------
    # Windows/Visual Studio specific rules
    def _vs_build_compile_rule( self ):
        self._ninja_writer.variable( 'msvc_deps_prefix', 'Note: including file:' )
        self._ninja_writer.rule( 
            name = 'compile', 
            command = f'$cc /showIncludes /D "BUILD_TIME_UTC=$buildtime" @$out.rsp $in /Fo: $out', 
            description = "Compiling: $in", 
            rspfile = '$out.rsp',
            rspfile_content = '$ccopts',
            deps = 'msvc' )
        self._ninja_writer.newline()
    
    def _win32_withrspfile_build_ar_rule( self ):
        self._ninja_writer.rule( 
            name = 'ar', 
            command = 'cmd.exe /C "$rm $out 1>nul 2>nul && $ar $aropts ${arout}${out} @$out.rsp"', 
            rspfile = '$out.rsp',
            rspfile_content = '$in',
            description = "Archiving Directory: $out" )
        self._ninja_writer.newline()

    def _win32_build_ar_rule( self ):
        self._ninja_writer.rule( 
            name = 'ar', 
            command = 'cmd.exe /C "$rm $out 1>nul 2>nul && $ar $aropts ${arout}${out} $in"', 
            description = "Archiving Directory: $out" )
        self._ninja_writer.newline()

    #--------------------------------------------------------------------------
    def _format_custom_c_define( self, sym ):
        return self._cflag_symdef   + self._cflag_symvalue_delimiter   + sym + self._cflag_symvalue_delimiter + ' '

    def _format_custom_asm_define( self, sym ):
        return self._asmflag_symdef + self._asmflag_symvalue_delimiter + sym + self._asmflag_symvalue_delimiter + ' '
    

    #--------------------------------------------------------------------------
    def _build_library_list( self, libs ):
        result = ''
        for pair in libs:
            l,f  = pair
            dirname = l[0]

            path = '..' + os.sep
            if ( f == 'absolute' ):
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
        self._printer.debug( '#      lastobjs:   ' + sv.lastobjs   + ("\n" if extraSpace else " "))
  
                                                                         
