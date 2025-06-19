// turn on 3 commented send JSONs
// turn on connecting to wifi

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Define the IR sensor pins
const int irSensorPins[2] = {17, 16}; // Array of IR obstacle sensor pins

// Define the color sensor pins
const int s0Pins[2] = {5, 18}; // Arrays for S0 pins for each color sensor
const int s1Pins[2] = {4, 19}; // Arrays for S1 pins for each color sensor
const int s2Pins[2] = {21, 22}; // Arrays for S2 pins for each color sensor
const int s3Pins[2] = {27, 32}; // Arrays for S3 pins for each color sensor
const int colorSensorOutPins[2] = {34, 35}; // Arrays for OUT pins for each color sensor]

// Define the NeoPixel ring pins
const int rgbRingPins[2] = {13, 14}; // Array of NeoPixel data pins
#define NUM_LEDS 24  // Number of LEDs per NeoPixel ring
// Create an array of Neopixel objects
Adafruit_NeoPixel rings[] = {
  Adafruit_NeoPixel(NUM_LEDS, rgbRingPins[0], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_LEDS, rgbRingPins[1], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_LEDS, rgbRingPins[2], NEO_GRB + NEO_KHZ800)
};

int detectedLandmines[2] = {0, 0}; // Check if landmine already affected points
int activeLandmine = random(0, 2); // Random number between 1 and 2
String connectedSensorColor = "blue"; // Tubes related to controler
int sensorState = 0; // variable for reading the sensor status
bool objectFound = false;

int pointsToChange = 0; // Definig variables releated to points
String pointsOperation = "";
String pointsTeam = "red";

const char* ssid = "YaelHotspot"; // Connectivity info
const char* password = "0542666480";
const char* serverUrl = "http://192.168.1.27:4000/api/submit";

void turnLandmineLedOn (int activeIndex) {
  for (int i = 0; i < 2; i++) {
    if (i == activeIndex) {
      // Turn ON the selected ring (green color)
      for (int j = 0; j < NUM_LEDS; j++) {
        rings[i].setPixelColor(j, rings[i].Color(0, 255, 0)); // Green
      }
    }
    else {
      // Turn OFF all other rings
      for (int j = 0; j < NUM_LEDS; j++) {
        rings[i].setPixelColor(j, rings[i].Color(0, 0, 0)); // Off
      }
    }
    rings[i].show(); // Update the LEDs
  }
}

void connectWiFi() {
  delay(4000);   //Delay needed before calling the WiFi.begin

  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}

int getColorFrequency (int s2State, int s3State, int i) {
  digitalWrite(s2Pins[i], s2State);
  digitalWrite(s3Pins[i], s3State);
  delay(10); // Short delay for stability
  return pulseIn(colorSensorOutPins[i], LOW);
}

String detectColor (int i) {
  int redFrequency = getColorFrequency(LOW, LOW, i);
  int blueFrequency = getColorFrequency(LOW, HIGH, i);
  int yellowFrequency = getColorFrequency(HIGH, LOW, i);

  // Print the frequencies for debugging
  Serial.print("Red Freq: ");
  Serial.print(redFrequency);
  Serial.print(" | Blue Freq: ");
  Serial.print(blueFrequency);
  Serial.print(" | Yellow Freq: ");
  Serial.println(yellowFrequency);

  // Dynamic threshold for yellow
  int yellowThreshold = 0;
  if (i == 0) {
    yellowThreshold = 35;  // Adjusted for sensor 0
  }
  else if (i == 1) {
    yellowThreshold = 250; // Sensor 1 higher values
  }

  // Decide color based on pattern we found in readings
  if (yellowFrequency < yellowThreshold) {
    return "yellow";
  }
  else if (redFrequency > blueFrequency) {
    return "blue";
  }
  else if (blueFrequency > redFrequency) {
    return "red";
  }
  else {
    return "other"; // This case handles ambiguous readings
  }
}

bool isColorMatch (String sensorColor, int i) {
  String detectedColor = detectColor(i);
  Serial.print("Detected Color: ");
  Serial.println(detectedColor);
  return detectedColor.equalsIgnoreCase(sensorColor);
}

