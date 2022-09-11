#ifndef Cpl_Dm_Mp_ElapsedPrecisionTime_h_
#define Cpl_Dm_Mp_ElapsedPrecisionTime_h_
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
#include "Cpl/System/ElapsedTime.h"

///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data a
    Cpl::System::ElapsedTime::Precision_T data structure

    The toJSON/fromJSON() format is:
    \code

    { name:"<mpname>", type=:<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:"DD HH:MM:SS.sss" }

    Note: When writing a value, the 'DD' and 'sss' fields are optional.

    \endcode


    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class ElapsedPrecisionTime : public ModelPointCommon_
{
public:
    /// Constructor. Invalid MP. 
    ElapsedPrecisionTime( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        :Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    ElapsedPrecisionTime( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, Cpl::System::ElapsedTime::Precision_T initialValue )
        :Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
    {
        m_data = initialValue;
    }

public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    inline bool read( Cpl::System::ElapsedTime::Precision_T& dstData, uint16_t* seqNumPtr=0 ) const noexcept
    {
        return readData( &dstData, sizeof( m_data ), seqNumPtr );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    inline uint16_t write( Cpl::System::ElapsedTime::Precision_T newValue, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return writeData( &newValue, sizeof( m_data ), lockRequest );
    }

    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const ElapsedPrecisionTime& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return copyDataAndStateFrom( src, lockRequest );
    }

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<ElapsedPrecisionTime> Observer;

    /// Type safe register observer
    void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept;

    /// Type safe un-register observer
    void detach( Observer& observer ) noexcept;


public:
    ///  See Cpl::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept;

    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept;

protected:
    /// See Cpl::Dm::Point.  
    void setJSONVal( JsonDocument& doc ) noexcept;

protected:
    /// My data
    Cpl::System::ElapsedTime::Precision_T m_data;
};



};      // end namespaces
};
};
#endif  // end header latch
