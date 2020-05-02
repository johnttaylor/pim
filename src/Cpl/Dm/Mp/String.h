#ifndef Cpl_Dm_Mp_String_h_
#define Cpl_Dm_Mp_String_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
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


/** This class provides a concrete implementation for a Point who's data is a
    null terminated string.  The storage for the internal string storage is
    allocated (from the heap) ONCE when the instance is constructed, i.e. fixed
    length (per instance) storage.

	The toJSON()/fromJSON format is:
	\code

	{ name:"<mpname>", type:"<mptypestring>", invalid:nn, seqnum:nnnn, locked:true|false, maxlen:nnn, val:"<newvalue>" }

	\endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class String : public Cpl::Dm::ModelPointCommon_
{
public:
    /** The MP's Data container.
        NOTE: The client(s) are RESPONSIBLE for honoring the max string length
              and ensuring that the resultant string is properly null
              terminated and the stringLen field is set correctly
     */
    typedef struct
    {
        char*  stringPtr;       //!< Pointer to the string data.  MUST ALWAYS point to a null terminated string!
        size_t stringLen;       //!< Length, in bytes, of the string data.  Does NOT include the null terminator.
        size_t maxLength;       //!< Maximum length, in bytes, for the string data NOT including the null terminator
    } Data;

protected:
    /// Storage for the MP's data
    Data m_data;

public:
    /// Constructor.  Invalid MP. The 'maxLength' specifies the size, in bytes, of the string storage EXCLUDING the null terminator
    String( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t maxLength );

    /// Constructor.  Valid MP. Requires an initial value. The 'maxLength' specifies the size, in bytes, of the string storage EXCLUDING the null terminator
    String( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, size_t maxLength, const char* initialValue );

    /// Destructor (free up allocate array memory)
    ~String();

public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    virtual int8_t read( Data& dstData, uint16_t* seqNumPtr=0 ) const noexcept;

    /// Type safe read. See Cpl::Dm::ModelPoint
    virtual int8_t read( Cpl::Text::String& dstData, uint16_t* seqNumPtr=0 ) const noexcept;

    /// Type safe write. See Cpl::Dm::ModelPoint
    virtual uint16_t write( const Data& srcData, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Type safe write of a null terminated string. See Cpl::Dm::ModelPoint
    virtual uint16_t write( const char* srcData, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Same as write(), except only writes at most 'srcLen' bytes
    virtual uint16_t write( const char* srcData, size_t srcLen, LockRequest_T lockRequest = eNO_REQUEST ) noexcept;

    /// Type safe read-modify-write client callback interface
    typedef Cpl::Dm::ModelPointRmwCallback<Data> Client;

    /** Type safe read-modify-write. See Cpl::Dm::ModelPoint

       NOTE: THE USE OF THIS METHOD IS STRONGLY DISCOURAGED because it has
             potential to lockout access to the ENTIRE Model Base for an
             indeterminate amount of time.  And alternative is to have the
             concrete Model Point leaf classes provide the application
             specific read, write, read-modify-write methods in addition or in
             lieu of the read/write methods in this interface.
     */
    virtual uint16_t readModifyWrite( Client& callbackClient, LockRequest_T lockRequest = eNO_REQUEST );

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<String> Observer;

    /// Type safe register observer
    virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// Type safe un-register observer
    virtual void detach( Observer& observer ) noexcept;


public:
	/// See Cpl::Dm::Point.  
	bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose=true ) noexcept;

    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;

    /// See Cpl::Dm::ModelPoint.  Note: the returned sized does DOES NOT the null terminator
    size_t getSize() const noexcept;


public:
	/// See Cpl::Dm::Point.  
	bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept;

    /// See Cpl::Dm::ModelPoint. Note: dstSize DOES NOT include the null terminator
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept;

    /// See Cpl::Dm::ModelPoint.  Note: srcSize DOES NOT include the null terminator
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept;

    /// See Cpl::Dm::ModelPoint.  
    bool isDataEqual_( const void* otherData ) const noexcept;

    /// See Cpl::Dm::ModelPoint.  
    const void* getImportExportDataPointer_() const noexcept;

    /// See Cpl::Dm::ModelPoint.  
    size_t getInternalDataSize_() const noexcept;

    /// See Cpl::Dm::ModelPoint.  
    bool importMetadata_( const void* srcDataStream, size_t& bytesConsumed ) noexcept;

    /// See Cpl::Dm::ModelPoint.  
    bool exportMetadata_( void* dstDataStream, size_t& bytesAdded ) const noexcept;

};



};      // end namespaces
};
};
#endif  // end header latch
