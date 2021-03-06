#!/usr/bin/env python3
#=============================================================================
# Helper script (that does most of work) for generating FSM source code from
# Cadifra FSM diagrams
#
# This script runs the 'sinelaboreRT' Finite State Machine Code Generator
# utility.  The SinelaboreRT tool is proprietary tool, but the output of
# the code generator is NOT.  This script is also specific to parsing
# FSM diagramS generated by the Cadifra drawing tool (yet another 
# proprietary tool).  Yes - using non open source tools is straying from
# the pure faith - but both tools are very good and affordable - and the
# generated source code and diagram content is still 'free'.
#
# http://www.sinelabore.com/
# http://www.cadifra.com/
#
#=============================================================================

#
import sys   
import os
import subprocess
#
from docopt.docopt import docopt


# 
usage = """ 
genfsm - Generates source code from Cadifra FSM Diagrams (.cdd files)
===============================================================================
Usage: genfsm [options] <basename> 

Arguments:
  <basename>       Base name for the FSM.  The Cadifra diagram must have this
                   same file name.  All generated output files will have the 
                   <basename> as part of their file names. <basename> IS 
                   case sensitive!
      
                   
Options:
  -d NEVENTS       When NEVENTS is greater then 0 code for an event queue is 
                   generated where NEVENTS is the size of the event queue. 
                   [Default: 0]
  -h, --help       Display command help.
        
         
NOTES:
    o The environment variable SINELABORE_PATH is required/used to specify
      the location of the Sinelabore code generator JAR files (and it 
      supporting JAR files).
    o The script assumes that Graphviz's dot.exe is in the command path.
      GraphViz is used to generated the FSM diagram for Doxygen. See 
      http://www.graphviz.org
      
"""

#
import subprocess
import re
import sys

#------------------------------------------------------------------------------
# Parse command line
def run( argv ):

    print(argv)
    
    # Process command line args...
    args = docopt(usage, version="0.0.1" )
     
    # Check the environment variables 
    sinpath = os.environ.get( "SINELABORE_PATH" )
    if ( sinpath == None ):
        exit( "ERROR: The SINELABORE_PATH environment variable is not set." )
    
    # Filenames
    fsmdiag = args['<basename>'] + ".cdd"
    fsm     = args['<basename>'] 
    cfg     = 'codegen.cfg'
    context = fsm + '_ctx'
    trace   = fsm + '_trace'
    
    # Create the config file for Sinelabore
    ctx_t  = fsm + "_context_t"
    geneatedCodegenConfig( cfg, fsm, ctx_t )
        
    # Build Sinelabore command
    cmd = 'java -jar -Djava.ext.dirs={} {}/codegen.jar -p CADIFRA -o {} -l cx -Trace {}'.format( sinpath, sinpath, fsm, fsmdiag )
    cmd = standardize_dir_sep( cmd )
  
    # Invoke Sinelabore command
    print(cmd)
    p = subprocess.Popen( cmd, shell=True )
    r = p.communicate()
    if ( p.returncode != 0 ):
        exit("ERROR: Sinelabore encounterd an error or failed to run." )
    
    # Clean-up auto generated sources files
    cleanup_context_include( fsm+".c", fsm )
    add_include( fsm+".c", trace+'.h' )
    cleanup_trace( trace+'.h', fsm, 'MY_ENABLE_TRACE_SWITCH' )
    
    
    # Clean-up unwanted files 
    os.remove( cfg )
    delete_file( fsm + "_trace.java" );
    
    # Generate Context Header file
    actions, guards = getContextMethods( fsmdiag )
    generatedContextClass( fsm, ctx_t, context, getHeader(), actions, guards, args['-d'].strip() )
    

