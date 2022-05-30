""" Provides an 'pexpect' object based on platform/connection type
"""
import pexpect
from pexpect.popen_spawn import PopenSpawn

#------------------------------------------------------------------------------
class ExpectWindowsConsole(object):
    def __init__( self, exename, logfile =None ):
        self.exename    = exename
        self.logfile    = logfile
        self.child      = pexpect.popen_spawn.PopenSpawn(exename,logfile=logfile,maxread=4096)

    def is_detached_uut(self):
        return False;

    def respawn( self ):
        self.child = pexpect.popen_spawn.PopenSpawn(self.exename,logfile=self.logfile,maxread=4096)

    def sendline( self, s ):
        self.child.sendline(s)

    def flush( self ):
        self.child.flush()

    def read_nonblocking( self, size=1, timeout=-1 ):
        return self.child.read_nonblocking( size, timeout )


    def expect( self, regex_list, timeout=-1, searchwindowsize=-1 ):
        return self.child.expect( regex_list, timeout=timeout, searchwindowsize=searchwindowsize )

    def expect_str( self, string_list, timeout=-1, searchwindowsize=-1 ):
        return self.child.expect_exact( string_list, timeout=timeout, searchwindowsize=searchwindowsize )


    def get_before( self ):
        r =  self.child.before
        if ( isinstance(r,bytes) == True ):
            r = str(r,'utf-8')
        return r

    def get_after( self ):
        r =  self.child.after
        if ( isinstance(r,bytes) == True ):
            r = str(r,'utf-8')
        return r

    def close( self ):
        try:
            self.child.kill(signal.SIGTERM)     
        except:
            pass

#------------------------------------------------------------------------------
class ExpectLinuxConsole(object):
    def __init__( self, exename, logfile=None ):
       self.exename     = exename
       self.logfile     = logfile
       self.child = pexpect.spawn(exename,logfile=logfile,maxread=1)

    def respawn( self ):
        self.child = pexpect.popen_spawn.PopenSpawn(self.exename,logfile=self.logfile,maxread=1)

    def is_detached_uut(self):
        return False;

    def sendline( self, s ):
        self.child.sendline(s)

    def flush( self ):
        self.child.flush()

    def read_nonblocking( self, size=1, timeout=-1 ):
        return self.child.read_nonblocking( size, timeout )

    def expect( self, regex_list, timeout=-1, searchwindowsize=-1 ):
        return self.child.expect( regex_list, timeout=timeout, searchwindowsize=searchwindowsize )

    def expect_str( self, string_list, timeout=-1, searchwindowsize=-1 ):
        return self.child.expect_exact( string_list, timeout=timeout, searchwindowsize=searchwindowsize )

    def get_before( self ):
        r =  self.child.before
        if ( isinstance(r,bytes) == True ):
            r = str(r,'utf-8')
        return r

    def get_after( self ):
        r =  self.child.after
        if ( isinstance(r,bytes) == True ):
            r = str(r,'utf-8')
        return r

    def close( self ):
        try:
            self.child.kill(signal.SIGTERM)     
        except:
            pass


#------------------------------------------------------------------------------
class ExpectNullConsole(object):
    def __init__( self, logfile=None ):
        self.logfile  = logfile

    def is_detached_uut(self):
        return False;

    def sendline( self, s ):
        pass

    def flush( self ):
        pass


    def read_nonblocking( self, size=1, timeout=None ):
        return pexpect.TIMEOUT

    # Return the last index -->assumption is that the last item in the list is EOF or TIMEOUT
    def expect( self, regex_list, timeout=-1, searchwindowsize=-1 ):
        return len(regex_list)  -1

    # Return the last index -->assumption is that the last item in the list is EOF or TIMEOUT
    def expect_str( self, string_list, timeout=-1, searchwindowsize=-1 ):
        return len(string_list)  -1

    def get_before( self ):
        return None 

    def get_after( self ):
        return None

    def close( self ):
        pass

