#ifndef Cpl_Dm_Mp_String_h_
#define Cpl_Dm_Mp_String_h_
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

#include "Cpl/Dm/ModelPointCommon_.h"


 ///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This mostly concrete class provides the base implementation for a Point 
    who's data is a null terminated string.  The concrete child class is 
    responsible for providing the string storage and the attach/detach
    methods.

    The toJSON()/fromJSON format is:
    \code

    { name:"<mpname>", type:"<mptypestring>", valid:true|false seqnum:nnnn, locked:true|false, val:{maxLen:<len>,text:"<newvalue>" }

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.

    NOTE: The MP's null terminator for the string storage IS imported/exported.
 */
class StringBase_ : public Cpl::Dm::ModelPointCommon_
{
protected:
    /** Constructor. Invalid MP.
     */
    StringBase_( Cpl::Dm::ModelDatabase& myModelBase,
                 const char*             symbolicName,
                 char*                   myDataPtr,
                 size_t                  dataSizeInBytesIncludingNullTerminator );

    /// Constructor. Valid MP.  Requires an initial value
    StringBase_( Cpl::Dm::ModelDatabase& myModelBase,
                 const char*             symbolicName,
                 char*                   myDataPtr,
                 size_t                  dataSizeInBytesIncludingNullTerminator,
                 const char*             initialValue );


public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    bool read( Cpl::Text::String& dstData, uint16_t* seqNumPtr=0 ) const noexcept;

    /// Type safe read. See Cpl::Dm::ModelPoint
    bool read( char* dstData, size_t dataSizeInBytesIncludingNullTerminator, uint16_t* seqNumPtr=0 ) const noexcept;

    /// Type safe write of a null terminated string. See Cpl::Dm::ModelPoint
    inline uint16_t write( const char* srcNullTerminatedString, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return write( srcNullTerminatedString, strlen( srcNullTerminatedString ), lockRequest );
    }

    /// Same as write(), except only writes at most 'srcLen' bytes
    uint16_t write( const char* srcData, size_t dataSizeInBytesIncludingNullTerminator, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Returns the maximum size WITHOUT the null terminator of the string storage
    inline size_t getMaxLength() const noexcept
    {
        return m_dataSize - 1;
    }

    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    uint16_t copyFrom( const StringBase_& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Cpl::Dm::Mp::String";
    }

public:
    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept;

    /// See Cpl::Dm::Point.  
    bool isDataEqual_( const void* otherData ) const noexcept;

protected:
    /// See Cpl::Dm::Point.  
    void setJSONVal( JsonDocument& doc ) noexcept;
};


/** This concrete template class provides the storage for a Point
    who's data is a null terminated string.  

    Template Args:
        S:=      Max Size of the String WITHOUT the null terminator!
 */
template<int S>
class String : public StringBase_
{
public:
    /** Constructor. Invalid Point.
     */
    String( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : StringBase_( myModelBase, symbolicName, m_data, sizeof( m_data ) )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value
    String( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, const char* initialValue )
        : StringBase_( myModelBase, symbolicName, m_data, sizeof( m_data ), initialValue )
    {
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<String> Observer;

    /// Type safe register observer
    inline void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept
    {
        attachSubscriber( observer, initialSeqNumber );
    }

    /// Type safe un-register observer
    inline void detach( Observer& observer ) noexcept
    {
        detachSubscriber( observer );
    }

    /** This method is used to read the MP contents and synchronize
        the observer with the current MP contents.  This method should ONLY be
        used in the notification callback method and the 'observerToSync'
        argument MUST be the argument provided by the callback method

        Note: The observer will be subscribed for change notifications after
              this call.
     */
    inline bool readAndSync( Cpl::Text::String& dstData, SubscriberApi& observerToSync )
    {
        uint16_t seqNum;
        bool result = read( dstData, &seqNum );
        attachSubscriber( observerToSync, seqNum );
        return result;
    }

    /// Same as readAndSync() above, except using a raw char array
    inline bool readAndSync( char* dstData, size_t dataSizeInBytesIncludingNullTerminator, SubscriberApi& observerToSync )
    {
        uint16_t seqNum;
        bool result = read( dstData, dataSizeInBytesIncludingNullTerminator, &seqNum );
        attachSubscriber( observerToSync, seqNum );
        return result;
    }


protected:
    /// The MP's raw storage
    char m_data[S + 1];
};


};      // end namespaces
};
};
#endif  // end header latch
