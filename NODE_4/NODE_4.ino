/*pins 0 - 1 are usedfor Rx and Tx no trigger pin required due to using
  TTL to RS485 Module Serial Port MCU Automatic Flow Control Module.
  
  simple servo cmri node which moves 1 servo and the sends back a bit back to jmri to indicate
  the servo has been moved
  also will control lights on the scene
  it will also read 5 sensors/switchs
  
*/
#include <CMRI.h>
#include <Servo.h>

// define node address each node needs its own address
#define CMRI_ADDR 4

// define turnout required positions each turnout will need calibrating
#define turnout1ClosedPosition 108
#define turnout1ThrownPosition 91

#define lightControl 4                            //jmri 4002


// Spare pins at the moment 

//define infrared detection T4 as inputs
#define Sensor1 A0                                //jmri 4002
#define Sensor2 A1                                //jmri 4003
#define Sensor3 A2                                //jmri 4004
#define Sensor4 A3                                //jmri 4005
#define Sensor5 A4                                //jmri 4005

//setup themove speed of servo
#define turnoutMoveSpeed 20                       // [ms] lower number is faster

int t1state = 0;                                  //0 = closed 1 = thrown
int turnout1 = 0;
unsigned long turnoutMoveDelay;

CMRI cmri(CMRI_ADDR, 24, 48);                     // defaults to a SMINI with address 0. SMINI = 24 inputs, 48 outputs
Servo turnOut1;

byte turnout1Position = turnout1ClosedPosition;
byte turnout1Target   = turnout1ClosedPosition;

void setup() {
  delay(2000);
  //setup output pins
  pinMode(lightControl, OUTPUT);

  //setup input pins
  pinMode(Sensor1, INPUT_PULLUP);
  pinMode(Sensor2, INPUT_PULLUP);
  pinMode(Sensor3, INPUT_PULLUP);
  pinMode(Sensor4, INPUT_PULLUP);
  pinMode(Sensor5, INPUT_PULLUP);



  turnOut1.attach(2);
  turnOut1.write(turnout1ClosedPosition);

  Serial.begin(19200);
}

void loop() {
  cmri.process();
  turnout1 = (cmri.get_bit(0));                                             //turnout 4001 jmri address

  if (turnout1 == 1 && t1state == 0) {
    turnout1Target = turnout1ThrownPosition;   
    t1state = 1;
    cmri.set_bit(0, HIGH);                                                  //4001 jmri address output
  }
  if (turnout1 == 0 && t1state == 1) {
    turnout1Target = turnout1ClosedPosition;    
    t1state = 0;
    cmri.set_bit(0, LOW);                                                   //4001
  }

  // read the cmri bit and switch on led 
  digitalWrite (lightControl, cmri.get_bit(1));                             //jmri SH1 4002 


  // get senors status and send to jmri
  cmri.set_bit(1, !digitalRead(A0));                                        //jmri 4002
  cmri.set_bit(2, !digitalRead(A1));                                        //jmri 4003
  cmri.set_bit(3, !digitalRead(A2));                                        //jmri 4004
  cmri.set_bit(4, !digitalRead(A3));                                        //jmri 4005
  cmri.set_bit(5, !digitalRead(A4));                                        //jmri 4006


  if (turnout1Position != turnout1Target) {
    if (millis() > turnoutMoveDelay) {
      turnoutMoveDelay = millis() + turnoutMoveSpeed;
      if (turnout1Position < turnout1Target) turnout1Position++;
      if (turnout1Position > turnout1Target) turnout1Position--;
      turnOut1.write(turnout1Position);
    }
  } 
}
