#ifndef Driver_RHTemp_Api_h_
#define Driver_RHTemp_Api_h_
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



///
namespace Driver {
///
namespace RHTemp {

/** This class defines an interface for reading and managing a combined RH
    and Temperature sensor. 
    
    Note: There is no requirement for how RH/Temperature is measured; or if 
          the sensor 'package' is connected to the MCU via discrete signals or 
          through a serial bus.

    The class is NOT thread safe.  The application is RESPONSIBLE for ensuring
    thread-safe usage.
 */
class Api
{
public:
    /** State of the non-blocking sampling 
     */
    enum SamplingState_T
    {
        eNOT_STARTED,   //!< No sample sequence has been started
        eSAMPLING,      //!< Sampling is in progress
        eSAMPLE_READY,  //!< A sample has been successfully acquired
        eERROR          //!< An error occurred.  Triggering a new sample request will clear this state.
    };

public:
    /** Starts the driver actively sampling.
     
        The method returns true when it is successful; else false is returned
        (e.g. a communication error occurred).
     */
    virtual bool start() noexcept = 0;

    /** Stops the driver from sampling inputs, and disables the on-board
        heater.
     */
    virtual void stop() noexcept = 0;
    
public:
    /** This method is used to read/sample both RH (percentage 0 to 100) and 
        Temperature (in degrees Centigrade).  
        
        This method blocks/busy-waits until the both RH and Temperature has 
        been captured.  The block/busy-wait time can be relatively long 
        (e.g. >15ms)

        The method returns true when it is successful is reading RH and
        Temperature; else false is returned (e.g. a communication error occurred).

        Note: This method will fail if a non-blocking sample is in progress

        Note: The 'requestedAccuracy' argument is a suggestion, i.e. the
              concrete implementation may or may not support the accuracy
              request.
     */
    virtual bool sample( float& rhOut, float& tempCOut ) noexcept = 0;

public:
    /** This method is used to start a non-blocking sampling sequence.  The
        application then 'polls' state of the sampling by calling getSamplingState()
        
        The method returns eSAMPLING when if successfully starts a sampling 
        sequence; else eERROR is returned (e.g. a sample sequence is already in 
        progress).
     */
    virtual SamplingState_T startSample() noexcept = 0;

    /** This method returns the current state of the non-blocking sampling
     */
    virtual SamplingState_T getSamplingState() noexcept = 0;

    /** Used to retrieve the sample result. Returns the current sample.  If the 
        returned state value does NOT equal eSAMPLE_READY, then 'rhOut' and 
        'tempCOut' arguments are invalid.
     */
    virtual SamplingState_T getSample( float& rhOut, float& tempCOut ) noexcept = 0;

public:
    /** This method is used to enable/disable a 'on-board heater' that is used to 
        evaporate condensation on the sensing element(s). 

        NOTE: The concrete implementation is NOT required to support this
              method, i.e. it is Application RESPONSIBILITY for ensuring it
              business logic that relies on this functionality - that it has
              create the appropriate concrete driver.

        When the heater is on, the Temperature readings are not guaranteed to 
        be accurate.

        The method returns true when it is successful; else false is returned
        (e.g. a communication error occurred).
     */
    virtual bool setHeaterState( bool enabled ) noexcept  { return true; }

public:
    /// Virtual destructor
    virtual ~Api(){}
};

} // End namespace(s)
}

/*--------------------------------------------------------------------------*/
#endif  // end header latch
