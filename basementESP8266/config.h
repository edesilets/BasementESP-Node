// config.h
// if your key is a string it needs to be in quotes. Note no semicolon after a hash define

// WIFI
// There is no need for specify encryption.
#define WLAN_SSID       "Name-Of-Router"
#define WLAN_PASS       "password"

// MQTT Setup
#define SERVER          "brokerName"
#define USERNAME        "brokerUsername" // Leave Blank if no username
#define PASSWORD        "brokerPassword" // Leave Blank if no password

// DHT (sensor)
#define DHTPIN 5        // GPIO pin

// Uncomment whatever type you're using!
// #define DHTTYPE DHT11   // DHT 11
// #define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
