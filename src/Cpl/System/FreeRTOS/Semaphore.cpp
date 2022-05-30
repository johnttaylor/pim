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

#include "Cpl/System/Semaphore.h"
#include "Cpl/System/FatalError.h"



//////////////////////////////////////////////////
Cpl::System::Semaphore::Semaphore( unsigned initialCount )
{
    m_sema = xSemaphoreCreateCounting( 0x7FFF, initialCount );
    if ( m_sema == NULL )
    {
        Cpl::System::FatalError::logf( "Cpl:System::Semaphore::Semaphore().  Failed to create semaphore" );
    }
}

Cpl::System::Semaphore::~Semaphore()
{
    vSemaphoreDelete( &m_sema );
}


//////////////////////////////////////////////////
int Cpl::System::Semaphore::signal( void ) noexcept
{
    xSemaphoreGive( m_sema );
    return 0;
}


/** NOTE: This method returns the 'higherPriorityTaskWoken' instead of the
          defined 'return zero on success' semantics.  This is to overcome
          the oddities of FreeRTOS.
 */
int Cpl::System::Semaphore::su_signal( void ) noexcept
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR( m_sema, &higherPriorityTaskWoken );
    return higherPriorityTaskWoken;
}


//////////////////////////////////////////////////
void Cpl::System::Semaphore::wait( void ) noexcept
{
    xSemaphoreTake( m_sema, portMAX_DELAY );
}


bool Cpl::System::Semaphore::timedWait( unsigned long timeout ) noexcept
{
    return xSemaphoreTake( m_sema, timeout * portTICK_PERIOD_MS ) == pdTRUE;
}


bool Cpl::System::Semaphore::tryWait( void ) noexcept
{
    return xSemaphoreTake( m_sema, 0 ) == pdTRUE;
}


void Cpl::System::Semaphore::waitInRealTime( void ) noexcept
{
    xSemaphoreTake( m_sema, portMAX_DELAY );
}


bool Cpl::System::Semaphore::timedWaitInRealTime( unsigned long timeout ) noexcept
{
    return xSemaphoreTake( m_sema, timeout * portTICK_PERIOD_MS ) == pdTRUE;
}



