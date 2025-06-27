#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

const char* ssid = "Heat Control";
const char* password = ""; // No password for open network

#define WIRE_1 21
#define WIRE_2 22
#define WIRE_3 23
#define WIRE_4 25
#define WIRE_5 26
#define WIRE_6 17
#define WIRE_7 18

#define WIRE_1_CHANNEL 1
#define WIRE_2_CHANNEL 2
#define WIRE_3_CHANNEL 3
#define WIRE_4_CHANNEL 4
#define WIRE_5_CHANNEL 5
#define WIRE_6_CHANNEL 6
#define WIRE_7_CHANNEL 7


#define PWM_RES 8
#define PWM_FREQ 1000
#define PWM_SETUP ledcSetup
#define PWM_ATTACH ledcAttachPin
#define SET_PWM ledcWrite

int data;
int pin_array[] = {0, WIRE_1, WIRE_2, WIRE_3, WIRE_4, WIRE_5, WIRE_6, WIRE_7};

AsyncWebServer server(80);
void initSoftAP();
void updatePWM(String userId, int pwmValue);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initSoftAP();


  pinMode(WIRE_1, OUTPUT);
  pinMode(WIRE_2, OUTPUT);
  pinMode(WIRE_3, OUTPUT);
  pinMode(WIRE_4, OUTPUT);
  pinMode(WIRE_5, OUTPUT);
  pinMode(WIRE_6, OUTPUT);
  pinMode(WIRE_7, OUTPUT);

  PWM_SETUP(0, PWM_FREQ, PWM_RES);
  PWM_ATTACH(WIRE_1, WIRE_1_CHANNEL);
  PWM_ATTACH(WIRE_2, WIRE_2_CHANNEL);
  PWM_ATTACH(WIRE_3, WIRE_3_CHANNEL);
  PWM_ATTACH(WIRE_4, WIRE_4_CHANNEL);
  PWM_ATTACH(WIRE_5, WIRE_5_CHANNEL);
  PWM_ATTACH(WIRE_6, WIRE_6_CHANNEL);
  PWM_ATTACH(WIRE_7, WIRE_7_CHANNEL);

  for (int i = 0; i < sizeof(pin_array) / sizeof(pin_array[0]); i++) {
    PWM_SETUP(i, PWM_FREQ, PWM_RES);
    PWM_ATTACH(pin_array[i], i);
    SET_PWM(i, 0); // Initialize all channels to 0
  }


  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value") && request->hasParam("wire")) {
      String pwmValueStr = request->getParam("value")->value();
      String userId = request->getParam("wire")->value();
      int pwmValue = pwmValueStr.toInt();
      updatePWM(userId, pwmValue);
      request->send(200, "text/plain", "PWM updated");
    } else {
      request->send(400, "text/plain", "Invalid parameters");
    }
  });

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 2){
    data = Serial.parseInt();
    SET_PWM(WIRE_1_CHANNEL, data);
    SET_PWM(WIRE_2_CHANNEL, data);
    SET_PWM(WIRE_3_CHANNEL, data);
    SET_PWM(WIRE_4_CHANNEL, data);
    SET_PWM(WIRE_5_CHANNEL, data);
    SET_PWM(WIRE_6_CHANNEL, data);
    SET_PWM(WIRE_7_CHANNEL, data);

    Serial.print("PWM set to: ");Serial.println(data);
  }
}

void initSoftAP() {
  // Start the SoftAP
  if (WiFi.softAP(ssid, password)) {
    Serial.println("SoftAP initialized successfully.");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Failed to initialize SoftAP.");
  }
}

void updatePWM(String userId, int pwmValue) {
  // Convert userId to integer
  int wireIndex = userId.toInt();

  // Validate wireIndex to ensure it corresponds to a valid channel
  if (wireIndex >= 0 && wireIndex <= 7) {
    int channel = wireIndex; // Map wireIndex to channel (WIRE_3_CHANNEL to WIRE_7_CHANNEL)
    SET_PWM(channel, pwmValue); // Update PWM for the respective channel
    Serial.print("PWM updated for wire ");
    Serial.print(wireIndex);
    Serial.print(" (channel ");
    Serial.print(channel);
    Serial.print("): ");
    Serial.println(pwmValue);
  } else {
    Serial.print("Wire:");Serial.print(wireIndex);
    Serial.print(" PWM:");Serial.print(pwmValue);
    Serial.println("Invalid wire index received.");

  }
}