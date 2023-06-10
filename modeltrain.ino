#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

byte bitstring = 0;

const byte REV9Bit = 0; //parking 0 = parked
const byte REV8Bit = 1; //direction 0 = forward. 1 = not forward
const byte MC1Bit = 2; //coasting. 0 = coast. 1 = accelerate  
const byte MC4Bit = 3; //throttle down 1/2
const byte MC3Bit = 4; //throttle down 3
const byte MC5Bit = 5; //throttle down 4

byte motorspeed = 0;
int motorsteps;
byte forwardpin = 3;
byte backwardpin = 5;
byte currentthrottle;
byte brakevalue;
byte motorpin;

byte maxmotorspeed = 155;

bool getBit(byte bitval){
  return ((bitstring >> bitval) & 1);
}


void setup() {
  // put your setup code here, to run once:
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  pinMode(forwardpin, OUTPUT);
  pinMode(backwardpin, OUTPUT);

  Serial.begin(9600);

}

void loop() {

    for (int i = 5; i >=0; i--){
      Serial.print((bitstring>>i)&1);
    }
    Serial.println(" ");

   if (radio.available()) {
    radio.read(&bitstring, sizeof(bitstring));
   }
   
   if (!(getBit(REV9Bit))){ //if parked
    digitalWrite(forwardpin, 0);
    digitalWrite(backwardpin,0);
    return; 
   }

   if (!(getBit(REV8Bit))){ //determine the direction for the motor. 0 = forward
      motorpin = forwardpin;
      digitalWrite(backwardpin,0);   
   }
   else{
      motorpin = backwardpin;
      digitalWrite(forwardpin,0);
   }

   if (getBit(MC5Bit)){
      motorsteps = 3;    
   }
   else if (getBit(MC3Bit)){
    motorsteps = 2;
   }
   else if (getBit(MC4Bit)){
    motorsteps = 1;
   }
   else if (getBit(MC1Bit)){
   // motorsteps = -1;
   }


   if (motorspeed < 155 && motorspeed >= 0 && getBit(MC1Bit)){
    motorspeed += motorsteps;
   }

  else if (motorspeed > 0 && !getBit(MC1Bit)){
    motorspeed = (motorspeed - 1);
   }

   analogWrite(motorpin, motorspeed);
       Serial.println(motorspeed);


  delay(100);
}
