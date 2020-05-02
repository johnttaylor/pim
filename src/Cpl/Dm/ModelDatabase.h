#ifndef Cpl_Dm_ModelDatabase_h_
#define Cpl_Dm_ModelDatabase_h_
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

#include "colony_config.h"
#include "Cpl/Dm/ModelDatabaseApi.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Json/Arduino.h"


/** This symbol defines the size, in bytes, of a single/global JSON document
    buffer that is used for the toJSON() and fromJSON() operations. Only one
    instance of this buffer is allocated.
*/
#ifndef OPTION_CPL_DM_MODEL_DATABASE_MAX_CAPACITY_JSON_DOC
#define OPTION_CPL_DM_MODEL_DATABASE_MAX_CAPACITY_JSON_DOC          (1024*2)
#endif

/** This symbol defines the size, in bytes, of temporary storage allocated for
    use by the fromJSON_() method (e.g. create a temporary array instance)
 */
#ifndef OPTION_CPL_DM_MODEL_DATABASE_TEMP_STORAGE_SIZE
#define OPTION_CPL_DM_MODEL_DATABASE_TEMP_STORAGE_SIZE              (1024*2)
#endif


///
namespace Cpl {
///
namespace Dm {


/** This concrete class implements a simple Model Database.
 
    Note: All of the methods are thread safe.  However, since the Model Database
          instances are typically created statically and statically allocated
          Model point self-register there is race condition on when the
          Model Database instance's Mutex is created/initialize.  As a result
          mutex is dynamically created/initialized on its first usage.
  */
class ModelDatabase : public ModelDatabaseApi
{
public:
    /// Constructor
    ModelDatabase() noexcept;

    /** This is a special constructor for when the Model Database is
        statically declared (i.e. it is initialized as part of
        C++ startup BEFORE main() is executed.  C/C++ does NOT guarantee
        the order of static initializers.  Since the Model Database contains
        a list of model points - and one or more of those model points can
        be statically created/initialized - we need to do something to ensure
        my internal list is properly initialized for this scenario - hence this
        constructor.
     */
    ModelDatabase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept;

    /// Destructor
    ~ModelDatabase() noexcept;

public:
    /// See Cpl::Dm::ModelDatabaseApi
    ModelPoint * lookupModelPoint( const char* modelPointName ) noexcept;

    /// See Cpl::Dm::ModelDatabaseApi
    ModelPoint* getFirstByName() noexcept;

    /// See Cpl::Dm::ModelDatabaseApi
    ModelPoint* getNextByName( ModelPoint& currentModelPoint ) noexcept;

    /// See Cpl::Dm::ModelDatabaseApi
    bool fromJSON( const char* src, Cpl::Text::String* errorMsg=0, ModelPoint** retMp = 0, uint16_t* retSequenceNumber=0 ) noexcept;

public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
       other classes in the Cpl::Dm namespace.  It is ONLY public to avoid
       the tight coupling of C++ friend mechanism.

       This method inserts a new Model Point into the Model Database.
   */
    void insert_( ModelPoint& mpToAdd ) noexcept;

    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Cpl::Dm namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method locks the Model Database.  For every call to lock() there must
        be corresponding call to unlock();
    */
    void lock_() noexcept;

    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Cpl::Dm namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method unlocks the Model Database.
    */
    void unlock_() noexcept;



public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Cpl::Dm namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method provides a single global lock for ALL Model Database
        instances. The method is used to protect global Model Database (e.g.
        the global parse buffer).

        This method locks the global Model Database lock. For every call to
        globalLock_() there must be corresponding call to globalUnlock_();
    */
    static void globalLock_() noexcept;

    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Cpl::Dm namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method unlocks the global Model Database lock
    */
    static void globalUnlock_() noexcept;

    /** This variable has 'PACKAGE Scope' in that is should only be called by
        other classes in the Cpl::Dm namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        Global/single instance of a JSON document. Model Point's need to have
        acquired the global lock before using this buffer
     */
    static StaticJsonDocument<OPTION_CPL_DM_MODEL_DATABASE_MAX_CAPACITY_JSON_DOC> g_doc_;

    /** This variable has 'PACKAGE Scope' in that is should only be called by
        other classes in the Cpl::Dm namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        Global temporary buffer. Model Point's need to have acquired the global 
        lock before using this buffer
     */
    static uint8_t   g_tempBuffer_[OPTION_CPL_DM_MODEL_DATABASE_TEMP_STORAGE_SIZE];


protected:
    /// Helper method to create the database lock
    bool createLock();

protected:
    /// Map to the store the Model Points
    Cpl::Container::Map<ModelPoint> m_map;

    /// Mutex for making the Database thread safe
    Cpl::System::Mutex*  m_lock;


private:
    /// Prevent access to the copy constructor -->Model Databases can not be copied!
    ModelDatabase( const ModelDatabase& m );

    /// Prevent access to the assignment operator -->Model Databases can not be copied!
    const ModelDatabase& operator=( const ModelDatabase& m );
};


};      // end namespaces
};
#endif  // end header latch
