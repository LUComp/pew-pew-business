// Turret controller: 2x 28BYJ-48 + ULN2003, commanded over USB serial.
//
// Serial Monitor control (115200 baud): type a letter and press Enter.
// Repeat letters for bigger moves, e.g. "ddd" = three steps right.
//   w / s   tilt up / down
//   a / d   pan left / right
//   space   stop
//   h       go back to home (startup position)
//
// Line commands (end with a newline), for the computer-vision code later:
//   P<steps>,T<steps>   relative move, e.g. "P-40,T12"
//
// Replies "D" when both motors have finished moving.

#include <AccelStepper.h>

// Pin order IN1, IN3, IN2, IN4 (28BYJ-48 quirk)
AccelStepper pan(AccelStepper::HALF4WIRE, 8, 10, 9, 11);
AccelStepper tilt(AccelStepper::HALF4WIRE, 2, 4, 3, 5);

// Soft limits in half-steps from the startup position (4096 = one full turn).
// Off by default; set USE_LIMITS to true to stop cables from wrapping.
const bool USE_LIMITS = false;
const long PAN_LIMIT  = 2048;   // +/- 180 degrees
const long TILT_LIMIT = 1024;   // +/- 90 degrees

// Steps per key press (256 = 22.5 degrees, 512 = 45 degrees)
const long JOG = 256;

// Flip to -1 if a key turns the wrong way
const int PAN_DIR  = 1;
const int TILT_DIR = 1;

bool moving = false;
String buf;

void startMoving() {
  pan.enableOutputs();
  tilt.enableOutputs();
  moving = true;
}

void movePanBy(long steps) {
  long t = pan.targetPosition() + steps;
  if (USE_LIMITS) t = constrain(t, -PAN_LIMIT, PAN_LIMIT);
  pan.moveTo(t);
}

void moveTiltBy(long steps) {
  long t = tilt.targetPosition() + steps;
  if (USE_LIMITS) t = constrain(t, -TILT_LIMIT, TILT_LIMIT);
  tilt.moveTo(t);
}

// Each letter adds one jog, so "ddd" moves three jogs
void jogPan(long steps)  { movePanBy(steps);  startMoving(); }
void jogTilt(long steps) { moveTiltBy(steps); startMoving(); }

bool handleKey(char c) {
  switch (tolower(c)) {
    case 'w': jogTilt( JOG * TILT_DIR); return true;
    case 's': jogTilt(-JOG * TILT_DIR); return true;
    case 'a': jogPan(-JOG * PAN_DIR);   return true;
    case 'd': jogPan( JOG * PAN_DIR);   return true;
    case ' ': pan.stop(); tilt.stop();  return true;   // decelerate to a stop
    case 'h': pan.moveTo(0); tilt.moveTo(0); startMoving(); return true;
  }
  return false;
}

void handleLine(const String& cmd) {
  if (!cmd.startsWith("P")) return;
  int comma = cmd.indexOf(",T");
  if (comma < 0) return;
  movePanBy(cmd.substring(1, comma).toInt());
  moveTiltBy(cmd.substring(comma + 2).toInt());
  startMoving();
}

void setup() {
  Serial.begin(115200);
  pan.setMaxSpeed(800);       // half-steps per second (~1000 is the practical max)
  pan.setAcceleration(2000);
  tilt.setMaxSpeed(800);
  tilt.setAcceleration(2000);
  pan.disableOutputs();
  tilt.disableOutputs();
}

void loop() {
  // Read serial without blocking
  while (Serial.available()) {
    char c = Serial.read();
    if (buf.length() == 0 && handleKey(c)) continue;  // single-key commands act immediately
    if (c == '\n' || c == '\r') {
      buf.trim();
      handleLine(buf);
      buf = "";
    } else if (buf.length() < 32) {
      buf += c;
    }
  }

  // Must be called as often as possible: each call does at most one step
  pan.run();
  tilt.run();

  // When both have arrived: cut coil power (saves current and heat) and report
  if (moving && pan.distanceToGo() == 0 && tilt.distanceToGo() == 0) {
    pan.disableOutputs();
    tilt.disableOutputs();
    moving = false;
    Serial.println("D");
  }
}