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

#include "AvlTree_.h"


// Debugging
//#include        <stdlib.h>
//#include        <stdio.h>
//#define tpd_assert(X)   { if (! (X)) { printf ("%d: %s\n", __LINE__, #X); exit (1); } }
//#define tpd_panic(X)    { printf ("%s\n", X); exit (1); }

//
using namespace Cpl::Container;

//////////////////////////////////
AvlTree_::AvlTree_()
{
    m_root.m_is_NOT_root_ = false;
}

AvlTree_::AvlTree_( const char* ignoreThisParameter_usedToCreateAUniqueConstructor )
    :m_root( ignoreThisParameter_usedToCreateAUniqueConstructor )
{
}

//////////////////////////////////
MapItem* AvlTree_::find( const Key& key ) const
{
    MapItem* nodePtr = m_root.getLeft();
    while ( nodePtr != 0 )
    {
        int8_t comparison = nodePtr->getKey().compareKey( key );
        if ( comparison > 0 )
        {
            nodePtr = nodePtr->getLeft();
        }
        else if ( comparison == 0 )
        {
            return nodePtr;
        }
        else
        {
            nodePtr = nodePtr->getRight();
        }
    }
    return 0;
}

bool AvlTree_::insert( MapItem& newNode )
{
    MapItem* nxtPtr;
    MapItem* curPtr  = m_root.getLeft();
    bool        done    = false;
    bool        taller  = false;
    int         depth   = 0;
    int8_t      comparison;
    int8_t      comparisons[OPTION_CPL_CONTAINER_AVLTREE_MAX_TREE_DEPTH];

    // Empty tree
    if ( curPtr == 0 )
    {
        newNode.initialize( &m_root );
        m_root.setLeft( &newNode );
        newNode.insert_( this );
        return true;
    }

    // Lets find the parent node for the new node
    do
    {
        //      tpd_assert( depth < _maxTreeDepth );
        int  result          = curPtr->getKey().compareKey( newNode.getKey() );
        comparison           = result < 0 ? -1 : result > 0 ? 1 : 0;
        comparisons[depth++] = comparison;

        if ( comparison > 0 )
        {
            if ( curPtr->getLeft() == 0 )
            {
                curPtr->setLeft( &newNode );
                done = true;
            }
            else
            {
                curPtr = curPtr->getLeft();
            }
        }
        else if ( comparison < 0 )
        {
            if ( curPtr->getRight() == 0 )
            {
                curPtr->setRight( &newNode );
                done = true;
            }
            else
            {
                curPtr = curPtr->getRight();
            }
        }
        else
        {
            return false;
        }
    } while ( !done );


    // Insert new node and check the resulting balance
    newNode.initialize( curPtr );
    do
    {
        //      tpd_assert( curPtr->m_is_NOT_root_ );
        nxtPtr = curPtr->getParent();
        depth--;

        if ( comparisons[depth] > 0 )
        {
            switch ( curPtr->m_balance_ )
            {
                case MapItem::eLEFT_:
                    taller = grewLeft( curPtr );
                    break;

                case MapItem::eEVEN_:
                    curPtr->m_balance_ = MapItem::eLEFT_;
                    taller = true;
                    break;

                case MapItem::eRIGHT_:
                    curPtr->m_balance_ = MapItem::eEVEN_;
                    taller = false;
                    break;

                default:
                    //                  tpd_panic ("avl_insert: no such balance 1");
                    break;
            }
        }
        else if ( comparisons[depth] < 0 )
        {
            switch ( curPtr->m_balance_ )
            {
                case MapItem::eLEFT_:
                    curPtr->m_balance_ = MapItem::eEVEN_;
                    taller = false;
                    break;

                case MapItem::eEVEN_:
                    curPtr->m_balance_ = MapItem::eRIGHT_;
                    taller = true;
                    break;

                case MapItem::eRIGHT_:
                    taller = grewRight( curPtr );
                    break;

                default:
                    //                  tpd_panic ("avl_insert: no such balance 1");
                    break;
            }
        }

        curPtr = nxtPtr;
    } while ( depth > 0 && taller );

    newNode.insert_( this );
    return true;
}

