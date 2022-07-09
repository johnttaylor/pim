#ifndef Storm_Utils_SimSystem_h_
#define Storm_Utils_SimSystem_h_
/*-----------------------------------------------------------------------------
* This file is from Michael Moran's OSCL library (http://mnmoran.org/oscl.html)
* and has no copyright and/our usage restrictions.  It has been slightly
* modified for inclusion in the colony.apps repository
*
* Copyright 2003 Michael Nelson Moran
* This software may be copied and used without restriction.
*----------------------------------------------------------------------------*/
/** @file */

///
namespace Storm {
///
namespace Utils {


/** This class simulates a "system" whose state is affected
    by its environment and any number of control systems
    that may change the environment over time. The design of
    this class was inspired by the electrical theory surrounding
    the charging and discharging of a capacitor by a voltage
    source (potential) through a resistance. The environment
    in exactly the same way as the various controlling sources,
    that is as a potential and a series resistance. Although
    this model is based upon electrical engineering, it is
    essentially unit-less and may be applied to similarly behaving
    systems (e.g. thermal systems).
 */
class SimSystem
{
private:
    /** Contains the time in seconds between system
        updates.
     */
    const double	_tickPeriodInSeconds;

    /** The potential of the system after the previous
        system update. This value is used as the starting
        point for the next system update calculation,
        and is modified appropriately at that time.
     */
    double		    _previousOutputPotential;

public:
    /** This value represents the default value of the
        resistance between the system and the environmental
        potential.
     */
    double	        _environmentResistance;

    /** This value represents the capacity of the system.
        E.g. in an electrical system this is the value of
        the capacitor. It affects the overall "inertia" of
        the simulated system.
     */
    double	        _capacitance;

    /** This value is used during the start() -> accumulate() ->
        finish() sequence to hold the accumulated "equivalent"
        potential of the combined environment and control
        sources. Its value is calculated in the same way as
        the "open-circuit voltage" in a Thevenin equivalent
        circuit.
     */
    double	        _peq;

    /** This value is used during the start() -> accumulate() ->
        finish() sequence to hold the accumulated "equivalent"
        resistance of the combined environment and control
        sources. Its value is calculated in the same way as
        the "equivalent series resistance" in a Thevenin equivalent
        circuit.
     */
    double	        _req;

public:
    /**
     * tickPeriodInSeconds is the time between each
     * call to the tick() operations.
     * controlResistance is the "internal" resistance
     * of the control input to other network.
     * environmentResistance is the "internal" resistance
     * of the environment.
     * capacitance is the amount of charge that can be
     * stored in the system.
     * initialOutputPotential is the initial voltage across
     * the capacitor/system.
     */
    SimSystem( double	tickPeriodInSeconds,
               double	environmentResistance,
               double	capacitance,
               double	initialOutputPotential ) noexcept;

    /** This operation is used to start an accumulation cycle.
        It allows several sources to affect the calculation
        of the system output over the next cycle. In this
        variant, the potential is that of the "environment"
        and the resistance is assumed to be that of the
        environment as specified by the constructor.
     */
    void	start( double potential ) noexcept;

    /** This operation is used to start an accumulation cycle.
        It allows several sources to affect the calculation
        of the system output over the next cycle. In this
        variant, the potential is that of the "environment"
        and the resistance is also that of the "environment"
        and is used instead of the constant supplied in the
        constructor.
     */
    void	start( double potential, double resistance ) noexcept;

    /** This operation is to be invoked zero or more times after
        one of the start() operations and before the finish()
        operation. Each invocation accumulates a new potential
        source into the system calculation. This allows any
        number of sources, each consisting of a potential and
        a resistance to affect the next state of the system.
     */
    void	accumulate( double potential, double resistance ) noexcept;

    /** This operation is issued after one of the start() operations
        and zero or more accumulate() operations to calculate the
        new output potential/state of the system, that is returned as
        a result.
     */
    double	finish() noexcept;

};

};      // end namespaces
};
#endif  // end header latch

