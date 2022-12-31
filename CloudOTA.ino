// This is a demo of OTA update via a cloud server (e.g. github repository) by checking a firmware 
// version file fw_version.txt on the server against the hardcoded currentFwVersion string, if the
// two values are not equal, it will then download the firmware.bin from the server. 
// This code is based on ESP32.

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

#include <WiFi.h>
#include "CloudOTA.h"

const char *ssid = "ssid";
const char *password = "password";

void setup() {

//  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (newFirmwareAvailable()) {
    updateFirmware();
  }
}

void loop() {
//  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//  delay(1000);
}
