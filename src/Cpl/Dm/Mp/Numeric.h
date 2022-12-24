#ifndef Cpl_Dm_Mp_Numeric_h_
#define Cpl_Dm_Mp_Numeric_h_
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
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/format.h"
#include "Cpl/Text/FString.h"
#include <string.h>


/// Endianess of a Bit array.  For little endian set to true; else set to false
#ifndef OPTION_CPL_DM_MP_BITARRAY_IS_LITTLE_ENDIAN
#define OPTION_CPL_DM_MP_BITARRAY_IS_LITTLE_ENDIAN        true
#endif


///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a C numeric primitive type of type: 'ELEMTYPE'.

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
        documented otherwise.
 */
template<class ELEMTYPE, class MPTYPE>
class Numeric : public Cpl::Dm::ModelPointCommon_
{
protected:
    /// The element's value
    ELEMTYPE    m_data;

protected:
    /// Constructor: Invalid MP
    Numeric( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        :Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    Numeric( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, ELEMTYPE initialValue )
        :Cpl::Dm::ModelPointCommon_( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
    {
        m_data = initialValue;
    }

public:
    /// Type safe read. See Cpl::Dm::ModelPoint
    inline bool read( ELEMTYPE& dstData, uint16_t* seqNumPtr = 0 ) const noexcept
    {
        return Cpl::Dm::ModelPointCommon_::readData( &dstData, sizeof( ELEMTYPE ), seqNumPtr );
    }

    /// Type safe write. See Cpl::Dm::ModelPoint
    inline uint16_t write( ELEMTYPE newValue, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        return Cpl::Dm::ModelPointCommon_::writeData( &newValue, sizeof( ELEMTYPE ), lockRequest );
    }

    /// Atomic increment
    inline uint16_t increment( ELEMTYPE incSize = 1, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = write( m_data + incSize, lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }

    /// Atomic decrement
    inline uint16_t decrement( ELEMTYPE decSize = 1, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = write( m_data - decSize, lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }

    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MPTYPE& src, LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
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

    /// See Cpl::Dm::ModelPointCommon
    inline bool readAndSync( ELEMTYPE& dstData, SubscriberApi& observerToSync )
    {
        return ModelPointCommon_::readAndSync( &dstData, sizeof( ELEMTYPE ), observerToSync );
    }

protected:
    /// See Cpl::Dm::Point.  
    void setJSONVal( JsonDocument& doc ) noexcept
    {
        doc["val"] = m_data;
    }

public:
    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
    {
        if ( src.is<ELEMTYPE>() )
        {
            retSequenceNumber = write( src.as<ELEMTYPE>(), lockRequest );
            return true;
        }
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }
};


/** This template class extends the Numeric<> class to provide bit operation
    on the numeric value.  The datatype of the numeric MUST be an integer
    type.

    The underlying storage of the bit array is N bit integers.  A side effect of
    this storage mechanism the bit ordering in the JSON 'val' string is dependent on the
    target platform's Endian architecture.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:"<bits>" }

        where <bits> is a string of N digits ('1' or '0') where the left most digit is
        is the MSb of byte[0] and the right most digit is the LSb of byte[N].
        Whether byte[0] is the MSB or LSB is dependent on the big/little Endian
        architecture of the target platform.

        For example a 16bit Array (as binary hex: dataword[0]=0x30, dataword[1]=0x09)

            val:"0011000000001001"

        \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.

 */
template <class WORDSIZE, class MPTYPE>
class BitArray_ : public Numeric<WORDSIZE, MPTYPE>
{
protected:
    /// Constructor. Invalid MP.
    BitArray_( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Numeric<WORDSIZE, MPTYPE>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    BitArray_( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, WORDSIZE initialValue )
        : Numeric<WORDSIZE, MPTYPE>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    /// Atomic operation to set the zero indexed bit to a 1.
    inline uint16_t setBit( uint8_t bitPosition, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = Numeric<WORDSIZE, MPTYPE>::write( Numeric<WORDSIZE, MPTYPE>::m_data | ( 1 << bitPosition ), lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }

    /// Atomic operation to set the zero indexed bit to a 0.
    inline uint16_t clearBit( uint8_t bitPosition, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = Numeric<WORDSIZE, MPTYPE>::write( Numeric<WORDSIZE, MPTYPE>::m_data & ( ~( 1 << bitPosition ) ), lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }

    /// Atomic operation to toggle the zero indexed bit.
    inline uint16_t flipBit( uint8_t bitPosition, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = Numeric<WORDSIZE, MPTYPE>::write( Numeric<WORDSIZE, MPTYPE>::m_data ^ ( 1 << bitPosition ), lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }


public:
    /// Atomic operation to clear ONLY the bits as specified by the bit mask.  
    inline uint16_t clearBitsByMask( uint16_t bitMask, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = Numeric<WORDSIZE, MPTYPE>::write( Numeric<WORDSIZE, MPTYPE>::m_data & ~( bitMask ), lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }

    /// Atomic operation to set the bits specified by the bit mask
    inline uint16_t setBitsByMask( uint16_t bitMask, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = Numeric<WORDSIZE, MPTYPE>::write( Numeric<WORDSIZE, MPTYPE>::m_data | bitMask, lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }

    /// Atomic operation to flip/toggle ONLY the bits as specified the bit mask
    inline uint16_t flipBitsByMask( uint16_t bitMask, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = Numeric<WORDSIZE, MPTYPE>::write( Numeric<WORDSIZE, MPTYPE>::m_data ^ bitMask, lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }



protected:
    /// See Cpl::Dm::Point.  
    void setJSONVal( JsonDocument& doc ) noexcept
    {
        Cpl::Text::FString<64> tmp;
        const void* dataPtr = &(Numeric<WORDSIZE, MPTYPE>::m_data);
        Cpl::Text::bufferToAsciiBinary( dataPtr, sizeof( Numeric<WORDSIZE, MPTYPE>::m_data ), tmp, false, OPTION_CPL_DM_MP_BITARRAY_IS_LITTLE_ENDIAN );
        doc["val"] = (char*) tmp.getString();
    }


public:
    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
    {
        if ( src.is<const char*>() )
        {
            const char* val   = src.as<const char*>();
            uint16_t    value = 0;
            if ( Cpl::Text::asciiBinaryToBuffer( &value, val, sizeof( value ), OPTION_CPL_DM_MP_BITARRAY_IS_LITTLE_ENDIAN ) > 0 )
            {
                retSequenceNumber = Numeric<WORDSIZE, MPTYPE>::write( value, lockRequest );
                return true;
            }
        }

        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }
};


/** This template class extends the implementation of Numeric<> class to support
    the pointers instead of integers

    NOTES:
    1) All methods in this class are NOT thread Safe unless explicitly
       documented otherwise.
 */
template <class MPTYPE>
class Pointer_ : public Numeric<size_t, MPTYPE>
{
protected:
    /// Constructor. Invalid MP.
    Pointer_( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
        : Numeric<size_t, MPTYPE>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Pointer_( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, void* initialValue )
        : Numeric<size_t, MPTYPE>( myModelBase, symbolicName, (size_t) initialValue )
    {
    }

public:
    /// See Cpl::Dm::Point.  
    void setJSONVal( JsonDocument& doc ) noexcept
    {
        Cpl::Text::FString<20> tmp;
        tmp.format( "%llX", ( unsigned long long ) Numeric<size_t, MPTYPE>::m_data );
        doc["val"] = (char*) tmp.getString();
    }

    /// See Cpl::Dm::Point.  
    bool fromJSON_( JsonVariant& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
    {
        if ( src.is<const char*>() )
        {
            const char*         val   = src.as<const char*>();
            unsigned long long  value = 0;
            if ( Cpl::Text::a2ull( value, val, 16 )  )
            {
                retSequenceNumber = Numeric<size_t, MPTYPE>::write( (size_t) value, lockRequest );
                return true;
            }
        }

        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }
};

};      // end namespaces
};
};
#endif  // end header latch
