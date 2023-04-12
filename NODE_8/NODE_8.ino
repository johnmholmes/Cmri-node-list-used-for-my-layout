/*pins 0 - 1 are usedfor Rx and Tx no trigger pin required due to using
  TTL to RS485 Module Serial Port MCU Automatic Flow Control Module.
  simple servo cmri node which moves 2 servo and the sends back a bit back to jmri to indicate
  the servo has been moved
  also will control 12 off signal leds
  it will also read 6 sensors/switchs
*/
#include <CMRI.h>
#include <Servo.h>

// ddefine node address each node needs its own address
#define CMRI_ADDR 8

// define turnout required positions each turnout will need calibrating
#define turnout1ClosedPosition 110
#define turnout1ThrownPosition 77
#define turnout2ClosedPosition 110
#define turnout2ThrownPosition 35

// define signal leds turnout 3 and 4
#define throughGreenLed 4
#define throughRedLed 5
#define divergingGreenLed 6
#define divergingRedLed 7
#define throughApproachGreenLed 8 
#define throughApproachRedLed 9
#define divergingApproachGreenLed 10
#define divergingApproachRedLed 11
#define sh13GreenLed 8 
#define sh13RedLed 9
#define sh14GreenLed 10
#define sh14RedLed 11

//define infrared detection peel

#define Sensor1 A0
#define Sensor2 A1
#define Sensor3 A2
#define Sensor4 A3
#define Sensor5 A4
#define Sensor6 A5

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
  //setup output pins
  pinMode(throughGreenLed, OUTPUT);
  pinMode(throughRedLed, OUTPUT);
  pinMode(divergingGreenLed, OUTPUT);
  pinMode(divergingRedLed, OUTPUT);
  pinMode(throughApproachGreenLed, OUTPUT);
  pinMode(throughApproachRedLed, OUTPUT);
  pinMode(divergingApproachGreenLed, OUTPUT);
  pinMode(divergingApproachRedLed, OUTPUT);
  pinMode(sh13GreenLed, OUTPUT);
  pinMode(sh13RedLed, OUTPUT);
  pinMode(sh14GreenLed, OUTPUT);
  pinMode(sh14RedLed, OUTPUT);

  //setup input pins
  pinMode(Sensor1, INPUT_PULLUP);
  pinMode(Sensor2, INPUT_PULLUP);
  pinMode(Sensor3, INPUT_PULLUP);
  pinMode(Sensor4, INPUT_PULLUP);
  pinMode(Sensor5, INPUT_PULLUP);
  pinMode(Sensor6, INPUT_PULLUP);

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

  // read the cmri bit and switch on led ! used as i am using + commom leds tmc signals
  digitalWrite (throughGreenLed, !cmri.get_bit(2));             //8003
  digitalWrite (throughRedLed, !cmri.get_bit(3));               //8004
  digitalWrite (divergingGreenLed, !cmri.get_bit(4));           //8005
  digitalWrite (divergingRedLed, !cmri.get_bit(5));             //8006
  digitalWrite (throughApproachGreenLed, !cmri.get_bit(6));     //8007  
  digitalWrite (throughApproachRedLed, !cmri.get_bit(7));       //8008
  digitalWrite (divergingApproachGreenLed, !cmri.get_bit(8));   //8009
  digitalWrite (divergingApproachRedLed, !cmri.get_bit(9));     //8010
  digitalWrite (sh13GreenLed, !cmri.get_bit(10));               //8011
  digitalWrite (sh13RedLed, !cmri.get_bit(11));                 //8012
  digitalWrite (sh14GreenLed, !cmri.get_bit(12));               //8013
  digitalWrite (sh14RedLed, !cmri.get_bit(13));                 //8014

  // get senors status and send to jmri
  
  cmri.set_bit(3, !digitalRead(A0));           //8002
  cmri.set_bit(4, !digitalRead(A1));           //8003
  cmri.set_bit(5, !digitalRead(A2));           //8004
  cmri.set_bit(6, !digitalRead(A3));           //8005
  cmri.set_bit(7, !digitalRead(A4));           //8006
  cmri.set_bit(8, !digitalRead(A5));           //8007
  

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