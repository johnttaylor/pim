# Introduction to the Data Model Architecture
The Data Model architecture is used to design highly decoupled code.  It allows 
for the exchange of data between modules with neither module have dependencies 
on each other. 

For example: given the coupled design below:
```

  ┌────────────┐             ┌───────────┐
  │            │ writes to   │           │
  │ Module 1   ├────────────►│ Module 2  │
  │            │             │           │
  └────────────┘             └───────────┘

```
Using the Data Model pattern, the above interaction is replaced with the following:
```

    ┌────────────┐             ┌───────────┐
    │            │             │           │
    │ Module 1   │             │ Module 2  │
    │            │             │           │
    └─────┬──────┘             └──────┬────┘
          │                           │
 writes to│                           │reads from                          
          │       ┌─────────────┐     │
          └──────►│Model Point A│◄────┘
                  └─────────────┘

```
In the above design both modules are passed a reference to Model Point A in 
their constructors or during initialization. While this may seem like you’re 
introducing an unnecessary layer of abstraction, in reality, you’re giving your 
design the following advantages:

- It makes it easier to extend existing functionality without modifying existing 
source code. For example, you could extend the above deisgn by introducing a third 
module that takes the output of Module 1 and modifies it for input to Module 2. 
In this case, we would only need to create a new model point. In the figure below, 
then, Module 3 is passed a reference to Model Point A and Model Point AA in its 
constructor or during initialization. The point here is that even though there
has been a feature change, the original Module 1 and Module 2 do not need to be 
rewritten. The only change would be to the code that constructs or initializes 
Module 2 so that it is passed a reference to Model Point AA (instead of Model 
Point A).
```

    ┌──────────────┐             ┌─────────────┐
    │              │             │             │        
    │  Module 1    │             │  Module 2   │
    │              │             │             │   
    └──────┬───────┘             └──────┬──────┘
           │                            │    
 writes to │                            │ reads from
           ▼                            ▼               
    ┌──────────────┐             ┌──────────────┐
    │Model Point A │             │Model Point AA│
    └──────────────┘             └──────────────┘
           ▲                            ▲
           │                            │
   reads from                       writes to
           │     ┌──────────────┐       │
           │     │              │       │ 
           └─────┤  Module 3    ├───────┘
                 │              │ 
                 └──────────────┘

```
- It breaks up what might normally be a sequential bottom-up, or top-down,
development process and facilitates parallel development, which is what allows 
you to add additional programmers to a project and have them actually contribute 
effectively toward shortening your development schedule.

- It simplifies the construction of unit tests.

- It simplifies writing platform-independent and compiler-independent source code.

- It simplifies the construction of a functional simulator.

- It creates a large number of reusable modules. 
 
In the data model architecture, the modules or components interact with each 
other with no direct dependencies on each other. A well-defined model point will 
have the following features:

- Model points support any primitive data type or any data structure as their 
value. A model point’s value should be strictly data and should not contain 
business rules or enforce policies (except for discrete, self-contained 
operations like value range checking).

- Individual model points are type specific with respect to their value. That 
is, all read/write operations are type safe.

- Model points have atomic operations that read or write their values.
This means that accessing model point values is a thread-safe
operation.

- Model points have valid or invalid states that are independent of their value.

- Model points provide a subscription mechanism for client entities to receive 
change notifications when the value of a model point changes or transitions to 
a valid or invalid state.

# More Details
Additional details can found in Chapters 9 & 15 in my book: [Patterns in the Machine: A Softawre Engineering Guide to Embedded Development](https://www.apress.com/us/book/9781484264393)