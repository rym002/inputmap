# inputmap
Creates virtual input devices from real ones, SteamController support included.

This program lets you create a virtual input device, by using the Linux `uinput` module, and map to it
any number of buttons or axis from real devices. Currently, it is able to use raw input devices and SteamControllers. 

The main motivation for this program is to be able to use the SteamController as a normal gamepad, but you can find many other uses for it.



## How it works

To create a virtual device you must write a file describing the real input devices and how they map to the virtual device you want to create. This file uses the INI file syntax.
Then you just run:

    $ inputmap configuration.ini

And you are ready to go. You may need to run the program as root, depending on your system configuration.

## Configuration file syntax

The INI configuration file is quite simple. You can only have two types of sections:

 * `[input]`: defines an input physical device.
 * `[output]`: defines an output virtual device.
 
You can write as many sections of any of these as you want.
 
### `[input]` section.
 
There are two mandatory values in this section:
 
  * `ID`: it is the device to be used as input. It can either be a name from `/dev/input/by-id/*` (without the path) to use the input device or the special name `Steam` for a SteamController.
  * `name`: the name to refer to this device from the rest of the file. A single unique word, made of letters numbers and underscore.
  
If `ID` is a `/dev/input/by-id/*` device, then there are these additional optional values:
 
  * `grab`: a boolean value (`Y` / `N`), defaults to `N`. If `Y` then the device is _grabbed_, that is it is prevented from sending input events to the rest of the system.
  
If the `ID` is `Steam` then there are these additional optional values:
 
  * `serial`: Only a SteamController with this serial number will be used for this input device. Useful if you have several of them.
  * `mouse`: a boolean value (`Y` / `N`), defaults to `N`. If `N` then the builtin mouse emulation of the controller will be disabled.
  * `auto_haptic`: a character string `L`, `R` or `LR`, defaults to empty. If it has a `L` then it will enable automatic haptic feedback on the left pad. If it has a `R` then it will do the same on the right pad.
  
### `[output]` section.
  
In this section you will define the virtual device. First you have a few optional values to describe the device:
  
   * `name`: The name of the virtual device, defaults to `InputMap`.
   * `phys`: The name of the _phys_ parameter of the virtual device, defaults to empty.
   * `bus`: The bus that the virtual device reports as using, it can be `VIRTUAL`, `USB`, `BLUETOOTH` or `PCI`, defaults to `VIRTUAL`.
   * `vendor`: An hexadecimal number to be reported as VendorId, defaults to 0. Useful to emulate well known devices.
   * `product`: An hexadecimal number to be reported as ProductId, defaults to 0.
   * `version`: The version of the device, mostly useless. Defaults to 1.
   
Additionally, you map all the buttons and axis of the virtual device and how the physical devices map to them.
 
#### Maps in the `[output]` section
 
To create a virtual button, you just define a value in the `[output]` section with the name of that button.
You can see the available button names in `/usr/include/linux/input-event-codes.h.
 
The value itself describes how the value of that button is obtained. The easiest way is to write the name of an input device, a period, and the name of the physical button.
 
For example, to map the keys 1 and 2 to the left and right mouse buttons:
 
    [input]
    ID=usb-USB_Keyboard-event-kbd
    name=keyb

    [output]
    BTN_LEFT=keyb.KEY_1
    BTN_RIGHT=keyb.KEY_2
     
To create a virtual axis, either relative or absolute, you do the same as with the buttons:

    [input]
    ID=usb-USB_Joystick-event-joystick
    name=joy
    
    [output]
    ABS_X=joy.ABS_Z
    ABS_Y=joy.ABS_RZ
   
You can also build a virtual input from physical inputs of a different kind.
For example, to create a virtual absolute axis from two buttons (useful for the _hat_ axis) you write the two buttons within parentheses separated by comma:

    [output]
    ABS_HAT0X=(keyb.KEY_A,keyb.KEY_D)
    ABS_HAT0Y=(keyb.KEY_S,keyb.KEY_W)
