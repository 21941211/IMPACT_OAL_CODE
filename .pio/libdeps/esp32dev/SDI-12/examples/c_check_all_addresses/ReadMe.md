# Example C: Check all Addresses for Active Sensors and Print Status<!-- {#example_c_page} -->

This is a simple demonstration of the SDI-12 library for Arduino.

It discovers the address of all sensors active on any pin on your board.

Each sensor should have a unique address already - if not, multiple sensors may respond simultaneously to the same request and the output will not be readable by the Arduino.

To address a sensor, please see Example B: b_address_change.ino

[//]: # ( @section c_check_all_addresses_pio PlatformIO Configuration )

[//]: # ( @include{lineno} c_check_all_addresses/platformio.ini )

[//]: # ( @section c_check_all_addresses_code The Complete Example )

[//]: # ( @include{lineno} c_check_all_addresses/c_check_all_addresses.ino )
