#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

// Network credentials
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

// Initialize the WiFi server on port 80
WiFiServer server(80);

// Define pin constants
const int pwmPin = 3;  // Pin for PWM output
const int analogPin = A0;  // Analog pin for current measurement

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
  Serial.begin(9600);
  pinMode(pwmPin, OUTPUT);
  
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected.");
  server.begin();
  Serial.print("Server started, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        
        if (c == '\n' && currentLineIsBlank) {
          // Send a standard HTTP response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          // Webpage content
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head><title>Arduino DPV Controller</title></head>");
          client.println("<body>");
          client.println("<h1>Differential Pulse Voltammetry Started</h1>");
          client.println("<p>Generating pulses...</p>");
          client.println("</body></html>");

          generatePulses();
          client.stop();
          Serial.println("Client disconnected.");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
  }
}

void generatePulses() {
  while (currentPotential <= endPotential) {
    applyPotential(currentPotential);
    delay(interval - pulseWidth);
    applyPulse(currentPotential, pulseAmplitude);
    delay(pulseWidth);
    currentPotential += stepPotential;

    // Measure current at the peak of each pulse
    //float current = measureCurrent();
    //Serial.print("Potential: "); Serial.print(currentPotential); Serial.print("V, ");
    //Serial.print("Current: "); Serial.println(current); Serial.println("A");
  }
  currentPotential = startPotential;  // Reset the potential for the next cycle
  applyPotential(currentPotential);
}

void applyPotential(float potential) {
  int pwmValue = map(potential * 1000, 0, 1500, 0, 255);
  analogWrite(pwmPin, pwmValue);
}

void applyPulse(float potential, int amplitude) {
  potential += amplitude / 1000.0;
  int pwmValue = map(potential * 1000, 0, 1500, 0, 255);
  analogWrite(pwmPin, pwmValue);
}

// float measureCurrent() {
//   int analogValue = analogRead(analogPin);
//   float voltage = (analogValue * 3.3) / 1023.0;
//   float current = voltage / feedbackResistor;
//   return current;
// }
