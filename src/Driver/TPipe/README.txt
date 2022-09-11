/** @namespace Driver::TPipe

The 'TPipe' namespace provides a point-to-point full duplex 'pipe' used 
to pass text based commands between to two end points.  Each command is 
encapsulated in a HDLC like frame (i.e. Start-of-Frame, End-of-Frame characters,
etc.).  The first token in the frame is a 'command verb'.  The command verb and
any additional frame data is application specific.  The 'command verb' must
start immediately after the SOF character (e.g. no leading whitespace)

The connection between the two end-point is done by using two Cpl::Io streams,
one each for TX and RX.  The physical media is defined by the concrete stream
classes.  

A Pipe can have many 'transmit' and 'receive' clients.  

Transmitting frames is guaranteed to be an atomic operation, i.e. if a frame
is being transmitted, a requested to transmit a new frame will be blocked
until the transmission of the current frame has completed.

Incoming frames are processed by Clients registering frame handlers that 
identify the individual frames/command they process.  The frame handlers execute 
in the Pipe's thread.  Each frame handler is responsible for providing 
thread-safe with respect to the application.

*/  


  