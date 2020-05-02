#ifndef Cpl_Dm_Mp_Basic_h_
#define Cpl_Dm_Mp_Basic_h_
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
#include "Cpl/System/Assert.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Text/FString.h"
#include <string.h>


///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This template class provides a mostly concrete implementation for a Model
	Point who's data is a C primitive type of type: 'ELEMTYPE'.

	NOTES:
		1) All methods in this class are NOT thread Safe unless explicitly
		documented otherwise.
 */
template<class ELEMTYPE>
class Basic : public Cpl::Dm::ModelPointCommon_
{
protected:
	/// The element's value
	ELEMTYPE    m_data;

public:
	/// Constructor: Invalid MP
	Basic( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo )
		:Cpl::Dm::ModelPointCommon_( myModelBase, &m_data, staticInfo, OPTION_CPL_DM_MODEL_POINT_STATE_INVALID )
	{
	}

	/// Constructor: Valid MP (requires initial value)
	Basic( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, ELEMTYPE initialValue )
		:Cpl::Dm::ModelPointCommon_( myModelBase, &m_data, staticInfo, Cpl::Dm::ModelPoint::MODEL_POINT_STATE_VALID )
	{
		m_data = initialValue;
	}

public:
	/// Type safe read. See Cpl::Dm::ModelPoint
	virtual int8_t read( ELEMTYPE& dstData, uint16_t* seqNumPtr = 0 ) const noexcept
	{
		return Cpl::Dm::ModelPointCommon_::read( &dstData, sizeof( ELEMTYPE ), seqNumPtr );
	}

	/// Type safe write. See Cpl::Dm::ModelPoint
	virtual uint16_t write( ELEMTYPE newValue, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
	{
		return Cpl::Dm::ModelPointCommon_::write( &newValue, sizeof( ELEMTYPE ), lockRequest );
	}

public:
	/// See Cpl::Dm::ModelPoint.  This method IS thread safe.
	size_t getSize() const noexcept
	{
		return sizeof( ELEMTYPE );
	}

public:
	/// See Cpl::Dm::ModelPoint
	void copyDataTo_( void* dstData, size_t dstSize ) const noexcept
	{
		CPL_SYSTEM_ASSERT( dstSize == sizeof( ELEMTYPE ) );
		*( (ELEMTYPE*) dstData ) = m_data;
	}

	/// See Cpl::Dm::ModelPoint
	void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
	{
		CPL_SYSTEM_ASSERT( srcSize == sizeof( ELEMTYPE ) );
		m_data = *( (ELEMTYPE*) srcData );
	}

	/// See Cpl::Dm::ModelPoint.  The default implementation is for integers
	bool isDataEqual_( const void* otherData ) const noexcept
	{
		ELEMTYPE left = *( (ELEMTYPE*) otherData );
		return m_data == left;
	}

	/// See Cpl::Dm::Point.  
	const void* getImportExportDataPointer_() const noexcept
	{
		return (const void*) ( &m_data );
	}

	/// See Cpl::Dm::Point.  
	size_t getInternalDataSize_() const noexcept
	{
		return sizeof( ELEMTYPE );
	}
};

/** This template class extends the implementation of Basic<ELEMTYPE> to support
	the toJSON() and fromJSON_() methods for integer types.

	NOTES:
		1) All methods in this class are NOT thread Safe unless explicitly
		   documented otherwise.
 */
template<class ELEMTYPE>
class BasicInteger : public Cpl::Dm::Mp::Basic<ELEMTYPE>
{
protected:
	/// Flag for to/from json() methods
	bool        m_decimal;

public:
	/// Constructor: Invalid MP
	BasicInteger( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, bool decimalFormat = true )
		:Basic<ELEMTYPE>( myModelBase, staticInfo )
		, m_decimal( decimalFormat )
	{
	}

	/// Constructor: Valid MP (requires initial value)
	BasicInteger( Cpl::Dm::ModelDatabase& myModelBase, Cpl::Dm::StaticInfo& staticInfo, ELEMTYPE initialValue, bool decimalFormat = true )
		:Basic<ELEMTYPE>( myModelBase, staticInfo, initialValue )
		, m_decimal( decimalFormat )
	{
	}


