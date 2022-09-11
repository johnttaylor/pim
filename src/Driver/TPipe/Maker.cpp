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

#include "Maker.h"

using namespace Driver::TPipe;

////////////////////////////////////////////
Maker::Maker( Cpl::Container::Map<RxFrameHandlerApi>& frameHandlerList,
              size_t                                  maxRxFrameSize,
              const char*                             verbDelimiters,
              char                                    startOfFrame,
              char                                    endOfFrame,
              char                                    escapeChar )
    : StreamDecoder( m_workBuffer, OPTION_DRIVER_TPIPE_RAW_INPUT_SIZE, nullptr, false )
    , m_framer( nullptr, startOfFrame, endOfFrame, escapeChar, false )
    , m_processor( frameHandlerList, *this, m_framer, maxRxFrameSize, verbDelimiters )
    , m_sof( startOfFrame )
    , m_eof( endOfFrame )
    , m_esc( escapeChar )
{
}

////////////////////////////////////////////
bool Maker::isStartOfFrame() noexcept
{
    return *m_dataPtr == m_sof;
}

bool Maker::isEofOfFrame() noexcept
{
    return *m_dataPtr == m_eof;
}

bool Maker::isEscapeChar() noexcept
{
    return *m_dataPtr == m_esc;
}

bool Maker::isLegalCharacter() noexcept
{
    return *m_dataPtr != '\0';
}
