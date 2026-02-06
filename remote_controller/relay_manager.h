// RELAIS_MANAGER.H                     //
// F1SSF -  02/01/2025                  //
// Gestion des variables                //

#pragma once
#include "config.h"
#include "rs485_manager.h"

// variable d'init //
String rxLine = "";

// Table des relais RL
const uint8_t rlPins[8] = {
  RELAY_RL1, RELAY_RL2, RELAY_RL3, RELAY_RL4,
  RELAY_RL5, RELAY_RL6, RELAY_RL7, RELAY_RL8
};

// État logique (pour TOGGLE)
bool rlState[8] = {0};

static inline void notifyRlState(uint8_t idx) {
  char buf[16];
  snprintf(buf, sizeof(buf), "RL,STATE,%u", idx);
  rs485SendLine(buf);
}




void allRelaysOff() {
  // Relais moteur
  digitalWrite(RELAY_UP,  HIGH);
  digitalWrite(RELAY_DW,  HIGH);
  digitalWrite(RELAY_CW,  HIGH);
  digitalWrite(RELAY_CCW, HIGH);

  // initialise Relais RL
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(rlPins[i], HIGH);
    rlState[i] = false;
  }
}

// fonction xor des relais ON OFF avec mise en memoire de l'etat
void setRelayExclusive(uint8_t idx, bool on) {
  for (uint8_t i = 0; i < 8; i++) {
    bool state = (i == idx) ? on : false;
    digitalWrite(rlPins[i], state ? LOW : HIGH);
    rlState[i] = state;
  }
  notifyRlState(on ? (idx + 1) : 0);
}

void toggleRelayExclusive(uint8_t idx) {
  bool newState = !rlState[idx];
  setRelayExclusive(idx, newState);
}

// Traitement des commandes reçues

void handleCmd(const String &cmd) {
  Serial.print("RX: ");
  Serial.println(cmd);

  if (cmd == "REQ,ANA") {
    int az = analogRead(A0);
    int el = analogRead(A1);
    delay(5);
    rs485SendLine(String("ANA,") + az + "," + el);
    return;
  }

  if (cmd == "OUT,ALL,0") {
    allRelaysOff();
    notifyRlState(0);
    return;
  }

  // Moteurs 
  if (cmd == "OUT,UP,1")  { digitalWrite(RELAY_DW, HIGH); digitalWrite(RELAY_UP, LOW);  return; }
  if (cmd == "OUT,UP,0")  { digitalWrite(RELAY_UP, HIGH); return; }
  if (cmd == "OUT,DW,1")  { digitalWrite(RELAY_UP, HIGH); digitalWrite(RELAY_DW, LOW);  return; }
  if (cmd == "OUT,DW,0")  { digitalWrite(RELAY_DW, HIGH); return; }
  if (cmd == "OUT,CW,1")  { digitalWrite(RELAY_CCW, HIGH); digitalWrite(RELAY_CW, LOW); return; }
  if (cmd == "OUT,CW,0")  { digitalWrite(RELAY_CW, HIGH); return; }
  if (cmd == "OUT,CCW,1") { digitalWrite(RELAY_CW, HIGH); digitalWrite(RELAY_CCW, LOW); return; }
  if (cmd == "OUT,CCW,0") { digitalWrite(RELAY_CCW, HIGH); return; }

  // RL exclusifs : OUT,RL,n,1|0|T
  if (cmd.startsWith("OUT,RL,")) {
    int p1 = cmd.indexOf(',', 7);
    if (p1 < 0) return;

    uint8_t idx = cmd.substring(7, p1).toInt();
    if (idx < 1 || idx > 8) return;
    idx--; // 0..7

    String act = cmd.substring(p1 + 1);

    if (act == "1") {
      setRelayExclusive(idx, true);
    } else if (act == "0") {
      allRelaysOff();
      notifyRlState(0);
    } else if (act == "T") {
      toggleRelayExclusive(idx);
    }
    return;
  }
}
