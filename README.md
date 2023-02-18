# CloudOTA

> CloudOTA currently only targets ESP32. It could be extended to work with ESP8266 with some modification.

CloudOTA.h consists of two functions used for OTA update via a cloud server (e.g. via github repository or any VPS). The CloudOTA.ino provides a simple demonstration and the boiler plate code on how to use the CloudOTA.

CloudOTA is based on ESP32 [Update](https://github.com/espressif/arduino-esp32/tree/master/libraries/Update) Library, it allows user to upload the firmware binary file and specific what is the latest firmware firmware version in a `fw_version.txt` file. This allows IoT devices deployed out in the field to make a request to a github repository(or any VPS) to check the latest firmware when the IoT device come in life from deep sleep or on a reset cycle.

CloudOTA abtracts away all the boiler plate code into the `CloudOTA.h` library, and user only need to add a few line of codes, usually before the end of `setup()` function, and with some configuration setting in the `CloudOTA.h`.

## When should I use CloudOTA
ESP32-Arduino Core provides a great library [ArduinoOTA](https://github.com/espressif/arduino-esp32/tree/master/libraries/ArduinoOTA) for providing an easy-to-user OTA functionality for devices that are constantly on and within a local network. However, it is not really suitable for ESP32 IoT devices deployed in the field that remain deepsleep and only wake-up a few times a day. Those devices need to be able to check the latest firmware version via a public-facing server and conduct an OTA for self-update. CloudOTA does not replace ArduinoOTA, the two serve different purpose.

## How to use it

### Add CloudOTA capability to your project

1. Copy the `CloudOTA.h` into your project directory or repository;
2. Modify the following parameters in the `CloudOTA.h` file based on your server/github environment;

```
String currentFwVersion{"2.0.0"};
String host = "raw.githubusercontent.com";
const int hostPort = 443;
String fwVersionURL = "/e-tinkers/CloudOTA/master/fw_version.txt";
String fwBinaryURL = "/e-tinkers/CloudOTA/master/firmware";
```

The `currentFwVersion` value should be the value of your current firmware version. In the example, `2.0.0` is the current firmware version of the example `CloudOTA.ino`.

The rest of the parameters are related to your server setting. The default settings are assuming of using github repository as the OTA hosting server. do remember to replace the `e-tinkers` with your github username. 

The `fwVersionURL` specified a text file which contains the latest firmware version number.

The `fwBinaryURL` specified the *prefix* of the firmware binary file.

3. Add the three lines of code below into your project sketch. Don't forget to add `#include "CloudOTA.h"` header in your sketch as well. This will enable your sketch with the CloudOTA capability whenever the sketch wake up from deepsleep or have a hardware/software reset.

```
#include "CloudOTA.h"

void setup() {
    // your normal setup code here

    // Add the Cloud OTA functionality
    if (newFirmwareAvailable()) {
        updateFirmware();
    }
}
```

4. Create `fw_version.txt` in the project directory, the content of the `fw_version.txt` should match the value used in `currentFwVersion` parameter in `CloudOTA.h`.

5. flash the ESP32 device with the latest OTA-enabled code. That's all! The `CloudOTA.ino` consists of a simple boiler plate sketch that has a firmware version `2.0.0`. It will check for latest firmware version from the `fw_version.txt` on next reset or wakeup from a deepsleep.

### Update the firmware

1. If you made the modification of your project in the future (for example, add the LED blinking in the example code).

2. Change your `currentFwVersion` to "2.0.1" in `CloadOTA.h`. 

3. Click on "Sketch -> Export Compiled Binary" from Arduino IDE to export the latest sketch;

4. Go to your sketch directory and there should be a binary file named based on your sketch file name as `<sketch_file_name>.ino.esp32.bin`. Rename it to `firmware2.0.1.bin` where the `firmware` matched the prefix name used in your configration parameter `fwBinaryURL` mentioned in `CloudOTA.h`, and the `2.0.1` is the current version number specified in the `currentFwVersion` in `CloudOTA.h`.

3. Update the `fw_version.txt` accordingly to `2.0.1`. 

4. Do a git push to push the latest project (and the `firmware.2.0.1.bin`) to the github. Your IoT device will automatically updated on the next reset or wake-up from deepsleep.



