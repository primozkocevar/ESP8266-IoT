Different versions:

Version V2 has parameters packed in a name=value format so it can be 
seen right from the request what we are sending. 

Version DEV doesnt send time by the esp8266 but the server sets the time.
IT IS NOT AS GOOD.

Version RTC sends the time with the data set by the RTC connected to the board via I2C. 
NEEDS THE RTC MODULE CONNECTED!

Version TP sends the time which is set internally by the library TIME each time we start
the script. We get the time via the written protocol from our server.
DONT NEED THE RTC MODULE FOR TIME!

Helpful links:
Full documentation esp8266 on arduino ide, very helpfull:
http://esp8266.github.io/Arduino/versions/2.1.0/doc/installing.html

A bit about NodeMcu:
http://www.esp8266.com/wiki/doku.php?id=nodemcu

A few helpfull links:
Comparison of the versions.
http://www.esp8266.com/wiki/doku.php?id=esp8266-module-family
http://frightanic.com/iot/comparison-of-esp8266-nodemcu-development-boards/

bmp180 on nodemcu:
https://github.com/adafruit/Adafruit-BMP085-Library

