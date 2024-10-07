#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

int bitstring = 0;
int brake = 90;

const byte REV9Bit = 0; //parking 0 = parked
const byte REV8Bit = 1; //direction 0 = forward. 1 = not forward
const byte MC1Bit = 2; //coasting. 0 = coast. 1 = accelerate  
const byte MC4Bit = 3; //throttle down 1/2
const byte MC3Bit = 4; //throttle down 3
const byte MC5Bit = 5; //throttle down 4
const byte REV6Bit = 6; //dead man switch

int motorspeed = 0;
int motorsteps;
byte forwardpin = 3;
byte backwardpin = 5;
byte motorpin;

int maxmotorspeed = 155;
int pwmoutput;

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

  //Serial.begin(9600);

}

void loop() {

    /*for (int i = 5; i >=0; i--){
      Serial.print((bitstring>>i)&1);
    }
    Serial.println(" ");*/
    //Serial.println(bitstring);

   if (radio.available()) {
    radio.read(&bitstring, sizeof(bitstring));
   }
   
   if (!(getBit(REV9Bit))){ //check if parked - do nothing
    digitalWrite(forwardpin, 0);
    digitalWrite(backwardpin,0);
    motorspeed = 0;
    return; 
   }


   if (!(getBit(REV8Bit))){ //determine the direction for the motor. 0 = forward 1 = backward
      motorpin = forwardpin;
      digitalWrite(backwardpin,0);   
   }
   else{
      motorpin = backwardpin;
      digitalWrite(forwardpin,0);
   }

  //check brake 
   //brake = bitstring >> 7;

   if(!getBit(REV6Bit)|| (getBit(MC1Bit) && brake < 70)){//dead man switch - EMERGENCY BRAKE. Also check brake and coast for race condition
      motorsteps = -5;
   }


   //now we check if we are accelerating
   else if (getBit(MC5Bit)){//
    motorsteps = 3;    
   }
   else if (getBit(MC3Bit)){
    motorsteps = 2;
   }
   else if (getBit(MC4Bit)){
    motorsteps = 1;
   }

   //check if we are coasting
   else if (!getBit(MC1Bit)){
      if (brake < 30){//CAN ONLY BRAKE WHEN NOT ACCELERATIING
        motorsteps = -3;//Brake deceleration
      }
      else if (brake < 60){
        motorsteps = -2;
      }
      else{
        motorsteps = -1;//Coast deceleration
      }
   }


   

    motorspeed += motorsteps;
   
   if (motorspeed >= maxmotorspeed){
    motorspeed = maxmotorspeed;
   }
   else if (motorspeed < 0){
    motorspeed = 0;
    }
    else if ((motorspeed > 54) && (motorspeed <= 60)){
      motorspeed = 0;
   }
    else if ((motorspeed >= 1) && (motorspeed <= 3)){
      motorspeed = 70;
}

   analogWrite(motorpin, motorspeed);

  delay(100);
}