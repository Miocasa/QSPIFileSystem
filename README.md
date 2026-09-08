# QSPIFileSystem library

Usage like Adafruit_InternalFs library for Xiao nrf52 Ble non mbed core.

## Initialization

```c++
#include <Adafruit_TinyUSB.h> // requered for serial and usb for Xiao nrf52
#include <Adafruit_SPIFlash.h>
#include "QSPIFileSystem.h"

Adafruit_FlashTransport_QSPI flashTransport;
Adafruit_SPIFlash externalFlash(&flashTransport);

QSPIFileSystem qspiFS(&externalFlash);
```

## Usage

```c++
#define TEXT ("Hello World")
#define FILE_NAME ("/file.txt")

void setup()
{
  Serial.begin(115200);
  while (!Serial) delay(100);

  if (qspiFS.begin())
  {
    Serial.println("QSPI Flash Successfull mount");
  }
  else
  {
    Serial.println("QSPI Flash failed mount");
  }
  Adafruit_LittleFS_Namespace::File file = qspiFS.open(FILE_NAME, Adafruit_LittleFS_Namespace::FILE_O_WRITE);

  if (!file)
  {
    Serial.println("FAILED to open in write mode");
    return;
  }

  size_t writen = file.write(TEXT);
  file.close();
  if (writen == sizeof(TEXT))
  {
    Serial.print("Successful writed to\"");
    Serial.print(FILE_NAME);
    Serial.print("\" text = ");
    Serial.println(TEXT);
  }
  else
  {
    Serial.print("Failed to open ");
    Serial.print(FILE_NAME);
    Serial.println(" for writing");
  }
}
```

## Requirements

Adafruit_LittleFS, it contains in xiao non mbed core and in some of Adafruit cores.
[Adafruit_SPIFlash](https://github.com/adafruit/Adafruit_SPIFlash.git)


