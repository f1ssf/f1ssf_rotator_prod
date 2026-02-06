// HMI_MANAGER.H     //
// F1SSF- 02/01/2025 //
// Pilotage du Nextion  //

#pragma once
#include "config.h"
#include "rs485_manager.h"

// Prototype de fonction //
void eepromSaveCalibration();
bool eepromLoadCalibration();
void handleHMI(const String &cmd);

// Variables    //
const char* cal_status_str = "";
bool up=false, dw=false, cw=false, ccw=false;

// Buffer de ligne. //
String hmiLine = "";

// ====== AJOUT : ETAT RELAIS + SYNC HMI ====== //
bool relay_state[9] = {0};
bool relay_dirty[9] = {0};

// Envoi commande Nextion (terminaison 0xFF 0xFF 0xFF)
static inline void hmiSendCmd(const String &s) {
  Serial3.print(s);
  Serial3.write(0xFF);
  Serial3.write(0xFF);
  Serial3.write(0xFF);
}

// Mise à jour visuelle d'un relais côté Nextion
static void hmiApplyRelayVisual(uint8_t idx) {
  if (idx < 1 || idx > 8) return;

  // bRL est le nom des boutons Nextion
  String obj = "bRL" + String(idx);

  // Assure la cohérence du toggle (val=0/1)
  hmiSendCmd(obj + ".val=" + String(relay_state[idx] ? 1 : 0));

  // Couleur fond (bco) + couleur texte (pco)
  if (relay_state[idx]) {
    hmiSendCmd(obj + ".bco=" + String(RELAY_BCO_ON));
    hmiSendCmd(obj + ".pco=" + String(RELAY_PCO_ON));
  } else {
    hmiSendCmd(obj + ".bco=" + String(RELAY_BCO_OFF));
    hmiSendCmd(obj + ".pco=" + String(RELAY_PCO_OFF));
  }
}
// ============================================


// ====== AJOUT RELAIS (MASTER) ======
static inline bool _isDigitStr(const String &s) {
  if (s.length() == 0) return false;
  for (uint16_t i = 0; i < s.length(); i++) {
    if (s[i] < '0' || s[i] > '9') return false;
  }
  return true;
}

static inline int _parseRelayIndexToken(const String &tok) {
  // Accepte: "RL1", "bRL1"
  if (tok.startsWith("bRL")) return tok.substring(3).toInt();
  if (tok.startsWith("RL"))  return tok.substring(2).toInt();
  return -1;
}

static void sendRelayCmd(int idx, char action) {
  // idx: 1..8 ; action: '1','0','T'
  if (idx < 1 || idx > 8) return;

  // Format pour le REMOTE : OUT,RL,<idx>,<action>
  char buf[24];
  snprintf(buf, sizeof(buf), "OUT,RL,%d,%c", idx, action);
  rs485Send(buf);

  // ===== CORRECTION  : OFF IMPLICITE =====
  // Si on toggle un relais déjà ON, on force l'OFF visuel immédiatement
  if (action == 'T' && relay_state[idx] == true) {

  relay_state[idx] = false;
  relay_dirty[idx] = true;

}
// =============================================


  // Statut (simple)
  static char st[16];
  snprintf(st, sizeof(st), "RL%d %c", idx, action);
  status_str = st;

  // IMPORTANT : on NE change PAS la couleur ici.
  // On attend le retour REMOTE "STATE,<idx>" pour synchroniser l'affichage.
}

