// REMOTE_CONTROLER.INO                 //
// F1SSF -  02/01/2025                  //
// AZ/180 et EL/90 pilotées par relais  //
// Recopie Analogique 0 to 5V)          //
// Master to Remote avec RS485          //
// Interface HMI Nextion                //
// Extension pilotage de 8 relais)      //
// Systeme de calibration  direct HMI   //
// Les relais sont commandés en logique 0     //
// Relais pin D22, 23, 24, 25, 26, 27, 28 , 29//



#include "config.h"
#include "rs485_manager.h"
#include "relay_manager.h"

// SETUP ///

void setup() {
  pinMode(RS485_EN, OUTPUT);
  rs485_rx_on();

  // Relais moteur
  pinMode(RELAY_UP,  OUTPUT);
  pinMode(RELAY_DW,  OUTPUT);
  pinMode(RELAY_CW,  OUTPUT);
  pinMode(RELAY_CCW, OUTPUT);

  // Relais RL1..RL8
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(rlPins[i], OUTPUT);
    digitalWrite(rlPins[i], HIGH); // OFF sécurisé
  }

  allRelaysOff();

  Serial.begin(115200);
  Serial1.begin(9600);

  Serial.println("REMOTE READY");
}

// LOOP ////

void loop() {
  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\r' || c == '\n') {
      if (rxLine.length()) {
        handleCmd(rxLine);
        rxLine = "";
      }
    } 
    else if (c >= 32 && c <= 126) {
      rxLine += c;
      if (rxLine.length() > 40) rxLine = "";
    }
  }
}


