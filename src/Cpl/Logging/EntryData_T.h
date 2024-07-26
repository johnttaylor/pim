#ifndef Cpl_Logging_EntryData_h_
#define Cpl_Logging_EntryData_h_
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


#include "Cpl/Logging/TimeApi.h"
#include "Cpl/Persistent/Payload.h"
#include <stdint.h>
#include <string.h>

/** The size, in bytes, reserved to store the text portion of the log entry.
    The size does NOT include the space reserved for the null terminator
 */
#ifndef OPTION_CPL_LOGGING_MAX_MSG_TEXT_LEN
#define OPTION_CPL_LOGGING_MAX_MSG_TEXT_LEN             128
#endif

 /** Maximum number of characters in the application CategoryID's enum symbols
  */
#ifndef OPTION_CPL_LOGGING_MAX_LEN_CATEGORY_ID_TEXT
#define OPTION_CPL_LOGGING_MAX_LEN_CATEGORY_ID_TEXT     16
#endif

  /** Maximum number of characters in the application MessageID's enum symbols
  */
#ifndef OPTION_CPL_LOGGING_MAX_LEN_MESSAGE_ID_TEXT
#define OPTION_CPL_LOGGING_MAX_LEN_MESSAGE_ID_TEXT      32
#endif

  /** The size, in bytes, need to 'format' the message text with the 'textified'
      category and message IDs.  The size does NOT include the space reserved
      for the null terminator
   */
#ifndef OPTION_CPL_LOGGING_MAX_FORMATTED_MSG_TEXT_LEN
#define OPTION_CPL_LOGGING_MAX_FORMATTED_MSG_TEXT_LEN   (OPTION_CPL_LOGGING_MAX_MSG_TEXT_LEN+OPTION_CPL_LOGGING_MAX_LEN_CATEGORY_ID_TEXT+1+OPTION_CPL_LOGGING_MAX_LEN_MESSAGE_ID_TEXT+2)
#endif


///
namespace Cpl {
///
namespace Logging {

/// Defines the content of the Log entry
class EntryData_T : public Cpl::Persistent::Payload
{
public:
    CplLoggingTime_T    timestamp;    //!< Time-stamp for the entry
    uint32_t            category;     //!< Category identifier
    uint16_t            msgId;        //!< Message type enumeration identifier.  
    char                msgText[OPTION_CPL_LOGGING_MAX_MSG_TEXT_LEN + 1];  //!< The 'text' associated with log entry. 

public:
    /// Total 'packed' length
    static constexpr unsigned entryLen = sizeof( timestamp ) + sizeof( category ) + sizeof( msgId ) + sizeof( msgText );

public:
    /// Constructor
    EntryData_T()
        : timestamp( 0 )
        , category( 0 )
        , msgId( 0 )
    {
        memset( msgText, 0, sizeof( msgText ) );
    }

public:
    /// See Cpl::Persistent::Payload (manual copy ensure no pad bytes are copied)
    size_t getData( void* dst, size_t maxDstLen ) noexcept
    {
        if ( maxDstLen >= entryLen )
        {
            uint8_t* dstPtr = (uint8_t*) dst;
            memcpy( dstPtr, &timestamp, sizeof( timestamp ) );
            memcpy( dstPtr + sizeof( timestamp ), &category, sizeof( category ) );
            memcpy( dstPtr + sizeof( timestamp ) + sizeof( category ), &msgId, sizeof( msgId ) );
            memcpy( dstPtr + sizeof( timestamp ) + sizeof( category ) + sizeof( msgId ), msgText, sizeof( msgText ) );
            return entryLen;
        }
        return 0;
    }

    /// See Cpl::Persistent::Payload
    bool putData( const void* src, size_t srcLen ) noexcept
    {
        if ( srcLen >= entryLen )
        {
            uint8_t* srcPtr = (uint8_t*) src;
            memcpy( &timestamp, srcPtr, sizeof( timestamp ) );
            memcpy( &category, srcPtr + sizeof( timestamp ), sizeof( category ) );
            memcpy( &msgId, srcPtr + sizeof( timestamp ) + sizeof( category ), sizeof( msgId ) );
            memcpy( msgText, srcPtr + sizeof( timestamp ) + sizeof( category ) + sizeof( msgId ), sizeof( msgText ) );
            return true;
        }
        return false;
    }
};



};      // end namespaces
};
#endif  // end header latch
