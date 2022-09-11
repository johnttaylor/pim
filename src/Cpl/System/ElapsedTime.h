#ifndef Cpl_System_ElaspedTime_h_
#define Cpl_System_ElaspedTime_h_
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
#include <memory.h>


/// 
namespace Cpl {
/// 
namespace System {

/** This class defines the interface for accessing the elapsed time since
    power up and/or reset of the platform.  Elapsed time in provided in
    three different formats:milliseconds, seconds, and seconds with
    milliseconds precision.  All time formats are guaranteed to be
    synchronized with each other.
 */
class ElapsedTime
{
public:
    /// Data type for time in seconds with a 'fractional' millisecond precision
    struct Precision_T
    {
        unsigned long  m_seconds;      //!< Total number of elapsed seconds
        uint16_t       m_thousandths;  //!< fractional number of milliseconds (i.e. a value between 0 and 999)

        /// Comparison operator (explicitly provided to avoid potential issue with pad bytes in the structure)
        bool operator == ( Precision_T const other ) const
        {
            return m_seconds == other.m_seconds && m_thousandths == other.m_thousandths;
        }

        /// Not equals operator
        bool operator != ( Precision_T const other ) const
        {
            return m_seconds != other.m_seconds || m_thousandths != other.m_thousandths;
        }

        /// Increment elapsed time by 'x'
        Precision_T& operator +=( const Precision_T& x );

        /// Greater than (i.e. is this instance newer/more-recent then 'other')
        bool operator > ( Precision_T const other ) const;

        /// Greater than or equal (i.e. is this instance the same as 'other' or is it newer/more-recent then 'other')
        bool operator >= ( Precision_T const other ) const;

        /// less than (i.e. is this instance older then 'other')
        bool operator < ( Precision_T const other ) const;

        /// less than or equal (i.e. is this instance the same as 'other' or is it older then 'other')
        bool operator <= ( Precision_T const other ) const;

        /// Sets the Precision value from a millisecond value
        Precision_T& setFromMilliseconds( uint32_t milliseconds );

        /// Assign my value based on total milliseconds
        Precision_T& operator =( uint32_t milliseconds );

        /// Converts the instance's time into as a single 'large' integer value in milliseconds
        uint64_t asFlatTime() const { return m_seconds * 1000 + m_thousandths; }

        /// Sets the instance's time from a single 'large' integer value in milliseconds
        void setFlatTime( uint64_t flatTimeInMs ) 
        { 
            m_seconds     = (unsigned long) (flatTimeInMs / 1000);
            m_thousandths = (uint16_t) (flatTimeInMs % 1000);
        }

        /** Converts the instance's time to milliseconds.  The result CAN be
            incorrect if the actual number of milliseconds is greater than what
            can be stored in an unsigned long (e.g. a 32 bit unsigned integer
            can only contain ~47 days 'of milliseconds'). USE WITH CAUTION.
         */
        unsigned long asMilliseconds() const
        {
            return m_seconds * 1000 + m_thousandths;
        }
         
    public:
        /// Constructor (to ensure any pad bytes get zero'd)
        Precision_T()
        {
            memset( (void*) this, 0, sizeof( Precision_T ) );   
        }

        /// Constructor (to ensure any pad bytes get zero'd)
        Precision_T( unsigned long  seconds, uint16_t thousandths )
        {
            memset( (void*) this, 0, sizeof( Precision_T ) );
            m_seconds     = seconds;
            m_thousandths = thousandths;
        }

        /// Constructor (to ensure any pad bytes get zero'd)
        Precision_T( uint64_t flatTimeInMs )
        {
            memset( (void*) this, 0, sizeof( Precision_T ) );
            setFlatTime( flatTimeInMs );
        }
        
        /// Copy Constructor (to ensure any pad bytes get zero'd)
        Precision_T( const Precision_T& other )
        {
            memcpy( (void*) this, (void*) &other, sizeof( Precision_T ) );
        }

        /// Copy operator
        Precision_T& operator =( const Precision_T& other )
        {
            memcpy( (void*) this, (void*) &other, sizeof( Precision_T ) );
            return *this;
        }
    };


public:
    /** This method returns the elapsed time, in seconds, since the system
        was powered on and/or reset.  The elapsed time is free running counter
        that will roll over once the range of the data type is exceeded.
     */
    static unsigned long    seconds() noexcept;


    /** This method returns the elapsed time, in milliseconds, since the system
        was powered on and/or reset.  The elapsed time is free running counter
        that will roll over once the range of the data type is exceeded.
     */
    static unsigned long    milliseconds() noexcept;


