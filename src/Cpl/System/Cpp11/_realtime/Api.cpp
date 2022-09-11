#include "Cpl/System/Api.h"
#include <thread>
#include <chrono>


/// 
using namespace Cpl::System;


////////////////////////////////////////////////////////////////////////////////
void Api::sleep( unsigned long milliseconds ) noexcept
{
    std::this_thread::sleep_for( std::chrono::milliseconds( milliseconds ) );
}
