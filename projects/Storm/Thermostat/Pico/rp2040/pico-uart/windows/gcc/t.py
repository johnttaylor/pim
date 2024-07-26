#!/usr/bin/python3
"""Generates Header file for PIO"""

import os
import subprocess

# Path - relative to the pimoroni root directory of the PIO Assembler file to compile
pio_source_file = os.path.join( 'drivers','st7789', 'st7789_parallel.pio' )

def run_shell( cmd ):
    p = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
    r = p.communicate()
    r0 = '' if r[0] == None else r[0].decode()
    r1 = '' if r[1] == None else r[1].decode()
    return (p.returncode, "{} {}".format(r0,r1))

# MAIN
if __name__ == '__main__':
    NQBP_XPKGS_ROOT = os.environ.get('NQBP_XPKGS_ROOT')
    if ( NQBP_XPKGS_ROOT == None ):
        sys.exit( "ERROR: The environment variable NQBP_XPKGS_ROOT is not set!" )


    pioexe = os.path.join( NQBP_XPKGS_ROOT, "pico-sdk", "tools", "pioasm" )
    src    = os.path.join( NQBP_XPKGS_ROOT, "pimoroni-pico", pio_source_file );
    cmd = f'{pioexe} -o c-sdk {src} st7789_parallel.pio.h' 
    print( cmd )
    result, testoutput = run_shell( cmd  )
    print( testoutput )
