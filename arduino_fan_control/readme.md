=== FAN Control

I found the basic project at http://www.electroschematics.com/9540/arduino-fan-speed-controlled-temperature/ and
build the hardware three times for three separate controlled fans with thermo sensors.

This is the schematics from the basic project

IMAGE docu/arduino-temperature-fan-speed-control.png

Thanks to "Electro Schematics" for the project!

I removed the LCD display and extended the software using serial communication
to have more control of the fan device.

If you connect a USB serial to the device you can send commands and settings to the controller.

Fan speed value is between 0 and 255. Values lesser then 30 will fans cause not to run correctly because of
the low voltage. To avoid it set fanAdd setting.

If you want the fan also to run for low themperatures set fanSleep.

The alert led will blink for every cycle. If alerting the led will be permanently on.


Settings consists of key=value everything else are commands. Send a enter or semikolon as line separator.

Commands:

status - Print status of all thermo sensors and fans, alert count and test alert counter
config - Print the full configuration
alerts.clean - Cleanup alert counter
uptime - Print human readable uptime
reset - Restart the device
config.save - Save the settings into the eeprom
config.load - Load the settings from eeprom (also done on startup)
config.clean - Delete the settings from eeprom, after restart the default settings are set

Settings:

tempMul:Float - Set multiplicator for thermo sensor, default is 0.48828125
tempAdd:Int - Set value to add to thermo sensor value

tempMin:Int - Minimum temperature value, if temp is lesser, fan will set to fanSleep value
tempMax:Int - Maximum temperature value, it this is reached all fans will drive at maximum and alert state is set

fanAdd:Int - Add this value to the fan speed if not sleeping
fanSleep:Int - Fan speed if sleeping (default 0)

testWarn:Int - Set number of cycles in alert mode, the value will decrased every cycle. If it reached zero it's going back to normal
monitor:Boolean - Switch on monitoring output. This will print the status for every cycle

active0:Boolean - Enable/Disable fan0 (default true)
active1:Boolean - Enable/Disable fan1 (default true)
active2:Boolean - Enable/Disable fan2 (default true)

delay:Int - Set delay time between cycles in milliseconds

synchronise:Boolean - Enable to tun all fans with the same (maximum of all) speed

