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

#include "helpers.h"
#include "Cpl/TShell/Maker.h"


////////////////////////////////////////////////////////////////////////////////

extern Cpl::Container::Map<Cpl::TShell::Command>   cmdlist;

static Cpl::TShell::Maker cmdProcessor_( cmdlist );

static Cpl::TShell::Cmd::Help	helpCmd_( cmdlist );
static Cpl::TShell::Cmd::Bye	byeCmd_( cmdlist  );
static Cpl::TShell::Cmd::Trace	traceCmd_( cmdlist );
static Cpl::TShell::Cmd::TPrint	tprintCmd_( cmdlist );


static Apple   mockApp;
static Bob     bobCmd( cmdlist, mockApp  );

