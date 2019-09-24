include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include <SPI.h>
#include "SparkFun_BNO080_Arduino_Library.h"
#include <ArduinoJson.h>
#include <SparkFun_ADS1015_Arduino_Library.h>

// ---- Constants ----

// ---- FLEXSENSOR ----

ADS1015 indexSensor;
float hand[4] = {0, 0};
//Calibration Array
uint16_t handCalibration[2][2] = {
  //{low, hi} switch these to reverse which end is 1 and which is 0
  {722, 1080}, //index
  {600, 980}  //middle
};

// ---- WLAN ----

// Define WLAN
const char* ssid = "alpaca-old";
const char* password = "testingesp32";

// ---- Define time ----
#define NTP_OFFSET   -7 * 60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "us.pool.ntp.org"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

// Define BNO080
BNO080 myIMU;

// Define Websockets
WebSocketsServer webSocket = WebSocketsServer(80);

// WebSocket message received
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  // Switch between WebSocket event types
  switch (type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
      }
      break;
    /*
      // Echo text message back to client
      case WStype_TEXT:
      Serial.printf("[%u] Text: %s\n", num, payload);
      webSocket.sendTXT(num, payload);
      break;
    */
    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void setup() {

  // Start Serial port
  Serial.begin(115200);

  // Connect to access point
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED )
  {
    delay(5500);
    Serial.print(".");
  }

  // Print our IP address
  Serial.println("Connected!");
  Serial.print("My IP address: ");
  Serial.println(WiFi.localIP());

  // Start time client
  timeClient.begin();

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  // Start wire
  Wire.begin();
  Wire.setClock(400000); //Increase I2C data rate to 400kHz

  // Start IMU
  myIMU.begin();
  myIMU.enableRotationVector(50); //Send data update every 50ms

  // Start Stepcount
  myIMU.enableStepCounter(500);

  // End 115200 serial, start 9600 baud
  Serial.end ();
  Serial.begin (9600);

  // Start flexSensor
  if (indexSensor.begin(ADS1015_ADDRESS_GND) == false)
  {
    Serial.println("Index not found. Check wiring.");
    while (1);
  }

  indexSensor.setGain(ADS1015_CONFIG_PGA_TWOTHIRDS); // Gain of 2/3 to works well with flex glove board voltage swings (default is gain of 2)

  //Set the calibration values for the hand.
  for (int channel; channel < 2; channel++)
  {
    for (int hiLo = 0; hiLo < 2; hiLo++)
    {
      indexSensor.setCalibration(channel, hiLo, handCalibration[channel][hiLo]);
    }
  }
}

void loop() {

  // Update time client
  timeClient.update();

  // Start websocket loop
  webSocket.loop();

  // Prepare flex for json
  uint16_t data;
  for (int channel = 0; channel < 2; channel++)
  {
    //Keep in mind that getScaledAnalogData returns a float
    hand[channel] = indexSensor.getScaledAnalogData(channel);
    data = indexSensor.getAnalogData(channel);
  }

  // Prepare time for json
  String time;
  time = timeClient.getFormattedDate();

  // Read quaternions from IMU
  if (myIMU.dataAvailable() == true)
  {
    float qReal = myIMU.getQuatReal();
    float qI = myIMU.getQuatI();
    float qJ = myIMU.getQuatJ();
    float qK = myIMU.getQuatK();
    float accu = myIMU.getQuatRadianAccuracy();
    float steps = myIMU.getStepCount();

    // Serial print quaternions
    //Serial.print(q0, 2);
    //Serial.print(F(","));
    //Serial.print(q1, 2);
    //Serial.print(F(","));
    //Serial.print(q2, 2);
    //Serial.print(F(","));
    //Serial.print(q3, 2);
    //Serial.println();
    //delay(1000);

    // Serialize quaternions to json with arduinojson
    const size_t capacity = JSON_OBJECT_SIZE(1);
    DynamicJsonDocument doc(122);
    doc["q0"] = (qReal);
    doc["q1"] = (qI);
    doc["q2"] = (qJ);
    doc["q3"] = (qK);
    doc["accu"] = (accu);
    doc["steps"] = (steps);
    doc["data"] = (data);
    doc["time"] = (time);
    serializeJson(doc, Serial);
    char json_string[128];
    serializeJson(doc, json_string);

    // Broadcast json via websocket to client
    char payload[128];
    snprintf_P(payload, sizeof(payload), json_string);
    webSocket.broadcastTXT(payload, strlen(payload));

  }
}
