#ifndef Cpl_Container_Key_h_
#define Cpl_Container_Key_h_
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

#include "Cpl/System/FatalError.h"
#include <stdint.h>
#include <string.h>
#include "colony_map.h"



/// 
namespace Cpl {
/// 
namespace Container {


/** This abstract class defines the interface that a contained object
    must support if it has comparable key associated with it.
 */
class Key
{
public:
    /// Ensure a Virtual destructor
    virtual ~Key() {}

public:
    /** Key Compare function.  Returns <0, 0, >0 if this key is less than,
        equal, or greater than respectively to the specified key!
        The actual type of the 'key' is up to the client sub-class
        that implements this interface.  It is the responsibility of
        the sub-class to correctly define/interpret the data type of the key.

     */
    virtual int compareKey( const Key& key ) const = 0;


    /** Returns the object's length (in bytes) and point to the start of
        key data.  If 'returnRawKeyLenPtr' is null, then no length is
        returned.
     */
    virtual const void* getRawKey( unsigned* returnRawKeyLenPtr = 0 ) const = 0;
};


/////////////////////////////////////////////////////////////////////////////

/** This template class is used to generate Key classes for most of the C/C++
    primitive data types.  This class is NOT intended for general use - it is
    used create the typedefs that follow.
 */
template<class DATATYPE>
class KeyPlainType : public Key
{
protected:
    /// Storage for the key
    DATATYPE m_keyData;

public:
    /// Constructor
    KeyPlainType( DATATYPE initialValue = 0 );


public:
    /// Updates the Key's content value
    void setValue( DATATYPE newValue ) noexcept;

    /// Returns the Key's content value
    DATATYPE getKeyValue( void ) const noexcept;


public: // Cpl::Container::Key
    ///
    int compareKey( const Key& key ) const;
    ///
    const void* getRawKey( unsigned* returnRawKeyLenPtr = 0 ) const;
};


/////////////////////////////////////////////////////////////////////////////
// Pre-defined types to some of the primitive C/C++ data types

/// Pre-defined key
typedef KeyPlainType<int>                 KeyInteger_T;

/// Pre-defined key
typedef KeyPlainType<unsigned>            KeyUnsigned_T;

/// Pre-defined key
typedef KeyPlainType<long>                KeyLong_T;

/// Pre-defined key
typedef KeyPlainType<unsigned long>       KeyULong_T;

/// Pre-defined key
typedef KeyPlainType<size_t>              KeySizet_T;

/// Pre-defined key
typedef KeyPlainType<int8_t>              KeyInteger8_T;

/// Pre-defined key
typedef KeyPlainType<uint8_t>             KeyUinteger8_T;

/// Pre-defined key
typedef KeyPlainType<int16_t>             KeyInteger16_T;

/// Pre-defined key
typedef KeyPlainType<uint16_t>            KeyUinteger16_T;

/// Pre-defined key
typedef KeyPlainType<int32_t>             KeyInteger32_T;

/// Pre-defined key
typedef KeyPlainType<uint32_t>            KeyUinteger32_T;

/// Pre-defined key
typedef KeyPlainType<int64_t>             KeyInteger64_T;

/// Pre-defined key
typedef KeyPlainType<uint64_t>            KeyUinteger64_T;


/** This class provides a 'Key' wrapper for a array of Character of length N,
    i.e. a string that is NOT null terminated.  Keys of this type can used to
    compare against other KeyStringBuffer, KeyLiteralString, or Items that use
    a Cpl::Text::String as their key
 */
class KeyStringBuffer : public Key
{
public:
    /// Storage for the key
    const char* m_stringKeyPtr;

    /// Number of bytes in the buffer
    size_t      m_len;

public:
    /// Constructor
    KeyStringBuffer( const char* startOfString, size_t lenOfStringInBytes );


public:
    /** Returns the Key's content value. Note: The returned values is NOT
        a null terminated string!
     */
    inline const char* getKeyValue( size_t& lenOfStringInBytes ) const noexcept { lenOfStringInBytes = m_len; return m_stringKeyPtr; }

public:
    /** Generic compare function for strings and string buffers
     */
    static int compare( const char* myString, unsigned myLen, const char* otherString, unsigned otherLen );


public: // Cpl::Container::Key
    ///
    int compareKey( const Key& key ) const;
    ///
    const void* getRawKey( unsigned* returnRawKeyLenPtr = 0 ) const;
};


/** This class provides a 'Key' wrapper for a C string literal.  Keys
    of this type can used to compare against other KeyLiteralString,
    KeyStringBuffer, or Items that use a Cpl::Text::String as their key.
 */
class KeyLiteralString : public KeyStringBuffer
{
public:
    /// Constructor
    KeyLiteralString( const char* string ):KeyStringBuffer( string, string ? strlen( string ) : 0 ) {}


public:
    /// Returns the Key's content value
    inline const char* getKeyValue( void ) const noexcept { return m_stringKeyPtr; }

    /// Cast to read-only character string pointer.
    inline operator const char* () const { return m_stringKeyPtr; }

    /// Returns a Read-only pointer to the "raw" (short-hand for getKeyValue())
    inline const char* operator()() const { return m_stringKeyPtr; }

};




/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template<class DATATYPE>
Cpl::Container::KeyPlainType<DATATYPE>::KeyPlainType( DATATYPE initialValue )
    :m_keyData( initialValue )
{
}

/////////////////
template<class DATATYPE>
void Cpl::Container::KeyPlainType<DATATYPE>::setValue( DATATYPE newValue ) noexcept
{
    m_keyData = newValue;
}

template<class DATATYPE>
DATATYPE Cpl::Container::KeyPlainType<DATATYPE>::getKeyValue( void ) const noexcept
{
    return m_keyData;
}

/////////////////
template<class DATATYPE>
int Cpl::Container::KeyPlainType<DATATYPE>::compareKey( const Key& key ) const
{
    unsigned  len = 0;
    DATATYPE* ptr = (DATATYPE*) key.getRawKey( &len );
    if ( len != sizeof( DATATYPE ) )
    {
        Cpl::System::FatalError::logf( "KeyPlainType::compare(): mismatch in key lengths. this=%p, my len=%u != %u", this, sizeof( DATATYPE ), len );
    }

    if ( m_keyData < *ptr )
    {
        return -1;
    }
    else if ( m_keyData > *ptr )
    {
        return 1;
    }

    return 0;
}

template<class DATATYPE>
const void*  Cpl::Container::KeyPlainType<DATATYPE>::getRawKey( unsigned* returnRawKeyLenPtr ) const
{
    if ( returnRawKeyLenPtr != 0 )
    {
        *returnRawKeyLenPtr = sizeof( DATATYPE );
    }

    return &m_keyData;
}



};      // end namespaces
};
#endif  // end header latch

