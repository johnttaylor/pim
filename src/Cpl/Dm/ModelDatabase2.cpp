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

#include "ModelDatabase.h"

/** NOTE: Put the allocation of the temporary buffer is separate file.  This
          is so that if Application does not have any Model Point that make
          use of the buffer -->then the buffer will not be allocated (because
          there will be no linker references to it).
*/

// global Buffer
uint8_t   Cpl::Dm::ModelDatabase::g_tempBuffer_[OPTION_CPL_DM_MODEL_DATABASE_TEMP_STORAGE_SIZE];

