#ifndef Cpl_Dm_Mp_Enum_h_
#define Cpl_Dm_Mp_Enum_h_
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

///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This class provides a MOSTLY concrete implementation for a Point who's data
	is a an a "Better Enum", a.k.a a "enum" defines using the BETTER_ENUM macro
	defined in Cpl/Type/enum.h.  A child class is still needed to provide the
	following methods:

		getTypeAsText()
		attach()
		detach()

	The toJSON()/fromJSON format is:
	\code

	{ name:"<mpname>", type:"<mptypestring>", invalid:nn, seqnum:nnnn, locked:true|false, val:"<enumsymbol>" }

	\endcode

	NOTE: All methods in this class ARE thread Safe unless explicitly
  		  documented otherwise.
 */
template<class BETTERENUM_TYPE>
class Enum : public Cpl::Dm::ModelPointCommon_
{
protected:
	/// The element's value
	BETTERENUM_TYPE    m_data;

public:
	/// Constructor. 
	Enum( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo )
		:ModelPointCommon_( myModelBase, &m_data, staticInfo, OPTION_CPL_DM_MODEL_POINT_STATE_INVALID )
		, m_data( BETTERENUM_TYPE::_values()[0] )
	{
	}

	/// Constructor. 
	Enum( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, BETTERENUM_TYPE initialValue )
		:ModelPointCommon_( myModelBase, &m_data, staticInfo, Cpl::Dm::ModelPoint::MODEL_POINT_STATE_VALID )
		, m_data( initialValue )
	{
	}


public:
	/// Type safe read. See Cpl::Dm::ModelPoint
	virtual int8_t read( BETTERENUM_TYPE& dstData, uint16_t* seqNumPtr=0 ) const noexcept
	{
		return ModelPointCommon_::read( &dstData, sizeof( int ), seqNumPtr );
	}

	/// Type safe write. See Cpl::Dm::ModelPoint
	virtual uint16_t write( BETTERENUM_TYPE newValue, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
	{
		return ModelPointCommon_::write( &newValue, sizeof( int ), lockRequest );
	}

	/// Type safe read-modify-write client callback interface
	typedef Cpl::Dm::ModelPointRmwCallback<BETTERENUM_TYPE> Client;

	/** Type safe read-modify-write. See Cpl::Dm::ModelPoint

	   NOTE: THE USE OF THIS METHOD IS STRONGLY DISCOURAGED because it has
			 potential to lockout access to the ENTIRE Model Base for an
			 indeterminate amount of time.  And alternative is to have the
			 concrete Model Point leaf classes provide the application
			 specific read, write, read-modify-write methods in addition or in
			 lieu of the read/write methods in this interface.
	 */
	virtual uint16_t readModifyWrite( Client& callbackClient, LockRequest_T lockRequest = eNO_REQUEST )
	{
		return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
	}



public:
	/// See Cpl::Dm::ModelPoint.  This method IS thread safe.
	size_t getSize() const noexcept
	{
		return sizeof( BETTERENUM_TYPE );
	}

public:
	/// See Cpl::Dm::ModelPoint
	void copyDataTo_( void* dstData, size_t dstSize ) const noexcept
	{
		CPL_SYSTEM_ASSERT( dstSize == sizeof( BETTERENUM_TYPE ) );
		*( (BETTERENUM_TYPE*) dstData ) = m_data;
	}

	/// See Cpl::Dm::ModelPoint
	void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
	{
		CPL_SYSTEM_ASSERT( srcSize == sizeof( BETTERENUM_TYPE ) );
		m_data = *( (BETTERENUM_TYPE*) srcData );
	}

	/// See Cpl::Dm::ModelPoint.  The default implementation is for integers
	bool isDataEqual_( const void* otherData ) const noexcept
	{
		BETTERENUM_TYPE left = *( (BETTERENUM_TYPE*) otherData );
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
		return sizeof( BETTERENUM_TYPE );
	}


public:
	/// See Cpl::Dm::Point.  
	bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose=true ) noexcept
	{
		// Get a snapshot of the my data and state
		m_modelDatabase.lock_();
		BETTERENUM_TYPE value  = m_data;
		uint16_t        seqnum = m_seqNum;
		int8_t          valid  = m_validState;
		bool            locked = m_locked;
		m_modelDatabase.unlock_();

		// Start the conversion
		JsonDocument& doc = beginJSON( valid, locked, seqnum, verbose );

		// Construct the 'val' key/value pair 
		if( IS_VALID( valid ) )
		{
			doc["val"] = (char*) (value._to_string());
		}

		// End the conversion
		endJSON( dst, dstSize, truncated, verbose );
		return true;
	}

public:
	/// See Cpl::Dm::Point.  
	bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
	{
		// Get the enum string
		const char* newValue = src;
		if( newValue == nullptr )
		{
			if( errorMsg )
			{
				*errorMsg = "Invalid syntax for the 'val' key/value pair";
			}
			return false;
		}

		// Convert the text to an enum value
		auto maybeValue = BETTERENUM_TYPE::_from_string_nothrow( newValue );
		if( !maybeValue )
		{
			if( errorMsg )
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
