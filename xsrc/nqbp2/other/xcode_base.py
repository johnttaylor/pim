#!/usr/bin/python3
"""
 
Xcode generates source code template/skeleton files
===============================================================================
usage: xcode [options] <template> [<args>...]
       xcode [options]

Options:
    -n NAMESPACE        Explicity provide the namespace. Format: "N1::N2::N3.."
                        The default operation is to derive the namespace(s) from
                        the current working directory.
    -v                  Be verbose. 
    -g                  Enable debug switch on the 'f4' utility
    -h, --help          Display help for common options/usage.
    

Type 'xcode help <template>' for help on a specific template.

"""

from __future__ import absolute_import
from __future__ import print_function
import sys
import os
from subprocess import call

from nqbplib.docopt.docopt import docopt
from nqbplib import utils
from other import utils2


# globals
WRKSPACE_ROOT = ''
PKG_ROOT      = ''


#------------------------------------------------------------------------------
def load_command( name ):
        try:
            command_module = __import__("other.xcodes.{}".format(name), fromlist=["other.xcodes"])
        except ImportError:
            exit("{} is not a xcode template. Use 'xcode help' for list of templates.".format(name) )

        return command_module
        
        
#------------------------------------------------------------------------------
def display_command_list():
    import pkgutil
    from . import xcodes
    p = xcodes
    
    print( ' ' )
    print( "Type 'xcode help <template>' for more details. Type 'xcode -h' for base usage." )
    print( "-------------------------------------------------------------------------------" )
    
    for importer, modname, ispkg in pkgutil.iter_modules(p.__path__):
        if ( not ispkg ):
            cmd = load_command( modname )
            cmd.display_summary()
            

    print( ' ' )

#------------------------------------------------------------------------------
def run( argv ):

    # Parse command line
    args = docopt(__doc__, version="0.1", options_first=True )

    # Trap help on a specific command
    if ( args['<template>'] == 'help' ):

        # Display list of commands if none specified
        if ( args['<args>'] == [] ):
            display_command_list()
        
        # Display command specific help
        else:
            load_command( args['<args>'][0] ).run( args, ['--help'] )


    # Trap no command specified        
    elif ( args['<template>'] == None ):
            docopt(__doc__,argv=['--help'])
    

    # Run the command (if it exists)
    else:
        global WRKSPACE_ROOT
        global PKG_ROOT
    
        # Get workspace/package info
        WRKSPACE_ROOT, PKG_ROOT = utils2.set_pkg_and_wrkspace_roots()
    
        # Get namespace as a list
        if ( args['-n'] ):
            namespaces = args['-n'].split('::')
        else:
            namespaces = utils2.get_relative_subtree( PKG_ROOT, 'src' ).split( os.sep )
        args['-n'] = ','.join( namespaces )
        
        # Convert the debug option to a f4 argument
        if ( args['-g'] ):
            args['-g'] = '-g'
        else:
            args['-g'] = ''
             
        # run the command
        load_command( args['<template>'] ).run( args, [args['<template>']] + args['<args>'] )


    

