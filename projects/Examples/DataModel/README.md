# Data Model Example

The Data Model example project is a basic tutorial for using the Data Model framework.  An introduction to the data model can be found [here](https://github.com/johnttaylor/pim/blob/master/README-Intro-DataModel.md) 

The examples illustrates the following:
- Decoupling 'drivers' from the Application
- Creating an application specific model point
  - Include customize read-modify-write operation for a model point
- Polled semantics for reading input values for the input driver
- The output driver uses event semantics
- Persistent storage of model points
- Console command to interrogate and modify model points


### Example Application
The example application contains the following modules/classes:

- __InputDriver__.  The Input Driver is representative of an 'input driver' that
samples a physical signal.  The driver generates random value in the range of 1 to 1000.
The Input Driver executes in the _Driver_ thread and generates values at 100Hz.

- __OutputDriver__.  The Output Driver is representative of an 'output driver' that
drives physical output signals. The driver asserts/de-asserts two boolean outputs,  
one signal each for the High and Low Alarms. The Output Driver executes in the _Driver_ 
thread and only changes its output values when the Alarms change state.

- __Algorithm__.  The Algorithm module is representative of processing that is done
on the Application's input and generates outputs per the Application requirements. 
The Algorithm module executes in the _Application_ thread. The Algorithm module does the 
following:

  - Calculates the minimum, maximum, and a running average of its input value.  These
    metrics are stored in the MpMetrics model point.
  - Generates a 'High Alarm' when the current input value goes above the value 
    of 950.  The Alarm is lowered once the current value drops below 50.
  - Generates a 'Low Alarm' when the current input value goes below the value 
    of 50.  The Alarm is lowered once the current value goes above 950.



- __MpMetrics__.  The MpMetrics class defines an application specific model point
(used by the Algorithm module).

- __MetricsRecord__.  The Metrics Record header file defines the persistent storage
record for the Application.  The Algorithm's metrics model point and the boot counter
model points is included in the persistent record.

- __ModelPoints__.  The Model Points module defines all of the modules used by 
the Application.  All of the model points are statically allocated.

- __TShell Console__.  The TShell console (think debug console) from the Cpl 
C++ Class library is included to allow the developer to read and write model 
points at run time.

- __Main__.  The Main module is responsible for constructing all of the modules
and/or classes along with the start-up and shutdown logic.

### Model Points
| Model Point | Type | Description |
|-------------|:----:|-------------|
| `mp::signalIn`    | `Cpl::Dm::Mp::Uint32`     | Input Signal.  Range is 1 to 1000|
| `mp::metrics`     | `MpMetrics`                | Algorithm generated metrics for the input signal |
| `mp::bootCounter` | `Cpl::Dm::Mp::Uint32`    | Number of the times the application has been run |
| `mp::hiAlarm`     | `MpAlarm`       | High Alarm.  When the alarm is _asserted_ the model point value is __valid__.  When the alarm is _de-asserted_ the model point is __invalid__. | 
| `mp::loAlarm`     | `Cpl::Dm::Mp::Bool`       | Low Alarm.  The low alarm has the same semantics/value range as the High Alarm| 

