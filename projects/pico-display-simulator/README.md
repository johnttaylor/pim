# GUI for the Pico-Sketch Simulator

This Visual Studio C# Solution is used to build the Windows executable that 
provides the simulated Display, RGB LED, and buttons for the Pico-Sketch application.

The executable communicates with the Pico-Sketch application via TCP sockets.
The executable is a _listener_ on port `5010`.  The GUI application must be
launch prior to running the simulated Pico-Sketch console application.

#### Keys
The GUI has 4 button boxes/widgets - one for each physical key.  The application
uses key-combination (e.g. A+X to move right) to get 8 logical keys.  In the GUI
you can __not__ generate key-combination by selecting the button widgets with the 
mouse - you must use the keyboard for the key-combination.