#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <Adafruit_SPIFlash.h>
#include <InternalFileSystem.h>

#include "QSPIFileSystem.h"
// #include "SaveManager.h"

// Adafruit_FlashTransport_QSPI flashTransport;
// Adafruit_SPIFlash externalFlash(&flashTransport);
Adafruit_FlashTransport_QSPI flashTransport;
Adafruit_SPIFlash externalFlash(&flashTransport);

QSPIFileSystem qspiFS(&externalFlash);


bool hasExternalFlash = false;


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
}

#define FILE_DIR ("/cfg")
#define FILE_PATH ("/cfg/File.txt")

void loop()
{
  string lol;
  if (Serial.available())
  {
    char cmd = Serial.read();

    switch (cmd)
    {
    case 's': // Save test data
      {
        qspiFS.mkdir(FILE_DIR);

        Adafruit_LittleFS_Namespace::File file = qspiFS.open(FILE_PATH,
                                                             Adafruit_LittleFS_Namespace::FILE_O_WRITE);
        if (!file)
        {
          Serial.println("FAILED to open");
        }
        else
          Serial.println("OK");
        file.write("test");
        file.close();

        file = qspiFS.open(FILE_PATH);
        if (!file)
          Serial.println("FAILED to open");
        else
          Serial.println("OK");
        Serial.printf("File data: %s", file.readString().c_str());
        file.close();
      }
      break;

    case 'l': // list files in dir
      {
        auto file = qspiFS.open("/cfg");
        Serial.println("List files:");
        while (auto f = file.openNextFile())
        {
          Serial.print(f.name());
          Serial.print(": ");
          Serial.println(f.readString());
          f.close();
        }
        file.close();
        Serial.println();
      }
      break;


    case 'f':
      qspiFS.format();
      break;

    case 'b':
      enterUf2Dfu();
      break;

    case 'r':
      NVIC_SystemReset();
      break;
    default: break;
    }
  }
}

