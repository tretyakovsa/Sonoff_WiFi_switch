# Sonoff (Arduino)

![alt](https://raw.githubusercontent.com/tretyakovsa/Sonoff_WiFi_switch/master/tutorial/screen.png)



## Upload instruction

### [NodeMCU Flasher (Easy)](https://github.com/tretyakovsa/Sonoff_WiFi_switch/tree/master/build)

### [Arduino Windows (Video)](https://www.youtube.com/watch?v=jMK9mySGHio)

### [Arduino Linux (Video)](https://www.youtube.com/watch?v=1TAHlRqZ46k)

#### Used Libraries
 - ESP8266WiFi.h
 - ESP8266WebServer.h
 - ESP8266SSDP.h
 - FS.h
 - time.h
 - Ticker.h
 - WiFiUdp.h
 - ESP8266HTTPUpdateServer.h
 - ESP8266httpUpdate.h
 - ESP8266HTTPClient.h
 - DNSServer.h
 - StringCommand.h           https://github.com/tretyakovsa/ESP8266-StringCommand
 - ArduinoJson.h             Ставим через менеджер библиотек
 - DHT.h                     https://github.com/markruys/arduino-DHT
 - OneWire.h                 Ставим через менеджер библиотек
 - DallasTemperature.h       Ставим через менеджер библиотек
 - PubSubClient.h            https://github.com/Imroy/pubsubclient
 - Adafruit_NeoPixel.h       https://github.com/adafruit/Adafruit_NeoPixel
 - WS2812FX.h                https://github.com/kitesurfer1404/WS2812FX

## Work example (Video)

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/NrIrLw1rOdk/0.jpg)](https://www.youtube.com/watch?v=NrIrLw1rOdk&list=PL6NJTNxbvy-IPTDQk8XjTV41oRrFafrRi)

## [Construction](https://github.com/tretyakovsa/Sonoff_WiFi_switch/blob/master/Sonoff_WiFi_switch.ino#L50-L78)

```
  #define TACH_PIN 0    // Кнопка управления
  #define PIR_PIN 2     // RIR sensors
  #define RELE1_PIN 12  // Реле
  #define LED_PIN 13    // Светодиод
  #define DHTPIN 14     // DHT сенсор.
  #define RGB_PIN 1     // WS2811/WS2812/NeoPixel LEDs
  #define impuls_PIN 3  //электросчетчик
```

### Schematic

![alt](https://raw.githubusercontent.com/tretyakovsa/Sonoff_WiFi_switch/master/tutorial/sonoff.jpg)

## Othe...

#### Jalousie: [https://github.com/tretyakovsa/jaluzi](https://github.com/tretyakovsa/jaluzi)

#### RGB: [https://github.com/renat2985/rgb](https://github.com/renat2985/rgb)


#### Themes you can download here: [http://bootswatch.com/](http://bootswatch.com/)


## Donation

If you like this project, you can give me a cup of coffee :)

### PayPal
[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=W4PURUNKWMRJW&lc=AU&item_name=esp8266&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donate_SM%2egif%3aNonHosted)

### WebMoney
- USD Z242847016348
- RUS R466173026928
- EUR E555861976545

