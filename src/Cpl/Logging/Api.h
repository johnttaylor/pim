#ifndef Cpl_Logging_Api_h_
#define Cpl_Logging_Api_h_
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
#include "Cpl/Logging/EntryData_T.h"
#include "Cpl/Logging/TimeApi.h"
#include "Cpl/Logging/Private_.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/Container/RingBufferMP.h"
#include <stdint.h>

 
/** Number of 'empty slots' required after encountering a full entry queue
    before resuming adding entries to the queue.  This value can NOT be less
    than 2.
 */
#ifndef OPTION_CPL_LOGGING_MIN_QUEUE_SPACE
#define OPTION_CPL_LOGGING_MIN_QUEUE_SPACE             3
#endif

  
///
namespace Cpl {
///
namespace Logging {


/*---------------------------------------------------------------------------*/
/** This method is used to initialize the Logging framework.  This method
    MUST be called BEFORE any 'logf' calls are made (i.e. call this method
    as early as possible in the application's startup-sequence).
 */
void initialize( Cpl::Container::RingBufferMP<EntryData_T>& logEntryFIFO,
                 uint32_t                                   categoryIdForQueueOverflow,
                 const char*                                categoryQueueOverflowText,
                 uint16_t                                   messageIdForQueueOverflow,
                 const char*                                messageQueueOverflowText ) noexcept;

/** This method is used to shutdown/stop the Logging framework.  This method
    should be called as part of orderly shutdown of the application.
 */
void shutdown() noexcept;

/*---------------------------------------------------------------------------*/
/** This method is used to enable one or more log Categories.  Returns the
    previous enabled/disabled mask. 

    Note: Upon initialization ALL categories are enabled

    The method is thread safe
 */
uint32_t enableCategory( uint32_t categoryMask ) noexcept;

/** This method is used to disable one or more log Categories.  Returns the
    previous enabled/disabled mask.

    The method is thread safe
 */
uint32_t disableCategory( uint32_t categoryMask ) noexcept;

/** This method is used to explicit set the category mask (i.e. the enable/
    disable methods are OR/AND operations, this is assignment operation)

    The method is thread safe
 */
void setCategoryMask( uint32_t newMask ) noexcept;

/** This method returns the current enabled/disabled Category mask

    The method is thread safe
 */
uint32_t getCategoryEnabledMask() noexcept;


/*---------------------------------------------------------------------------*/
/** This method is used to create log entry. The method has printf() semantics.
    The method is thread safe.

    This method is NOT intended to be called directly by the Application.  The
    recommended paradigm is for the each Application to define it own set
    of Logging Categories and Message IDs.  This include providing type-safe

    NOTES:
        - The CategoryId Enum must have an underlying integer type of: uint32_t
        - A maximum of 32 symbols can be defined for the CategoryID
        - Recommended that each CategoryID symbol must be less than 16 characters
        - The MessageId Enums must have an underlying integer type of: uint16_t
        - There is no limited (other than the BETTER_NUM limits) to number of 
          message IDS.
        - Recommended that each MessageID symbol must be less than 32 characters


    \code

    Given:
        BETTER_ENUM( CategoryId, uint32_t
             , CRITICAL    = 0x00000001
             , WARNING     = 0x00000002
             , EVENT       = 0x00000004
             , ALARM       = 0x00000008
             , SECURITY    = 0x00000010
             , INFO        = 0x00000020
             , METRICS     = 0x00000040
             , LOGGER      = 0x00000080 );

        BETTER_ENUM( CriticalMsg, uint16_t, OUT_OF_MEMORY, BOB_HAPPENED, .... );
        BETTER_ENUM( WarningMsg, uint16_t, BATTERY_LOW, CONNECTION_DROPPED, .... );
        ...

        void logf( CriticalMsg msgId, const char* msgTextFormat, ... ) noexcept
        {
            va_list ap;
            va_start( ap, msgTextFormat );
            vlogf<CategoryId,CriticalMsg>( CategoryId::CRITICAL, msgId, msgTextFormat, ap );
            va_end( ap );
        }

        void logf( WarningMsg msgId, const char* msgTextFormat, ... ) noexcept
        {
            va_list ap;
            va_start( ap, msgTextFormat );
            vlogf<CategoryId,WarningMsg>( CategoryId::WARNING, msgId, msgTextFormat, ap );
            va_end( ap );
        }

        ...

      \endcode
 */
template <class CATEGORY_ID, class MESSAGE_ID>
void vlogf( CATEGORY_ID catId, MESSAGE_ID msgId, const char* msgTextFormat, va_list ap ) noexcept
{
    // Generate and 'log' the entry
    createAndAddLogEntry_( catId, catId._to_string(), msgId, msgId._to_string(), msgTextFormat, ap );
}

/*---------------------------------------------------------------------------*/
/** This method is implemented the application - it converts numeric values
    for the Category and Message IDs into text strings.  The method returns
    true when successful, else false is returned if one or more the input
    values are not 'valid'

    Note: The output is only "as good" as the input, i.e. if the message IDs are
          not 'from' the category enum - the result will be indeterminate
 */
bool getIDStrings( uint32_t             categoryNumericValue,
                   uint16_t             messageIdNumericValue,
                   Cpl::Text::String&   dstCategoryString,
                   Cpl::Text::String&   dstMessageString ) noexcept;

};      // end namespaces
};
#endif  // end header latch
