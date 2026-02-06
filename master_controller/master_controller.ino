// MASTER_CONTROLER.INO  //
// F1SSF- 02/01/2025     //
// AZ/180 et EL/90 pilotées par relais  //
// Recopie Analogique 0 to 5V           //
// Master to Remote avec RS485          //
// Interface HMI Nextion                //
// Extension pilotage de 8 relais       //
// Systeme de calibration  direct HMI   //
// Mémorisation de la calibration.      //

#include "config.h"
#include "rs485_manager.h"
#include "hmi_manager.h"

const char* status_str = "READY";

int az_raw = 0;
int el_raw = 0;

// Setup //

void setup() {
  pinMode(RS485_EN, OUTPUT);
  rs485_rx_on();

  Serial.begin(115200);
  Serial3.begin(115200);
  Serial1.begin(9600);

  Serial.println("MASTER READY");
  if (eepromLoadCalibration()) {
  cal_status_str = "CAL EP OK";
  } else {
  cal_status_str = "CAL EP EMPTY";
  }
}

//  Loop //

void loop() {
  static unsigned long tAna = 0;
  if (millis() - tAna >= 500) {
    tAna = millis();
    rs485Send("REQ,ANA");
  }

  readHMI();
  readRS485Debug();
  hmiUpdateValues();

}