#------------------------------------------------------------------------------
def standardize_dir_sep( pathinfo ):
    return pathinfo.replace( '/', os.sep ).replace( '\\', os.sep )
    
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
#
def cleanup_context_include( sourcefile, base_name ):
    tmpfile       = sourcefile + ".tmp"
    done          = False
    first_include = False
    with open( sourcefile ) as inf:
        with open( tmpfile, "w") as outf:  
            for line in inf:
                if ( line.find( '#include "{}.h'.format( base_name) ) != -1 ):
                    outf.write( '#include "{}_ctx.h"\n'.format( base_name ) )
                else: 
                    outf.write( line )
    
    os.remove( sourcefile )
    os.rename( tmpfile, sourcefile )

#
def add_include( sourcefile, new_include_file ):
    tmpfile       = sourcefile + ".tmp"
    done          = False
    first_include = False
    with open( sourcefile ) as inf:
        with open( tmpfile, "w") as outf:  
            for line in inf:
                if ( done == False and line.find( '#include' ) != -1 ):
                    first_include = True
                elif ( first_include and line.find( '#include' ) == -1 ):
                    outf.write( '#include "{}"\n'.format( new_include_file ) )
                    first_include = False
                    done          = True
                
                outf.write( line )
    
    os.remove( sourcefile )
    os.rename( tmpfile, sourcefile )

def cleanup_trace( traceheader_file, base_name, ifdef_sym ):
    tmpfile = traceheader_file + ".tmp"
    with open( traceheader_file ) as inf:
        with open( tmpfile, "w") as outf:  
            for line in inf:
                if ( line.find( '(int evt)' ) != -1 ):
                    outf.write('#ifndef {}\n'.format( ifdef_sym ) )
                    outf.write('#define {}TraceEvent(e)\n'.format( base_name ) )
                    outf.write('#else\n' )
                    outf.write('void {}TraceEvent(int evt);\n'.format( base_name ) )
                    outf.write('#endif\n' )
                else:
                    outf.write( line )
    
    os.remove( traceheader_file )
    os.rename( tmpfile, traceheader_file )
      
     
     
     
#------------------------------------------------------------------------------
def getContextMethods( fname ):
    actions = []
    guards  = []
    with open(fname) as f:
        for line in f:
            g = re.search(r'[a-zA-Z0-9]+\(ctx\)(?!\;)',line)
            a = re.search(r'[a-zA-Z0-9]+\(ctx\)\;', line)
            if ( g != None ):
                guards.append( g.group(0) )
            if ( a != None ):
                actions.append( a.group(0).split(';')[0] )
    
    # Remove any duplicates from the grep'd methods
    return sorted(list(set(actions))), sorted(list(set(guards)))

    

def generatedContextClass( base_name, ctx_t, file_name, header, actions, guards, depth ):
    fname  = file_name + '.h'
    if ( os.path.exists( fname ) ):
        tmpfile = fname + ".tmp"
        with open( fname ) as inf:
            with open( tmpfile, "w") as outf:  
                for line in inf:
                    if ( line.find( 'BEGIN AUTO-GEN MARKER' ) != -1 ):
                        write_auto_contextClass( outf, ctx_t, actions, guards )
                        break
                    else:
                        outf.write( line )
    
        os.remove( fname )
        os.rename( tmpfile, fname )
        
    else:
        with open(fname,"w") as f:
            f.write( "#ifndef {}_h_\n".format( file_name ) )
            f.write( "#define {}_h_\n".format( file_name ) )
            f.write( header )
            f.write( "\n" )
            f.write( '#include "{}.h"\n'.format(base_name) )
            f.write( "\n" )
            f.write( "\n" )
            f.write( "/*-------------- CONTEXT STRUCTURE -----------------*/ \n" )
            f.write( "/** Application context instance data */\n" )
            f.write( "typedef struct {}\n".format( ctx_t+"ag" ) )
            f.write( "{\n" )
            f.write( "    /* APPLICATION SPECIFIC data goes here....\n" );
            f.write( "\n" )
            f.write( "    /*\n" ) 
            f.write( "    ** REQUIRED FOR ALL FSMs (do NOT edit!)\n" )
            f.write( "    */\n" )
            f.write( "    /** FSM instance struct */\n" )
            f.write( "    {}_INSTANCEDATA_T instanceVar;\n".format( base_name.upper() ) )
            f.write( "\n" )
            f.write( "    /** Event que stuff here */\n" )
            f.write( "    int myEventQueue;\n" )
            f.write( "{} {};\n".format( "}", ctx_t ) )
            f.write( "\n" )
            f.write( "\n" )
            f.write( "/** Helper macro to set Context's FSM instance ID */\n" )
            f.write( '#define SET_{}_CONTEXT_FSM_INSTID(thisPtr,instanceId)     (thisPtr)->instanceVar.inst_id = instanceId\n'.format(base_name.upper()) )
            f.write( "\n" )
            f.write( "\n" )
            f.write( "/* NOTE: The following code snippet is example of statically initializing\n" )
            f.write( "         the above Context structure.  The application MUST still call\n" ) 
            f.write( "         SET_{}_CONTEXT_FSM_INSTID() at run time to properly set the FSM ID */\n".format( base_name.upper()) )
            f.write( "/* {} myInstance = {} ..., {}_INSTANCEDATA_INIT, eventQueueState {}; */\n".format( ctx_t, '{', base_name.upper(), '}' ) )
            f.write( "\n" )
            f.write( "\n" )
            write_auto_contextClass( f, ctx_t, actions, guards )
            
