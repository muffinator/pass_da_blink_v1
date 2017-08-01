#include <whoasense.h>
#include "whoaButton.h"
#include "nrf.h"
#include <SPI.h>
#include "Arduino.h"

#define ssp 23
#define CE 17
#define CHANNEL 100

uint8_t rxaddr[5] = {0xA5, 0xA5, 0xA5, 0xA5, 0xA5};
uint8_t bytetoss[4];
// If you want to add some custom functionality, to this script, put it in this function
void doSomeOtherNonsense() { return; }

bool toLog = false;
void setup() {

  Serial.begin(115200); 
  
  initWhoaBoard();

  ENABLE_logOnTransition = false;
  whoaConfig.ENABLE_logging = false;
  toLog = whoaConfig.ENABLE_logging || ENABLE_logOnTransition;


  nrfWrite(EN_AA,(1<<ENAA_P0));  // auto ack pipe0,1
  nrfWrite(EN_RXADDR,(1<<ERX_P0));  //enable data pipe 0,1
  nrfWrite(SETUP_AW,(3<<AW));  //5 bite address width
  nrfWrite(SETUP_RETR,((5)<<ARD)|(5<<ARC));  //default 250uS retx delay, 4x retx
  nrfSetRxAddr(rxaddr,5,0);
  nrfSetTxAddr(rxaddr,5);
  nrfRead(RX_ADDR_P0,5);
  nrfRead(TX_ADDR,5);
  nrfWrite(RF_CH, CHANNEL); //channel 2
  nrfWrite(RF_SETUP,(1<<RF_DR_LOW)|(1<<RF_PWR_LOW)|(1<<RF_PWR_HIGH)); //low data rate, low power (0b100000)
  //nrfWrite(0x1c,0x01); //enable dynamic payload data pipe 0
  //nrfWrite(0x1d,0x06); //enable dynamic payload with ack
  nrfWrite(NRF_CONFIG, (1<<CRCO)|(1<<PWR_UP)|(0<<PRIM_RX)); //power on, PTX,2-byte crc
  nrfRead(RF_SETUP, 1);
  nrfWrite(NRF_STATUS, (1<<MAX_RT)); //clear MAX_RT IRQ
  nrfRead(0x00, 1);
  //nrfFillTx(test,32);
  nrfRead(0x17, 1);
  nrfWrite(FLUSH_TX, 0); //clear tx fifo
  nrfRead(0x17, 1);
  //nrfWrite(0xa0,0xaa);
  nrfRead(0x17, 1);
}

int blinkState[] = {500, 500, 500, 500};

unsigned long t; 
unsigned long prev; 

void loop() {
    senseAll(1700, true);

    updateTouchState();
/*
    for (int i = 0; i < 4; i++) {
      if (blinkState[i] > 0) 
      {
        blinkState[i]--;
        if ((blinkState[i]) % (blinkState[i] / 100 + 2) == 0 && blinkState[i] < 500) 
        {
          glow[i] = !glow[i];
        }
      }
      else 
      {
        glow[i] = false;
      }
    }
  */
  glow[0]= true;
  glow[1]= true;
  glow[2]= true;
  glow[3]= true;

  

    if (isTouched && switchedCount == 0) { 
      isTouched = false;
      for (int chan = 0; chan < channelCount; chan++) {
        if (whereTouched[chan]) { 
          whereTouched[chan] = false;
          Serial.println(blinkState[chan]);
          if (blinkState[chan] > 0) 
          {
            //glow[chan] = false;
            blinkState[chan] = 0;
          }
          else if (blinkState[chan] == 0) 
          {
            //glow[chan] = true;
            blinkState[chan] = 600;
          }
        }
      }

      switchedCount = whoaConfig.rawSenseSize + 
                      whoaConfig.senseHistorySize + 
                      whoaConfig.sortedRawWindowSize;
    }
//
    if (whoaConfig.ENABLE_logging) { 
       Serial.println(logBuffer);
    }
    

    


    // Doing this twice helps with sensitivity, for some arcane timing reason.  
    disableELSupply_withSync();
    switchOutputs(glow);
    enableELSupply();
    disableELSupply_withSync();
    switchOutputs(glow);
    enableELSupply();

    if (switchedCount > 0) {
      switchedCount--;
    }

}



void nrfFillTx(uint8_t data){
  digitalWrite(ssp, LOW);
  SPI.transfer(W_TX_PAYLOAD);
  SPI.transfer(data);
  digitalWrite(ssp, HIGH);
}

void nrfFillTx(uint8_t *data, uint8_t numBytes){
  digitalWrite(ssp, LOW);
  SPI.transfer(W_TX_PAYLOAD);
  for(char i=0;i<numBytes;i++){
    SPI.transfer(data[i]);
  }
  digitalWrite(ssp, HIGH);
}

void nrfSetRxAddr(uint8_t *data, uint8_t numBytes,uint8_t chan){
  digitalWrite(ssp, LOW);
  SPI.transfer((0x20|(10+chan)));
  for(char i=0;i<numBytes;i++){
    SPI.transfer(data[i]);
  }
  digitalWrite(ssp, HIGH);
}

void nrfSetTxAddr(uint8_t *data, uint8_t numBytes){
  digitalWrite(ssp, LOW);
  SPI.transfer(0x20|TX_ADDR);
  for(char i=0;i<numBytes;i++){
    SPI.transfer(data[i]);
  }
  digitalWrite(ssp, HIGH);
}

char nrfRead(int address, int numBytes) {
  int data[numBytes];
  digitalWrite(ssp, LOW);
  int STATUS = 0;
  STATUS = SPI.transfer(address);
  for (int i = 0; i < numBytes; i++) {
    data[i] = SPI.transfer(0);
  }
  digitalWrite(ssp, HIGH);
  Serial.print("status: ");
  Serial.println(STATUS, BIN);
  for (int i = 0; i < numBytes; i++) {
    Serial.print("reg ");
    Serial.print(address);
    Serial.print(" - data ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(data[i], BIN);
  }
  return STATUS;
}

void nrfWrite(char address, unsigned char data) {
  digitalWrite(ssp, LOW);
  int STATUS = 0;
  STATUS = SPI.transfer(0x20 | address);
  SPI.transfer(data);
  digitalWrite(ssp, HIGH);
}

void float2Byte(float val,byte* bytes_array){
  // Create union of shared memory space
  union {
    float float_variable;
    byte temp_array[4];
  } u;
  // Overite bytes of union with float variable
  u.float_variable = val;
  // Assign bytes to input array
  memcpy(bytes_array, u.temp_array, 4);
}


void nrfSend(float datatx){
  nrfWrite(7, 0x20);
  float2Byte(datatx,bytetoss);
  nrfFillTx(bytetoss,4);
  digitalWrite(CE, HIGH);
  while(nrfRead(7,1)&0x20!=0x20){}
  nrfWrite(7,0x20);
  delay(10);
  digitalWrite(CE, LOW);
}

void nrfSendArr(int id, int x, int y, int z, int w)
{
  //nrfWrite(7, 0x20);
  uint8_t sendArray[10] = {(id&0xFF00)>>8, id&0xFF, (x&0xFF00)>>8, x&0xFF, (y&0xFF00)>>8, y&0xFF, (z&0xFF00)>>8, z&0xFF, (w&0xFF00)>>8, w&0xFF};
  nrfFillTx(sendArray, 10);
  digitalWrite(CE, HIGH);
  while(nrfRead(7,1)&0x20!=0x20){}
  nrfWrite(7,0x20);
  delay(9); 
  digitalWrite(CE, LOW);
}
