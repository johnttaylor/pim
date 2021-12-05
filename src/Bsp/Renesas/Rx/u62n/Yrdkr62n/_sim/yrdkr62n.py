#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
Test script for simulating the Graphic LCD, LEDs, buttons, etc for
the Renesas YRDKR62N demostration board.

The simulation runs as a 'socket server' and is intended to accept a
TCP socket connection from a Windows/Linux console application that is
a functional simulation of an embeded application that is targeted to
the YRDKR62N demo board.

The default Socket I listen on: 5002.  To change the socket number pass
the socket number as the first argument to this script.


Commands (sent to the simulation)
---------------------
.<time> write <row> <col> <str> <len>;
.<time> font <font>;
.<time> clearline <row>;
.<time> clearscreen;
.<time> led <num> <state> [<num> <state>]*;
.<time> exit;
.<time> nop;

<time>   is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
<str>    is quoted string.  To include a quote char ("), inside <str> use
         the '\' character to escape it.  To include the escape char (\) inside
         <str> use '\\'
      
<row>    row number (zero index based)
<col>    column number (zero index based)
<len>    number of character in <str> to write to the display
<num>    led number. A index of 0 will turn on/off all of the 'circle' LEDs.  
         Index 1 maps to physical LED4, Index 12 maps to physical LED15.
<state>  is 'on' or 'off'
<font>   Font to use: Values:= [ '5x7', '8x8', '8x16', 'bitmaps', 'symbols' ]
 

Events (generate by to simulation)
---------------------
^switches <num> <state>[ <num> <state>]*;


<num> is the switch number.  Range: 1, 2, 3
<state> is 'pressed', 'released'

"""

from Tkinter import Tk, Canvas, Text, StringVar, BOTH, W, N, NW, E, S, RAISED
from ttk import Frame, Button, Label, LabelFrame, Style
from Tkinter import tkinter
import math
import socket, errno
import sys
import threading, time
              
#------------------------------------------------------------------------------
class MyFont:
    def __init__(self, canvas, x_coord, y_coord, fontName, style, maxRow, maxCol, perCol, perRow, fontSize, scale, bgcolor ):
        self.canvas   = canvas
        self.xoff     = x_coord + 1
        self.yoff     = y_coord + 1
        self.font     = fontName
        self.style    = style
        self.maxRow   = maxRow
        self.maxCol   = maxCol
        self.perCol   = perCol
        self.perRow   = perRow
        self.scale    = scale
        self.bgcolor  = bgcolor
        self.fontSize = fontSize * scale
        self.chars    = []
        self.blanks   = []
         
        for row in range(maxRow):
            for col in range(maxCol):
                self.chars.append( self.canvas.create_text( 0,0, anchor=NW, font=(fontName, 0 - self.fontSize, style), text=" " ) )
                x1, y1 = self.index_to_xy( row, col, 0 )
                self.blanks.append( self.canvas.create_rectangle(x1, y1, x1+perCol*scale-1, y1+perRow*scale-1, outline=self.bgcolor, fill=self.bgcolor, width=1) )
                
    def index_to_xy( self, row, col, idx ):
        return (col+idx)*self.perCol*self.scale + self.xoff, row*self.perRow*self.scale + self.yoff
        
    def write( self, row, col, message, maxlen ):
        strlen = len(message)
        maxlen = maxlen if strlen > maxlen else strlen
        
        if ( row < self.maxRow and col < self.maxCol ):
            start = row * self.maxCol + col
            for i in range( maxlen ):
                if ( col+i < self.maxCol ):
                    x, y = self.index_to_xy( row, col, i )
                    self.canvas.tag_raise( self.blanks[start+i] )
                    self.canvas.delete( self.chars[start+i] )
                    self.chars[start+i] = self.canvas.create_text( x, y, anchor=NW, font=(self.font, 0-self.fontSize, self.style ), text=message[i:i+1] )
                    

class Leds(Canvas):
    def __init__(self, parent, x_coord, y_coord, radius, scale ):
        Canvas.__init__(self, parent, relief=RAISED, borderwidth=2, bg="grey", width=(radius+x_coord/2) * scale * 2 , height=(radius+y_coord/2) * scale * 2)
        self.parent       = parent
        self.scale        = scale
        self.radius       = radius*scale
        self.xcenter      = x_coord + self.radius
        self.ycenter      = y_coord + self.radius
        self.ledradius    = 2 * scale
        self.ledcolor     = []
        self.ledxy        = []
        self.led          = []
        
        positions = [ 75, 45, 15,  270+75, 270+45, 270+15,  180+75, 180+45, 180+15,  90+75, 90+45, 90+15]
        colors    = [ "green", "green", "red",  "red","green", "green",  "red", "red", "green",  "green", "red", "red" ] 
        idx       = 0
        for i in positions:
            p = math.radians(i)
            xy = (math.cos(p) *self.radius + self.xcenter, self.ycenter - math.sin(p)*self.radius ) 
            self.ledcolor.append( colors[idx] )
            self.ledxy.append( xy )
            self.led.append( self.create_oval( xy[0] - self.ledradius, xy[1] - self.ledradius, xy[0] + self.ledradius, xy[1] + self.ledradius, outline="black", width=1 ) )
            idx += 1
            
    def set_led( self, led_num, state=True):
        if ( led_num < 12 ):
            self.delete( self.led[led_num] )
            xy        = self.ledxy[led_num]
            fillcolor = self.ledcolor[led_num] if state == True else "white"
            self.led[led_num] = self.create_oval( xy[0] - self.ledradius, xy[1] - self.ledradius, xy[0] + self.ledradius, xy[1] + self.ledradius, outline="black", fill=fillcolor, width=1 )


class Lcd(Canvas):
    def __init__(self, parent, x_coord, y_coord, scale, max_x, max_y, bgcolor ):
        Canvas.__init__(self, parent, relief=RAISED, borderwidth=2, bg=bgcolor, width=x_coord*2 + max_x * scale, height=y_coord*2 + max_y * scale )
        
        self.parent       = parent
        self.scaled_max_x = max_x * scale
        self.scaled_max_y = max_y * scale
        self.scale        = scale
        
        self.create_rectangle(x_coord, y_coord, x_coord+max_x*scale +2, x_coord+max_y*scale +2, outline="#f11", width=2)
        
        self.font_5x7     = MyFont( self, x_coord, y_coord, "Courier", "normal", 8, 19, 5, 8, 7, scale, bgcolor )
        self.font_8x8     = MyFont( self, x_coord, y_coord, "Monospace", "bold",   8, 12, 8, 8, 7, scale, bgcolor )
        self.font_8x16    = MyFont( self, x_coord, y_coord, "Monospace", "normal", 4, 12, 8, 16, 15, scale, bgcolor )
        self.font         = self.font_5x7
        
    def set_font_5x7(self):
        self.font = self.font_5x7
        
    def set_font_8x8(self):
        self.font = self.font_8x8
                 
    def set_font_8x16(self):
        self.font = self.font_8x16
   
    def set_font_bitmaps(self):
        pass
   
    def set_font_symbols(sefl):
        pass
        
    def write( self, line_num, col_num, message, maxlen ):
        self.font.write( line_num, col_num, message, maxlen )
   
    def clear_line( self, line_num ):
        self.font.write( line_num, 0, " " * self.font.maxCol, self.font.maxCol )
          
    def clear_screen( self ):
        font = self.font
        self.set_font_8x16()
        for i in range( 4 ):
            self.clear_line(i)
        self.font = font
        

class PushButton(Button):
    def __init__(self, parent, bt_text, switches, width=0, sw1=None, sw2=None, sw3=None ):
        Button.__init__(self, parent, text=bt_text, width=width)
        self.text     = bt_text
        self.switches = switches
        self.sw1      = sw1
        self.sw2      = sw2
        self.sw3      = sw3
         
        self.bind("<Button-1>", self.down)
        self.bind("<ButtonRelease-1>", self.up)
    
    def down(self, event):
        if self.sw1:
            self.switches.set_sw1_state(True)
        if self.sw2:
            self.switches.set_sw2_state(True)
        if self.sw3:
            self.switches.set_sw3_state(True)

        self.switches.update_app()
     
    def up(self, event):
        if self.sw1:
            self.switches.set_sw1_state(False)
        if self.sw2:
            self.switches.set_sw2_state(False)
        if self.sw3:
            self.switches.set_sw3_state(False)

        self.switches.update_app()
        
        

class Sim(Frame):
  
    def __init__(self, parent,switches):
        Frame.__init__(self, parent)   
        self.parent = parent        
        self.columnconfigure(0,pad=6)
        self.columnconfigure(1,pad=6)
        self.columnconfigure(2,pad=6)
        self.columnconfigure(3,pad=6)
        self.rowconfigure(0,pad=6)
        self.rowconfigure(1,pad=6)
        self.rowconfigure(2,pad=6)
        self.rowconfigure(3,pad=6)
        self.rowconfigure(4,pad=6)
        self.rowconfigure(5,pad=6)
        self.rowconfigure(6,pad=6)
        
        self.parent.title("Sim-YRDKR62N Board")        
        self.style = Style()
        self.style.theme_use("default")
        self.pack(fill=BOTH, expand=1)


        l1 = Label(self, text="96x64 Graphic LCD", font=("Helvetica", 16))
        l1.grid(row=0, column=0, columnspan=3 )
        l2 = Label(self, text="LED15     LED4   ", font=("Helvetica", 16))
        l2.grid(row=0, column=3 )

        self.lcd = Lcd(self,10,10,3,96,64, "grey")
        self.lcd.grid(row=1, column=0, columnspan=3, sticky=NW )

        self.led = Leds(self, 50, 20, 30, 2 )
        self.led.grid(row=1, column=3, sticky=NW )

        sw1   = PushButton(self, "SW1", switches, sw1=True )
        sw1.grid(row=2, column=0)
        sw2   = PushButton(self, "SW2", switches, sw2=True )
        sw2.grid(row=2, column=1)
        sw3   = PushButton(self, "SW3", switches, sw3=True )
        sw3.grid(row=2, column=2)
        sw12  = PushButton(self, "SW1+SW2", switches, sw1=True, sw2=True  )
        sw12.grid(row=3, column=0 )
        sw13  = PushButton(self, "SW1+SW3", switches, sw1=True, sw3=True  )
        sw13.grid(row=3, column=1 )
        sw23  = PushButton(self, "SW2+SW3", switches, sw2=True, sw3=True  )
        sw23.grid(row=3, column=2 )
        sw123 = PushButton(self, "SW1+SW2+SW3", switches, sw1=True, sw2=True, sw3=True  )
        sw123.grid(row=4, column=1 )
        
        dummy = Label(self, text="", font=("Helvetica", 16) )
        dummy.grid(row=5,column=3,sticky=E, padx=5)
        
        heading = Label(self, text="Last Time:", font=("Helvetica", 14) )
        heading.grid(row=6, column=0, columnspan=1, sticky=E, padx=2 )
        self.cputime = StringVar()
        self.cputime.set("dd hh:mm:ss.sss")
        cputime = Label(self, textvariable=self.cputime, font=("Monospace", 14), foreground="blue" )
        cputime.grid(row=6, column=1, columnspan=2, sticky=W, padx=0 )
         
        bexit = Button(self, text="Exit", command=self.exit )
        bexit.grid(row=6,column=3,sticky=E, padx=5)


    def update_cputime(self, cputime ):
        self.cputime.set( cputime )

                
    def exit(self):
        self.parent.quit()
              

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
        print 'Socket created'
 
        # Bind socket to local host and port
        try:
            self.s.bind( (host,port) )
        except socket.error as msg:
            print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
            sys.exit()
     
        print 'Socket bind complete'
 
   
        # Start listening on socket
        self.s.listen(1)
        print 'Listening on port ', port
    
        # wait to accept a connection - blocking call
        print 'Waiting for a connection...'
        self.socket, self.client_addr = self.s.accept()
        self.socket.setblocking(0)
        print 'Connected with ' + self.client_addr[0] + ':' + str(self.client_addr[1])

        # Signal the main thread to proceed (then let the listener thread end)
        event.set()
        self.s.close()
        
                
    def __del__(self):
        print "Shutting down socket connections..."
        try:
            self.s.close()
            self.socket.close()
        except:
            pass
            
    def get_socket(self):
        return self.socket
        
        
class Switch_state:
    def __init__(self, socket, sw1,sw2,sw3 ):
        self.sw1    = sw1
        self.sw2    = sw2
        self.sw3    = sw3
        self.socket = socket
        
    def set_sw1_state(self, state ):
        self.sw1 = state
        
    def set_sw2_state(self, state ):
        self.sw2 = state
        
    def set_sw3_state(self, state ):
        self.sw3 = state

    def update_app(self):
        self.socket.send( '^switches 1 {} 2 {} 3 {};\n'.format( 'pressed' if self.sw1 else 'released', 'pressed' if self.sw2 else 'released', 'pressed' if self.sw3 else 'released' ) )
            

class Input_handler:
    def __init__( self, root, sim, lcd, leds, socket ):
        self.root      = root
        self.sim       = sim
        self.socket    = socket
        self.lcd       = lcd
        self.leds      = leds  
        self.framer    = Framer()
        self.tokenizer = Tokenizer()
        self.commands  = {}
        self.commands.update( {'write':self.cmd_write} )
        self.commands.update( {'font':self.cmd_font} )
        self.commands.update( {'clearline':self.cmd_clearline} )
        self.commands.update( {'clearscreen':self.cmd_clearscreen} )
        self.commands.update( {'led':self.cmd_led} )
        self.commands.update( {'exit':self.cmd_exit} )
        self.commands.update( {'nop':self.cmd_nop} )
        self.fonts = {}
        self.fonts.update( {'5x7':self.lcd.set_font_5x7} )     
        self.fonts.update( {'8x8':self.lcd.set_font_8x8} )     
        self.fonts.update( {'8x16':self.lcd.set_font_8x16} )     
        self.fonts.update( {'bitmaps':self.lcd.set_font_bitmaps} )     
        self.fonts.update( {'symbols':self.lcd.set_font_symbols} )     


                  
    def read_socket(self):
        try:
            t = self.socket.recv(1024)
            
            # Cancel app if the socket is disconnected
            if not t:
                self.root.quit()            
                return
                
        except socket.error, e:
            # No data -->try again latter
            if e.args[0] == errno.EWOULDBLOCK: 
                self.root.after( 10, self.read_socket )
                return
                
            # Cancel app if there is a socket error
            else:
                print "Socket Error:", e
                self.root.quit()
                return
                
        # Process the input
        frames = self.framer.deframe( t )
        while len(frames) > 0:
            line = frames.pop(0)
            tokens = self.tokenizer.tokenize( line )
            if ( len(tokens) < 2 ):
                print "Invalid format: [{}]".format( line )
            else:
                try:
                    cmd = self.commands[tokens[1]]
                except:
                    print "Invalid command: [{}]".format( line )
                    continue
               
                # Run the command
                self.sim.update_cputime( tokens[0] )
                cmd( tokens )
                
        # All done -->try again latter for more input
        self.root.after( 10, self.read_socket )

  
    #-------------------------------------
    # <time> write <row> <col> <str> <len>
    def cmd_write( self, tokens ):
        
        # Valid command/args
        if ( len(tokens) != 6 ):
            self.invalid_syntax( tokens )
            return
        else:
            if ( not tokens[2].isdigit() or not tokens[3].isdigit() or not tokens[5].isdigit() ):
                self.invalid_args( tokens )
                return
                     
        # Run the command
        self.lcd.write( int(tokens[2]), int(tokens[3]), tokens[4], int(tokens[5]) )              
        
    #-------------------------------------
    # <time> font <font>
    def cmd_font( self, tokens ):
        # Valid command/args
        if ( len(tokens) != 3 ):
            self.invalid_syntax( tokens )
            return
        else:
            try:
                font = self.fonts[tokens[2]]
            except:
                self.invalid_args( tokens )
                return
    
        font()
                    
    #-------------------------------------
    # <time> clearline <row>
    def cmd_clearline( self, tokens ):
        if ( len(tokens) != 3 ):
            self.invalid_syntax( tokens )
            return
        else:
            if ( not tokens[2].isdigit() ):
                self.invalid_args( tokens )
                return

        # do command
        self.lcd.clear_line( int(tokens[2]) )
        
                
    #-------------------------------------
    # <time> clearscreen
    def cmd_clearscreen( self, tokens ):
        self.lcd.clear_screen()
        
        
    #-------------------------------------
    # <time> led <num> <state> [<num> <state>]*
    def cmd_led( self, tokens ):
        # Valid command/args
        if ( len(tokens) < 4 or len(tokens) % 2 != 0 ):
            self.invalid_syntax( tokens )
            return
        else:
            for i in range( (len(tokens) -2) /2 ):
                j = i*2 + 2
                if ( not tokens[j].isdigit() or (tokens[j+1] != 'on' and tokens[j+1] != 'off') ):
                    self.invalid_args( tokens )
                    return
        
        # update the LED(s)
        for i in range( (len(tokens) -2) /2 ):
            j      = i*2 + 2
            lednum = int(tokens[j])
            if ( lednum == 0 ):
                for n in range( 12 ):
                    self.leds.set_led( n, True if tokens[j+1] == 'on' else False )
            else:
                self.leds.set_led( lednum-1, True if tokens[j+1] == 'on' else False )
                
                
    #-------------------------------------
    # <time> exit
    def cmd_exit( self, tokens ):
        self.root.quit()
        
  
    #-------------------------------------
    # <time> nop
    def cmd_nop( self, tokens ):
        pass
        
        
    #-------------------------------------
    def invalid_syntax(self, tokens):
        print "Invalid syntax:", ' '.join(tokens)
    
    def invalid_args(self, tokens):
        print "Invalid args:", ' '.join(tokens)
        

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
  
    # Default the port number
    port = 5002
    if ( len(sys.argv) > 1 ):
        if ( sys.argv[1].isdigit() ):
            port = int(sys.argv[1])
    
    # Start listener and wait for a connection
    event = threading.Event()
    io   = Listener( "", port, event ) 
    while True:
        time.sleep(0.1)
        if event.is_set():
            break

    # Have conneciton -->Start the GUI
    print "Starting GUI..."
    switches = Switch_state( io.get_socket(), False, False, False )
    switches.update_app()
    root = Tk()
    sim  = Sim(root,switches)
    hdlr = Input_handler( root, sim, sim.lcd, sim.led, io.get_socket() )
    root.geometry("560x440")
    root.after( 10, hdlr.read_socket )
    root.mainloop()  


if __name__ == '__main__':
    main()  
    
    
