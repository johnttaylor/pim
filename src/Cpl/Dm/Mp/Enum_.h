#ifndef Cpl_Dm_Mp_Enum_h_
#define Cpl_Dm_Mp_Enum_h_
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

/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a BETTER_ENUM type. The child classes must provide the 
    following:

        getTypeAsText() method and a typedef for child specific 'Observer'

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
        documented otherwise.
 */
template<class BETTERENUM_TYPE, class MPTYPE>
class Enum_ : public Cpl::Dm::ModelPointCommon_
{
protected:
    /// The element's value
    BETTERENUM_TYPE m_data;

public:
    /// Constructor: Invalid MP
    Enum_( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
        , m_data( BETTERENUM_TYPE::_from_index_unchecked( 0 ) )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    Enum_( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, BETTERENUM_TYPE initialValue )
        :Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
        , m_data( BETTERENUM_TYPE::_from_index_unchecked( 0 ) )
    {
        m_data = initialValue;
    }

public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    inline bool read( BETTERENUM_TYPE& dstData, uint16_t* seqNumPtr = 0 ) const noexcept
    {
        return readData( &dstData, sizeof( BETTERENUM_TYPE ), seqNumPtr );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    inline uint16_t write( BETTERENUM_TYPE newValue, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        return writeData( &newValue, sizeof( BETTERENUM_TYPE ), lockRequest );
    }

    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MPTYPE& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return copyDataAndStateFrom( src, lockRequest );
    }

    /// Type safe register observer
    inline void attach( Cpl::Dm::Subscriber<MPTYPE>& observer, uint16_t initialSeqNumber = SEQUENCE_NUMBER_UNKNOWN ) noexcept
    {
        attachSubscriber( observer, initialSeqNumber );
    }

    /// Type safe un-register observer
    inline void detach( Cpl::Dm::Subscriber<MPTYPE>& observer ) noexcept
    {
        detachSubscriber( observer );
    }


protected:
    /// See Cpl::Dm::Point.  
    void setJSONVal( JsonDocument& doc ) noexcept
    {
        doc["val"] = (char*) m_data._to_string();
    }

public:
    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
    {
        // Get the enum string
        const char* newValue = src;
        if ( newValue == nullptr )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid syntax for the 'val' key/value pair";
            }
            return false;
        }

        // Convert the text to an enum value
        auto maybeValue = BETTERENUM_TYPE::_from_string_nothrow( newValue );
        if ( !maybeValue )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Invalid enum value (%s)", newValue );
            }
            return false;
        }

        retSequenceNumber = write( *maybeValue, lockRequest );
        return true;
    }
};



};      // end namespaces
};
};
#endif  // end header latch
