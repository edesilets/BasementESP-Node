/************** INCLUDE!!! **************/
#include "config.h"
/******** MQTT library Requirements *****/
#include "ESP8266WiFi.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
/******** DHT library Requirements (thermometer, humidity) *****/
#include "DHT.h"
/******** DS18B20 library Requirements *****/
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/************** Define!!!! See config.h **************/
// SEE config.h to set your wifi password.
// SEE config.h to set your wifi ssid.
// SEE config.h to set your MQTT server, username and passwordpassword.

DHT dht(DHTPIN, DHTTYPE); // GPIOpins, sensor type These are defined in config.h.

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
byte ThermometerInlet[] = { 0x28, 0x81, 0x93, 0xDE, 0x06, 0x00, 0x00, 0xFF };
byte ThermometerOutlet[] = { 0x28, 0xBC, 0x97, 0xDE, 0x06, 0x00, 0x00, 0x47 };

/************ Setup MQTT information ******************/
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM   = SERVER;
const char MQTT_USERNAME[] PROGMEM = USERNAME;
const char MQTT_PASSWORD[] PROGMEM = PASSWORD;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, 1883, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/
Adafruit_MQTT_Publish temperatureRoom = Adafruit_MQTT_Publish(&mqtt, "/home/basement/thermostat/temperature/basement");
Adafruit_MQTT_Publish humidityRoom = Adafruit_MQTT_Publish(&mqtt, "/home/basement/thermostat/humidity/basement");
Adafruit_MQTT_Publish temperatureOutletPipe = Adafruit_MQTT_Publish(&mqtt, "/home/basement/boiler/temperature/outletpipe");
Adafruit_MQTT_Publish temperatureInletPipe = Adafruit_MQTT_Publish(&mqtt, "/home/basement/boiler/temperature/inletpipe");
/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(BUILTIN_LED, OUTPUT); // ON Board LED
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    digitalWrite(BUILTIN_LED, LOW);
  }

  digitalWrite(BUILTIN_LED, HIGH);
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  dht.begin();
  /********************** Tempature ******************************/
  sensors.begin();
  sensors.setResolution(ThermometerInlet, TEMPERATURE_PRECISION);
  sensors.setResolution(ThermometerOutlet, TEMPERATURE_PRECISION);
}

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  // ping the server to keep the mqtt connection alive
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

  float roomHumidity = dht.readHumidity();
  float roomTemperature = dht.readTemperature(true);

  // These If statements are a hack. Something weird is happening with the dht
  // and the DS18B20. This makes sure that data is returned insted of nan.
  if (!isnan(roomHumidity)) {
    humidityRoom.publish(roomHumidity);
  }

  if (!isnan(roomTemperature)) {
    temperatureRoom.publish(roomTemperature);
  }

  sensors.requestTemperatures();
  temperatureInletPipe.publish(printTemperature(ThermometerInlet));
  temperatureOutletPipe.publish(printTemperature(ThermometerOutlet));
}

//function to print the temperature for a device
float printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  float tempF = DallasTemperature::toFahrenheit(tempC);
  return tempF;
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 2 seconds...");
       mqtt.disconnect();
       delay(2000);  // wait 2 seconds
  }
  Serial.println("MQTT Connected!");
}