MapItem* AvlTree_::removeItem( MapItem& node )
{
    // Trap: Node is not in this tree
    if ( node.isInContainer_( this ) == false )
    {
        return 0;
    }

    if ( node.getLeft() != 0 && node.getRight() != 0 )
    {
        exchangeWithPrev( &node );
    }

    MapItem*  parentPtr = node.getParent();
    MapItem*  childPtr  = 0;
    if ( node.getLeft() != 0 )
    {
        childPtr = node.getLeft();
    }
    else if ( node.getRight() != 0 )
    {
        childPtr = node.getRight();
    }
    else
    {
        childPtr = 0;
    }

    bool is_left = &node == parentPtr->getLeft();
    if ( is_left )
    {
        parentPtr->setLeft( childPtr );
    }
    else
    {
        //      tpd_assert( parentPtr->getRight() == node );
        parentPtr->setRight( childPtr );
    }

    if ( childPtr != 0 )
    {
        childPtr->setParent( parentPtr );
    }

    bool shorter = true;
    while ( shorter && parentPtr->m_is_NOT_root_ )
    {
        MapItem* nxtPtr       = parentPtr->getParent();
        bool        next_is_left = parentPtr == nxtPtr->getLeft();
        if ( is_left )
        {
            switch ( parentPtr->m_balance_ )
            {
                case MapItem::eLEFT_:
                    parentPtr->m_balance_ = MapItem::eEVEN_;
                    shorter = true;
                    break;

                case MapItem::eEVEN_:
                    parentPtr->m_balance_ = MapItem::eRIGHT_;
                    shorter = false;
                    break;

                case MapItem::eRIGHT_:
                    shorter = leftShorter( parentPtr );
                    break;

                default:
                    //                  tpd_panic ("avl_delete:  no such balance");
                    break;
            }
        }
        else
        {
            switch ( parentPtr->m_balance_ )
            {
                case MapItem::eLEFT_:
                    shorter = rightShorter( parentPtr );
                    break;

                case MapItem::eEVEN_:
                    parentPtr->m_balance_ = MapItem::eLEFT_;
                    shorter = false;
                    break;

                case MapItem::eRIGHT_:
                    parentPtr->m_balance_ = MapItem::eEVEN_;
                    shorter = true;
                    break;

                default:
                    //                  tpd_panic ("avl_delete:  no such balance");
                    break;
            }
        }

        parentPtr = nxtPtr;
        is_left   = next_is_left;
    }


    // Remove all references in the deleted node to the tree
    node.initialize( 0 );
    Item::remove_( &node );
    return &node;
}

MapItem* AvlTree_::first() const
{
    MapItem* nodePtr = m_root.getLeft();
    if ( nodePtr == 0 )
    {
        return 0;   // Trap empty tree
    }

    while ( nodePtr->getLeft() != 0 )
    {
        nodePtr = nodePtr->getLeft();
    }

    return nodePtr;
}

MapItem* AvlTree_::last() const
{
    MapItem* nodePtr = m_root.getLeft();
    if ( nodePtr == 0 )
    {
        return 0;   // Trap empty tree
    }

    while ( nodePtr->getRight() != 0 )
    {
        nodePtr = nodePtr->getRight();
    }

    return nodePtr;
}


MapItem* AvlTree_::next( MapItem& node )
{
    MapItem* nxtPtr = 0;
    if ( node.getRight() )
    {
        nxtPtr = node.getRight();

        while ( nxtPtr->getLeft() != 0 )
        {
            nxtPtr = nxtPtr->getLeft();
        }
    }
    else if ( node.getParent()->getLeft() == &node )
    {
        if ( node.getParent()->m_is_NOT_root_ )
        {
            nxtPtr = node.getParent();
        }
    }
    else if ( node.getParent()->getRight() == &node )
    {
        MapItem* parentPtr = node.getParent();
        while ( parentPtr->m_is_NOT_root_ && parentPtr->getParent()->getRight() == parentPtr )
        {
            parentPtr = parentPtr->getParent();
        }

        if ( parentPtr->getParent()->m_is_NOT_root_ )
        {
            nxtPtr = parentPtr->getParent();
        }
    }

    return nxtPtr;
}

