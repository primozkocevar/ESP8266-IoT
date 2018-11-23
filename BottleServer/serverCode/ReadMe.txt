Different versions:

Version V2 has parameters packed in a name=value format so it can be 
seen right from the request what we are sending. 

Version DEV doesnt send time by the esp8266 but the server sets the time.
IT IS NOT AS GOOD.

Version RTC sends the time with the data set by the RTC connected to the board via I2C. 
NEEDS THE RTC MODULE CONNECTED! It doesnt need to send the time. Only change from the V2 
is that now esp8266 sends the time of the measurements and not the server.

Version TP sends the time which is set internally by the library TIME each time we start
the script. We get the time via the written protocol from our server.
DONT NEED THE RTC MODULE FOR TIME!