    /// Atomic increment
    virtual uint16_t increment( ELEMTYPE incSize = 1, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = Basic<ELEMTYPE>::write( Basic<ELEMTYPE>::m_data + incSize, lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }

    /// Atomic decrement
    virtual uint16_t decrement( ELEMTYPE decSize = 1, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = Basic<ELEMTYPE>::write( Basic<ELEMTYPE>::m_data - decSize, lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }


public:
	/// See Cpl::Dm::Point.  
	bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose=true ) noexcept
	{
		// Get a snapshot of the my data and state
		Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
		ELEMTYPE value  = Basic<ELEMTYPE>::m_data;
		uint16_t seqnum = Basic<ELEMTYPE>::m_seqNum;
		int8_t   valid  = Basic<ELEMTYPE>::m_validState;
		bool     locked = Basic<ELEMTYPE>::m_locked;
		Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();

		// Start the conversion
		JsonDocument& doc = Cpl::Dm::ModelPointCommon_::beginJSON( valid, locked, seqnum, verbose );

		// Construct the 'val' key/value pair (as a simple numeric)
		if( Cpl::Dm::ModelPointCommon_::IS_VALID( valid ) )
		{
			if( m_decimal )
			{
				doc["val"] = value;
			}

			// Construct the 'val' key/value pair (as a HEX string)
			else
			{
				Cpl::Text::FString<20> tmp;
				tmp.format( "0x%llX", ( unsigned long long ) value );
				doc["val"] = (char*) tmp.getString();
			}
		}

		// End the conversion
		Cpl::Dm::ModelPointCommon_::endJSON( dst, dstSize, truncated, verbose );
		return true;
	}

	/// See Cpl::Dm::Point.  
	bool fromJSON_( JsonVariant& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
	{
		ELEMTYPE newValue = 0;

		// Attempt to parse the value key/value pair (as a simple numeric)
		if( m_decimal )
		{
			ELEMTYPE checkForError = src | (ELEMTYPE) 2;
			newValue = src | (ELEMTYPE) 1;
			if( newValue == (ELEMTYPE) 1 && checkForError == (ELEMTYPE) 2 )
			{
				if( errorMsg )
				{
					*errorMsg = "Invalid syntax for the 'val' key/value pair";
				}
				return false;
			}
		}

		// Attempt to parse the value as HEX string
		else
		{
			const char*        val = src;
			unsigned long long value;
			if( Cpl::Text::a2ull( value, val, 16 ) == false )
			{
				if( errorMsg )
				{
					*errorMsg = "Invalid syntax for the 'val' key/value pair";
				}
				return false;
			}

			newValue = (ELEMTYPE) value;
		}

		retSequenceNumber = Basic<ELEMTYPE>::write( newValue, lockRequest );
		return true;
	}
};

/** This template class extends the implementation of Basic<ELEMTYPE> to support
	the toJSON() and fromJSON_() methods for read/floating point types.

	NOTES:
	1) All methods in this class are NOT thread Safe unless explicitly
	documented otherwise.
 */
template<class ELEMTYPE>
class BasicReal : public Cpl::Dm::Mp::Basic<ELEMTYPE>
{
public:
	/// Constructor: Invalid MP
	BasicReal( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, bool decimalFormat = true )
		:Basic<ELEMTYPE>( myModelBase, staticInfo )
	{
	}

	/// Constructor: Valid MP (requires initial value)
	BasicReal( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, ELEMTYPE initialValue, bool decimalFormat = true )
		:Basic<ELEMTYPE>( myModelBase, staticInfo, initialValue )
	{
	}


public:
    /// Atomic increment
    virtual uint16_t increment( ELEMTYPE incSize = 1, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = Basic<ELEMTYPE>::write( Basic<ELEMTYPE>::m_data + incSize, lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }

    /// Atomic decrement
    virtual uint16_t decrement( ELEMTYPE decSize = 1, Cpl::Dm::ModelPoint::LockRequest_T lockRequest = Cpl::Dm::ModelPoint::eNO_REQUEST ) noexcept
    {
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
        uint16_t result = Basic<ELEMTYPE>::write( Basic<ELEMTYPE>::m_data - decSize, lockRequest );
        Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();
        return result;
    }


public:
	/// See Cpl::Dm::Point.  
	bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose=true ) noexcept
	{
		// Get a snapshot of the my data and state
		Cpl::Dm::ModelPointCommon_::m_modelDatabase.lock_();
		ELEMTYPE value  = Basic<ELEMTYPE>::m_data;
		uint16_t seqnum = Basic<ELEMTYPE>::m_seqNum;
		int8_t   valid  = Basic<ELEMTYPE>::m_validState;
		bool     locked = Basic<ELEMTYPE>::m_locked;
		Cpl::Dm::ModelPointCommon_::m_modelDatabase.unlock_();

		// Start the conversion
		JsonDocument& doc = Cpl::Dm::ModelPointCommon_::beginJSON( valid, locked, seqnum, verbose );

		// Construct the 'val' key/value pair (as a simple numeric)
		if( Cpl::Dm::ModelPointCommon_::IS_VALID( valid ) )
		{
			doc["val"] = value;
		}

		// End the conversion
		Cpl::Dm::ModelPointCommon_::endJSON( dst, dstSize, truncated, verbose );
		return true;
	}

	/// See Cpl::Dm::Point.  
	bool fromJSON_( JsonVariant& src, Cpl::Dm::ModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
	{
		ELEMTYPE checkForError = src | (ELEMTYPE) 2;
		ELEMTYPE newValue      = src | (ELEMTYPE) 1;
		if( newValue <= (ELEMTYPE) 1 && checkForError >= (ELEMTYPE) 2 )
		{
			if( errorMsg )
			{
				*errorMsg = "Invalid syntax for the 'val' key/value pair";
			}
			return false;
		}

		retSequenceNumber = Basic<ELEMTYPE>::write( newValue, lockRequest );
		return true;
	}
};

};      // end namespaces
};
};
#endif  // end header latch
