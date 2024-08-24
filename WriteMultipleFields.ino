#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h" 
#include "DHT.h"

// WiFi connection info
char ssid[] = "AkshitMittal";   
char pass[] = "akshitmittal";   
int keyIndex = 0;            
WiFiClient client;

// ThingSpeak info
unsigned long myChannelNumber = 2625180;
const char* myWriteAPIKey = "3EKN0280U9XKL6T1";

// DHT sensor info
#define DHTPIN 2     
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

// Initialize our values
float h = 0;  // Humidity
float t = 0;  // Temperature (Celsius)
float hic = 0;  // Heat index (Celsius)
String myStatus = "";

void setup() {
  Serial.begin(115200);      
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  ThingSpeak.begin(client);  
  dht.begin(); // Initialize DHT sensor
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // Read sensor data
  h = dht.readHumidity();
  t = dht.readTemperature();
  hic = dht.computeHeatIndex(t, h, false);

  // Check if any reads failed
  if (isnan(h) || isnan(t) || isnan(hic)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // set the fields with the values
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3, hic);

  // set the status message
  myStatus = "DHT sensor data update";

  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel 
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  delay(60000); // Wait 60 seconds to update the channel again
}
