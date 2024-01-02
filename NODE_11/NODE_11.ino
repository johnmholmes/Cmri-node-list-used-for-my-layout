/*pins 0 - 1 are usedfor Rx and Tx no trigger pin required due to using
  TTL to RS485 Module Serial Port MCU Automatic Flow Control Module.
  
  simple servo cmri node which moves 1 servo and the sends back a bit back to jmri to indicate
  the servo has been moved
  also will control 6 off signal leds
  it will also read 2 sensors/switchs

*/
#include <CMRI.h>
#include <Servo.h>

// define node address each node needs its own address
#define CMRI_ADDR 11

// define turnout required positions each turnout will need calibrating
#define turnout1ClosedPosition 108
#define turnout1ThrownPosition 80

// define signal leds turnout 1 pins 0 and 1 rx & tx ADDRESS 11001 USED FOR BIT STATE OF TURNOUT
#define throughApproachGreenLed 4                 //jmri 11002
#define throughApproachRedLed 5                   
#define divergingApproachGreenLed 6               
#define divergingApproachRedLed 7                 
#define throughGreenLed 8                         
#define throughRedLed 9                           

// Spare pins at the moment 3 10 to 13 And A3 to A7 

//define infrared detection T11 as inputs
#define Sensor1 A0                                //jmri 11002
#define Sensor2 A1                                //jmri 11003
#define Sensor3 A2                                //jmri 11004

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
  pinMode(throughApproachGreenLed, OUTPUT);
  pinMode(throughApproachRedLed, OUTPUT);
  pinMode(divergingApproachGreenLed, OUTPUT);
  pinMode(divergingApproachRedLed, OUTPUT);
  pinMode(throughGreenLed, OUTPUT);
  pinMode(throughRedLed, OUTPUT);

  //setup input pins
  pinMode(Sensor1, INPUT_PULLUP);
  pinMode(Sensor2, INPUT_PULLUP);
  pinMode(Sensor3, INPUT_PULLUP);


  digitalWrite(throughApproachGreenLed, LOW);      // light led 1 on start up  to show its working
  delay(2000);
  digitalWrite(throughApproachGreenLed, HIGH);
  turnOut1.attach(2);
  turnOut1.write(turnout1ClosedPosition);

  Serial.begin(19200);
}

void loop() {
  cmri.process();
  turnout1 = (cmri.get_bit(0));                                             //turnout 11001 jmri address

  if (turnout1 == 1 && t1state == 0) {
    turnout1Target = turnout1ThrownPosition;   
    t1state = 1;
    cmri.set_bit(0, HIGH);                                                  //11001 jmri address output
  }
  if (turnout1 == 0 && t1state == 1) {
    turnout1Target = turnout1ClosedPosition;    
    t1state = 0;
    cmri.set_bit(0, LOW);                                                   //11001
  }

  // read the cmri bit and switch on led ! used as i am using + commom leds tmc signals
  digitalWrite (throughApproachGreenLed, !cmri.get_bit(1));                 //jmri SH1 11002 
  digitalWrite (throughApproachRedLed, !cmri.get_bit(2));
  digitalWrite (divergingApproachGreenLed, !cmri.get_bit(3));
  digitalWrite (divergingApproachRedLed, !cmri.get_bit(4));
  digitalWrite (throughGreenLed, !cmri.get_bit(5));
  digitalWrite (throughRedLed, !cmri.get_bit(6));                           

  // get senors status and send to jmri
  cmri.set_bit(1, !digitalRead(A0));                                        //jmri 11002
  cmri.set_bit(2, !digitalRead(A1)); 
  cmri.set_bit(3, !digitalRead(A2));


  if (turnout1Position != turnout1Target) {
    if (millis() > turnoutMoveDelay) {
      turnoutMoveDelay = millis() + turnoutMoveSpeed;
      if (turnout1Position < turnout1Target) turnout1Position++;
      if (turnout1Position > turnout1Target) turnout1Position--;
      turnOut1.write(turnout1Position);
    }
  } 
}
