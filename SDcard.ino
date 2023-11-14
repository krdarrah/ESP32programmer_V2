
//HSPI MOSI=13, MISO=12, CLK=14, CS=25
//VSPI MOSI=23, MISO=19, CLK=18, CS=25

void getFileNames() {
  Serial.println("Looking for files");
  File root;
  root = SD.open("/");
  while (true) {

    File entry = root.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    char *pointerTobootloaderKey = strstr(entry.name(), bootloaderKey);  //go find keyword
    if (pointerTobootloaderKey != NULL) {
      char *pointerToHiddenFile = strstr(entry.name(), hiddenFileKey);
      if (pointerToHiddenFile == NULL) {  //good not hidden file
        Serial.print("Found Bootloader File: ");
        strcpy(bootloaderName, "/");
        strcat(bootloaderName, entry.name());
        //strncpy(bootloaderName, entry.name(), sizeof(bootloaderName));
        Serial.println(bootloaderName);
      }
    }
    char *pointerTobootKey = strstr(entry.name(), bootKey);  //go find keyword
    if (pointerTobootKey != NULL) {
      char *pointerToHiddenFile = strstr(entry.name(), hiddenFileKey);
      if (pointerToHiddenFile == NULL) {  //good not hidden file
        Serial.print("Found Boot File: ");
        strcpy(bootName, "/");
        strcat(bootName, entry.name());
        //strncpy(bootName, entry.name(), sizeof(bootName));
        Serial.println(bootName);
      }
    }
    char *pointerToPartitionsKey = strstr(entry.name(), partitionsKey);  //go find keyword
    if (pointerToPartitionsKey != NULL) {
      char *pointerToHiddenFile = strstr(entry.name(), hiddenFileKey);
      if (pointerToHiddenFile == NULL) {  //good not hidden file
        Serial.print("Found Partitions File: ");
        strcpy(partitionsName, "/");
        strcat(partitionsName, entry.name());
        //strncpy(partitionsName, entry.name(), sizeof(partitionsName));
        Serial.println(partitionsName);
      }
    }
    char *pointerToFirmwareKey = strstr(entry.name(), firmwareKey);  //go find keyword
    if (pointerToFirmwareKey != NULL) {
      char *pointerToHiddenFile = strstr(entry.name(), hiddenFileKey);
      if (pointerToHiddenFile == NULL) {  //good not hidden file
        Serial.print("Found Firmware File: ");
        strcpy(firmwareName, "/");
        strcat(firmwareName, entry.name());
        //strncpy(firmwareName, entry.name(), sizeof(firmwareName));
        Serial.println(firmwareName);
      }
    }
    char *pointerToFirmwareSecondKey = strstr(entry.name(), firmwareSecondKey);  //go find keyword
    if (pointerToFirmwareSecondKey != NULL) {
      char *pointerToHiddenFile = strstr(entry.name(), hiddenFileKey);
      if (pointerToHiddenFile == NULL) {  //good not hidden file
        Serial.print("Found Firmware File: ");
        strcpy(firmwareName, "/");
        strcat(firmwareName, entry.name());
        //strncpy(firmwareName, entry.name(), sizeof(firmwareName));
        Serial.println(firmwareName);
      }
    }

    char *pointerToSPIFFSKey = strstr(entry.name(), SPIFFSKey);  //go find keyword
    if (pointerToSPIFFSKey != NULL) {
      char *pointerToHiddenFile = strstr(entry.name(), hiddenFileKey);
      if (pointerToHiddenFile == NULL) {  //good not hidden file
        Serial.print("Found SPIFFS File: ");
        strcpy(spiffsName, "/");
        strcat(spiffsName, entry.name());
        //strncpy(spiffsName, entry.name(), sizeof(spiffsName));
        Serial.println(spiffsName);
        flashSPIFFS = true;
      }
    }

    Serial.println(entry.name());
    //    if (entry.isDirectory()) {
    //      Serial.println(" / ");
    //      //printDirectory(entry, numTabs + 1);
    //    } else {
    //      // files have sizes, directories do not
    //      Serial.print("\t\t");
    //      Serial.println(entry.size(), DEC);
    //    }
    entry.close();
  }
}


void initSDcard() {
  //spiSD.begin(14, 12, 13, SD_CS); //SCK,MISO,MOSI,SS //HSPI1
  spiSD.begin(18, 19, 23, SD_CS_pin);  //SCK,MISO,MOSI,SS //HSPI1
  while (!SD.begin(SD_CS_pin, spiSD)) {
    Serial.println("Card Mount Failed");
    delay(1000);
  }


  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }


  String bootNameOffsetStr = SD_findString(F("bootNameOffset"));
  String bootloaderNameOffsetStr = SD_findString(F("bootloaderNameOffset"));
  String partitionsNameOffsetStr = SD_findString(F("partitionsNameOffset"));
  String firmwareNameOffsetStr = SD_findString(F("firmwareNameOffset"));
  String spiffsNameOffsetStr = SD_findString(F("spiffsNameOffset"));


  /* default
bootNameOffset=0xe000
bootloaderNameOffset=0x1000
partitionsNameOffset=0x8000
firmwareNameOffset=0x10000
spiffsNameOffset=0x3D0000
  */

  char *bootNameOffsetchar = const_cast<char *>(bootNameOffsetStr.c_str());
  bootNameOffset = strtoul(bootNameOffsetchar, NULL, 0);

  char *bootloaderNameOffsetchar = const_cast<char *>(bootloaderNameOffsetStr.c_str());
  bootloaderNameOffset = strtoul(bootloaderNameOffsetchar, NULL, 0);

  char *partitionsNameOffsetchar = const_cast<char *>(partitionsNameOffsetStr.c_str());
  partitionsNameOffset = strtoul(partitionsNameOffsetchar, NULL, 0);

  char *firmwareNameOffsetchar = const_cast<char *>(firmwareNameOffsetStr.c_str());
  firmwareNameOffset = strtoul(firmwareNameOffsetchar, NULL, 0);

  char *spiffsNameOffsetchar = const_cast<char *>(spiffsNameOffsetStr.c_str());
  spiffsNameOffset = strtoul(spiffsNameOffsetchar, NULL, 0);


  Serial.printf("%s -> 0x%.2x bootNameOffset\n", bootNameOffsetStr, bootNameOffset);
  Serial.printf("%s -> 0x%.2x bootloaderNameOffset\n", bootloaderNameOffsetStr, bootloaderNameOffset);
  Serial.printf("%s -> 0x%.2x partitionsNameOffset\n", partitionsNameOffsetStr, partitionsNameOffset);
  Serial.printf("%s -> 0x%.2x firmwareNameOffset\n", firmwareNameOffsetStr, firmwareNameOffset);
  Serial.printf("%s -> 0x%.2x spiffsNameOffset\n", spiffsNameOffsetStr, spiffsNameOffset);






  //  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  //  Serial.printf("SD Card Size: % lluMB\n", cardSize);

  getFileNames();

  if (bootName[0] == NULL || bootloaderName[0] == NULL || partitionsName[0] == NULL || firmwareName[0] == NULL) {
    Serial.println("ERROR - not all files found!!!");
    while (1) {
    }
  }
}
