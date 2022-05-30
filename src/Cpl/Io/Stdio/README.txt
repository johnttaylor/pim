/** @namespace Cpl::Io::Stdio
The Stdio namespace provides concrete classes for the C Library's 'stdin', 
'stdout', and 'stderr' input/output streams.  The class definitions (i.e the
header files) are platform independent so the Application can use them directly
without incurring any platform specific dependencies.  However, It is the 
Application's responsibility to build & link the appropriate platform specific
implementation of these classes. 

This namespace also supplies a few concrete helper classes that can be used for
implementations of several Cpl::Io abstract interfaces using the same platform 
specific implementing of the 'File Streams' that was/is used for the standard 
Input, Output, Error streams.  

Note: The "_ansi/' implementation of the Stdio classes uses the C library's 
      'FILE*' methods (fwrite, fread, etc.).

 */  


  