# Paul's PCB Hot Air Pre-Heater
This project is all about creating a '**full featured**' PCB Heater. This heater provides a temperature controlled environment using hot air to warm PCB boards.
Most other PCB Heaters (within a reasonable price bracket) are either Infrared, not well controlled or provide no feedback/indication of actual board heat status. 

**Three** temperature monitors provide all round real time protection and tracking.

- **An Ambient monitor** for tracking the internal temperature or the preheater  
The *'Ambient'* monitor is for preheater protection and will cause a shutdown if a preset threshold is exceeded.
- **A Target temperature probe** for the top of the PCB (known as the 'Target' temperature) and  
The *'Target'* temperature is the temperature that is desired.  
- **A Guard temperature probe** for the bottom of the PCB (known as the 'Guard' temperature)  
The *'Guard'* temperature, is to prvent over heating or **'shock'** heating of the under side of the PCB board.  

The following features are provided:
- Precise temperature control
- Selectable Temperature Profiles
- Manual Temperature settings
- Heater enable/Disable
- LED Indicatior status profiles
- Rotary menu selection / manual temperature setting
- TFT Graphing of measured temperatures
- Serial streaming of Temperature readings for off-heater plotting and/or recording
- **Fuzzy Logic** / AI based Temperature control engine

## Implementation Details
### Hardware
The underlying micro-controller is a standard Arduino Uno R3. This is coupled with a heating element from a standard hot air gun via a SOlid State Relay. AC Zero crossing detection is also provided in order to minimize EMI. Schematics may be found in the *'/Hardware'* folder along with a functional block diagram [Hardware Block Diagram](doc/PCBPreHeaterHW.pdf) in the *'/doc'* folder.

A set of photos are available in the **'Photos'** folder showing the completed construction.

### Software
The PCB Heater organized as a cooperating set of functional areas. The 
[Software Architecture](doc/PaulsPCBHeaterV2.pdf) PDF provides a diagrammatic overview of the architecture

This preheater has a non-pre-emptive operating system at it's core. This is a 'Real' OS, but only a small fraction of the OS features are utilized due to memory and codespace limitations.
The architecture is organized with two tasks (a forground/UI task and a background heater task). The Background task is further subdivided by a number of threads that invoke the various background heating and control elements. It was originally planned to have multiple discreet background tasks, but memory constraints dictated that individual threads within the single background heater task were used instead 

### Source Code Organization
**/src/pos** contains operating system source files  
**/src** contains application class and header source files  

**src/PaulsPCBHeater.ino** is the Entry point and contains *'setup()'* and *'loop()'*

