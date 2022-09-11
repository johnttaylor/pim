#ifndef Cpl_Container_Stack_h_
#define Cpl_Container_Stack_h_
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

#include <stdint.h>


/// 
namespace  Cpl {
/// 
namespace Container {


/** This template class implements a Stack that has a fixed depth and stores
    copies of the data items.

    Template Args:
        ITEM:=      Type of the data stored in the Stack
 */
template <class ITEM>
class Stack
{
private:
    /// Current number of items in the stack
    unsigned m_count;

    /// Maximum number of items in that the stack can hold
    unsigned m_maxItems;

    /// Memory for the Elements
    ITEM*    m_elements;



public:
    /** Constructor.  The application is responsible for providing the memory
        for the Stack.  The argument ''maxElements' is the number of items that
        will fit in the memory allocated by 'memoryForElements' - it is NOT
        the number of bytes of 'memoryForElements'.
     */
    Stack( unsigned maxElements, ITEM memoryForElements[] ) noexcept;


public:
    /** Adds an item to the top of the stack.  Returns true if successful;
        else false is returned (e.g. on stack overflow).
     */
    bool push( ITEM dst ) noexcept;


    /** Removes the top item of the stack.  If the stack is empty, then
        a value of zero is returned. The optional argument 'statusPtr'
        is used to return stats/result of the pop operation, i.e. when 
        *statusPtr == false then the pop operation failed and the returned 
        value is invalid.
     */
    ITEM pop( bool* statusPtr=0 ) noexcept;


    /** Returns the item on the top of the Stack. The returned item
        remains on the stack. If the stack is empty, then a value of zero
        is returned. The optional argument 'statusPtr'is used to return
        stats/result of the pop operation, i.e. when *statusPtr == false then 
        the peek operation failed and the returned value is invalid.
     */
    ITEM peekTop( bool* statusPtr=0 ) const noexcept;


public:
    /** This method returns true if the Stack is empty
     */
    bool isEmpty( void ) const noexcept;

    /** This method returns true if the Stack is full
     */
    bool isFull( void ) const noexcept;


    /** This method returns the current number of items in
        the Stack
     */
    unsigned getNumItems( void ) const noexcept;


    /** This method returns the maximum number of items that
        can be stored in the Stack.
     */
    unsigned getMaxItems( void ) const noexcept;

public:
    /** Empties the Stack.  All references to the item(s) in the stack are
        lost.
     */
    void clearTheStack() noexcept;



private:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    Stack( const Stack& m );

    /// Prevent access to the assignment operator -->Containers can not be copied!
    const Stack& operator=( const Stack& m );

};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////


template <class ITEM>
Stack<ITEM>::Stack( unsigned maxElements, ITEM memoryForElements[] ) noexcept
    :m_count( 0 ), m_maxItems( maxElements ), m_elements( memoryForElements )
{
}


template <class ITEM>
inline void Stack<ITEM>::clearTheStack() noexcept
{
    m_count = 0;
}


template <class ITEM>
inline bool Stack<ITEM>::push( ITEM item ) noexcept
{
    if ( isFull() )
    {
        return false;
    }

    m_elements[m_count++] = item;
    return true;
}


template <class ITEM>
inline ITEM Stack<ITEM>::pop( bool* statusPtr ) noexcept
{
    if ( isEmpty() )
    {
        if ( statusPtr )
        {
            *statusPtr = false;
        }

        return (ITEM) 0;
    }


    if ( statusPtr )
    {
        *statusPtr = true;
    }

    return m_elements[--m_count];
}


template <class ITEM>
inline ITEM Stack<ITEM>::peekTop( bool* statusPtr ) const noexcept
{
    if ( isEmpty() )
    {
        if ( statusPtr )
        {
            *statusPtr = false;
        }

        return (ITEM) 0;
    }


    if ( statusPtr )
    {
        *statusPtr = true;
    }

    return m_elements[m_count - 1];
}


template <class ITEM>
inline bool Stack<ITEM>::isEmpty( void ) const noexcept
{
    return m_count == 0;
}

template <class ITEM>
inline bool Stack<ITEM>::isFull( void ) const noexcept
{
    return m_count == m_maxItems;
}

template <class ITEM>
inline unsigned Stack<ITEM>::getNumItems( void ) const noexcept
{
    return m_count;
}

template <class ITEM>
inline unsigned Stack<ITEM>::getMaxItems( void ) const noexcept
{
    return m_maxItems;
}



};      // end namespaces
};
#endif  // end header latch
