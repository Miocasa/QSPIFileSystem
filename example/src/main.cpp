#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <Adafruit_SPIFlash.h>
#include <InternalFileSystem.h>

#include "../../src/QSPIFileSystem.h"

Adafruit_FlashTransport_QSPI flashTransport;
Adafruit_SPIFlash externalFlash(&flashTransport);
QSPIFileSystem qspiFS(&externalFlash);

Adafruit_LittleFS_Namespace::File file(qspiFS);

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
}


void read_file()
{
  file = qspiFS.open(FILE_NAME, Adafruit_LittleFS_Namespace::FILE_O_WRITE);

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

void loop()
{
  if (Serial.available())
  {
    char cmd = Serial.read();

    switch (cmd)
    {
    case 'w': // Save test data
      {
        read_file();
      }
      break;

    case 'r': // list files in dir
      {
        file = qspiFS.open(FILE_NAME, Adafruit_LittleFS_Namespace::FILE_O_READ);

        if (!file)
        {
          Serial.println("FAILED to open in read mode");
          return;
        }

        static const char* content = "Hello World";
        char buffer[128];
        buffer[0] = '\0';

        size_t read = file.read(buffer, sizeof(buffer));
        file.close();

        if (read == sizeof(TEXT))
        {
          Serial.print("Successful read, content =\"");
          Serial.println(buffer);
        }
        else
        {
          Serial.print("Failed to open ");
          Serial.print(FILE_NAME);
          Serial.println(" for writing");
        }
      }
      break;
    case 'l': // todo list files in dir
      {

      }
      break;
    case 'f':
      qspiFS.format();
      break;

    case 'b':
      enterUf2Dfu();
      break;

    case 'n':
      NVIC_SystemReset();
      break;
    default: break;
    }
  }
}