void sendJsonFile (int pointsNum, String operationType, String teamId) {
  if (WiFi.status() == WL_CONNECTED) { // Check WiFi connection status
    HTTPClient http;
    http.begin(serverUrl); // Specify destination for HTTP request
    http.addHeader("Content-Type", "application/json"); // Specify content-type header

    String requestMessage = "{\"points\": " + String(pointsNum) + ", \"operation\": \"" + operationType + "\", \"team\": \"" + teamId + "\"}"; // Create JSON compatible string
    Serial.print("JSON message: ");
    Serial.println(requestMessage);
    int httpResponseCode = http.POST(requestMessage); // Send the actual POST request and assign variable
    
    if (httpResponseCode > 0) { // No Error
      String response = http.getString(); // Get the response to the request
      Serial.println("Return code");
      Serial.println(httpResponseCode); // Print return code
      Serial.println("Request answer");
      Serial.println(response); // Print request answer
    }
    else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode); // Print error code
    }
    http.end(); // To free resources
  }
  else {
    Serial.println("Error in WiFi connection");
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize variables each time
  activeLandmine = random(0, 2);
  sensorState = 0;
  objectFound = false;

  Serial.print("Active landmine: ");
  Serial.println(activeLandmine);

  // Initialize IR sensor pins as inputs
  for (int i = 0; i < 2; i++) {
    pinMode(irSensorPins[i], INPUT);
  }

  // Initialize color sensor pins as outputs (S0, S1, S2, S3) and OUT as input
  for (int i = 0; i < 2; i++) {
    pinMode(s0Pins[i], OUTPUT);
    pinMode(s1Pins[i], OUTPUT);
    pinMode(s2Pins[i], OUTPUT);
    pinMode(s3Pins[i], OUTPUT);
    pinMode(colorSensorOutPins[i], INPUT);
  }

  // Set frequency scaling to 100%
  for (int i = 0; i < 2; i++) {
    digitalWrite(s0Pins[i], HIGH);
    digitalWrite(s1Pins[i], LOW);
  }

  // Initialize NeoPixel pins
  for (int i = 0; i < 2; i++) {
    pinMode(rgbRingPins[i], OUTPUT); // Set the data pins for NeoPixels as OUTPUT
  }
  turnLandmineLedOn(activeLandmine);

  connectWiFi();
}

void loop() {
  for (int i = 0; i < 2; i++) {
    if (detectedLandmines[i] == 0) {
      int sensorState = digitalRead(irSensorPins[i]);   // read the state of the sensor value:
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.print(" state is: ");
      Serial.println(sensorState);

      if (sensorState == 0) { // Something detected
        Serial.println("Object detected");
        detectedLandmines[i] = 1; // Set landmine as detected
        if (i != activeLandmine) { // If landmine inactive
          Serial.println("Landmine is inactive: ");
          Serial.println(i);
          pointsToChange = 1;
          pointsOperation = "minus";

          Serial.print("Points to change: "); // Print info
          Serial.print(pointsOperation);
          Serial.println(pointsToChange);

          sendJsonFile(pointsToChange, pointsOperation, pointsTeam);
        }
        else if (isColorMatch(connectedSensorColor, i)) { // If landmine active and color match
          Serial.println("Color is a match");
          pointsToChange = 13;
          pointsOperation = "plus";

          Serial.print("Points to change: "); // Print info
          Serial.print(pointsOperation);
          Serial.println(pointsToChange);

          sendJsonFile(pointsToChange, pointsOperation, pointsTeam);
        }
        else { //If landmine active but color doesn't match
          Serial.println("No color match");
          pointsToChange = 10;
          pointsOperation = "plus";

          Serial.print("Points to change: "); // Print info
          Serial.print(pointsOperation);
          Serial.println(pointsToChange);

          sendJsonFile(pointsToChange, pointsOperation, pointsTeam);
        }
      }
      else {
        Serial.println("No object detected");
      }
    }
    else {
      Serial.println("Sensor was used already");
    }
    delay(7000);
  }
}