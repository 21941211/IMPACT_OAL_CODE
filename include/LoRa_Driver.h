#ifndef LORA_H
#define LORA_H

#include "Arduino.h"
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <debug.h>
#include <pinmapping.h>
#include "DataProcessing_Driver.h"
#include "SPI_Driver.h"
#include <bits/stdc++.h> 
#include <sstream>
#include "SD_Driver.h"
#include "DeepSleep_Driver.h"
#include "DHT22_Driver.h"


// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 60;

// LoRa keys
static u1_t DEVID[1];
static u1_t APPEUI[8];
static u1_t DEVEUI[8];
static u1_t APPKEY[16];


extern RTC_DATA_ATTR unsigned int LoRaTX_Complete;
extern uint8_t NoJoin;


void printHex2(unsigned v);
void onEvent (ev_t ev);
void do_send(osjob_t* j);
void LoRaSetup(void);
void os_getArtEui (u1_t* buf);
void os_getDevEui (u1_t* buf) ;
void os_getDevKey (u1_t* buf);
void setMyData(float temp, float humid);

#endif