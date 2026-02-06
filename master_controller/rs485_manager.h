// RS485.H           //
// F1SSF- 02/01/2025 //
// Gestion du procotole et parser //

#pragma once
#include "config.h"

static inline void rs485_tx_on() { digitalWrite(RS485_EN, HIGH); }
static inline void rs485_rx_on() { digitalWrite(RS485_EN, LOW);  }

void rs485Send(const char *cmd) {
  rs485_tx_on();
  delay(1);
  Serial1.print(cmd);
  Serial1.print("\r\n");
  Serial1.flush();
  delay(1);
  rs485_rx_on();
}

// Parse tokens simples : "STATE,6" ou "STATE,6,1" (supporté aussi)
static inline bool parseStateLine(const char *line, uint8_t &idx, int &val, bool &hasVal) {
  // attend "STATE,"
  if (strncmp(line, "STATE,", 6) != 0) return false;

  const char *p = line + 6;
  idx = (uint8_t)atoi(p);
  if (idx < 1 || idx > 8) return false;

  hasVal = false;
  val = 0;

  const char *c = strchr(p, ',');
  if (c) {
    // Format optionnel: STATE,<n>,<0/1>
    hasVal = true;
    val = atoi(c + 1);
  }
  return true;
}

void readRS485Debug() {
  static char buf[64];
  static uint8_t idx = 0;

  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\r' || c == '\n') {
      if (idx > 0) {
        buf[idx] = '\0';

        // DEBUG EXISTANT
        Serial.print("RS485 TXT: ");
        Serial.println(buf);

        // ===== NOUVEAU : STATE,<n> (sync relais) =====
        uint8_t r = 0;
        int v = 0;
        bool hasVal = false;

        if (parseStateLine(buf, r, v, hasVal)) {

          // ===== CORRECTION : STATE,n = TOGGLE EXCLUSIF =====

          // Cas 1 : le relais était OFF → il passe ON, tous les autres OFF
          if (relay_state[r] == false) {

           for (uint8_t i = 1; i <= 8; i++) {
           if (i == r) {
           relay_state[i] = true;
           relay_dirty[i] = true;
          } else {
         if (relay_state[i]) {
        relay_state[i] = false;
        relay_dirty[i] = true;
      }
    }
  }

}
// Cas 2 : le relais était ON → il passe OFF (STOP local implicite)
else {

  relay_state[r] = false;
  relay_dirty[r] = true;

}
// ===============================================


          // Statut simple
          static char st[16];
          snprintf(st, sizeof(st), "STATE RL%d", r);
          status_str = st;

          // On ne return pas forcément, on peut laisser ANA continuer si besoin
        }

        // ===== ANA,az,el =====
        if (strncmp(buf, "ANA,", 4) == 0) {
          char *p = buf + 4;
          az_raw = atoi(p);

          p = strchr(p, ',');
          if (p) {
            el_raw = atoi(p + 1);
          }

#if ENABLE_DEG
          az_deg = constrain(
            map(az_raw, az_raw_min, az_raw_max, 0, 360),
            0, 360
          );

          el_deg = constrain(
            map(el_raw, el_raw_min, el_raw_max, 0, 90),
            0, 90
          );
#endif
        }

        idx = 0;
      }
    }
    else if (c >= 32 && c <= 126) {
      if (idx < sizeof(buf) - 1) {
        buf[idx++] = c;
      } else {
        idx = 0;
      }
    }
  }
}
