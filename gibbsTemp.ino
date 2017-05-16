

#include <dht11.h>  // Change this to dht.h if needed
//#include <dht.h>
DHT11 DHT11;   // Change second term to DHT if needed

#define DHT11PIN 2  // DHT11 attach to digital pin 2 on controller board
#define RELAYPIN 5  // Use digital pin 5 for relay signal

#define MAXTEMP 25  // In the future, send warnings if these are reached.
#define MINTEMP 20


// The temperature is an int type
int ONTEMP = 21;   // Turn relay on if we get to 21C
int OFFTEMP = 24;  // Turn relay off if we get to 24C

boolean relay_state = false;  // false=off, true=on

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


void setup() {
    Serial.begin(9600);

    Serial.print("On threshold is:\t");
    Serial.println(ONTEMP);
    Serial.print("Off threshold is:\t");
    Serial.println(OFFTEMP);

    // Set the relay pin as an OUTPUT pin
    pinMode(RELAYPIN, OUTPUT);
    // Make sure it's off for safety
    relay_off();
}


void loop() {
  
    // READ DATA
    Serial.print("DHT11,\t");
    //read the value returned from sensor
    int chk = DHT11.read(DHT11PIN);
    switch (chk)
    {
      case DHTLIB_OK:    // Only do interesting stuff in here
  		  Serial.print("OK \t"); 
        // DISPLAY DATA
        
        Serial.print("Tem: ");         
        Serial.print(DHT11.temperature); //print the temperature on serial monitor
        Serial.print(" C\t");
        Serial.print("Hum: ");
        Serial.print(DHT11.humidity); //print the humidity on serial monitor
        Serial.println(" %"); 

        if (DHT11.temperature >= MAXTEMP) {
          relay_off();
          send_warning(MAXTEMP);
        } else if (DHT11.temperature >= OFFTEMP) {
          relay_off();     
        } else if (DHT11.temperature <= ONTEMP) {
          relay_on();          
        } else if (DHT11.temperature <= MINTEMP) {
          relay_on();          
          send_warning(MINTEMP);
        } else if (DHT11.temperature >= ONTEMP && DHT11.temperature <= OFFTEMP) {
          // In the sweet spot, log if we are cooling or heating
          if (relay_state) {
            Serial.println("On a heating cycle"); 
          } else {
            Serial.println("On a cooling cycle"); 
          }
        } else {
          Serial.println("Should not reach this statement.");
          relay_off();
        }
  		  break;

      // These next three cases are bad sensor data/readings  
      case DHTLIB_ERROR_CHECKSUM: 
  		  Serial.print("Checksum error,\t"); 
  		  break;
      case DHTLIB_ERROR_TIMEOUT: 
  		  Serial.print("WTF Time out error,\t"); 
  		  break;
      default: 
  		  Serial.print("Unknown error,\t"); 
  		  break;
    }
    Serial.println();

   delay(5000); //wait a while 

}
