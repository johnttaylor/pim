/** @namespace Cpl::Dm

The 'Dm' namespace is the root namespace for a framework of the Data Model
architecture pattern.

The Data Model software architecture pattern is a data-oriented pattern where
modules interact with each other via data instances (a.k.a. model points) with
no direct dependencies between modules.

The data model consists of all the model point instances in your design, and it 
is the canonical authority for the data, which determines the application 
behavior. Model points have the following features:

- The value stored in a model point can be of any type or any data
  structure. A model point’s value should be strictly data and not an
  object. That is, model points do not contain business rules or enforce
  policies (except for things like value range checking).

- Model point instances are type safe. That is, all read and write
  operations are specific to the model point’s value type.

- Model points have atomic operations for accessing their value or
  state. This means that accessing model point values is a thread-safe
  operation.

- Model points have a valid or invalid state independent of their value.

- Model points provide a subscription mechanism for clients to receive
  change notifications when a model point’s value changes or if the  
  model point’s validity changes.

*/  

  