
/*
  boot_app0.bin
  bootloader_qio_80m.bin
  trigBoardV8_BaseFirmware.ino.bin
  trigBoardV8_BaseFirmware.ino.partitions.bin
*/
/*
/Users/kevindarrah/Library/Arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool --chip esp32c3 --port /dev/cu.usbserial-D30DAC4E --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 

0x0 /var/folders/gv/dqd77lfs72xgzhcbwrp3f6vc0000gn/T/arduino_build_30965/Fargo_ESP32C3_SlotTask.ino.bootloader.bin  

0x8000 /var/folders/gv/dqd77lfs72xgzhcbwrp3f6vc0000gn/T/arduino_build_30965/Fargo_ESP32C3_SlotTask.ino.partitions.bin  

0xe000 /Users/kevindarrah/Library/Arduino15/packages/esp32/hardware/esp32/2.0.11/tools/partitions/boot_app0.bin  

0x10000 /var/folders/gv/dqd77lfs72xgzhcbwrp3f6vc0000gn/T/arduino_build_30965/Fargo_ESP32C3_SlotTask.ino.bin
 * 
 * 
 */

void putInBootMode() {
  Serial2.end();
  digitalWrite(En3VPin, LOW);
  digitalWrite(dtrPin, LOW);
  delay(1000);
  digitalWrite(En3VPin, HIGH);
  delay(1000);
  digitalWrite(dtrPin, HIGH);
}
bool startFlashing() {
  //delay(1000);
  putInBootMode();
  ESPFlasherInit(true, &Serial);//sets up Serial communication to wifi module, with debug messages, to Print Class of your choice
  if (ESPFlasherConnect()) {       //connects to wifi module
    if (ESPFlashBin(bootName, bootNameOffset)) { //bootapp
      putInBootMode();
      ESPFlasherInit(true, &Serial);
      if (ESPFlasherConnect()) {
        if (ESPFlashBin(bootloaderName, bootloaderNameOffset)) { //bootloader
          putInBootMode();
          ESPFlasherInit(true, &Serial);
          if (ESPFlasherConnect()) {
            if (ESPFlashBin(partitionsName, partitionsNameOffset)) { //partitions
              putInBootMode();
              ESPFlasherInit(true, &Serial);
              if (ESPFlasherConnect()) {
                if (ESPFlashBin(firmwareName, firmwareNameOffset)) { //firmware
                  if (flashSPIFFS) {//only if SPIFFS bin is present!
                    putInBootMode();
                    ESPFlasherInit(true, &Serial);
                    if (ESPFlasherConnect()) {
                      if (ESPFlashBin(spiffsName, spiffsNameOffset)) { //SPIFFS - minimal SPIFFS settings, you need to change this for your settings!
                        Serial.println("!!!EVERYTHING IS FLASHED AND COMPLETE!!!");
                        return true;
                      } else
                        return false;
                    } else
                      return false;
                  } else {
                    Serial.println("!!!EVERYTHING IS FLASHED AND COMPLETE!!!");
                    

                    digitalWrite(dtrPin, LOW);
                    digitalWrite(En3VPin, LOW);
                    delay(1000);
                    digitalWrite(dtrPin, HIGH);
                    delay(1000);
                    digitalWrite(En3VPin, HIGH);
                    return true;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return false;
}
