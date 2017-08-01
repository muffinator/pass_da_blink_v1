//#include "nrf.h"
//#include <SPI.h>
//#include "Arduino.h"
//
//#define ssp 23
//#define CE 17
//
//
//
//void nrfFillTx(uint8_t data){
//  digitalWrite(ssp, LOW);
//  SPI.transfer(W_TX_PAYLOAD);
//  SPI.transfer(data);
//  digitalWrite(ssp, HIGH);
//}
//
//void nrfFillTx(uint8_t *data, uint8_t numBytes){
//  digitalWrite(ssp, LOW);
//  SPI.transfer(W_TX_PAYLOAD);
//  for(char i=0;i<numBytes;i++){
//    SPI.transfer(data[i]);
//  }
//  digitalWrite(ssp, HIGH);
//}
//
//void nrfSetRxAddr(uint8_t *data, uint8_t numBytes,uint8_t chan){
//  digitalWrite(ssp, LOW);
//  SPI.transfer((0x20|(10+chan)));
//  for(char i=0;i<numBytes;i++){
//    SPI.transfer(data[i]);
//  }
//  digitalWrite(ssp, HIGH);
//}
//
//void nrfSetTxAddr(uint8_t *data, uint8_t numBytes){
//  digitalWrite(ssp, LOW);
//  SPI.transfer(0x20|TX_ADDR);
//  for(char i=0;i<numBytes;i++){
//    SPI.transfer(data[i]);
//  }
//  digitalWrite(ssp, HIGH);
//}
//
//char nrfRead(int address, int numBytes) {
//  int data[numBytes];
//  digitalWrite(ssp, LOW);
//  int STATUS = 0;
//  STATUS = SPI.transfer(address);
//  for (int i = 0; i < numBytes; i++) {
//    data[i] = SPI.transfer(0);
//  }
//  digitalWrite(ssp, HIGH);
//  Serial.print("status: ");
//  Serial.println(STATUS, BIN);
//  for (int i = 0; i < numBytes; i++) {
//    Serial.print("reg ");
//    Serial.print(address);
//    Serial.print(" - data ");
//    Serial.print(i);
//    Serial.print(": ");
//    Serial.println(data[i], BIN);
//  }
//  return STATUS;
//}
//
//void nrfWrite(char address, unsigned char data) {
//  digitalWrite(ssp, LOW);
//  int STATUS = 0;
//  STATUS = SPI.transfer(0x20 | address);
//  SPI.transfer(data);
//  digitalWrite(ssp, HIGH);
//}
//
//void float2Byte(float val,byte* bytes_array){
//  // Create union of shared memory space
//  union {
//    float float_variable;
//    byte temp_array[4];
//  } u;
//  // Overite bytes of union with float variable
//  u.float_variable = val;
//  // Assign bytes to input array
//  memcpy(bytes_array, u.temp_array, 4);
//}
//
//
//void nrfSend(float datatx){
//  nrfWrite(7, 0x20);
//  float2Byte(datatx,bytetoss);
//  nrfFillTx(bytetoss,4);
//  digitalWrite(CE, HIGH);
//  while(nrfRead(7,1)&0x20!=0x20){}
//  nrfWrite(7,0x20);
//  delay(10);
//  digitalWrite(CE, LOW);
//}
//
//void nrfSendArr(int id, int x, int y, int z, int w)
//{
//  //nrfWrite(7, 0x20);
//  uint8_t sendArray[10] = {(id&0xFF00)>>8, id&0xFF, (x&0xFF00)>>8, x&0xFF, (y&0xFF00)>>8, y&0xFF, (z&0xFF00)>>8, z&0xFF, (w&0xFF00)>>8, w&0xFF};
//  nrfFillTx(sendArray, 10);
//  digitalWrite(CE, HIGH);
//  while(nrfRead(7,1)&0x20!=0x20){}
//  nrfWrite(7,0x20);
//  delay(9); 
//  digitalWrite(CE, LOW);
//}
