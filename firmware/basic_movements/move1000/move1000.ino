#include <AccelStepper.h>

AccelStepper pan(AccelStepper::HALF4WIRE, 8, 10, 9, 11);
AccelStepper tilt(AccelStepper::HALF4WIRE, 2, 4, 3, 5);



void setup() {
  pan.setMaxSpeed(800);  pan.setAccelerati-on(1000);
  tilt.setMaxSpeed(800); tilt.setAcceleration(1000);
  pan.moveTo(1000);
  tilt.moveTo(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  pan.run();
  tilt.run();
}
