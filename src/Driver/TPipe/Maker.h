#ifndef Driver_TPipe_Maker_h_
#define Driver_TPipe_Maker_h_
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
/** @file */

#include "colony_config.h"
#include "Driver/TPipe/Pipe.h"


/** The size, in bytes, of the work buffer used to read from the input stream.
    Note: This size does NOT limit the how large a received frame can be
 */
#ifndef OPTION_DRIVER_TPIPE_RAW_INPUT_SIZE
#define OPTION_DRIVER_TPIPE_RAW_INPUT_SIZE		128
#endif

///
namespace Driver {
///
namespace TPipe {


/** This concrete class is a "Maker" that assembles the objects needed
    for TPipe.
 */
class Maker : public Cpl::Text::Frame::StreamDecoder
{
public:
    /** Constructor.  
        @param frameHandlerList The set of received frame handlers.  Note: frame
                                handler's self register
        @param maxRxFrameSize   The size, in bytes, of the buffer used to hold
                                an incoming Frame.  
        @param verbDelimiters   The delimiter characters used to separate the
                                command verb from the rest of commands tokens/data.
                                This string must stay in scope for life of the
                                Maker instance.
        @param startOfFrame     Framing: Start-of-Frame character
        @param endOfFrame       Framing: End-of-Frame character
        @param escapeChar       Framing: Escape character
     */
    Maker( Cpl::Container::Map<RxFrameHandlerApi>& frameHandlerList,
           size_t                                  maxRxFrameSize,
           const char*                             verbDelimiters = " ",
           char                                    startOfFrame   = '^',
           char                                    endOfFrame     = ';',
           char                                    escapeChar     = '`'  );


public:
    /// Returns a reference to the TPipe processor
    Pipe& getPipeProcessor( void ) noexcept { return m_processor; }

    /// Cast-operator: Short-hand for getPipeProcessor()
    operator Pipe& () { return m_processor; }

protected:
    /// See Cpl::Text::Frame::Decoder_
    bool isStartOfFrame() noexcept;

    /// See Cpl::Text::Frame::Decoder_
    bool isEofOfFrame() noexcept;

    /// See Cpl::Text::Frame::Decoder_
    bool isEscapeChar() noexcept;

    /// See Cpl::Text::Frame::Decoder_
    bool isLegalCharacter() noexcept;


protected:
    /// Framer for the output
    Cpl::Text::Frame::StreamEncoder m_framer;

    /// Pipe Processor
    Pipe                            m_processor;

    /// Work buffer for raw incoming data (is not a null terminated string)
    char                            m_workBuffer[OPTION_DRIVER_TPIPE_RAW_INPUT_SIZE];

    /// SOF character
    const char                      m_sof;

    /// EOF character
    const char                      m_eof;

    /// Escape character
    const char                      m_esc;
};


};      // end namespaces
};
#endif  // end header latch