MapItem* AvlTree_::previous( MapItem& node )
{
    MapItem* prevPtr = 0;
    if ( node.getLeft() )
    {
        prevPtr = node.getLeft();
        while ( prevPtr->getRight() != 0 )
        {
            prevPtr = prevPtr->getRight();
        }
    }
    else if ( node.getParent()->getRight() == &node )
    {
        prevPtr = node.getParent();
    }
    else if ( node.getParent()->getLeft() == &node )
    {
        MapItem* parentPtr = node.getParent();
        while ( parentPtr->m_is_NOT_root_ && parentPtr->getParent()->getLeft() == parentPtr )
        {
            parentPtr = parentPtr->getParent();
        }

        if ( parentPtr->m_is_NOT_root_ && parentPtr->getParent()->m_is_NOT_root_ )
        {
            prevPtr = parentPtr->getParent();
        }
    }

    return prevPtr;
}


//////////////////////////////////
bool AvlTree_::rightShorter( MapItem* parentPtr )
{
    MapItem* leftPtr;
    MapItem* grandChildPtr;
    bool        shorter = true;

    leftPtr = parentPtr->getLeft();
    //  tpd_assert (leftPtr != 0);

    switch ( leftPtr->m_balance_ )
    {
        case MapItem::eLEFT_:
            rotateRight( parentPtr );
            parentPtr->m_balance_ = MapItem::eEVEN_;
            leftPtr->m_balance_   = MapItem::eEVEN_;
            shorter = true;
            break;

        case MapItem::eEVEN_:
            rotateRight( parentPtr );
            parentPtr->m_balance_ = MapItem::eLEFT_;
            leftPtr->m_balance_   = MapItem::eRIGHT_;
            shorter = false;
            break;

        case MapItem::eRIGHT_:
            grandChildPtr = leftPtr->getRight();
            switch ( grandChildPtr->m_balance_ )
            {
                case MapItem::eLEFT_:
                    parentPtr->m_balance_ = MapItem::eRIGHT_;
                    leftPtr->m_balance_   = MapItem::eEVEN_;
                    break;

                case MapItem::eEVEN_:
                    parentPtr->m_balance_ = MapItem::eEVEN_;
                    leftPtr->m_balance_   = MapItem::eEVEN_;
                    break;

                case MapItem::eRIGHT_:
                    parentPtr->m_balance_ = MapItem::eEVEN_;
                    leftPtr->m_balance_   = MapItem::eLEFT_;
                    break;

                default:
                    //                  tpd_panic ("rightShorter:  no such balance");
                    break;
            }

            grandChildPtr->m_balance_ = MapItem::eEVEN_;
            rotateLeft( leftPtr );
            rotateRight( parentPtr );
            shorter = true;
            break;

        default:
            //          tpd_panic ("rightShorter:  no such balance");
            break;
    }

    return shorter;
}

bool AvlTree_::leftShorter( MapItem* parentPtr )
{
    MapItem* right;
    MapItem* grandChildPtr;
    bool        shorter=true;

    right = parentPtr->getRight();
    //  tpd_assert (right != 0);

    switch ( right->m_balance_ )
    {
        case MapItem::eRIGHT_:
            rotateLeft( parentPtr );
            parentPtr->m_balance_ = MapItem::eEVEN_;
            right->m_balance_     = MapItem::eEVEN_;
            shorter = true;
            break;

        case MapItem::eEVEN_:
            rotateLeft( parentPtr );
            parentPtr->m_balance_ = MapItem::eRIGHT_;
            right->m_balance_     = MapItem::eLEFT_;
            shorter = false;
            break;

        case MapItem::eLEFT_:
            grandChildPtr = right->getLeft();
            switch ( grandChildPtr->m_balance_ )
            {
                case MapItem::eRIGHT_:
                    parentPtr->m_balance_ = MapItem::eLEFT_;
                    right->m_balance_     = MapItem::eEVEN_;
                    break;

                case MapItem::eEVEN_:
                    parentPtr->m_balance_ = MapItem::eEVEN_;
                    right->m_balance_     = MapItem::eEVEN_;
                    break;

                case MapItem::eLEFT_:
                    parentPtr->m_balance_ = MapItem::eEVEN_;
                    right->m_balance_     = MapItem::eRIGHT_;
                    break;

                default:
                    //                  tpd_panic ("leftShorter:  no such balance");
                    break;
            }

            grandChildPtr->m_balance_ = MapItem::eEVEN_;
            rotateRight( right );
            rotateLeft( parentPtr );
            shorter = true;
            break;

        default:
            //          tpd_panic ("leftShorter:  no such balance");
            break;
    }

    return shorter;
}

