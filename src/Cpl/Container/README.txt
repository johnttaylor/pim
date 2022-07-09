/** @namespace Cpl::Container

The Container namespace provides classes for various types of containers.
All of the containers use 'intrusive listing' mechanism, i.e. every item
that is put into a container contains the memory/fields that are necessary
to be in the container.  No memory is allocated when an 'item' in inserted
into a container and all of the containers can contain an infinite number
(RAM permitting) number of items.  There are two major side effects of 
intrusive containers.

    1) All items/classes that are to put into containers, must inherit
       from the base class ::Item.

    2) A given item/instance can be in AT MOST one and ONLY one container.


*/  


  