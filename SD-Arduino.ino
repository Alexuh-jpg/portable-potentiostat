#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

const char* ssid = SECRET_SSID;     // network SSID (name)
const char* password = SECRET_PASS; // network password

int status = WL_IDLE_STATUS;

// Define pin constants
const int pwmPin = 3; // Pin for PWM output
const int analogPin = A0; // Analog pin for current measurement

// Define differential pulse voltammetry parameters
const float startPotential = 0.0; // Starting potential in volts
const float endPotential = 1.0;   // Ending potential in volts
const float stepPotential = 0.05; // Step potential in volts
const int pulseWidth = 100;       // Pulse width in milliseconds
const int pulseAmplitude = 500;   // Pulse amplitude in mV
const int interval = 500;         // Time interval between steps in milliseconds

// Global variables to keep track of the current potential and time
float currentPotential = startPotential;

void setup() {
  // Initialize serial and wait for the port to open:
  Serial.begin(9600);
  pinMode(pwmPin, OUTPUT);
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
  if (currentPotential <= endPotential) {
    applyPotential(currentPotential); // Apply the base potential
    delay(interval-pulseWidth); // Wait for the interval
    applyPulse(currentPotential, pulseAmplitude); // Apply the pulse
    delay(pulseWidth); // Wait for the interval
    currentPotential += stepPotential; // Increment potential for the next step

  } else {
    // Reset the potential for the next cycle
    currentPotential = startPotential;
  }
  
}

void applyPotential(float potential) {
  int pwmValue = map(potential * 1000, 0, 1500, 0, 255);
  analogWrite(pwmPin, pwmValue); // Set PWM output to the desired potential
  //Serial.println("Potential:" + String(potential));
}

void applyPulse(float potential, int amplitude) {
  // Increase the potential by the amplitude for the pulse
  potential += amplitude / 1000.0;
  int pwmValue = map(potential * 1000, 0, 1500, 0, 255);//millivolts
  analogWrite(pwmPin, pwmValue); // Apply the pulse potential
  //Serial.println("Pulse:" + String(potential));

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