static bool handleRelayHMI(const String &cmd) {
  // Formats acceptés (dans le frame entre < >):
  // 1) "RL,<n>,<action>"  action: 1/0/T/ON/OFF/TOG
  // 2) "BTN,RL1,<action>" action: ON/OFF/TOG/P/R
  // 3) "BTN,bRL1,<action>" idem

  // --- Cas "RL,<n>,<action>" ---
  if (cmd.startsWith("RL,")) {
    int p1 = cmd.indexOf(',', 3);
    if (p1 < 0) return false;

    String sIdx = cmd.substring(3, p1);
    String sAct = cmd.substring(p1 + 1);
    sAct.trim();

    if (!_isDigitStr(sIdx)) return false;
    int idx = sIdx.toInt();
    if (idx < 1 || idx > 8) return false;

    char action = 0;
    if (sAct == "1" || sAct == "ON") action = '1';
    else if (sAct == "0" || sAct == "OFF") action = '0';
    else if (sAct == "T" || sAct == "TOG" || sAct == "TOGGLE") action = 'T';
    else return false;

    sendRelayCmd(idx, action);
    return true;
  }

  // --- Cas "BTN,<token>,<action>" ---
  if (cmd.startsWith("BTN,")) {
    int p1 = cmd.indexOf(',', 4);
    if (p1 < 0) return false;

    String tok = cmd.substring(4, p1); // "RL1" ou "bRL1"
    String act = cmd.substring(p1 + 1);
    act.trim();

    int idx = _parseRelayIndexToken(tok);
    if (idx < 1 || idx > 8) return false;

    // Nextion: P => action, R => release
    if (act == "P") {
      sendRelayCmd(idx, 'T');
      return true;
    }
    if (act == "R") {
      return true;
    }

    char action = 0;
    if (act == "1" || act == "ON") action = '1';
    else if (act == "0" || act == "OFF") action = '0';
    else if (act == "T" || act == "TOG" || act == "TOGGLE") action = 'T';
    else return false;

    sendRelayCmd(idx, action);
    return true;
  }

  return false;
}
// ====== FIN AJOUT RELAIS ======


void readHMI() {
  static bool inFrame = false;
  static String frame = "";

  while (Serial3.available()) {
    char c = Serial3.read();

    if (c == '<') {
      frame = "";
      inFrame = true;
    }
    else if (c == '>') {
      if (inFrame && frame.length() > 0) {
        handleHMI(frame);
      }
      inFrame = false;
    }
    else if (inFrame) {
      if (c >= 32 && c <= 126) {
        frame += c;
      }
    }
  }
}

void handleHMI(const String &cmd) {
  Serial.print("HMI: ");
  Serial.println(cmd);

  // RELAIS
  if (handleRelayHMI(cmd)) {
    return;
  }

  // UP
  if (cmd == "BTN,UP,P")  {
    up = true;
    status_str = "UP";
    rs485Send("OUT,UP,1");
  }
  if (cmd == "BTN,UP,R")  {
    up = false;
    status_str = "READY";
    rs485Send("OUT,UP,0");
  }

  // DW
  if (cmd == "BTN,DW,P")  {
    dw = true;
    status_str = "DW";
    rs485Send("OUT,DW,1");
  }
  if (cmd == "BTN,DW,R")  {
    dw = false;
    status_str = "READY";
    rs485Send("OUT,DW,0");
  }

  // CW
  if (cmd == "BTN,CW,P")  {
    cw = true;
    status_str = "CW";
    rs485Send("OUT,CW,1");
  }
  if (cmd == "BTN,CW,R")  {
    cw = false;
    status_str = "READY";
    rs485Send("OUT,CW,0");
  }

  // CCW
  if (cmd == "BTN,CCW,P")  {
    ccw = true;
    status_str = "CCW";
    rs485Send("OUT,CCW,1");
  }
  if (cmd == "BTN,CCW,R")  {
    ccw = false;
    status_str = "READY";
    rs485Send("OUT,CCW,0");
  }

  if (cmd == "BTN,STOP,P") {
  up = dw = cw = ccw = false;
  rs485Send("OUT,ALL,0");

  // ===== FORCER EXTINCTION VISUELLE RELAIS =====
  for (uint8_t i = 1; i <= 8; i++) {
    if (relay_state[i]) {
      relay_state[i] = false;
      relay_dirty[i] = true;
    }
  }
  // ============================================

  status_str = "STOP";
  Serial.println("STOP");
}


  // ===== CALIBRATION =====
  if (cmd == "CAL,AZ,MIN") {
    az_raw_min = az_raw;
    cal_status_str = "CAL AZ MIN";
    return;
  }
  if (cmd == "CAL,AZ,MAX") {
    az_raw_max = az_raw;
    cal_status_str = "CAL AZ MAX";
    return;
  }
  if (cmd == "CAL,EL,MIN") {
    el_raw_min = el_raw;
    cal_status_str = "CAL EL MIN";
    return;
  }
  if (cmd == "CAL,EL,MAX") {
    el_raw_max = el_raw;
    cal_status_str = "CAL EL MAX";
    return;
  }
  if (cmd == "CAL,SAVE") {
    eepromSaveCalibration();
    cal_status_str = "CAL SAVED";
    return;
  }
  if (cmd == "CAL,ABORT") {
    cal_status_str = "CAL ABORT";
    return;
  }
}