MapItem* AvlTree_::rotateLeft( MapItem* nodePtr )
{
    MapItem* tempPtr;

    //  tpd_assert (nodePtr->getRight() != 0);

    tempPtr = nodePtr->getRight();
    nodePtr->setRight( tempPtr->getLeft() );
    tempPtr->setLeft( nodePtr );

    if ( nodePtr->getParent()->getLeft() == nodePtr )
    {
        nodePtr->getParent()->setLeft( tempPtr );
    }
    else
    {
        //      tpd_assert (nodePtr->getParent()->getRight() == nodePtr);
        nodePtr->getParent()->setRight( tempPtr );
    }

    if ( nodePtr->getRight() != 0 )
    {
        nodePtr->getRight()->setParent( nodePtr );
    }

    tempPtr->setParent( nodePtr->getParent() );
    nodePtr->setParent( tempPtr );
    return tempPtr;
}

MapItem* AvlTree_::rotateRight( MapItem* nodePtr )
{
    MapItem* tempPtr;

    //  tpd_assert (nodePtr->getLeft() != 0);

    tempPtr = nodePtr->getLeft();
    nodePtr->setLeft( tempPtr->getRight() );
    tempPtr->setRight( nodePtr );

    if ( nodePtr->getParent()->getRight() == nodePtr )
    {
        nodePtr->getParent()->setRight( tempPtr );
    }
    else
    {
        //      tpd_assert (nodePtr->getParent()->getLeft() == nodePtr);
        nodePtr->getParent()->setLeft( tempPtr );
    }

    if ( nodePtr->getLeft() != 0 )
    {
        nodePtr->getLeft()->setParent( nodePtr );
    }

    tempPtr->setParent( nodePtr->getParent() );
    nodePtr->setParent( tempPtr );
    return tempPtr;
}


bool AvlTree_::grewLeft( MapItem* curPtr )
{
    MapItem* childPtr;
    MapItem* grandChildPtr;
    MapItem* newRootPtr;
    bool        taller=true;

    childPtr = curPtr->getLeft();
    switch ( childPtr->m_balance_ )
    {
        case MapItem::eLEFT_:
            curPtr->m_balance_   = MapItem::eEVEN_;
            childPtr->m_balance_ = MapItem::eEVEN_;
            rotateRight( curPtr );
            taller = false;
            break;

        case MapItem::eEVEN_:
            //          tpd_panic ("grewLeft:  what balance?");
            break;

        case MapItem::eRIGHT_:
            grandChildPtr = childPtr->getRight();
            switch ( grandChildPtr->m_balance_ )
            {
                case MapItem::eLEFT_:
                    curPtr->m_balance_   = MapItem::eRIGHT_;
                    childPtr->m_balance_ = MapItem::eEVEN_;
                    break;

                case MapItem::eEVEN_:
                    curPtr->m_balance_   = MapItem::eEVEN_;
                    childPtr->m_balance_ = MapItem::eEVEN_;
                    break;

                case MapItem::eRIGHT_:
                    curPtr->m_balance_   = MapItem::eEVEN_;
                    childPtr->m_balance_ = MapItem::eLEFT_;
                    break;

                default:
                    //                  tpd_panic ("grewLeft:  bad node 2");
                    break;
            }

            grandChildPtr->m_balance_ = MapItem::eEVEN_;
            newRootPtr                = rotateLeft( childPtr );
            curPtr->setLeft( newRootPtr );
            newRootPtr->setParent( curPtr );
            rotateRight( curPtr );
            taller = false;
            break;

        default:
            //          tpd_panic ("grewLeft:  bad node 2");
            break;
    }

    return taller;
}

