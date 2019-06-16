# 433RC_TX

This program sends  bits to a simple 433MHz OOK ASK transmitter module
Used to replace a AC114-01B remote control
Button UP / STOP  /  DOWN  only
bits for each command were scanned through RTL-SDR dongle + Universal Radio Hacker

2 versions : ESP8266 and Raspberry pi

1 - ESP8266
ESP8266 transmits bits to 433MHz TX module
WEB interface &amp; API for remote control
Control via simple WEB interface or directly   http://ESP_IP/up  or /stop or /down

inital Wifi connect throught normal AP for ESP8266.
Reset all WIFI settings by 3 seconds press on Flash button

2 - Raspberry Pi
Compile the code to get a simple command :
Syntax : 
433RC_TX up/down/stop [GPIO_PIN]
GPIO Pin is optionnal,   Default GPIO Pin is 0  (GPIO17) see wiringPi library for pinout

Exemple : 
433RC_TX up  :  sends the data "up" on pin 0
433RC_TX up 2  :  sends the data "up" on pin 2


phfu  -  June 2019  
