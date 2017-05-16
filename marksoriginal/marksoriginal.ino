
/* DHTClient - ESP8266 client with a DHT sensor as an input
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <dht11.h>  // Change this to dht.h if needed
//#include <dht.h>
//dht11 dht;   // Change second term to DHT if needed

#define DHT11PIN 0  // DHT11 attach to digital pin 2 on controller board
#define RELAYPIN 2  // Use digital pin 5 for relay signal

#define MAXTEMP 25  // In the future, send warnings if these are reached.
#define MINTEMP 20

const char* ssid     = "BELL769";
const char* password = "66EE3265";
// The temperature is an int type
int ONTEMP = 21;   // Turn relay on if we get to 21C
int OFFTEMP = 24;  // Turn relay off if we get to 24C

boolean relay_state = false;  // false=off, true=on

WiFiClient client;
byte server[] = { 
  192, 168, 2, 22 }; // http server - PUT HERE YOUR SERVER IP as bytes
String serverStr = "192.168.2.22"; // http server - PUT HERE YOUR SERVER IP as string

// Initialize DHT sensor - adafruit note
// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01
//DHT dht(DHTPIN, DHTTYPE, 15); // 11 works fine for ESP8266

float humidity, temperature;  // Values read from sensor
String webString = "";   // String to display
// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor

// Required for LIGHT_SLEEP_T delay mode
extern "C" {
#include "user_interface.h"
}


// Use functions for the relay in case 
// we need to expand their code
void relay_on() {
  digitalWrite(RELAYPIN, HIGH);  // turn relay on
  relay_state = true;
}

void relay_off() {
  digitalWrite(RELAYPIN, LOW);  // turn relay off
  relay_state = false;
}

void send_warning(int threshold) {
  Serial.print("DHT11 has reached the value: ");
  Serial.println(threshold);
}
void setup(void)
{  
  // You can open the Arduino IDE Serial Monitor window to see what the code is doing
  Serial.begin(9600);  // Serial connection from ESP-01 via 3.3v console cable
  Serial.println("\n\r \n\rWorking to connect");
  for (uint8_t t = 4; t > 0; t--) {
    Serial.print("[SETUP] WAIT ");
    Serial.println(t);
    delay(1000);
    Serial.print("On threshold is:\t");
    Serial.println(ONTEMP);
    Serial.print("Off threshold is:\t");
    Serial.println(OFFTEMP);
     // Set the relay pin as an OUTPUT pin
    pinMode(RELAYPIN, OUTPUT);
    // Make sure it's off for safety
    relay_off();
  }

  //DHT11.begin();           // initialize temperature sensor
  Serial.println("\n\r \n\rDHT done");
  gettemperature();
  Serial.println(String(temperature));

  delay(2);
}

void loop(void)
{
  delay(2);
  gettemperature();

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("DHT Weather Reading Client");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println(ESP.getChipId()) ;
  Serial.println("Client started");

  if (client.connect(server, 80)) {  // http server is running on default port 80
    Serial.println("connected");
    client.print("GET /SensorWriteToFile.php?temp=");  // PUT HERE YOUR SERVER URL e.g. from http://192.168.1.11/SensorWriteToFile.php?temp=10.5&hum=58

    client.print(String(temperature));
    client.print("&hum=");
    client.print(String(humidity));
    client.println(" HTTP/1.1");
    client.print("Host: ");   // http server is running on port 80, so no port is specified
    client.println(serverStr); 
    client.println("User-Agent: Mihi IoT 01");   // PUT HERE YOUR USER-AGENT that can be used in your php program or Apache configuration
    client.println(); // empty line for apache server

    //Wait up to 10 seconds for server to respond then read response
    int i = 0;
    while ((!client.available()) && (i < 1000)) {
      delay(10);
      i++;
    }

    while (client.available())
    {
      String Line = client.readStringUntil('\r');
      Serial.print(Line);
    }
    client.stop();
  } 
  else {
    Serial.println("connection failed");
  }
  Serial.println();
  Serial.println(WiFi.status());

  //WiFi.disconnect(); // DO NOT DISCONNECT WIFI IF YOU WANT TO LOWER YOUR POWER DURING LIGHT_SLEEP_T DELLAY !
  //Serial.println(WiFi.status());  

  wifi_set_sleep_type(LIGHT_SLEEP_T);

  delay(60000*3-800); // loop every 3 minutes

}

void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor
    previousMillis = currentMillis;

    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    //humidity = DHT11.readHumidity();          // Read humidity (percent)
    //temperature = dht.readTemperature(true);     // Read temperature as Fahrenheit
    //temperature = DHT11.readTemperature();     // Read temperature as *C
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}

