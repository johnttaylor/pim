#ifndef Cpl_MApp_MApp_h_
#define Cpl_MApp_MApp_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/MApp/MAppApi.h"
#include "Cpl/Container/Map.h"

/*----------------------------------------------------------------------------*/
///
namespace Cpl {
///
namespace MApp {


/** This partial concrete class provide common infrastructure that is common
    to all MApp instances.
 */
class MApp_ : public MAppApi
{
protected:
    /// Constructor.  
    MApp_( Cpl::Container::Map<MAppApi>&  mappList,
           const char*                    mappName,
           const char*                    description,
           const char*                    usage );

public:
    /// See Cpl::MApp::Api
    const char* getName() const noexcept;

    /// See Cpl::MApp::Api
    const char* getDescription() const noexcept;

    /// See Cpl::MApp::Api
    const char* getUsage() const noexcept;

protected:
    /// See Cpl::Container::Key
    const Cpl::Container::Key& getKey() const noexcept;

protected:
    /// Command name
    Cpl::Container::KeyLiteralString    m_name;

    /// Description
    const char* m_description;

    /// Usage
    const char* m_usage;

    /// Started state
    bool        m_started;
};

};      // end namespaces
};
#endif  // end header latch
