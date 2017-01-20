#include "mpr121.h"
#include <Wire.h>

int irqpin = 2;  // Digital 2
int resetButton = 11;
int numOfLights = 8;
int taserPin0 = A0;
int taserPin1 = A1;
int taserPin2 = A2;
boolean touchStates[4]; //to keep track of the previous touch states
boolean reset;

void setup(){
  pinMode(irqpin, INPUT);
  pinMode(resetButton, INPUT);
    
  for (int i = 3; i < 11; i++){
    pinMode(i, OUTPUT);
  }
  digitalWrite(irqpin, HIGH); //enable pullup resistor

  pinMode(taserPin0, OUTPUT);
  pinMode(taserPin1, OUTPUT);
  pinMode(taserPin2, OUTPUT);
  
  Serial.begin(9600);
  Wire.begin();

  mpr121_setup();
}

void loop(){

  int buttonState = 0;
  
  buttonState = digitalRead(resetButton);
  
  if (buttonState == HIGH) {
    numOfLights = 8;
    Serial.println("Pressed");
  }

  
  for (int i = 3; i < (numOfLights+ 3); i ++){
    digitalWrite(i, HIGH);
  }
  
    
  boolean hit = readTouchInputs();
    
  if ( hit == true ){
    shock(); 
  }
  
  hit = false;
  
  for (int i = (numOfLights+3); i < 11; i++){
    digitalWrite(i, LOW);
  }
  
  if (numOfLights == 0){
    boolean fin = false;
    while (fin == false) {
      for (int i = 3; i < 11; i++) {
        digitalWrite(i, HIGH);
      }
      delay(300);
      for (int i = 3; i < 11; i++){
        digitalWrite(i, LOW);
      }
      delay(300);
      buttonState = digitalRead(resetButton);
      if (buttonState == HIGH) {
        fin = true;
        numOfLights = 8;
      }
    }
  }
}

void shock(){
  digitalWrite(taserPin0, HIGH);
  digitalWrite(taserPin1, HIGH);
  digitalWrite(taserPin2, HIGH);
  delay(500);
  digitalWrite(taserPin0, LOW);
  digitalWrite(taserPin1, LOW);
  digitalWrite(taserPin2, LOW);
}


boolean readTouchInputs(){
  boolean hit;
  if(!checkInterrupt()){
    
    //read the touch state from the MPR121
    Wire.requestFrom(0x5A,2); 
    
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states

    
    for (int i=0; i < 4; i++){  // Check what electrodes were pressed
      if(touched & (1<<i)){
      
        if(touchStates[i] == 0){
           hit = true;
           Serial.println("hit");
           shock();
           numOfLights = numOfLights - 1;
           Serial.println(numOfLights);
        }
        touchStates[i] = 1; 
        hit = false;    
      }else{
        if(touchStates[i] == 1){
          hit = false;
       }
        touchStates[i] = 0;
      }
    
    }
    
  }
  return hit;
}




void mpr121_setup(void){

  set_register(0x5A, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, TOU_THRESH);
  set_register(0x5A, ELE0_R, REL_THRESH);
 
  set_register(0x5A, ELE1_T, TOU_THRESH);
  set_register(0x5A, ELE1_R, REL_THRESH);
  
  set_register(0x5A, ELE2_T, TOU_THRESH);
  set_register(0x5A, ELE2_R, REL_THRESH);
  
  set_register(0x5A, ELE3_T, TOU_THRESH);
  set_register(0x5A, ELE3_R, REL_THRESH);
  
  set_register(0x5A, ELE4_T, TOU_THRESH);
  set_register(0x5A, ELE4_R, REL_THRESH);
  
  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);
  
  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);
  
  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);
  
  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);
  
  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);
  
  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);
  
  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(0x5A, ATO_CFG0, 0x0B);
  set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(0x5A, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V
  
  set_register(0x5A, ELE_CFG, 0x0C);
  
}


boolean checkInterrupt(void){
  return digitalRead(irqpin);
}


void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}
