#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

// Network credentials from arduino_secrets.h
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

WiFiServer server(80);

const int pwmPin = 3; // Pin for PWM output
const int analogPin = A0; // Analog pin for current measurement

const float startPotential = 0.0; // Starting potential in volts
const float endPotential = 1.0;   // Ending potential in volts
const float stepPotential = 0.10; // Step potential in volts
const int pulseWidth = 100;       // Pulse width in milliseconds
const int pulseAmplitude = 500;   // Pulse amplitude in mV
const int interval = 500;         // Time interval between steps in milliseconds

float currentPotential = startPotential;
const float feedbackResistor = 100; // Feedback resistor value for current measurement

void setup() {
  Serial.begin(9600);
  pinMode(pwmPin, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    WiFi.begin(ssid, password);
    delay(5000);
  }
  
  server.begin();
  Serial.println("WiFi connected");
  Serial.print("Server started, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) { // If a new client connects,
    Serial.println("New Client."); // Print a message out in the serial port
    String currentLine = ""; // Make a String to hold incoming data from the client
    while (client.connected()) { // Loop while the client's connected
      if (client.available()) { // If there's bytes to read from the client,
        char c = client.read(); // Read a byte, then
        Serial.write(c); // Print it out the serial monitor
        if (c == '\n') { // If the byte is a newline character

          // If the current line is blank, you got two newline characters in a row.
          // That's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            generatePulses(client); // Generate pulses and send data
            break;
          } else { // If you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') { // If you got anything else but a carriage return character,
          currentLine += c; // Add it to the end of the currentLine
        }
      }
    }
    // Close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void generatePulses(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.print("{\"status\":\"DPV Started\",\"data\":[");

    bool firstReading = true;
    while (currentPotential <= endPotential) {
        applyPotential(currentPotential);
        delay(10); // Stabilization delay
        float baselineCurrent = measureCurrent();

        applyPulse(currentPotential, pulseAmplitude);
        delay(pulseWidth); // Ensure measurement at peak of pulse
        float peakCurrent = measureCurrent();

        float differentialCurrent = peakCurrent - baselineCurrent;

        if (!firstReading) {
            client.print(",");
        }
        client.print("{\"potential\":");
        client.print(currentPotential, 4);
        client.print(",\"differentialCurrent\":");
        client.print(differentialCurrent, 6);
        client.print("}");

        currentPotential += stepPotential; // Increment after measurement
        firstReading = false;
    }
    currentPotential = startPotential; // Reset potential for next cycle
    applyPotential(currentPotential); // Reset to base potential

    client.println("]}");
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

float measureCurrent() {
  int analogValue = analogRead(analogPin);
  float voltage = (analogValue * 3.3) / 1023.0; // Convert to voltage assuming a reference voltage of 3.3V
  float current = voltage / feedbackResistor; // Convert voltage to current
  return current; // Return the current in amperes
}
