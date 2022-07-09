/** @namespace Cpl::Persistent

The 'Persistent' namespace provides a basic persistent storage mechanism 
for non-volatile data.  The persistent sub-system has the following 
features:

    o The sub-system organizes persistent data into Records.  The 
      Application is responsible for defining what the data content of record. 
      It is the responsibility of the concrete Record instances to initiate 
      updates to the persistent media. A 'Record' is the unit of atomic 
      read/write operations from/to persistent storage.

    o On start-up the Records are read and the concrete Record instances
      process the incoming data.

    o All persistently stored data is CRC'd to detect data corruption. 
      Note: The CRCs are only validated on start-up. 
    
    o Record instances are responsible for defaulting their data (and subsequently
      initiating an update to the persistent media) when the stored data
      has been detected as corrupt (i.e. bad CRC)

    o The sub-system is independent of the physical persistent storage
      media 

    o The Record Server can process an unlimited number of records.  It is 
      also okay to have more than one Record Server instance.

Threading Model:
    The Record Server is a runnable-object and/or a Data Model Mailbox, aka is
    a thread.  All read/write operations to the persistent media are performed
    in this thread.  It is assumed that the business logic for individual records
    is also performed in this thread - and that each record instance is thread
    safe with respect to the rest of the system.

*/  


  