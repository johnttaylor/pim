/** @namespace Cpl::Io::File::Arduino

The Arduino namespace provides an interface for Arduino boards that support
a 'file system'.  

The 'SdFat' file system is currently support with the following caveats:

    o Short File names only, i.e. 8.3 with no spaces in the path/file names
    
    o The canonicalPath() method is not supported
    
    o ONLY the Cpl::Io::File standard directory separator - '/' - is supported,
      i.e. any Windows path separator and/or drive identifiers will NOT work

*/  



  