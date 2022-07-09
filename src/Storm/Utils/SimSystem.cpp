/*-----------------------------------------------------------------------------
* This file is from Michael Moran's OSCL library (http://mnmoran.org/oscl.html)
* and has no copyright and/our usage restrictions.  It has been slightly
* modified for inclusion in the colony.apps repository
*
* Copyright 2003 Michael Nelson Moran
* This software may be copied and used without restriction.
*----------------------------------------------------------------------------*/


#include "SimSystem.h"
#include <math.h>


using namespace Storm::Utils;

////////////////////////////////////////////
SimSystem::SimSystem( double	tickPeriodInSeconds,
                      double	environmentResistance,
                      double	capacitance,
                      double	initialOutputPotential ) noexcept
    : _tickPeriodInSeconds( tickPeriodInSeconds )
    , _previousOutputPotential( initialOutputPotential )
    , _environmentResistance( environmentResistance )
    , _capacitance( capacitance )
{
}

void	SimSystem::start( double inPotential ) noexcept
{
    _peq	= inPotential;
    _req	= _environmentResistance;
}

void	SimSystem::start( double inPotential, double envResistance ) noexcept
{
    _peq	= inPotential;
    _req	= envResistance;
}

void	SimSystem::accumulate( double inPotential, double envResistance ) noexcept
{
    _peq	= ( ( inPotential - _peq ) * ( _req / ( envResistance + _req ) ) ) + _peq;
    _req	= 1.0 / ( ( 1.0 / _req ) + ( 1.0 / envResistance ) );
}

double	SimSystem::finish() noexcept
{
    double	vdelta	= _peq - _previousOutputPotential;
    double	vchange	= vdelta * ( 1 - exp( -( _tickPeriodInSeconds / ( _req * _capacitance ) ) ) );
    _previousOutputPotential	= vchange + _previousOutputPotential;
    return _previousOutputPotential;
}



