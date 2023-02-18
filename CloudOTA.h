#ifndef CLOUD_OTA_H
#define CLOUD_OTA_H

#include <WiFiClientSecure.h>
#include <Update.h>

// ------ Change the following paramters according to your server //
String currentFwVersion{"2.0.0"};
String host = "raw.githubusercontent.com";
const int hostPort = 443;
String fwVersionURL = "/e-tinkers/CloudOTA/master/fw_version.txt";
String fwBinaryURL = "/e-tinkers/CloudOTA/master/firmware";
// -------------------------------------------------------------- //

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

String latestFirmware{};
const int HTTP_TIMEOUT{20000};


String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

void updateFirmware(void) {
  
  long contentLength{0};
  bool isValidContentType{false};
  
  WiFiClientSecure client;
  client.setInsecure();

  if (!client.connect(host.c_str(), hostPort)) {
     Serial.println("Err: Connection failed");
     return;
  }
    
  String fileName = fwBinaryURL + latestFirmware + ".bin";
  Serial.println("Fetching: firmware" + latestFirmware + ".bin...");
  
  client.println("GET " + fileName + " HTTP/1.1");
  client.println("Host: " + host);
  client.println("Cache-Control: no-cache");
  client.println("Keep-Alive: timeout=15, max=1000");
  client.println("Connection: Keep-Alive\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > HTTTP_TIMEOUT) {
       Serial.println("Err: Client Timeout");
       return;
    }
    delay(1);
  }

  // parse headers
  while(client.available()) {
    String line = client.readStringUntil('\n');

    if (line.startsWith("HTTP/1.1")) {
      if (line.indexOf("200") < 0) {
        Serial.println("Err: request error: " + line);
        break;
      }
    }

    if (line.startsWith("Content-Length: ")) {
      String clen = getHeaderValue(line, "Content-Length: ");
      contentLength = clen.toInt();
    }

    if (line.startsWith("Content-Type: ")) {
      String contentType = getHeaderValue(line, "Content-Type: ");
      contentType.trim();
      if (contentType.equals("application/octet-stream")) {
        isValidContentType = true;
      }
    }
   
    if (line.startsWith("\r")) break;
  }

  if (contentLength && isValidContentType) {
    bool canBegin = Update.begin(contentLength);
    if (canBegin) {
      
      Serial.println("OTA started...");
      long written = Update.writeStream(client);

      if (Update.end()) {
        Serial.println("OTA finished...");
        Serial.printf("Written : %ld bytes...\n", written);
        if (Update.isFinished()) {
          Serial.println("Rebooting...");
          ESP.restart();
        }
      } else {
        Serial.printf("Err: %d\n", Update.getError());
      }
    } else {
      Serial.printf("Err: Not enough memory, requires %ld bytes", contentLength);
    }
  } 
  
  client.flush();
  client.stop();
  
}

bool newFirmwareAvailable(void) {

  bool newVersion{false};

  Serial.println("This firmware: " + currentFwVersion);

  WiFiClientSecure client;
  client.setInsecure();
  if (!client.connect(host.c_str(), hostPort)) {
     Serial.println("Err: Connection failed");
     return false;
  }

  client.println("GET " + fwVersionURL + " HTTP/1.1");
  client.println("Host: " + host);
  client.println("Cache-Control: no-cache");
  client.println("Connection: close\r\n");
  
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > HTTP_TIMEOUT) {
       Serial.println("Err: Client Timeout");
       return false;
    }
    delay(1);
  }

  // skipt header
  while(client.available()) {
    String line = client.readStringUntil('\n');
    if (line.startsWith("\r")) break;
  }

  if (client.available()) {
    latestFirmware = client.readStringUntil('\n');
    latestFirmware.trim();
    Serial.println("Latest firmware: " + latestFirmware);
    if (!latestFirmware.equals(currentFwVersion)) {
      newVersion = true;
    }
  }
  client.flush();
  client.stop();

  return newVersion;
  
}

#endif
