# Sonoff (Arduino)

![alt](https://raw.githubusercontent.com/tretyakovsa/Sonoff_WiFi_switch/master/tutorial/screen.png)



##Upload instruction

###[NodeMCU Flasher (Easy)](https://github.com/tretyakovsa/Sonoff_WiFi_switch/tree/master/build)

###[Arduino Windows (Video)](https://www.youtube.com/watch?v=jMK9mySGHio)

###[Arduino Linux (Video)](https://www.youtube.com/watch?v=1TAHlRqZ46k)

####Used Libraries
- Arduino core for ESP8266 WiFi chip https://github.com/esp8266/Arduino
- Arduino ESP8266 filesystem uploader https://github.com/esp8266/arduino-esp8266fs-plugin
- C++ JSON library for IoT. Simple and efficient. https://github.com/bblanchon/ArduinoJson
- Efficient DHT library for Arduino https://github.com/markruys/arduino-DHT

##Work example (Video)

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/NrIrLw1rOdk/0.jpg)](https://www.youtube.com/watch?v=NrIrLw1rOdk&list=PL6NJTNxbvy-IPTDQk8XjTV41oRrFafrRi)

##Construction
```
#define TACH_PIN 0    // Кнопка управления
#define RELE1_PIN 12  // Реле
#define LED_PIN 13    // Светодиод
#define DHTPIN 14     // Pin which is connected to the DHT sensor.
#define PIR_PIN 2     // RIR sensors
// PIN ADC (A0)       // URL: /analog.json or /chart.htm#analog
```
###Schematic

![alt](https://raw.githubusercontent.com/tretyakovsa/Sonoff_WiFi_switch/master/tutorial/sonoff.jpg)

##Othe...

####Jalousie: [https://github.com/tretyakovsa/jaluzi](https://github.com/tretyakovsa/jaluzi)

####RGB: [https://github.com/renat2985/rgb](https://github.com/renat2985/rgb)


####Themes you can download here: [http://bootswatch.com/](http://bootswatch.com/)


##Donation

If you like this project, you can give me a cup of coffee :)

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=W4PURUNKWMRJW&lc=AU&item_name=esp8266&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donate_SM%2egif%3aNonHosted)