# nkshoe</br>
Showing position and flex of a shoe wireless in realtime on the web using:
</br>
</br>
-Hardware:</br>
--- esp32 (https://www.sparkfun.com/products/14689)</br>
--- IMU (https://www.sparkfun.com/products/14686)</br>
--- flexsensor (https://www.sparkfun.com/products/14666)</br>
</br>
-Software:</br>
--ESP32:</br>
--- Arduino IDE (https://www.arduino.cc/en/main/software)</br>
--- Websockets (https://github.com/Links2004/arduinoWebSockets)</br>
--- ArduinoJson (https://arduinojson.org/?utm_source=meta&utm_medium=library.properties)</br>
--- SparkFun ADS1015 Arduino Library (https://github.com/sparkfun/SparkFun_ADS1015_Arduino_Library)</br>
--- SparkFun BNO080 Arduino Library (https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library)</br>
--- NTPClient (https://github.com/arduino-libraries/NTPClient)</br>
</br>
--Web:</br>
--- html/css</br>
--- Three JS (https://www.threejs.org)</br>
--- Moment JS (https://momentjs.com/)</br>
--- Chart JS (https://www.chartjs.org/)</br>
--- Chart Streaming Plugin (https://nagix.github.io/chartjs-plugin-streaming/)</br>
</br>
</br>
1.) Install Arduino IDE and follow the instructions to setup the esp32 feather board:
https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/using-with-arduino-ide
</br>
2.) Install all the required libraries from above (Software/ESP32) 
</br>
3.) Change the Wifi Credentials to your home wifi.
</br>
4.) Change the board definitions in Arduino IDE to adafruit esp32 feather and upload the sketch. (baud speed 921600/80/none)
</br>
5.) Open Google Chrome or Mozilla Firefox with CORS disabled. (See: https://medium.com/@siddhartha.ng/disable-cross-origin-on-chrome-for-localhost-c644b131db19)
</br>
6.) Make sure you and the shoe are in the same Wifi network. 
</br>
7.) Open esp32shoe.html 
</br>
8.) Check  developertools/network/ws for websocket connections.
