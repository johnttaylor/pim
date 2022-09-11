#ifndef Cpl_Dm_ModelDatabaseApi_h_
#define Cpl_Dm_ModelDatabaseApi_h_
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

#include "Cpl/Text/String.h"

///
namespace Cpl {
///
namespace Dm {

/// Forward reference to a Model point -->used to avoid circular dependencies
class ModelPoint;

/** This class defines the basic operations that can be performed on a Model
    Base.  A Model Database is a collection of instantiated Model Points.

    All methods in this class are thread Safe unless explicitly documented 
    otherwise.
 */
class ModelDatabaseApi
{
public:
    /** This method looks-up the model point instance by name and returns a
        pointer to the instance.  If the model point name cannot be found, the
        method returns 0.

        NOTE: The ModelPoint returned is a generic Model Point in that any
              operations that require the concrete data type of the Model 
              Point ARE NOT available.  It is 'okay' for the caller (IF AND ONLY
              IF the caller 'knows' what the concrete type is) to down cast the 
              returned pointer to a type specific child class instance of the 
              Model Point.

        NOTE: There is no 'add' method.  This is because Model Points self
              register with their assigned Model Database when the Model Points
              are created.
     */
    virtual ModelPoint* lookupModelPoint( const char* modelPointName ) noexcept = 0;


public:
    /** This method returns a pointer to the first Model Point in the Database.
        The model points are traversed in order by model point name.  If there
        are not Model Points in the Database, the method returns 0.
     */
    virtual ModelPoint* getFirstByName() noexcept = 0;

    /** This method returns the next (in sorted order) Model Point in the
        Database. If the current model point is the last Model Point in the
        Database the method returns 0.
     */
    virtual ModelPoint* getNextByName( ModelPoint& currentModelPoint ) noexcept = 0;


public:
    /** This method attempts to convert the null terminated JSON formated 'src' 
        string to its binary format and copies the result to the Model Point's 
        internal data. The expected format of the JSON string is specific to 
        the concrete leaf class.

        The method optional returns - via 'retMp' - a pointer to the Model Point
        identified by 'mpname'.  If the method false return, then 'retMp' has
        no meaning.

        The method optional returns - via 'retSequenceNumber' - the Model Point's
        sequence number after the conversion. If the method false return, then 
        'retSequenceNumber' has no meaning.

        If the conversion is successful true is returned. If the contents of 
        the 'src' is not a valid JSON object and/or not parse-able, OR the Point 
        does not support a full/complete conversion from Text to binary, OR 
        the conversion fails, OR the specified model point name does not exist
        then the method returns false.  When the conversion fails, the optional 
        'errorMsg' argument is updated with a plain text error message.

        
        The general input format: 
        \code

        { name="<mpname>", locked:true|false }              // Locks/unlocks the MP
        { name="<mpname>", invalid=1 }                      // Invalidates the MP
        { name="<mpname>", invalid=22 }                     // Invalidates the MP with a non-default invalid state/value
        { name="<mpname>", invalid=1, locked=true }         // Invalidates the MP and locks the MP
        { name="<mpname>", val:<value> }                    // Writes a new (valid) value to the MP
        { name="<mpname>", val:<value>, locked=true }       // Writes a new (valid) value to the MP and locks the MP

         
        \endcode
     */
    virtual bool fromJSON( const char* src, Cpl::Text::String* errorMsg=0, ModelPoint** retMp = 0, uint16_t* retSequenceNumber=0 ) noexcept = 0;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~ModelDatabaseApi() {}
};


};      // end namespaces
};
#endif  // end header latch
