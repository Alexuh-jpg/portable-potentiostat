#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

const char* ssid = SECRET_SSID;     // network SSID (name)
const char* password = SECRET_PASS; // network password

int status = WL_IDLE_STATUS;

void setup() {
  // Initialize serial and wait for the port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  // Check for the presence of the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // Don't continue further if the module is not present
    while (true);
  }

  // Attempt to connect to the WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, password);
    delay(10000); // Wait 10 seconds between retries
  }

  Serial.println("You're connected to the network");
  printWiFiStatus();
}

void loop() {
  // add other logic here
}

void printWiFiStatus() {
  // Print the local IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Print the SSID of the network you're connected to:
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());

  // Print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
