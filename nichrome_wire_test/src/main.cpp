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

#define PWM_RES 8
#define PWM_FREQ 1000
#define PWM_SETUP ledcSetup
#define PWM_ATTACH ledcAttachPin
#define SET_PWM ledcWrite

int data;

AsyncWebServer server(80);
void initSoftAP();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(WIRE_1, OUTPUT);
  pinMode(WIRE_2, OUTPUT);
  pinMode(WIRE_3, OUTPUT);
  pinMode(WIRE_4, OUTPUT);
  pinMode(WIRE_5, OUTPUT);
  pinMode(WIRE_6, OUTPUT);
  pinMode(WIRE_7, OUTPUT);

  PWM_SETUP(0, PWM_FREQ, PWM_RES);
  PWM_ATTACH(WIRE_1, 0);
  PWM_ATTACH(WIRE_2, 0);
  PWM_ATTACH(WIRE_3, 0);
  PWM_ATTACH(WIRE_4, 0);
  PWM_ATTACH(WIRE_5, 0);
  PWM_ATTACH(WIRE_6, 0);
  PWM_ATTACH(WIRE_7, 0);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    String userId;
    if (request->hasParam("")) {
      userId = request->getParam("value")->value();
    }
  });

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 2){
    data = Serial.parseInt();
    SET_PWM(0, data);
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