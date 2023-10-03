#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

const byte MC1Pin = 1; //coast bit. off = coast. on = accelerate. 
const byte REV8Pin = 5; //used to identify direction. 0 = forward. 1 = back
const byte REV9Pin = 6;//park pin. 1 = park 
const byte MC5Pin = 4; //throttle down 4
const byte MC3Pin = 2; //throttle down 3
const byte MC4Pin = 3; //throttle down 1/2
const byte brakePin = 14; //brake switch wired
const byte REV6Pin = 0; //Dead Man Switch connects here (REV6 not used)

byte bitstring = 0;
//bit 0 = parked
//bit 1 = direction
//bit 2 = throttle down 0 (coast)
//bit 3 = throttle down 1/2
//bit 4 = throttle down 3
//bit 5 = throttle down 4
//bit 6 = brake in use
//bit 7 = dead man swch 

const byte REV9Bit = 0; //parking 0 = parked
const byte REV8Bit = 1; //direction 0 = forward. 1 = not forward
const byte MC1Bit = 2; //coasting. 0 = coast. 1 = accelerate  
const byte MC4Bit = 3; //throttle down 1/2
const byte MC3Bit = 4; //throttle down 3
const byte MC5Bit = 5; //throttle down 4
const byte brakeBit = 6; //brake is on
const byte REV6Bit = 7; //dead man switch

void setup() {

  //Serial.begin(9600);
  
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  pinMode(MC1Pin, INPUT);
  pinMode(MC3Pin, INPUT);
  pinMode(MC4Pin, INPUT);
  pinMode(MC5Pin, INPUT);
  pinMode(REV8Pin, INPUT);
  pinMode(REV9Pin, INPUT);
  pinMode(brakePin, INPUT);
  pinMode(REV6Pin, INPUT);


  if (digitalRead(REV9Pin)){//read state of switch 
    setBit(REV9Bit); 
  }

}

void setBit(byte pos){
  bitstring |= (1<<pos);
}
void clearBit(byte pos){
  bitstring &= ~(1<<pos);
}

void loop() {

  //dead man switch
  if (digitalRead(REV6Pin)){ 
    setBit(REV6Bit);
  }
  else{
    clearBit(REV6Bit);
  }

  //parking bit
  if (digitalRead(REV9Pin)){ 
    setBit(REV9Bit);
  }
  else{
    clearBit(REV9Bit);
  }

  //direction bit //FIX THIS
  if (digitalRead(REV8Pin)){ 
    setBit(REV8Bit);
  }
  else{
    clearBit(REV8Bit);
  }

  //brake bit - NEED TO CONVERT TO ANALOG
  if (digitalRead(brakePin)){ 
    setBit(brakeBit);
  }
  else{
    clearBit(brakeBit);
  }

  //Coast
  if (digitalRead(MC1Pin)){ 
    setBit(MC1Bit);
  }
  else{
    clearBit(MC1Bit);
  }

  //throttle down 2
  if (digitalRead(MC4Pin)){ 
    setBit(MC4Bit);
  }
  else{
    clearBit(MC4Bit);
  }

   //throttle down 3
  if (digitalRead(MC3Pin)){ 
    setBit(MC3Bit);
  }
  else{
    clearBit(MC3Bit);
  }
//throttle down 4
  if (digitalRead(MC5Pin)){ 
    setBit(MC5Bit);
  }
  else{
    clearBit(MC5Bit);
  }

  //Serial.println(digitalRead(REV8Pin));

  
  radio.write(&bitstring, sizeof(bitstring));
  delay(100);
  
}