#            
def write_auto_contextClass( f, ctx_t, actions, guards ):
    ctxarg = ctx_t + "* ctxP"
    f.write( "\n\n/* BEGIN AUTO-GEN MARKER */\n" )
    f.write( "/* The contents below this line are auto-generated DO NOT MANUALLY EDIT this file beyond this marker! */\n\n" )
    f.write( "/*-------------- ACTIONS -----------------*/ \n" )
    for a in actions:
        a = a.replace("ctx", ctxarg );
        f.write( "/** Action */\n" )
        f.write( "void {};\n".format( a ) )
        f.write( "\n" )
    f.write( "\n" )
    f.write( "/*-------------- GUARDS -----------------*/ \n" )
    for g in guards:
        g = g.replace("ctx", ctxarg );
        f.write( "/** Guard */\n" )
        f.write( "bool {};\n".format( g ) )
        f.write( "\n" )
    f.write( "\n" )
    f.write( "#endif /* end header latch */\n" )
         
    

#------------------------------------------------------------------------------
def geneatedCodegenConfig( fname, base, ctx_t ):
    cfg = '''Create inline doxygen of state machine function
# Output configuration options for the given language. Pipe them into a file for further use!
#
#Allows to define naming conventions for events
PrefixEvents=
#
#Allows to define naming conventions for simple states
PrefixSimpleStates=
#
#Allows to define naming conventions for composite states
PrefixCompositeStates=
#
#Allows to define naming conventions for choice states
PrefixChoice=

#
#Path to 'dot.exe'.
#DotPath="C:\\Program Files\\Graphviz2.22\\bin\\dot.exe"
#DotPath=/usr/local/bin/dot
#
#Port the graphical statediagram.simulator listens for event strings. 
UdpPort=4445
#
#Options 'yes' and 'no' are possible. If set to 'yes' only hot transitions are shown
ShowOnlyHotTransitions=no
#
#It is possible to limit the length of the event text. This keeps the image compact.
NumberOfTransitionChars=32
#
#If set to 'yes' only correct models can be saved.
SaveCheckedOnly=yes
#
#If set to 'yes' action code is displayed in the state diagram of the integrated statediagram.editor.
DisplayEntryExitDoCode=yes
#
#Limit action code in the integrated statediagram.editor to the given number of chars.
NumberOfEntryExitDoCodeChars=32
#
#

#Defines the text each generated file starts with.
Copyright=$$COPYRIGHT$$
#
#Defines if real tabs or spaces are used for indentation.
Realtab=no
#
#If realtab is 'no' this key defines how many spaces to use per tab
Tabsize=3
#
#Some systems can use special compiler keywords to place the debug strings in program memory or a specifc segment
TypeOfDbgString=const char
#
#If set to 'no' the data and time info is supressed in each file header
IncludeDateFileHeaders=yes

#
#Prefix of the state machine function in the H file.
StateMachineFunctionPrefixHeader=
#
#Prefix of the state machine function in the C file.
StateMachineFunctionPrefixCFile=
#
#Defines if event definitions start from zero or another value.
EventFirstValue=0
#
#Defines the C mechanism used for event definition. Options are 'define' or 'enum'.
EventDeclarationType=define
#
#In case events are declared as 'define' the event type can be specified here.
EventTypeInCaseOfDefine=unsigned char


#
#This flag can be used to instruct the code generator to generate bit-coded events.
EventsAreBitCoded=no
#
#Defines if the state machine function has a pointer to the instance data as parameter.
HsmFunctionWithInstanceParameters=yes
#
#If set to 'yes' an event is generated as parameter for the state handler function. HsmFunctionWithInstanceParameter must be set to 'yes' also.
HsmFunctionWithEventParameter=yes
#HsmFunctionUserDefinedInstParameter=float
#
#A user provided type that is used as parameter for the state handler function. HsmFunctionWithInstanceParameter must be set to 'yes' also.
HsmFunctionUserDefinedParameter=$$CONTEXT_T$$
#
#Prefix the message variable (msg) within the state machine with the machine name.
PrefixMsgWithMachineName=no
#
#If set to 'no' instance data is accessed by value. If set to 'yes' instance data is accessed by reference. 
UseInstancePointer=yes
#
#If set to 'yes' the code to change the state variables is inlined.
InlineChangeToStateCode=yes
#
#Allows to define local variables etc. within the state machine.
AdditionalLocalMachineVars=
#
#Allows to define code that is executed if a state variable holds an invalid state.
UnknownStateHandler=
#
#Return a value that indicates if an event was processed.
ReturnEventProcessed=yes
#
# Set to yes to creates validation code in the state machine and the validation c/h files
# You have to provide the validation handler that actually calls the validate function.
# If validation fails it is up to you to define the reaction.
ValidationCall=no
#
#
# Provide your own inlcude file here which defines the required data types
AdditionalValidateIncludes=
#
#
#Provide your own include file for the definition of simple data types used in the state machine header file.
AdditionalMachineInclude=#include <stdint.h>
#
#Provide an own definition for this data type
UINT8=uint8_t
#
#Provide an own definition for this data type
UINT16=uint16_t
#
#Provide an own definition for this data type
BOOL=uint8_t
#
#Switch on generation of events with a given hamming distance between each event value.
UseHammingCodesForEvents=no
#
#Switch on generation of states with a given hamming distance between each state value.
UseHammingCodesForStates=no
#
#Hamming distance used for states and events if switched on.
HammingDistance=2
#
#
#Prefix state definitions with machine name (yes/no)
PrefixStateNamesWithMachineName=no
#
#Prefix state definitions with parent name (yes/no)
PrefixStateNamesWithParentName=no
#
#Prefix instanceVar string with machine name (yes/no)
PrefixInstanceVarWithMachineName=no
'''
    # Replace tokens
    cfg = cfg.replace( "$$CONTEXT_T$$", ctx_t )
    cfg = cfg.replace( "$$COPYRIGHT$$", getHeaderCfg() )
    
    # create file
    with open(fname,"w") as f:
        f.write( cfg )

    

#------------------------------------------------------------------------------
def getHeader():
    return  '/*-----------------------------------------------------------------------------\n* Copyright notice Blah...\n*----------------------------------------------------------------------------*/\n/** @file */\n\n'

def getHeaderCfg():
    return r'/*-----------------------------------------------------------------------------\n* Copyright notice Blah...\n*----------------------------------------------------------------------------*/\n/** @file */\n\n'

