/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "colony_config.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/FString.h"
#include "Cpl/Text/format.h"
#include "Cpl/Io/InputOutput.h"
#include "Cpl/Container/RingBuffer.h"
#include <string.h>


#define SECT_     "_0test"

#ifndef OPTION_TEST_END_OF_FRAME_CHAR
#define OPTION_TEST_END_OF_FRAME_CHAR    0x0D
#endif


extern void echo_test( Cpl::Io::InputOutput& fd );
extern size_t getErrorCounts( bool clearCounts = false );

#define ECHO_BUFFER_SIZE        (1024*10)

#define RX_READ_BUFFER_SIZE     64

////////////////////////////////////////////////////////////////////////////////
namespace {

class Receiver : public Cpl::System::Runnable
{
public:
    ///
    Cpl::Io::InputOutput&           m_fd;
    ///
    uint8_t                         m_rxMsg[RX_READ_BUFFER_SIZE];
    ///
    Cpl::Text::FString<128>         m_tmpBuf;
    ///
    int                             m_numEchoBytes;
    ///
    uint8_t                         m_echoMemory[ECHO_BUFFER_SIZE + 1];

public:
    Receiver( Cpl::Io::InputOutput& fd )
        : m_fd( fd )
    {
    }

public:
    void appRun()
    {
        // Throw any trash bytes on startup
        while ( m_fd.available() )
        {
            int bytesRead;
            m_fd.read( m_rxMsg, sizeof( m_rxMsg), bytesRead );
        }

        size_t   byteCount = 0;
        uint8_t* dstPtr    = m_echoMemory;
        m_numEchoBytes     = 0;

        for ( ;;)
        {

            int bytesRead;
            if ( m_fd.read( m_rxMsg, sizeof( m_rxMsg ), bytesRead ) )
            {
                Bsp_Api_toggle_debug1();
                byteCount += bytesRead;

                // Copy the data to the buffer while looking for ^Q
                uint8_t* ptr = m_rxMsg;
                while ( bytesRead-- )
                {
                    uint8_t inbyte = *ptr++;
                    *dstPtr++ = inbyte;
                    m_numEchoBytes++;

                    // End-of-Frame is the 'trigger' to output data
                    if ( inbyte == OPTION_TEST_END_OF_FRAME_CHAR )
                    {
                        echoMemory( byteCount );
                        dstPtr         = m_echoMemory;
                        m_numEchoBytes = 0;
                    }
                }
            }
        }
    }

    void echoMemory( size_t byteCount )
    {
        int bytes = m_numEchoBytes;

        uint8_t* srcData = m_echoMemory;
        while ( m_numEchoBytes )
        {
            int outlen = 16;
            if ( m_numEchoBytes < 16 )
            {
                outlen = m_numEchoBytes;
            }

            Cpl::Text::bufferToViewer( srcData, outlen, m_tmpBuf );
            srcData        += outlen;
            m_numEchoBytes -= outlen;
            m_fd.write( m_tmpBuf );
            m_fd.write( "\n" );
        }

        m_tmpBuf.format( "\nRX: frame=%6d, total count=%7lu (total errs=%3lu):\n", bytes, byteCount, getErrorCounts() );
        m_fd.write( m_tmpBuf );

    }
};




};  // end namespace


////////////////////////////////////////////////////////////////////////////////


void echo_test( Cpl::Io::InputOutput& fd )
{
    printf( "\nStarting Echo test...\n\n" );
    Receiver* rxPtr = new(std::nothrow) Receiver( fd  );
    Cpl::System::Thread::create( *rxPtr, "RX" );

    // Start the scheduler
    Cpl::System::Api::enableScheduling();
}


