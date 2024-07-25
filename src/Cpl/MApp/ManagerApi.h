#ifndef Cpl_MApp_ManagerApi_h_
#define Cpl_MApp_ManagerApi_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/MApp/MAppApi.h"
#include "Cpl/Text/String.h"

///
namespace Cpl {
///
namespace MApp {


/** This abstract class defines the "manager" interface to for starting,
    stopping, etc. individual MApp instances.

    NOTE: ALL of the following method can ONLY be called from a different thread
          than where the MApp instances execute!  This is because synchronous
          ITC is (assumed to be) used for the implementation.
 */
class ManagerApi 
{
public:
    /** This method is used to the specified MApp.  If a MApp is currently executing,
        that MApp will be stopped before the new MApp is started. If the specified 
        MApp name is not found false is returned; else true is returned. 

        NOTE: If the client has no 'optionalArgs' to pass to the MApp instance, 
              it must provide an empty/blank null terminated string, i.e can 
              NOT pass a nullptr.
     */
    virtual bool startMApp( const char* mappName, char* optionalArgs ) noexcept = 0;

    /** This method is used to stop the specified running MApp.  If the MApp
        is currently not in the started state, then the method does nothing
        and false is returned; else true is returned.
     */
    virtual bool stopMApp( const char* mappName ) noexcept = 0;

    /** This method is used to stop ALL currently started MApp instances.
     */
    virtual void stopAllMApps() noexcept = 0;

public:
    /** This method returns a list of started MApps.  If the number of started
        MApps exceeded the provided buffer space false is returned; else
        return  true is returned.  The number of instances found is returned via
        'numElemsFound'.

        NOTE: There is no order to the returned instances.
     */
    virtual bool getAvailableMApps( Cpl::MApp::MAppApi* dstList[], size_t dstMaxElements, size_t& numElemsFound ) noexcept = 0;


    /** This method returns a list of started MApps.  If the number of started
        MApps exceeded the provided buffer space false is returned; else
        return true is returned. The number of instances found is returned via
        'numElemsFound'.

        NOTE: There is no order to the returned instances.
     */
    virtual bool getStartedMApps( Cpl::MApp::MAppApi* dstList[], size_t dstMaxElements, size_t& numElemsFound ) noexcept = 0;

    /** This method returns a pointer to the named MApp instance.  If no such
        MApp exists, a null pointer is returned;
     */
    virtual Cpl::MApp::MAppApi* lookUpMApp( const char* mappName ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~ManagerApi() {}
};

};      // end namespaces
};
#endif  // end header latch
