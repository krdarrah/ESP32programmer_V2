

#include "src/ESPSerialFlasher/src/ESPSerialFlasher.h"  //inspired by https://github.com/winner10920/ESPSerialFlasher
#include <SPI.h>
#include "SD.h"
const int SD_CS_pin = 25;
const int startProgramming_pin = 21;
const int En3VPin = 14;
const int dtrPin = 4;
unsigned long greenLEDstartTime;

#define FILE_NAME "/config.txt"  // the programmer offsets here
#define KEY_MAX_LENGTH 30        // change it if key is longer
#define VALUE_MAX_LENGTH 30      // change it if value is longer

// offsets from config.txt file
uint32_t bootNameOffset = 0x00;
uint32_t bootloaderNameOffset = 0x00;
uint32_t partitionsNameOffset = 0x00;
uint32_t firmwareNameOffset = 0x00;
uint32_t spiffsNameOffset = 0x00;
String bootNameOffsetStr;
String bootloaderNameOffsetStr;
String partitionsNameOffsetStr;
String firmwareNameOffsetStr;
String spiffsNameOffsetStr;

//the 4 files we're looking for must contain these keywords in the title
const char bootKey[] = "boot_app";
const char bootloaderKey[] = "bootloader";
const char partitionsKey[] = "partitions";
const char firmwareKey[] = ".ino.bin";         //when you flash over USB, it looks like this
const char firmwareSecondKey[] = "esp32.bin";  //when you export compiled binary, this is the file it generates
const char SPIFFSKey[] = "spiffs.bin";         ///Users/kevindarrah/Library/Arduino15/packages/esp32/tools/esptool_py/3.0.0/esptool --chip esp32 --port /dev/cu.usbserial-DA00XJ7V  --baud 230400 read_flash 0x3D0000 0x30000 spiffs.bin
boolean flashSPIFFS = false;                   //that way we only flash if present on card

const char hiddenFileKey[] = "._";  //on mac, we may find duplicates that start with this, so we filter out

//these title names will be stored here
char bootName[100] = { NULL };
char bootloaderName[100] = { NULL };
char partitionsName[100] = { NULL };
char firmwareName[100] = { NULL };
char spiffsName[100] = { NULL };


SPIClass spiSD(VSPI);

void initSDcard();
bool startFlashing();
void getFileNames();
bool SD_available(const __FlashStringHelper *key);
int SD_findInt(const __FlashStringHelper *key);
float SD_findFloat(const __FlashStringHelper *key);
String SD_findString(const __FlashStringHelper *key);
int SD_findKey(const __FlashStringHelper *key, char *value);
uint32_t HELPER_ascii2Int(char *ascii, int length);
float HELPER_ascii2Float(char *ascii, int length);
String HELPER_ascii2String(char *ascii, int length);

void setup() {
  Serial.begin(115200);
  delay(1000);
  redLED(true);
  initSDcard();
  pinMode(startProgramming_pin, INPUT_PULLUP);
  pinMode(En3VPin, OUTPUT);
  pinMode(dtrPin, OUTPUT);
  digitalWrite(dtrPin, HIGH);
  delay(10);
  digitalWrite(En3VPin, HIGH);
  redLED(false);
  Serial2.begin(115200);
}

void loop() {
  if (millis() - greenLEDstartTime > 500) {
    greenLEDstartTime = millis();
    greenLEDflash();
  }
  if (Serial2.available()) {
    Serial.print("TARGET----");
    while (Serial2.available()) {
      Serial.write(Serial2.read());
    }
  }

  if (!digitalRead(startProgramming_pin)) {
    if (!startFlashing()) {
      digitalWrite(En3VPin, LOW);
      while (1) {
        greenLED(false);
        yelLED(false);
        redLED(true);
      }
    }
    Serial2.end();
    Serial2.begin(115200);
    digitalWrite(En3VPin, LOW);
    delay(1000);
    digitalWrite(En3VPin, HIGH);
  }
}
