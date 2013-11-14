#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>

#define LED 13

// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  1500     // microseconds
#define DISTANCE_THRESHOLD 400  // A0 DAC  

// these might need to be tuned for different motor types
#define REVERSE_SPEED     200 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     200
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     300 // ms
#define SEARCH_DURATION   100 // ms
#define CHARGE_DURATION   300 // ms

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];

ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

void waitForButtonAndCountDown()
{
  digitalWrite(LED, HIGH);
  button.waitForButton();
  digitalWrite(LED, LOW);

  // play audible countdown
  for (int i = 0; i < 3; i++)
  {
    delay(1000);
    buzzer.playNote(NOTE_G(3), 200, 15);
  }
  delay(1000);
  buzzer.playNote(NOTE_G(4), 500, 15);  
  delay(1000);
}

void forward()
{
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
}

void reverse()
{
  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  delay(REVERSE_DURATION); 
}

void charge()
{
  forward();
  //delay(CHARGE_DURATION);
}

void setup()
{
  // uncomment if necessary to correct motor directions
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);

  pinMode(LED, HIGH);

  Serial.begin(9600);

  waitForButtonAndCountDown();
}

void loop()
{
  if (button.isPressed()) {
    // if button is pressed, stop and wait for another press to go again
    motors.setSpeeds(0, 0);
    button.waitForRelease();
    waitForButtonAndCountDown();
  }


//  motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
  
  int distanceSensorValue = analogRead(A0);
  sensors.read(sensor_values);

  Serial.print("Left Edge Sensor: ");
  Serial.print(sensor_values[0]);
  Serial.print(" Distance Sensor: ");
  Serial.print(distanceSensorValue);
  Serial.print(" Right Edge Sensor: ");
  Serial.print(sensor_values[5]);
  Serial.println("");

  if (sensor_values[0] < QTR_THRESHOLD) {
    // if leftmost sensor detects line, reverse and turn to the right
    reverse();

    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);

    // otherwise, go straight
    forward();
  } else if (sensor_values[5] < QTR_THRESHOLD) {
    // if rightmost sensor detects line, reverse and turn to the left
    reverse();

    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);

    // otherwise, go straight
    forward();
  } else if (distanceSensorValue > 120) {
    // if opponent is in range, charge  
    charge();
  } else {
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(SEARCH_DURATION);
  }
}