    /** This method returns the elapsed time, in seconds with milliseconds
        precision, since the system was powered on and/or reset.  The elapsed
        second time is free running counter that will roll over once the range
        of the data type is exceeded.
     */
    static Precision_T  precision() noexcept;


public:
    /** This method returns the delta time, in milliseconds, between the
        specified 'startTime' and 'endTime'.  'endTime' is defaulted to
        NOW (i.e. a call to milliseconds(). The calculation properly
        handles the scenario of when the has been 'roll over' between the
        two times.
     */
    inline static unsigned long deltaMilliseconds( unsigned long startTime, unsigned long endTime = milliseconds() ) noexcept
    {
        return endTime - startTime;
    }


    /** This method returns the delta time, in seconds, between the
        specified 'startTime' and 'endTime'.  'endTime' is defaulted to
        NOW (i.e. a call to seconds(). The calculation properly
        handles the scenario of when the has been 'roll over' between the
        two times.
     */
    inline static unsigned long deltaSeconds( unsigned long startTime, unsigned long endTime = seconds() ) noexcept
    {
        return endTime - startTime;
    }

    /** This method returns the delta time, in Precision time, between the
        specified 'startTime' and 'endTime'.  'endTime' is defaulted to
        NOW (i.e. a call to precision(). The calculation properly
        handles the scenario of when the has been 'roll over' between the
        two times.
     */
    static Precision_T deltaPrecision( Precision_T startTime, Precision_T endTime = precision() ) noexcept;


public:
    /** This method returns true if the specified amount of time has elapsed
        since the 'timeMarker'.  The calculation properly handles the scenario
        of when the has been 'roll over' between the 'timeMarker' and NOW.
     */
    inline static bool expiredMilliseconds( unsigned long timeMarker, unsigned long duration, unsigned long currentTime = milliseconds() ) noexcept
    {
        return deltaMilliseconds( timeMarker, currentTime ) >= duration;
    }

    /** This method returns true if the specified amount of time has elapsed
        since the 'timeMarker'.  The calculation properly handles the scenario
        of when the has been 'roll over' between the 'timeMarker' and NOW.
     */
    inline static bool expiredSeconds( unsigned long timeMarker, unsigned long duration, unsigned long currentTime = seconds() ) noexcept
    {
        return deltaSeconds( timeMarker, currentTime ) >= duration;
    }


    /** This method returns true if the specified amount of time has elapsed
        since the 'timeMarker'.  The calculation properly handles the scenario
        of when the has been 'roll over' between the 'timeMarker' and NOW.
     */
    static bool expiredPrecision( Precision_T timeMarker, Precision_T duration, Precision_T currentTime = precision() ) noexcept;



public:
    /** This method will initialize the contents 'dst' to the number of seconds
        specified by 'seconds' and set the m_thousandths field to zero.
     */
    inline static void initializeWithSeconds( Precision_T& dst, unsigned long seconds )
    {
        dst.m_seconds = seconds; dst.m_thousandths = 0;
    }

    /** This method will initialize the contents of 'dst' to the number of
        milliseconds specified by 'msec'.  If 'msec' is greater than 1000, the
        m_seconds field will be populate with the "overflow".
     */
    inline static void initializeWithMilliseconds( Precision_T& dst, unsigned long msec )
    {
        dst.m_seconds = msec / 1000; dst.m_thousandths = msec % 1000;
    }

public:
    /** This method is the same as seconds(), EXCEPT that is ALWAYS guaranteed
        to return elapsed time in 'real time'.  See the Cpl::System::SimTick for
        more details about real time vs. simulated time.  It is recommended
        the application NOT CALL this method because then that code can NOT
        be simulated using the SimTick interface.
     */
    static unsigned long    secondsInRealTime() noexcept;


    /** This method is the same as milliseconds(), EXCEPT that is ALWAYS
        guaranteed to return elapsed time in 'real time'.  See the
        Cpl::System::SimTick for more details about real time vs. simulated
        time.  It is recommended the application NOT CALL this method because
        then that code can NOT be simulated using the SimTick interface.
     */
    static unsigned long    millisecondsInRealTime() noexcept;


    /** This method is the same as precision(), EXCEPT that is ALWAYS guaranteed
        to return elapsed time in 'real time'.  See the Cpl::System::SimTick for
        more details about real time vs. simulated time.  It is recommended
        the application NOT CALL this method because then that code can NOT
        be simulated using the SimTick interface.
     */
    static Precision_T      precisionInRealTime() noexcept;



};


};      // end namespaces
};
#endif  // end header latch

