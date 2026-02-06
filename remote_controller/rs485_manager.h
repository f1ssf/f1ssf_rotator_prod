// RS485_MANAGERE.H                   //
// F1SSF -  02/01/2025                //
// GESTION DU PROTOICOLE RS485        //

#pragma once
#include "config.h"

// Definition pin de de control EN sur module RS485
static inline void rs485_tx_on() { digitalWrite(RS485_EN, HIGH); }
static inline void rs485_rx_on() { digitalWrite(RS485_EN, LOW);  }

// envoi chaine de carractere en TX, et repasse en RX apres vidage(flush)

void rs485SendLine(const String &s) {
  rs485_tx_on();
  delay(1);
  Serial1.print(s);
  Serial1.print("\r\n");
  Serial1.flush();
  delay(5);
  rs485_rx_on();
}