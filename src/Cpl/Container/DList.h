#ifndef Cpl_Container_DList_h_
#define Cpl_Container_DList_h_
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


/// 
namespace Cpl {
/// 
namespace Container {


/** This template class implements a Doubly linked list
    which maintains the ordering imposed on it by the application.

    NOTE: ITEM must be a subclass of ExtendedItem.
 */
template <class ITEM>
class DList
{
private:
    /** Points to the first item in the list.
     */
    ITEM * m_headPtr;

    /** Points to the last item in the list.
     */
    ITEM*  m_tailPtr;

public:
    /** Public constructor initializes head and tail pointers.
     */
    DList() noexcept;

    /** This is a special constructor for when the list is
        statically declared (i.e. it is initialized as part of
        C++ startup BEFORE main() is executed.  C/C++ guarantees
        that all statically declared data will be initialized
        to zero by default (see r.8.4 in C++ Programming Language,
        Second Edition).  Since the head & tail pointers are
        initialized to zero - then the C/C++ default is OK.  Why
        do I care about all this?  Because if you attempt to build
        static list(s), then the order of when the list is
        constructed vs. when the static elements are added to the
        list is a problem!  To avoid this problem, a alternate
        constructor that does NOT initialize the head & tail pointers
        is provided.  It assumes that the pointers are already set
        zero because the list is "static". Whew!
     */
    DList( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept;


public: ///@name Operations on the List itself
    ///@{
    /** Moves the content of the this List to the specified List.
     */
    void move( DList<ITEM>& dst );

    /** Empties the list.  All references to the item(s) in the
        list are lost.
     */
    void clearTheList();
    ///@}

public: ///@name View as a FIFO
    ///@{
    /** Removes the first item in the list.  Returns 0 if the list
        is empty.
     */
    ITEM*  get( void ) noexcept;

    /** Adds the item as the last item in the list
     */
    void   put( ITEM& item ) noexcept;

    /** Return a pointer to the first item in the list.
        The returned item remains in the list.  Returns 0
        if the list is empty.
     */
    ITEM*  head( void ) const noexcept;

    /** Return a pointer to the last item in the list.
        The returned item remains in the list.  Returns 0
        if the list is empty.
     */
    ITEM*  tail( void ) const noexcept;
    ///@}


public: ///@name View as a STACK
    ///@{
    /** Removes the top element from stack and return a pointer to
        it as a result. Returns 0, if the stack is empty
     */
    ITEM*  pop( void ) noexcept;

    /** Adds the ITEM item to top of the stack.
     */
    void   push( ITEM& item ) noexcept;

    /** Return a pointer to the top ITEM item in the stack.
        The returned item remains in the queue.  Returns 0
        if the stack is empty.
     */
    ITEM*  top( void ) const noexcept;
    ///@}

public: ///@name View as Ordered List
    ///@{
    /** Removes the first item in the list.  Returns 0 if the list
        is empty.
     */
    ITEM*  getFirst( void ) noexcept;

    /** Removes the last item in the list.  Returns 0 if the list
        is empty.
     */
    ITEM*  getLast( void ) noexcept;

    /** Adds the item as the first item in the list.
     */
    void   putFirst( ITEM& item ) noexcept;

    /** Adds the item as the last item in the list.
     */
    void   putLast( ITEM& item ) noexcept;

    /** Remove specified ITEM element from the list.
        Returns true, if the specified element was found and
        removed from the list, else false.
     */
    bool   remove( ITEM& item ) noexcept;

    /** Insert the "item" ITEM into the list behind the
        "after" ITEM element.  If 'after' is 0, then 'item'
        is added to the head of the list.
     */
    void   insertAfter( ITEM& after, ITEM& item ) noexcept;

    /** Insert the "item" ITEM into the list ahead of the
        "before" ITEM element. If 'before' is 0, then 'item'
        is added to the tail of the list.
     */
    void   insertBefore( ITEM& before, ITEM& item ) noexcept;

    /** Returns true of the specified item/pointer is
        already in the list, else false.
     */
    bool   find( const ITEM& item ) const noexcept;

    /** Return a pointer to the first item in the list.
        The returned item remains in the list.  Returns 0
        if the list is empty.
     */
    ITEM*  first( void ) const noexcept;

    /** Return a pointer to the last item in the list.
        The returned item remains in the list.  Returns 0
        if the list is empty.
     */
    ITEM*  last( void ) const noexcept;

    /** Return a pointer to the item after the item "current".
        Both items remain in the list.  Returns 0 when the
        end-of-list is reached.
     */
    ITEM*  next( ITEM& current ) const noexcept;

    /** Return a pointer to the item before the item "current".
        Both items remain in the list.  Returns 0 when the
        end-of-list is reached.
     */
    ITEM*  previous( ITEM& current ) const noexcept;
    ///@}


private: ///@name Prevent the container from being copied
    ///@{
    /// Prevent access to the copy constructor -->Containers can not be copied!
    DList( const DList& m );

    /// Prevent access to the assignment operator -->Containers can not be copied!
    const DList& operator=( const DList& m );
    ///@}
};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////


template <class ITEM>
DList<ITEM>::DList( void ) noexcept
    :m_headPtr( 0 ), m_tailPtr( 0 )
{
}

template <class ITEM>
DList<ITEM>::DList( const char* ) noexcept
{
}


template <class ITEM>
inline void DList<ITEM>::move( DList<ITEM>& dst )
{
    // clear the destination list
    dst.clearTheList();

    // Copy each item (so the debug info is correct)
    ITEM* nextPtr;
    while ( (nextPtr=get()) )
    {
        dst.put( *nextPtr );
    }
}

template <class ITEM>
inline void DList<ITEM>::clearTheList()
{
    // Drain list so the debug traps work correctly
    while ( get() )
        ;
}

template <class ITEM>
inline ITEM* DList<ITEM>::get( void ) noexcept
{
    ITEM* firstPtr = m_headPtr;
    if ( firstPtr )
    {
        remove( *firstPtr );
    }
    return firstPtr;
}

template <class ITEM>
inline ITEM* DList<ITEM>::getFirst( void ) noexcept
{
    return get();
}

template <class ITEM>
inline ITEM* DList<ITEM>::getLast( void ) noexcept
{
    ITEM* lastPtr = m_tailPtr;
    if ( lastPtr )
    {
        remove( *m_tailPtr );
    }
    return lastPtr;
}

template <class ITEM>
inline void DList<ITEM>::putFirst( ITEM& item ) noexcept
{
    if ( item.insert_( this ) )
    {
        if ( m_headPtr )
        {
            item.m_nextPtr_       = m_headPtr;
            m_headPtr->m_prevPtr_ = &item;
            m_headPtr             = &item;
        }
        else
        {
            m_headPtr       = m_tailPtr = &item;
            item.m_nextPtr_ = 0;
        }
        item.m_prevPtr_ = 0;
    }
}

template <class ITEM>
inline void DList<ITEM>::put( ITEM& item ) noexcept
{
    if ( item.insert_( this ) )
    {
        if ( m_headPtr )
        {
            m_tailPtr->m_nextPtr_ = &item;
            item.m_prevPtr_       = m_tailPtr;
        }
        else
        {
            m_headPtr       = &item;
            item.m_prevPtr_ = 0;
        }
        item.m_nextPtr_ = 0;
        m_tailPtr       = &item;
    }
}

template <class ITEM>
inline void DList<ITEM>::putLast( ITEM& item ) noexcept
{
    put( item );
}

template <class ITEM>
inline bool DList<ITEM>::remove( ITEM& item ) noexcept
{
    if ( item.isInContainer_( this ) )
    {
        ITEM* prvPtr = (ITEM*) item.m_prevPtr_;
        ITEM* nxtPtr = (ITEM*) item.m_nextPtr_;
        if ( prvPtr )
        {
            if ( !(prvPtr->m_nextPtr_=nxtPtr) )
            {
                m_tailPtr = prvPtr;              // Case: Remove tail object
            }
            else
            {
                nxtPtr->m_prevPtr_ = prvPtr;     // Case: Remove intermediate object
            }
        }
        else
        {
            if ( !(m_headPtr=nxtPtr) )
            {
                m_tailPtr = 0;             // Case: Remove last object
            }
            else
            {
                nxtPtr->m_prevPtr_ = 0;    // Case: Remove Head object
            }
        }

        Item::remove_( &item );
        return true;
    }

    return false;
}


template <class ITEM>
inline void DList<ITEM>::insertAfter( ITEM& after, ITEM& item ) noexcept
{
    if ( item.insert_( this ) )
    {
        ITEM* nxtPtr    = (ITEM*) (item.m_nextPtr_ = after.m_nextPtr_);
        item.m_prevPtr_  = &after;
        after.m_nextPtr_ = &item;
        if ( !nxtPtr )
        {
            m_tailPtr = &item;
        }
        else
        {
            nxtPtr->m_prevPtr_ = &item;
        }
    }
}

template <class ITEM>
inline void DList<ITEM>::insertBefore( ITEM& before, ITEM& item ) noexcept
{
    if ( item.insert_( this ) )
    {
        ITEM* prvPtr      = (ITEM*) (item.m_prevPtr_ = before.m_prevPtr_);
        item.m_nextPtr_   = &before;
        before.m_prevPtr_ = &item;
        if ( !prvPtr )
        {
            m_headPtr = &item;
        }
        else
        {
            prvPtr->m_nextPtr_ = &item;
        }
    }
}

template <class ITEM>
inline bool DList<ITEM>::find( const ITEM& item ) const noexcept
{
    return  item.isInContainer_( this );
}

template <class ITEM>
inline ITEM* DList<ITEM>::first( void ) const noexcept
{
    return m_headPtr;
}

template <class ITEM>
inline ITEM* DList<ITEM>::last( void ) const noexcept
{
    return m_tailPtr;
}

template <class ITEM>
inline ITEM* DList<ITEM>::next( ITEM& cur ) const  noexcept
{
    return (ITEM*) cur.m_nextPtr_;
}

template <class ITEM>
inline ITEM* DList<ITEM>::previous( ITEM& cur ) const  noexcept
{
    return (ITEM*) cur.m_prevPtr_;
}

template <class ITEM>
inline void DList<ITEM>::push( ITEM& item ) noexcept
{
    putFirst( item );
}

template <class ITEM>
inline ITEM* DList<ITEM>::pop( void ) noexcept
{
    return get();
}

template <class ITEM>
inline ITEM* DList<ITEM>::top( void ) const noexcept
{
    return first();
}

template <class ITEM>
inline ITEM* DList<ITEM>::head( void ) const noexcept
{
    return first();
}

template <class ITEM>
inline ITEM* DList<ITEM>::tail( void ) const noexcept
{
    return last();
}


};      // end namespaces
};
#endif  // end header latch
