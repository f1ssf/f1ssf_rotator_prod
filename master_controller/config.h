// CONFIG.H          //
// F1SSF- 02/01/2025 //
// declaration des variables            //

#pragma once
#define RS485_EN 8

// Definition EEPROM
#include <EEPROM.h>

#define EEPROM_MAGIC 0xA5
#define EEPROM_ADDR_MAGIC   0
#define EEPROM_ADDR_AZ_MIN  1
#define EEPROM_ADDR_AZ_MAX  5
#define EEPROM_ADDR_EL_MIN  9
#define EEPROM_ADDR_EL_MAX  13

// Etats boutons rotator
extern bool up, dw, cw, ccw;

// RX HMI
extern String hmiLine;

// Mesures analogiques valeur du DAC
extern int az_raw;
extern int el_raw;

// variable d'etat statut
extern const char* status_str;

// variable d'état calibration
extern const char* cal_status_str;

// Variables Azimut
extern int az_raw_min;    //valeur du DAC min
extern int az_raw_max;    //valeur du DAC max
extern int az_deg;        //valeur en degrés

// Elevation
extern int el_raw_min;     //valeur du DAC min
extern int el_raw_max;     //valeur du DAC min
extern int el_deg;         //valeur en degrés

// --- Conversion degrés ---
#define ENABLE_DEG 1       // switch entre valeur du DAC et Degres pour debug

// mes bornes mécaniques (seront ajustées plus tard par la calibration)
int az_raw_min = 0004;
int az_raw_max = 1023;
int el_raw_min = 0001;
int el_raw_max = 1023;

// valeurs AZ et EL calculées (NON affichées directement si ENABLE_DEG=0)
int az_deg = 0;
int el_deg = 0;

// ====== AJOUT : ETAT RELAIS + SYNC HMI ======
// 1..8 utilisés, index 0 ignoré.
extern bool relay_state[9];
extern bool relay_dirty[9];

// Couleurs Nextion (format RGB565) envoyées pour chagement de couleur
// Rouge pur = 63488 
// Gris nextion = 50712 
#define RELAY_BCO_ON   63488
#define RELAY_BCO_OFF  50712
#define RELAY_PCO_ON   65535   // texte blanc
#define RELAY_PCO_OFF  0       // texte noir
// ============================================