void hmiUpdateValues() {

  // ===== AZ =====
  Serial3.print("tAzVal.txt=\"");
#if ENABLE_DEG
  Serial3.print(az_deg);
#else
  Serial3.print(az_raw);
#endif
  Serial3.print("\"");
  Serial3.write(0xFF); Serial3.write(0xFF); Serial3.write(0xFF);

  // ===== EL =====
  Serial3.print("tElVal.txt=\"");
#if ENABLE_DEG
  Serial3.print(el_deg);
#else
  Serial3.print(el_raw);
#endif
  Serial3.print("\"");
  Serial3.write(0xFF); Serial3.write(0xFF); Serial3.write(0xFF);

  // ===== STATUS =====
  Serial3.print("tStatus.txt=\"");
  Serial3.print(status_str);
  Serial3.print("\"");
  Serial3.write(0xFF); Serial3.write(0xFF); Serial3.write(0xFF);

  // ===== CAL STATUS =====
  Serial3.print("tCalStatus.txt=\"");
  Serial3.print(cal_status_str);
  Serial3.print("\"");
  Serial3.write(0xFF); Serial3.write(0xFF); Serial3.write(0xFF);

  // ===== AZ GAUGE =====
#if ENABLE_DEG
  Serial3.print("n0.val="); Serial3.print(az_deg);
#else
  Serial3.print("n0.val="); Serial3.print(az_raw);
#endif
  Serial3.write(0xFF); Serial3.write(0xFF); Serial3.write(0xFF);

  // ===== EL GAUGE =====
#if ENABLE_DEG
  Serial3.print("n2.val="); Serial3.print(el_deg);
#else
  Serial3.print("n2.val="); Serial3.print(el_raw);
#endif
  Serial3.write(0xFF); Serial3.write(0xFF); Serial3.write(0xFF);

  // ====== AJOUT : PUSH RELAIS SI "dirty" ======
  for (uint8_t i = 1; i <= 8; i++) {
    if (relay_dirty[i]) {
      hmiApplyRelayVisual(i);
      relay_dirty[i] = false;
    }
  }
  // ============================================
}

// Sauvegarde des parametres calibration
void eepromSaveCalibration() {
  EEPROM.update(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
  EEPROM.put(EEPROM_ADDR_AZ_MIN, az_raw_min);
  EEPROM.put(EEPROM_ADDR_AZ_MAX, az_raw_max);
  EEPROM.put(EEPROM_ADDR_EL_MIN, el_raw_min);
  EEPROM.put(EEPROM_ADDR_EL_MAX, el_raw_max);
}

bool eepromLoadCalibration() {
  if (EEPROM.read(EEPROM_ADDR_MAGIC) != EEPROM_MAGIC) {
    return false;
  }
  EEPROM.get(EEPROM_ADDR_AZ_MIN, az_raw_min);
  EEPROM.get(EEPROM_ADDR_AZ_MAX, az_raw_max);
  EEPROM.get(EEPROM_ADDR_EL_MIN, el_raw_min);
  EEPROM.get(EEPROM_ADDR_EL_MAX, el_raw_max);
  return true;
}
