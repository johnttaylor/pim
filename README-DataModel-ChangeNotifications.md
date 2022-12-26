# Data Model Change Notifications
This page provides in-depth details about the Data Model's change notification 
mechanism and semantics as implemented by the [CPL C++ Class library](https://github.com/johnttaylor/colony.core). A brief overview to the Data Model can be found 
[here](https://github.com/johnttaylor/pim/blob/master/README-Intro-DataModel.md) 

# The Basics
Model points provide a subscription mechanism for clients to receive change 
notifications when a model point’s value changes or if the model point’s 
validity changes.  The callback function is type specific to the model point
being monitored.

To receive a change notification a client must do the following:
1. Create an _Observer_ instance that remains in scope for entire time that the
client is subscribed for the change notifications.

1. Define a callback method that is invoked when the model point changes.  Note: 
The callback method is a _class member method_ (i.e. a _this_ pointer is required
to call the method).

1. Subscribe to the change notifications using the model point's `attach()`
method.  Once subscribed, the client's callback method in step 2 will be invoked
every time the Model Point's data/state changes.

1. Cancel the change notifications subscription before the client's _Observer_
instance goes out of scope.

By definition change notification callbacks are asynchronous events.  This 
requires an _event loop_ for processing and invoking the callback method.  In
the pseudo code below the event loop is provided by the `clientMailbox`
argument.

The following is pseudo code for the basic subscription process.  A working 
example project that illustrates Data Model change notifications can be found
[here](https://github.com/johnttaylor/pim/blob/master/projects/Examples/DataModel/README.md)

```
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/Dm/SubscriberComposer.h"

class MyClient
{
public:
    /// Constructor.
    MyClient( Cpl::Dm::MailboxServer& clientMailbox,
              Cpl::Dm::Mp::Uint32&    modelPointToMonitor )
        : m_mp( modelPointToMonitor )
        , m_observerMp( clientMailbox, *this, &MyClient::modelPointChange )
    {
    }


    /// This method is used to initialize the Client instance. 
    void start()
    {
        // Subscribe to change notifications 
        m_mp.attach( m_observerMp );
    }

    /// This method is used to shutdown the Client instance. 
    void stop()
    {
        // Cancel change notifications subscription
        m_mp.detach( m_observerMp );
    }

protected:
    /// Callback for model point change notifications
    void modelPointChange( Cpl::Dm::Mp::Uint32&    modelPointThatChanged, 
                           Cpl::Dm::SubscriberApi& clientObserver ) noexcept
    {
        uint32_t mpValue;
        if ( modelPointThatChanged.readAndSync( mpValue, clientObserver ) )
        {
            // The model has a valid value (stored in mpValue).  Do something...
        }
    }

protected:
    /// Model Point to monitor
    Cpl::Dm::Mp::Uint32&    m_mp;

    /// Observer instance for monitoring the model point
    Cpl::Dm::SubscriberComposer<MyClient, Cpl::Dm::Mp::Uint32>  m_observerMp;
};
```

# Change Mechanism
The change-detect mechanism uses a sequence number.  Each Model Point and each 
Subscriber (aka Observer instance) has a sequence number.  When the Subscriber's 
sequence number does not equals the Model Point's sequence number - the Subscriber
receives a change notification and the Subscriber's sequence number is updated 
to match the Model Point's sequence number at the time of the change notification.

When a Client subscribes to a model point, the client must provide an initial
sequence number for its Observer instance.  The `attach()` method provides a
default of `Cpl::Dm::ModelPoint::SEQUENCE_NUMBER_UNKNOWN`. When 
`SEQUENCE_NUMBER_UNKNOWN` is used during the subscription, the client will 
get an _immediate_ callback to synchronize the Observer and model point sequence
numbers.  In this content _immediate_ means the next iteration of the event loop
processing. 

Alternatively the client can specify the model point's current sequence
number (e.g. by calling `getSequenceNumber()` on the model point) when 
subscribing. In this case, no callback occurs until the model point is updated.  

#### What's a Change
What actually a constitutes a change to a model point? There are two conditions
that if either one of them is true, a change is declared (i.e. the model point's 
sequence number is updated).

1. The model point has a transition in its valid/invalid state. OR
2. The model point's internal `isDataEqual_()` method returns false.

What is important to note above, is the __each__ concrete/type-specific model point
determines its own definition of _is-equal_.  For example, the CPL framework
provides a model point type `Cpl::Dm::Mp::Float` that contains a `float` 
value.  The implementation of this model point uses the framework's 
`Cpl::Math::areFloatsEqual` when determining _is-equal_. This means that any
change with an epsilon value greater than 1.192092896e-07F will trigger a 
change notification. This is less than optimal when using an event-driven 
paradigm. A alternative solution would be to create a new model point type, perhaps
`Cpl::Dm::Mp::FloatDelta`, that still holds float values but implements
a minimum delta threshold in its `isDataEqual_()` method  (e.g. require
a delta change greater than 0.01F).

__Summary__: Do not make blanket assumptions about all model points in how they define _is-equal_.


# Edges and More Edges
The data model’s change notification semantics guarantee that a client will get 
a notification when there is a change to a model point’s value or state. However, 
the client is not guaranteed to get notified for every change - just a notification 
for the last one. For example, consider a system that is configured for the following
behavior:
- A Sensor Driver is updating model point A every millisecond with
a different analog value.

- Module B subscribes for change notifications from model point A.

- The Sensor Driver and Module B execute in different threads.
Depending on thread priorities, CPU performance, and other activities in
the system, Module B is unlikely to be able to process a change notification
every millisecond. If, let’s say, Module B’s thread is delayed by 5ms before the
change notification can be executed, Module B will receive a single change
notification (not five), and the value (in the callback function) of the model 
point A will be the latest value written by the Sensor Driver.

#### Duplicate callbacks
Another nuisance to changes notification is the possibility of __duplicate__ 
changes notifications, i.e. the change notification callback function
is called twice where the model point has the same value and the same sequence
number.  When the event loop detects a change in the model point, i.e. a model 
point's sequence number is different than a subscribed Observer.  The event 
loop updates the Observer's sequence number to match the model point's sequence 
number before invoking the Observer's callback function.  This process is an atomic
operation within the event loop.  However, the actual executing of the callback
function is __not__ included as part of this atomic operation.  This means that 
there is a small window of time where if the model point is updated before the callback 
function actually executes there will be a __duplicate__ callback. For example:

1. A client is subscribed to model Point A.  The client is running in thread Y.

2. Then in thread Z, model point A is set invalid state and then immediately set 
to a valid value.

3. A possible execution flow is:

   i. Thread Z: model point A is set to Invalid.  
      - The mp sequence number is `N`
      - The client's observer sequence number is `N-1`

   i. Thread Y: The change notification processing occurs for the transition to 
      invalid and updates the client observer’s sequence number – but has not 
      yet invoked the change notification callback to the client. 
      - The mp sequence number is `N`
      - The client's observer sequence number is `N`

   i. Thread Z: Model point A is set to a valid value. At this point in the time
      model point A’s sequence number is now different than the client’s observer 
      sequence number.
      - The mp sequence number is `N+1`
      - The client's observer sequence number is `N`

   i. Thread Y: The change notification callback executes.  The value of model 
      point A at this point in time is a valid value – not the invalid state.
      - The mp sequence number is `N+1`
      - The client's observer sequence number is `N`

   i. Thread Y: In the next event loop processing cycle, the model point sequence 
      number is still different from the observer instance’s sequence number and
      triggers the change notification processing to occur again.
      - The mp sequence number is `N+1`
      - The client's observer sequence number is `N+1`

   i. Thread Y: The change notification callback executes.  The value of model 
      point A is the same value as in step (iv) above.
      - The mp sequence number is `N+1`
      - The client's observer sequence number is `N+1`

#### Duplicate callbacks Solutions
- __Do nothing__.  The client's logic with respect to model point change 
notifications doesn't' matter (or doesn't matter _enough_) if there are 
duplicate callbacks.

- __Add client specific logic to detect/filter out duplicate callback__.  This 
solution is __not__ recommended.

- __Use the model point's readAndSync() methods__.  The `readAndSync()` and 
`isNotValidAndSync()` methods synchronize the client's Observer instance's
sequence number with the model point's sequence number when the reading the 
model point's value/state.  This is the __recommended__ solution.

  - What do the above methods do?  There are actually what I call convenience 
    methods in that they simply package up - into single function call - behavior 
    that is repeated frequently.  All model point read operations optionally
    return the model point's sequence number at the time of the read call.  The 
    sequence number returned from the read operation is then used to re-subscribe 
    the Observer instance to the model point.  Below is pseudo code that 
    illustrates how the `readAndSync()` method works.
    ```
    inline bool readAndSync( uint32_t& dstData, SubscriberApi& observerToSync )
    {
        uint16_t seqNum;
        bool result = read( dstData, &seqNum );
        attach( observerToSync, seqNum );
        return result;
    }

    ```
   - The `readAndSync()` and `isNotValidAndSync()` methods should only be used
     inside a change notification callback function.

# Threading
The change notifications require an event loop where the event loop maps one-to-one
with a thread. The following constraints are imposed on the client with respect
to threading:
1. The thread that the subscription (i.e. the `attach()` method is called) will
be the __same__ thread/event-loop that invokes the change notification callback
function.
2. The client is required to unsubcribe (i.e. the `detach()` method is called)
while executing in the same thread where the subscription occurred.

The above translates to: when designing a module that monitors model points for
changes it must have _in-thread_ initialization and shutdown methods in which it
calls the `attach()` and `detach()` methods.  Below is updated pseudo code that 
illustrates how to implement the _in-thread_ initialization using the CPL 
framework.  The code uses the framework's message based Inter-Thread-Communication 
(ITC) to receive _open_ and _close_ requests/messages that execute in the client's
thread.  In this context, the _in-thread_ means executes in the thread that 
is associated with the `clientMailbox`

```
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Itc/CloseSync.h"


// Inherit from the CloseSync class to provide the synchronous 
// open()/close() ITC methods
class MyClient: public Cpl::Itc::CloseSync
{
public:
    /// Constructor.
    MyClient( Cpl::Dm::MailboxServer& clientMailbox,
              Cpl::Dm::Mp::Uint32&    modelPointToMonitor )
        : Cpl::Itc::CloseSync( clientMailbox )
        , m_mp( modelPointToMonitor )
        , m_observerMp( clientMailbox, *this, &MyClient::modelPointChange )
    {
    }


    /** This ITC request is used to initialize the Client instance. 
        This method executes in the context of the 'clientMailbox'
        thread.
     */
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
    {
        // Subscribe to change notifications 
        m_mp.attach( m_observerMp );
        
        // Return the ITC message 
        msg.returnToSender();
    }

    /** This ITC request is used to shutdown the Client instance. 
        This method executes in the context of the 'clientMailbox'
        thread.
     */
    void request( Cpl::Itc::OpenRequest::CloseMsg& msg )
    {
        // Cancel change notification subscription
        m_mp.detach( m_observerMp );
        
        // Return the ITC message 
        msg.returnToSender();
    }

protected:
    /** Callback for model point change notification. 
        This method executes in the context of the 'clientMailbox'
        thread.
     */
    void modelPointChange( Cpl::Dm::Mp::Uint32&    modelPointThatChanged, 
                           Cpl::Dm::SubscriberApi& clientObserver ) noexcept
    {
        uint32_t mpValue;
        if ( modelPointThatChanged.readAndSync( mpValue, clientObserver ) )
        {
            // The model has a valid value (stored in mpValue).  Do something...
        }
    }

protected:
    /// Model Point to monitor
    Cpl::Dm::Mp::Uint32&    m_mp;

    /// Observer instance for monitoring the model point
    Cpl::Dm::SubscriberComposer<MyClient, Cpl::Dm::Mp::Uint32>  m_observerMp;
};
```

# Generic Subscriptions
The model point framework supports subscribing to change notifications from 
non-type specific model points, i.e. the data type for the model point argument 
in the callback function is `Cpl::Dm::ModelPoint&` .  This means that
callback function is restricted to __only__ calling public methods that are 
common to all model points.  For example, the public `read(...)` and `write(...)`
methods for model points are type-specific and can __not__ be called (i.e. 
compiler enforced).

So why is there support for generic subcriptions?  There are some use cases where
being able to monitor a model point of any/all types comes in handy.  For example,
the framework provides persistent storage for model points. The persistent storage
logic uses the generic subscriptions to update NVRAM when one or more model points
change state.  There reason that this work is that model point's `toJSON()`,
`exportData()`, and `importData()` are public _read_/_write_ methods in the `Cpl::Dm::ModelPoint` class.   

__Summary:__ Avoid using generic subscriptions.  And definitely do __not__ use
generic subscriptions and then brute force down cast the model point reference 
to a specific type.



# More FAQs
- To receive change notifications, all clients must _run in the context_ of a `Cpl::Dm::EventLoop`, `Cpl::Dm::MailboxServer` or `Cpl::Dm::PeriodicScheduler`

- Change notifications can be used in a baremetal design (i.e. without threads)
when the applicatoin's _main loop_ is setup to be event based using one of 
the following: `Cpl::Dm::EventLoop`, `Cpl::Dm::MailboxServer` or `Cpl::Dm::PeriodicScheduler`

- The callback for change notifications are invoked when the _event loop_ 
logic processes the list of pending change notifications.  In addition, at
most one change notification is dispatched per the event loop processing
cycle.

- There is no limit to the number of Subscribers that can attach to a Model 
Point.
 
- The model point's `attach()` method can be called even if the Client/Subscriber 
is already attached.  When this happens, the attach process is 'restarted', i.e.
the 'initialSeqNumber' is used for the Subscriber's sequence number.

- The model point's `detach()` method can be called even if the Client/Subscriber
is __not__ currently attached.  When this happens, the call does nothing.

- The model point's `attach()` and `detach()` methods can be called within the Change Notification 
callback.

- A client can subscribe/un-subscribe to change notifications as many times as
needed, as long as the Observer instance being used is in scope for the duration
of a subscription.

- There is one-to-one relationship between a Client's Observer instance and
a model point instance that the Client subscribes to for change notifications.
However, a single callback function can be used/shared across multiple Observer
instances.  The only restriction is that model point type in the callback function
must match the model point type that is being subscribed to.


# More Details
Additional details can found in Chapters 9 & 15 in my book: [Patterns in the Machine: A Softawre Engineering Guide to Embedded Development](https://www.apress.com/us/book/9781484264393)