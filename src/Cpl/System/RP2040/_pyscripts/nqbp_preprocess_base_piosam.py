#!/usr/bin/python3
""" Base script that runs the PIO Assembler for .pio files a given directory
    The output file is C Header file that has the same file name as the source
    file, but with a trailing '.h' appended to the name

    usage: nqbp_preprocessing_base_pioasm <arg1> ... <arg6> where:
             <arg1>: build|clean         
             <arg2>: verbose|terse
             <arg3>: <workspace-dir>
             <arg4>: <package-dir> 
             <arg5>: <project-dir> 
             <arg6>: <current-dir> 
"""

import os, sys
import subprocess

#------------------------------------------------------------------------------
def get_pio_source_files( dir ):
    files = []
    for file in os.listdir(dir):
        if file.endswith(".pio"):
            files.append( file )
    return files

def run_shell( cmd ):
    p = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
    r = p.communicate()
    r0 = '' if r[0] == None else r[0].decode()
    r1 = '' if r[1] == None else r[1].decode()
    return (p.returncode, "{} {}".format(r0,r1))

def delete_file( fname ):
    """ remove file(s) and suppress error if 'fname' does not exist """
    
    result = False
    try:
        os.remove( fname )
        result = True
    except OSError:
        pass
           
    return result
#------------------------------------------------------------------------------
# Parse command line
def run( argv ):
    #print(f"Hi there. {sys.argv}")

    # Path to the PIOASM executable
    pioexe = os.path.join( sys.argv[4], "xsrc", "pico-sdk", "tools", "pioasm" )

    # Process the PIO files
    for f in get_pio_source_files( sys.argv[6] ):
        oname = f + '.h'
        outfile = os.path.join( sys.argv[6], oname )
        if ( sys.argv[1] == 'clean' ):
            if ( sys.argv[2] != 'terse' ):
                print("Deleting {output}" )
            delete_file( outfile )
        else:
            cmd = f'{pioexe} -o c-sdk {os.path.join(sys.argv[6],f)} {outfile}'
            if ( sys.argv[2] != 'terse' ):
                print(f"PIOASM: {cmd}" )
            run_shell( cmd )



## Path - relative to the pimoroni root directory of the PIO Assembler file to compile
#pio_source_file = os.path.join( 'drivers','st7789', 'st7789_parallel.pio' )


## MAIN
#if __name__ == '__main__':
#    NQBP_XPKGS_ROOT = os.environ.get('NQBP_XPKGS_ROOT')
#    if ( NQBP_XPKGS_ROOT == None ):
#        sys.exit( "ERROR: The environment variable NQBP_XPKGS_ROOT is not set!" )


#    pioexe = os.path.join( NQBP_XPKGS_ROOT, "pico-sdk", "tools", "pioasm" )
#    src    = os.path.join( NQBP_XPKGS_ROOT, "pimoroni-pico", pio_source_file );
#    cmd = f'{pioexe} -o c-sdk {src} st7789_parallel.pio.h' 
#    print( cmd )
#    result, testoutput = run_shell( cmd  )
#    print( testoutput )
