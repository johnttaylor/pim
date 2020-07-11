"""Collection of helper functions"""
import sys   
import os
import subprocess

#------------------------------------------------------------------------------
###
def string_to_number(s):
    try:
        z = int(s)
        return z, True
    except ValueError:
        try:
            z = float(s)
            return z, True
        except ValueError:
            return s, False


#------------------------------------------------------------------------------
###
class Output(object):
    def __init__( self, outname=None ):
        if ( outname != None ):
            try:
                self.handle = open( outname, 'w' )
            except EnvironmentError:
                self.handle = None
                sys.exit( "ERROR: Cannot open output file: {}".format( outname ) )
        else:
            self.handle = sys.stdout

    def write( self, data ):
        if ( self.handle != None ):
            self.handle.write( data )
        else:
            sys.exit( "ERROR: Invalid output handle" )
         
    def close( self ):
        if ( self.handle is not sys.stdout ):
            self.handle.close()     


#------------------------------------------------------------------------------
###
def set_pkg_and_wrkspace_roots():
    result, wrk_root = run_shell( 'orc --qry-w' )
    if ( result != 0 ):
        sys.exit( "ERROR: Cannot execute Outcast's 'orc' command.  Please setup your Outcast environment." )
    result, pkg_root = run_shell( 'orc --qry-p' )
    if ( result != 0 ):
        sys.exit( "ERROR: Cannot execute Outcast's 'orc' command.  Please setup your Outcast environment." )

    return wrk_root[0].strip(), pkg_root[0].strip()


###
def run_shell( cmd ):
    p = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
    r = p.communicate()
    r0 = '' if r[0] == None else r[0].decode()
    r1 = '' if r[1] == None else r[1].decode()
    return (p.returncode, "{} {}".format(r0,r1))


    
###
def get_relative_subtree( root, subtree_dir ):
    path   = os.getcwd();
    src    = os.path.join( root, subtree_dir )
    return path[len(src)+1:]


#------------------------------------------------------------------------------
###
def verbose( cmd, output, args ):
    if ( args['-v'] ):
        print(cmd)
        print_shell_output( output )
    

### 
def print_shell_output( output ):
    for l in output:
        print(l)
        