#ifndef CLOUD_OTA_H
#define CLOUD_OTA_H

#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>

String currentFwVersion{"2.0.0"};
String latestFirmware{};

String fwVersionURL = "https://raw.githubusercontent.com/e-tinkers/CloudOTA/master/fw_version.txt";
String fwBinaryURL = "https://raw.githubusercontent.com/e-tinkers/CloudOTA/master/firmware";

// This is the root CA for github
// DigiCert Global CA (expired 10 Nov 2031 00:00:00 GMT)
const char * githubRootCA = \
"-----BEGIN CERTIFICATE-----\n"
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
"-----END CERTIFICATE-----\n";

void updateFirmware(void) {

  httpUpdate.onStart([]() {
    Serial.println("OTA download started");
  });
  
  httpUpdate.onEnd([]() {
    Serial.println("OTA download finished");
  });
  
  httpUpdate.onProgress([](int cur, int total) {
#ifdef LED_BUILTIN
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
#else
    Serial.printf("Downloading %d of %d bytes...\n", cur, total);
#endif
  });
  
  httpUpdate.onError([](int err) {
    Serial.printf("OTA download error %d\n", err);
  });
  
  WiFiClientSecure client;
  client.setInsecure();
  httpUpdate.update(client, fwBinaryURL + latestFirmware + ".bin");

}

bool newFirmwareAvailable(void) {

  bool newVersion{false};

  Serial.printf("This firmware: v%s\n", currentFwVersion.c_str());
    
  WiFiClientSecure client;
  HTTPClient https;
  client.setInsecure();
  https.addHeader("Cache-Control", "no-cache");
  if (https.begin(client, fwVersionURL + "?" + currentFwVersion)) {
    int httpCode = https.GET();
    if (httpCode == HTTP_CODE_OK) {
      latestFirmware = https.getString(); // get version from server
      latestFirmware.trim();
      Serial.printf("Latest firmware: %s\n", latestFirmware.c_str());
      if (!latestFirmware.equals(currentFwVersion))
        newVersion = true;
    } else {
      Serial.printf("\nError: %d\n", httpCode);
    }
    https.end();
  }
  return newVersion;
  
}

#endif
