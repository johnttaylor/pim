/** @namespace Cpl::Dm

The 'Dm' namespace is the root name space for a Data Model centric Run Time 
Environment. 

The Rte namespace is multi-threaded data driven paradigm that provides a
Model-View-Controller (MVC) approach for Application components to interact with
each other. All application data that is shared across the Application
components is owned by the Model.  Individual Application components can read
(viewer) and/or write (controller) any of the data in the Model. Viewers can be
asynchronous, i.e. event driven, or synchronous, i.e. queries.  Controllers are
always synchronous. The data in the Model is always in RAM with the ability to
have a backing persistence storage to non-volatile  media.



Model-View-Controller
---------------------
The MVC paradigm is based around 'Points'. Points can be viewed as data items
and/or structures that are stored in the model and that Application uses viewers
or controllers to  read, write the data items.  Some terminology:

- A 'Model Data Base' is a collection of Model Points.  The Application is 
  responsible for instantiating all of the Model Points in a Model Data Base.  
  Typically the life time scope of a Model Data Base is the life of the 
  Application.

- A 'Model Point' is an instantiated Point within a Model Data Base.  A Model 
  Point is the canonical value/state of a Point.  Model Points are type safe 
  (because the underlying Point is type safe). Because each Model Point 
  definition is unique this means that from the Application perspective there 
  is not a base class/interface that can be used to as a generic reference to 
  Model Points. However, the great majority of the concrete Model Points will 
  share common methods and semantics.

- A 'Point' is a physical collection of data, i.e. it is a C structure.  

- A 'Viewer Point' is an instantiated Point within a Application module that
  is used to read the contents of a Model Point.  All reads of an Model Point
  is done synchronously.  However, all Points support the ability for client(s)
  to register for asynchronous change notifications. There can be many Viewer 
  Points reading a single Model Point.  Typically a Viewer Point is created
  on the stack.  Note: There is no 'Viewer' data-type/class. The term Viewer
  Point refers to how a Point instance is used.

- A 'Controller Point' an instantiated Point within a Application module that 
  is used to synchronously write to a Model Point.  There can be many Controller 
  Points writing a single Model Point. Typically Controller Points are created 
  on the stack.  Note: There is no 'Controller' data-type/class. The term 
  Controller Point refers to how a Point instance is used.

*/  


  