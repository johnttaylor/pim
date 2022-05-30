#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
Test script for simulating a UUT that is connected via TCP.

The simulation runs as a 'socket server' and is intended to accept a
TCP socket connection from a Telnet client and/or the RATT test tool.

The default Socket I listen on: 5002.  To change the socket number pass
the socket number as the first argument to this script.

NOTE: This script is NOT a good example of Python programming - it is brute
      force approach to getting a test tool working (to test a test tool).


Commands (sent to the simulation)
---------------------
.write  <reg> <num>|<str>;
.read   <reg>;
.spam   <msgid> <freq> <str>;
.delayw <msec>;
.delayr <msec>;
.exit;
.help;
.nop;

<reg>    Simulated register index (zero based) to write a value to.
<num>    Numeric value
<msgid>  Identifies which "Spam Message" to update/modify
<freq>   Frequency, in msec, that Spam message is outputed.  A value of zero
         will disable the Spam message
<str>    is quoted string.  To include a quote char ("), inside <str> use
         the '\' character to escape it.  To include the escape char (\) inside
         <str> use '\\'
<msec>   The delay, in msec, before the write|read commands return with the
         command prompt
 
"""

from __future__ import print_function
from __future__ import absolute_import
import socket, errno
import sys
import threading, time
from datetime import datetime
from time import sleep

NEWLINE = "\r\n"

#----------------------------------------------------------------------------------
class Sim:
    def __init__(self, poll_freq, outhdlr ):
        self.poll_freq   = poll_freq
        self.msecs       = 0
        self.output      = outhdlr
        self.spam        = {}
    
    def process(self):
        # Update elapsed time
        self.msecs += self.poll_freq

        # Generate spam output
        for (k,v) in self.spam.items():
            if ( v[0] <= self.msecs ):
                t = [ v[0] + v[1], v[1], v[2] ]
                self.spam[k] = t
                self.output.write_line( self.msecs, v[2] )


    def update_spam_msg( self, id, freq, str ):
        # Delete message when frequency is zero
        if ( freq == 0 ):
            try:
                del self.spam[id]
            except:
                pass

        # Update an existing entry
        else:
            # Entry is: [mark, freq, str]
            t = [ self.msecs+freq, freq, str ]            
            self.spam[id] = t

#----------------------------------------------------------------------------------
class Listener:
    def __init__(self, host, port, event ):
        # Create thread so that Ctrl-C works (in the main thread) to exit the program prior to the client connection
        t = threading.Thread( target=self.listen, args=(host, port, event) )
        t.setDaemon(True)
        t.start()
        
    def listen( self, host, port, event ):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        print( 'Socket created' )
 
        # Bind socket to local host and port
        try:
            self.s.bind( (host,port) )
        except socket.error as msg:
            print( 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1] )
            sys.exit()
     
        print( 'Socket bind complete' )
 
   
        # Start listening on socket
        self.s.listen(1)
        print( 'Listening on port ', port )
    
        # wait to accept a connection - blocking call
        print( 'Waiting for a connection...' )
        self.socket, self.client_addr = self.s.accept()
        self.socket.setblocking(0)
        print( 'Connected with ' + self.client_addr[0] + ':' + str(self.client_addr[1]) )

        # Signal the main thread to proceed (then let the listener thread end)
        event.set()
        self.s.close()
        
                
    def __del__(self):
        print(  "Shutting down socket connections..." )
        try:
            self.s.close()
            self.socket.close()
        except:
            pass
            
    def get_socket(self):
        return self.socket
        
#----------------------------------------------------------------------------------
class Output_handler:
    def __init__(self, socket):
        self.socket = socket

    def write_line( self, sim_time, string, no_prefix=True ):
        if ( no_prefix ):
            out = "{}\n".format( string )
        else:
            out = "[{}, {}] {}\n".format( datetime.now(), sim_time, string )

        self.socket.send( out.replace("\n", NEWLINE ).encode() )
 
    def write_prompt( self, string ):
        self.socket.send( string.encode() )

#----------------------------------------------------------------------------------
class Input_handler:
    def __init__( self, sim, outhdlr, socket, local_echo=True ):
        self.sim        = sim
        self.output     = outhdlr
        self.socket     = socket
        self.local_echo = local_echo
        self.framer     = Framer()
        self.tokenizer  = Tokenizer()
        self.exit_flag  = False
        self.commands   = {}
        self.commands.update( {'write':self.cmd_write} )
        self.commands.update( {'read':self.cmd_read} )
        self.commands.update( {'spam':self.cmd_spam} )
        self.commands.update( {'delayw':self.cmd_delayw} )
        self.commands.update( {'delayr':self.cmd_delayr} )
        self.commands.update( {'exit':self.cmd_exit} )
        self.commands.update( {'nop':self.cmd_nop} )
        self.commands.update( {'help':self.cmd_help} )
                  
    def read_socket(self):
        try:
            t = self.socket.recv(1024).decode('utf-8','ignore')
            
            # Cancel app if the socket is disconnected
            if not t:
                sys.exit("Socket has been disconnected from the client side.")
                return False
                
        except socket.error as e:
            # No data -->try again latter
            if e.args[0] == errno.EWOULDBLOCK: 
                return False
                
            # Cancel app if there is a socket error
            else:
                sys.exit( "Socket Error: " + str(e) )
                return False
                
        # Process the input
        result = False
        frames = self.framer.deframe( t )
        while len(frames) > 0:
            line = frames.pop(0)
            tokens = self.tokenizer.tokenize( line )
            if ( self.local_echo ):
                self.output.write_line( self.sim.msecs, line, no_prefix=True )

            try:
                cmd = self.commands[tokens[0]]
            except:
                self.output.write_line(  self.sim.msecs, "Invalid command: [{}]".format( line ), no_prefix=True )
                result = True
                continue
               
            # Run the command
            cmd( tokens )
            result = True

        # Return true when at least one frame was proccessed
        return result
  
    #-------------------------------------
    # write <reg> <num>|<str>
    def cmd_write( self, tokens ):
        pass
        
    #-------------------------------------
    # read <reg>
    def cmd_read( self, tokens ):
        pass
                    
    #-------------------------------------
    # spam <msgid> <freq> <str>
    def cmd_spam( self, tokens ):
        # Valid command/args
        if ( len(tokens) != 4 ):
            self.invalid_syntax( tokens )
            return

        # Update the spam message
        self.sim.update_spam_msg( tokens[1], int(tokens[2]), tokens[3])                

    #-------------------------------------
    # delayw <msec>
    def cmd_delayw( self, tokens ):
        pass
        
    #-------------------------------------
    # delayr <msec>
    def cmd_delayr( self, tokens ):
        pass
                
    #-------------------------------------
    # exit
    def cmd_exit( self, tokens ):
        self.exit_flag = True
        
    #-------------------------------------
    # nop
    def cmd_nop( self, tokens ):
        pass

    #-------------------------------------
    # help
    def cmd_help( self, tokens ):
        h = """.write  <reg> <num>|<str>;
.read   <reg>;
.spam   <msgid> <freq> <str>;
.delayw <msec>;
.delayr <msec>;
.exit;
.help;
.nop;

<reg>    Simulated register index (zero based) to write a value to.
<num>    Numeric value
<msgid>  Identifies which "Spam Message" to update/modify
<freq>   Frequency, in msec, that Spam message is outputed.  A value of zero
         will disable the Spam message
<str>    is quoted string.  To include a quote char ("), inside <str> use
         the '\' character to escape it.  To include the escape char (\) inside
         <str> use '\\'
<msec>   The delay, in msec, before the write|read commands return with the
         command prompt
"""
        self.output.write_line( self.sim.msecs, h, no_prefix=True )
                
        
    #-------------------------------------
    def invalid_syntax(self, tokens):
        self.output.write_line( self.sim.msecs, "Invalid syntax:" + ' '.join(tokens), no_prefix=True )
    
    def invalid_args(self, tokens):
        self.output.write_line( self.sim.msecs, "Invalid args:" + ' '.join(tokens), no_prefix=True )
        

#-----------------------------------------------------------------------------
class Tokenizer:
    def __init__(self, separator=' '):
        self.sep = separator
        
    def tokenize( self, line ):
        tokens    = []
        token     = []
        inTextStr = False;
        escaped   = False;
        
        # Remove leading/trailing whitespace
        line = line.strip()
        
        # scan the entire line
        for c in line:
            # NOT in text string
            if ( inTextStr == False ):
                if ( c == '"' ):
                    inTextStr = True
                elif ( c == self.sep ):
                    # Special case: When using a space for the separtor -->consective spaces DO NOT count as multiple separators
                    if ( len(token) == 0 and self.sep == ' ' ):
                        pass
                        
                    # Nominal case        
                    else:
                        tokens.append( ''.join(token) )
                        token = []
                    
                else:
                    token.append( c )
            
            # IN a text string
            else:
                if ( escaped ):
                    escaped = False
                    token.append( c )
                
                elif ( c == '"' ):
                    inTextStr = False
                
                elif ( c == '\\' ):
                    escaped = True
                    
                else:
                    token.append( c )
                    
        # End-of-line
        tokens.append( ''.join(token) )
        return tokens
                                    
                          
class Framer:
    def __init__(self, sofchar='.', eofchar=';'):
        self.sof       = False
        self.sofChar   = sofchar
        self.eofChar   = eofchar
        self.inTextStr = False
        self.escaped   = False
        self.frame     = []
        
        
    def deframe( self, line ):
        frames = []
        
        for c in line:
            # Start of Frame
            if ( self.sof == False ):
                if ( c == self.sofChar ):
                    self.sof = True
                    self.frame   = []
                
            # In frame AND not in 'text string'    
            elif ( self.inTextStr == False ):
                if ( c != '"' ):
                    if ( c != self.eofChar ):
                        self.frame.append( c )
                    else:
                        frames.append( ''.join(self.frame) )
                        self.sof = False
                                    
                else:
                    self.frame.append( c )
                    self.inTextStr = True
            
            # In frame AND IN a 'text string'
            else:
                self.frame.append( c )
                if ( self.escaped == False ):
                    if ( c == '"' ):
                        self.inTextStr = False
                    elif ( c == '\\' ):
                        self.escaped = True
                else:
                    self.escaped = False
                    
        # Return zero or more frames found
        return frames
                                             
#-----------------------------------------------------------------------------
def main():
    # Default command prompt
    prompt = "$>"

    # Default the port number
    port = 5002
    if ( len(sys.argv) > 1 ):
        if ( sys.argv[1].isdigit() ):
            port = int(sys.argv[1])

    # Default polling interval (in msec)
    poll_freq = 20
    if ( len(sys.argv) > 2 ):
        if ( sys.argv[2].isdigit() ):
            poll_freq = int(sys.argv[2])
    
    # Start listener and wait for a connection
    event = threading.Event()
    io   = Listener( "", port, event ) 
    while True:
        time.sleep(0.1)
        if event.is_set():
            break

    # Have conneciton -->Start the GUI
    print( "Starting Simulate UUT...." )
    outhdlr   = Output_handler( io.get_socket() )
    sim       = Sim( poll_freq, outhdlr )
    inhdlr    = Input_handler( sim, outhdlr, io.get_socket() )

    # Loop forever
    outhdlr.write_prompt( prompt )
    while( inhdlr.exit_flag == False ):
        if ( inhdlr.read_socket() ): 
            outhdlr.write_prompt( prompt )
        sim.process()
        sleep( poll_freq / 1000.0 )

    print( "Simulate UUT terminated." )


if __name__ == '__main__':
    main()  
    
    
