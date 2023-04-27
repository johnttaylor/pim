/*-----------------------------------------------------------------------------
* This file is part of the Shift-Right Open Repository.  The Repository is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the root directory or on the Internet at
* http://www.shift-right.com/openrepo/license.htm
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Bsp/ST/NUCLEO-F767ZI/alpha1/console/Output.h"
#include "Cpl/System/Trace.h"

#ifndef USE_PRINTF_IO

// Have trace and the console share the same stream
Cpl::Io::Output* Cpl::System::Trace::getDefaultOutputStream_( void ) noexcept
{
    return &g_bspConsoleStream;
}


////////////////////////////////////////////////////////////////////////////////
#else
// Prototype for io_putchar() function
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
extern "C"                  int __io_putchar( int ch );
#define CALL_PUTCHAR(c)     __io_putchar(c)

#else
extern "C"                  int fputc( int ch, FILE * f );
#define CALL_PUTCHAR(c)     fputc(c,nullptr)


#endif /* __GNUC__ */



/// Anonymous namespace
namespace
{
/** This concrete class implement the CPL IO Output stream using GCC's low
    level __io_putchar() call (aka same output stream as printf).
 */
class IoPutChar : public Cpl::Io::Output
{
public:
    /// Constructor
    IoPutChar(){}

    /// See Cpl::Io::Output
    bool write( const void* buffer, int maxBytes, int& bytesWritten )
    {
        bytesWritten = maxBytes;
        const uint8_t* pt = (const uint8_t*) buffer;
        while ( maxBytes-- )
        {
            CALL_PUTCHAR( *pt++ );
        }
        return true;
    }

    /// See Cpl::Io::Output
    void flush() {}
    void close() {}
    bool isEos() { return false; }

};

} // end anonymous namespace



////////////////////////////////////////////////////////////////////////////////
///
static IoPutChar fd_;

Cpl::Io::Output* Cpl::System::Trace::getDefaultOutputStream_( void ) noexcept
{
    return &fd_;
}
#endif  // end USE_PRINTF_IO