/*pins 0 - 1 are usedfor Rx and Tx no trigger pin required due to using
  TTL to RS485 Module Serial Port MCU Automatic Flow Control Module.
  simple servo cmri node which moves 2 servo and the sends back a bit back to jmri to indicate
  the servo has been moved
 
  it will also read 6 sensors/switchs A0 to A5 & 5 sensors on pins 4 to 8
*/
#include <CMRI.h>
#include <Servo.h>

// ddefine node address each node needs its own address
#define CMRI_ADDR 8

// define turnout required positions each turnout will need calibrating
#define turnout1ClosedPosition 40
#define turnout1ThrownPosition 100
#define turnout2ClosedPosition 110
#define turnout2ThrownPosition 35


//define infrared detection dock siding & platform 2

#define Sensor1 A0
#define Sensor2 A1
#define Sensor3 A2
#define Sensor4 A3
#define Sensor5 A4
#define Sensor6 A5

//define infrared detection dock platform 1

#define Sensor7 4
#define Sensor8 5
#define Sensor9 6
#define Sensor10 7
#define Sensor11 8

//setup the move speed of servo
#define turnoutMoveSpeed 20   // [ms] lower number is faster

int t1state = 0;                                      //0 = closed 1 = thrown
int turnout1 = 0;
int t2state = 0;                                      //0 = closed 1 = thrown
int turnout2 = 0;

unsigned long turnoutMoveDelay;

CMRI cmri(CMRI_ADDR, 24, 48); // defaults to a SMINI with address 0. SMINI = 24 inputs, 48 outputs
Servo turnOut1;
Servo turnOut2;

byte turnout1Position = turnout1ClosedPosition;
byte turnout1Target   = turnout1ClosedPosition;
byte turnout2Position = turnout2ClosedPosition;
byte turnout2Target   = turnout2ClosedPosition;


void setup() {
  delay(2000);


  //setup input pins
  pinMode(Sensor1, INPUT_PULLUP);
  pinMode(Sensor2, INPUT_PULLUP);
  pinMode(Sensor3, INPUT_PULLUP);
  pinMode(Sensor4, INPUT_PULLUP);
  pinMode(Sensor5, INPUT_PULLUP);
  pinMode(Sensor6, INPUT_PULLUP);


  pinMode(Sensor7, INPUT_PULLUP);
  pinMode(Sensor8, INPUT_PULLUP);
  pinMode(Sensor9, INPUT_PULLUP);
  pinMode(Sensor10, INPUT_PULLUP);
  pinMode(Sensor11, INPUT_PULLUP);
 
  turnOut1.attach(2);
  turnOut1.write(turnout1ClosedPosition);
  turnOut2.attach(3);
  turnOut2.write(turnout2ClosedPosition);

  Serial.begin(19200);
}

void loop() {
  cmri.process();
  turnout1 = (cmri.get_bit(0));                                 //turnout 8001 jmri address
  turnout2 = (cmri.get_bit(1));                                 //turnout 8002 jmri address

  if (turnout1 == 1 && t1state == 0) {
    turnout1Target = turnout1ThrownPosition;   
    t1state = 1;
    cmri.set_bit(0, HIGH);                                      //8001 jmri address output
  }
  if (turnout1 == 0 && t1state == 1) {
    turnout1Target = turnout1ClosedPosition;    
    t1state = 0;
    cmri.set_bit(0, LOW);                                      //8001 jmri address output
  }
  if (turnout2 == 1 && t2state == 0) {
    turnout2Target = turnout2ThrownPosition;   
    t2state = 1;
    cmri.set_bit(1, HIGH);                                     //8002 jmri address output
  }
  if (turnout2 == 0 && t2state == 1) {
    turnout2Target = turnout2ClosedPosition;   
    t2state = 0;
    cmri.set_bit(1, LOW);                                     //8002 jmri address output
  }

  
  // get senors status and send to jmri
  
  cmri.set_bit(2, !digitalRead(A0));           //8003
  cmri.set_bit(3, !digitalRead(A1));           //8004
  cmri.set_bit(4, !digitalRead(A2));           //8005
  cmri.set_bit(5, !digitalRead(A3));           //8006
  cmri.set_bit(6, !digitalRead(A4));           //8007
  cmri.set_bit(7, !digitalRead(A5));           //8008

  cmri.set_bit(8, !digitalRead(4));            //8009
  cmri.set_bit(9, !digitalRead(5));            //8010
  cmri.set_bit(10, !digitalRead(6));           //8011
  cmri.set_bit(11, !digitalRead(7));           //8012
  cmri.set_bit(12, !digitalRead(8));           //8013

   if (turnout1Position != turnout1Target) {
    if (millis() > turnoutMoveDelay) {
      turnoutMoveDelay = millis() + turnoutMoveSpeed;
      if (turnout1Position < turnout1Target) turnout1Position++;
      if (turnout1Position > turnout1Target) turnout1Position--;
      turnOut1.write(turnout1Position);
    }
  } 
  if (turnout2Position != turnout2Target) {
    if (millis() > turnoutMoveDelay) {
      turnoutMoveDelay = millis() + turnoutMoveSpeed;
      if (turnout2Position < turnout2Target) turnout2Position++;
      if (turnout2Position > turnout2Target) turnout2Position--;
      turnOut2.write(turnout2Position);
    }
  }
}