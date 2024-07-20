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

## Development Tools**
- Eclipse IDE with **Sloeber** Arduino support
- KiCad V7 for Schematic entry
- VeeCad for Stripboard layout
- Emulare Arduino Simulator [Emulare](https://emulare.sourceforge.net/)
- DWLink Arduino [Debugger](https://github.com/felias-fogg/dw-link/blob/master/readme.md)

This should build using the standard Arduino IDE, but it is not something I've tested.

## Implementation Details
### Hardware
The underlying micro-controller is a standard Arduino Uno R3. This is coupled with a heating element from a standard hot air gun via a SOlid State Relay. AC Zero crossing detection is also provided in order to minimize EMI. Schematics may be found in the *'/Hardware'* folder along with a functional block diagram [Hardware Block Diagram](doc/PCBPreHeaterHW.pdf) in the *'/doc'* folder.
![Hardware Block Diagram](doc/PCBPreHeaterHW.pdf)

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

A large number of capabilities (such as menu management and Rotary Selection etc) have been designed and implemented specifically for this project to be as memory efficient as possible. Various libraries exist around the Internet for these types of capabilities, but this design needed tight control over features and resources used and so the libraries were not utilized.

Heavy use of interrupts are made in this project. All external bit oriented inputs (buttons, rotary selection, zero crossing etc) are provided by using bit level interrupts.

# Operation
Operation of the Pre Heater is provided through a set of nested menus with status and feedback provided via the LED and TFT screen.

## Buttons
- Green **Back** Button  
When a menu option has been selected, this green button will cancel and return to the previous menu
- **Reset**  
System level Arduino rest
- **Rotary Push** selection  
Invokes a selected menu option. While in *'Manual Temperature'*, Pushing the rotary select button, will toggle between the target and guard temperature adjustments.
- **Rotary twist**  
Either moves up / down through Menu or for a manual temp selection, it will increase/decrease the temperature.

## LED Indications
The LED flashes to indicate the current status. The following LED flash profiles are provided:
- Self Test -> 3 short flashes at power up or reset.
- Detecting AC Power signal, Heater is *disabled* -> Single LED flash, once per second
- Detecting AC Power signal, Heater is *enabled* -> Double LED flash, once per second
- Over Heating Warning -> Three short LED flashes every 2 seconds
- Over Heated! -> Continuous short flashes and the Heating element will be disabled.

When either the over heating warning or overheated condition is reversed (temperature has been lowered), the condition will fall back to either the AC detection or Heater enabled profile.

## Main menu
- **ScreenReset**  
Debug option to reset TFT. No other operation is affected
-  **Unused RAM**  
Debug option to show unused / spare system RAM
- **Temps**  
Provides continuous high precision temperature readings of all three temperature probes
- **Heater**  
Provides debug information about the internal state of the heater controller
- **Profiles**  
Provides menu to select profile or manual temperature
- **Graph**  
Shows graph of current detected temperatures, along with the target+Guard limits

## Profile Menu
- **ManTemp**  
Manual temperature selection. This will deselect any previously selected pre set profile and allow for discreet temperature adjustment. Use the rotary push select to toggle between *'Target'* and *'Guard'* temperatures
- **ProfileSel**  
Allows for the selection of one of the pre-built profiles
- **Start/Stop**  
Starts or Stops a selected Profile. This is mainly for resetting the graphical display (on top level menu)
- **Restart**  
Restarts a previously started profile graphical display
- **Htr Enb/Dis**  
Enables or Disables the actual heater blower. Once a profile has been selected and started, the heater won't engage untill it is manually enabled as a safety precaution.
