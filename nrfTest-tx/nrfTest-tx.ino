#include <SPI.h>
#include <nrf.h>

// set pin 10 as the slave select for the digital pot:
const int ssp = 23; //CSN
const int CE = 17; //CE
int inbyte = 0;
uint8_t test[32];
uint8_t datarx[32];

void setup() {
  // set the slaveSelectPin as an output:
  pinMode(ssp, OUTPUT);
  pinMode(CE, OUTPUT);
  digitalWrite(CE, LOW);
  // initialize SPI:
  delay(200);
  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.endTransaction();
  Serial.begin(115200);
  for(uint8_t i=0;i<32;i++){
    test[i]=i;
  }
  delay(1000);
  nrfWrite(EN_AA,0);//disable autoack(1<<ENAA_P0));  // auto ack pipe0
  nrfWrite(EN_RXADDR,(1<<ERX_P0));  //enable data pipe 0
  nrfWrite(SETUP_AW,(3<<AW));  //5 bite address width
  nrfWrite(SETUP_RETR,((15)<<ARD)|(5<<ARC));  //default 250uS retx delay, 3x retx
  nrfWrite(RF_CH, 2); //channel 2
  nrfWrite(RF_SETUP,(1<<RF_DR_LOW)|(1<<RF_PWR_LOW)|(1<<RF_PWR_HIGH));
  //nrfWrite(6, 0x20); //low data rate, low power (0b100000)
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
  unsigned char blah[3] = {22, 42, 69};
}

void loop() {
  if (Serial.available() > 0) {
    inbyte = Serial.read();
    if (inbyte == 't') {
      //nrfWrite(0xa0,0xaa);
      //nrfWrite(0xa0,0xaa);
      if((nrfRead(7,1)&0x10)==0x10){
        nrfWrite(7,0x10);
        nrfWrite(FLUSH_TX,0);
        Serial.println("MAX RETX");
        digitalWrite(CE, HIGH);
        delay(1);
        digitalWrite(CE, LOW);
      }
      nrfWrite(7, 0x20);
      nrfRead(7, 1);
      nrfFillTx(test,10);
      nrfRead(0x17, 1);
      nrfRead(OBSERVE_TX,1);
      digitalWrite(CE, HIGH);
      delay(1);
      digitalWrite(CE, LOW);
      nrfRead(0x17, 1);
      nrfRead(7, 1);
    }
  }else{
    if((nrfReadq(NRF_CONFIG,1)[0]&(1<<PRIM_RX))!=(1<<PRIM_RX)){
    nrfWrite(NRF_CONFIG, (1<<CRCO)|(1<<PWR_UP)|(1<<PRIM_RX)); //power on, PRX,2-byte crc
    digitalWrite(CE, HIGH);
    }
  }
  delay(10);
}

uint8_t *nrfReadq(unsigned char address, unsigned char numBy){
  digitalWrite(ssp, LOW);
  datarx[numBy] = SPI.transfer(address);
  for(int i=0; i<(numBy);i++){
    datarx[i]=SPI.transfer(0);
  }
  digitalWrite(ssp, HIGH);
  return datarx;
}

void nrfFillTx(uint8_t data){
  digitalWrite(ssp, LOW);
  SPI.transfer(0xa0);
  SPI.transfer(data);
  digitalWrite(ssp, HIGH);
}

void nrfFillTx(uint8_t *data, uint8_t numBytes){
  digitalWrite(ssp, LOW);
  SPI.transfer(0xa0);
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

