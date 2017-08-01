#include <whoasense.h>
#include "whoaButton.h"

float t = 0;
int blinkState[] = {0,0,0,0};
int tstate[] = {0,0,0,0};
int mblink[] = {3,0,1,2};
int pblink[] = {1,2,3,0};
int dblink[] = {2,3,0,1};

void setup() {
  // This makes it possible to send messages back to the computer from the board
  // Not strictly necessary for this example, but nice to have around for debugging
  //Serial.begin(9600); 

  // This initializes the hardware configuration of the board
  initWhoaBoard();

  // Sanity check, we will be using the Glow array to turn the EL channels on and off
  for (int i = 0; i < 4; i++) { 
    glow[i] = true; 
  }
}

// We will be alternating between having the EL channels on and off
// this controls the total time of the cycle.
//
// Beware, if you set this to something larger than 32,767, 
// it will overflow! https://www.arduino.cc/en/Reference/int
int period_us = 4000;
// This steps the dutyCycle up to demonstrate how the brightness can be faded
int stepSize_us = 25;

// This temporary variable keeps track of the time on (in microseconds) 
int dutyCycle_us = 0;

void loop() {

//  switchOutputs(glow);
//  delayMicroseconds(dutyCycle_us);
//  switchOutputs(noGlow);
//  delayMicroseconds(period_us - dutyCycle_us);
//  //Serial.println(dutyCycle_us);
//  dutyCycle_us = 2000+2000*cos(t);
//
//  t+=.001;
//  if(t>(2*PI)) {t=0;}
//
//  if((t>(PI+PI/2))||(t<(PI/2))){
//    t+=.03;
//
//  
    senseAll(1700, true);

    updateTouchState();

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
            blinkState[chan] = 150;
            tstate[chan] = 1;
          }
        }
      }

      switchedCount = whoaConfig.rawSenseSize + 
                      whoaConfig.senseHistorySize + 
                      whoaConfig.sortedRawWindowSize;
    }

    disableELSupply_withSync();
    switchOutputs(glow);
    enableELSupply();
    disableELSupply_withSync();
    switchOutputs(glow);
    enableELSupply();

    if (switchedCount > 0) {
      switchedCount--;
    }
    
//  }else{
//    t+=.01;
//      //senseAll(0, false);
//  }

    for (int i = 0; i < 4; i++) {
//      glow[i] = true;
//      switchOutputs(glow);
//      delay(1000);
//      glow[i] = false;
//      switchOutputs(glow);
//      delay(1000);
      if (blinkState[i] > 0) 
      {
        blinkState[i]-= 5;
        //glow[i] = false;
        if ((blinkState[i]) % (100) == 0 && blinkState[i] < 150) 
        {
          glow[i] = !glow[i];
        }
        if((blinkState[i] < 150)&&(tstate[i] == 1)){
          tstate[i]=0;
          blinkState[mblink[i]] = 170;
          blinkState[pblink[i]] = 170;
          blinkState[dblink[i]] = 250;
        }
      }
      else 
      {
        glow[i] = true;
      }
    }
}