bool AvlTree_::grewRight( MapItem* curPtr )
{
    MapItem* childPtr;
    MapItem* grandChildPtr;
    MapItem* newRootPtr;
    bool        taller = true;

    childPtr = curPtr->getRight();
    switch ( childPtr->m_balance_ )
    {
        case MapItem::eLEFT_:
            grandChildPtr = childPtr->getLeft();
            switch ( grandChildPtr->m_balance_ )
            {
                case MapItem::eLEFT_:
                    curPtr->m_balance_   = MapItem::eEVEN_;
                    childPtr->m_balance_ = MapItem::eRIGHT_;
                    break;

                case MapItem::eEVEN_:
                    curPtr->m_balance_   = MapItem::eEVEN_;
                    childPtr->m_balance_ = MapItem::eEVEN_;
                    break;

                case MapItem::eRIGHT_:
                    curPtr->m_balance_   = MapItem::eLEFT_;
                    childPtr->m_balance_ = MapItem::eEVEN_;
                    break;

                default:
                    //                  tpd_panic ("grewRight:  bad node 2");
                    break;
            }

            grandChildPtr->m_balance_ = MapItem::eEVEN_;
            newRootPtr                = rotateRight( childPtr );
            curPtr->setRight( newRootPtr );
            newRootPtr->setParent( curPtr );
            rotateLeft( curPtr );
            taller = false;
            break;

        case MapItem::eEVEN_:
            //          tpd_panic ("grewRight:  balance is even?");
            break;

        case MapItem::eRIGHT_:
            curPtr->m_balance_   = MapItem::eEVEN_;
            childPtr->m_balance_ = MapItem::eEVEN_;
            rotateLeft( curPtr );
            taller = false;
            break;

        default:
            //          tpd_panic ("grewRight:  bad node 2");
            break;
    }

    return taller;
}

void AvlTree_::exchangeWithPrev( MapItem* nodePtr )
{
    //  tpd_assert (nodePtr->m_is_NOT_root_);

    MapItem* prevPtr           = nodePtr->getLeft();
    MapItem* leftPtr           = nodePtr->getLeft();
    MapItem* rightPtr          = nodePtr->getRight();
    MapItem* parentPtr         = nodePtr->getParent();
    bool node_is_left             = nodePtr->getParent()->getLeft() == nodePtr;
    bool prev_is_left             = true;
    bool prev_is_child            = true;
    MapItem::Balance_T balance = nodePtr->m_balance_;

    //  tpd_assert( prevPtr != 0 );

    while ( prevPtr->getRight() != 0 )
    {
        prevPtr       = prevPtr->getRight();
        prev_is_child = false;
        prev_is_left  = false;
    }

    nodePtr->setLeft( prevPtr->getLeft() );
    nodePtr->setRight( prevPtr->getRight() );
    nodePtr->m_balance_ = prevPtr->m_balance_;

    if ( !prev_is_child )
    {
        nodePtr->setParent( prevPtr->getParent() );
    }
    else
    {
        nodePtr->setParent( prevPtr );
    }

    if ( prev_is_child )
    {
        prevPtr->setLeft( nodePtr );
    }
    else
    {
        prevPtr->setLeft( leftPtr );
    }

    prevPtr->setRight( rightPtr );
    prevPtr->setParent( parentPtr );
    prevPtr->m_balance_ = balance;

    if ( prev_is_left )
    {
        nodePtr->getParent()->setLeft( nodePtr );
    }
    else
    {
        nodePtr->getParent()->setRight( nodePtr );
    }

    if ( node_is_left )
    {
        prevPtr->getParent()->setLeft( prevPtr );
    }
    else
    {
        prevPtr->getParent()->setRight( prevPtr );
    }

    prevPtr->getLeft()->setParent( prevPtr );

    if ( prevPtr->getRight() )
    {
        prevPtr->getRight()->setParent( prevPtr );
    }

    if ( nodePtr->getLeft() )
    {
        nodePtr->getLeft()->setParent( nodePtr );
    }

    if ( nodePtr->getRight() )
    {
        nodePtr->getRight()->setParent( nodePtr );
    }
}



//////////////////////////////////////////////////////////////////////////////
AvlTree_::AvlRoot::AvlRoot()
{
}

AvlTree_::AvlRoot::AvlRoot( const char* ignoreThisParameter_usedToCreateAUniqueConstructor )
    :MapItem( ignoreThisParameter_usedToCreateAUniqueConstructor )
{
}

const Key& AvlTree_::AvlRoot::getKey() const noexcept
{
    return *this;
}

int AvlTree_::AvlRoot::compareKey( const Key& key ) const
{
    return -1;
}

const void* AvlTree_::AvlRoot::getRawKey( unsigned* returnRawKeyLenPtr ) const
{
    return 0;
}


