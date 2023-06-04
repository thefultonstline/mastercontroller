#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

const byte MC1Pin; //coast bit. off = coast. on = accelerate. 
const byte REV8Pin; //used to identify direction
const byte REV9Pin;//park pin. 0 = park 
const byte MC5Pin; //throttle down 4
const byte MC3Pin; //throttle down 3
const byte MC4Pin; //throttle down 2
uint16_t bitstring = 0;
//bits 0-7 = brake
//bit 8 = parked
//bit 9 = direction
//bit 10 = throttle down 0
//bit 12 = throttle down 2
//bit 13 = throttle down 3
//bit 14 = throttle down 4

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  pinMode(MC1Pin, INPUT_PULLUP);
  pinMode(MC3Pin, INPUT_PULLUP);
  pinMode(MC4Pin, INPUT_PULLUP);
  pinMode(MC5Pin, INPUT_PULLUP);
  pinMode(REV8Pin, INPUT_PULLUP);
  pinMode(REV9Pin, INPUT_PULLUP);

  pinMode(A0, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(MC1Pin), MC1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MC3Pin), MC3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MC4Pin), MC4, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MC5Pin), MC5, CHANGE);
  attachInterrupt(digitalPinToInterrupt(REV9Pin), Park, CHANGE);
  attachInterrupt(digitalPinToInterrupt(REV8Pin), Direction, CHANGE);
}

void loop() {
  bitstring ^= analogRead(A0);
  radio.write(&bitstring, sizeof(bitstring));
}

void MC1(){
  bitstring ^= 1024;//bit 10 // coast
} 

void MC3(){
  bitstring ^= 2048;//bit 11 throttle down 3
}

void MC4(){
  bitstring ^= 1024;//bit 10 throttle down 2
}

void MC5(){
  bitstring ^= 4096;//bit 12 throttle down 4
}

void Park(){
  bitstring ^= 16384;//bit 14
}

void Direction(){
  bitstring ^= 8192;//bit 13
}
