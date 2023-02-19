/** @namespace Cpl::Logging

The 'Logging' namespace provides a framework for Logging events.  The framework
provides the following features:

- The interface is thread safe.

- When an entry is logged it is timestamped.  The application is required to
  provide the implementation of the time-stamp interface.

- Each entry 'logged' has a Category and Message Identifiers.  The Category
  identifier is required to be globally unique to all logged items.  The 
  Message Identifiers are per Category (and are only unique within a Category)

- A Log entry contains the following:
    - Time-stamp
    - Category ID
    - Message ID
    - Text  (interface uses printf semantics when creating a log entry)
     
- The framework is designed to allow/require the application provide the Category
  and Message identifiers/types. 
  
- The logging framework supports selectively enabling/disabling logging by
  Category. Log entries for disabled Categories are silently discarded. 

- Log entries are echoed to the CPL trace engine (when trace is enabled). The
  CategoryID text is used as the trace 'section'

- The framework supports caching log entries BEFORE the application's media. 

- The application is responsible for storing log entries to a (presumably) 
  persistent storage media.  The logging framework dispatches each log entry to 
  a thread-safe FIFO.  It is the application's responsibility to consume the
  entries from the FIFO and record them in persistent storage.  The FIFO supports
  change notifications via a model point - see Cpl::Containers::RingBufferMP.

- The log entry FIFO can overflow if the application is not storing log entries
  faster than the applicatoin is generating entries. When this happens, the 
  framework discards log entries until space frees up in the queue.  The framework 
  tracks how many log entries where discarded and generates a special log entry 
  with the details once there is space in the queue.

*/  


  