#include <AFMotor.h>  
#include <NewPing.h>
#include <Servo.h> 

// Constants
#define TRIG_PIN A0 
#define ECHO_PIN A1 
#define MAX_DISTANCE 200 
#define MAX_SPEED 190 
#define MAX_SPEED_OFFSET 20

#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

// Buttons
#define BUTTON_OBSTACLE A2
#define BUTTON_FOLLOW   A3
#define BUTTON_SING     A4

// Motors and sensors
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); 
AF_DCMotor motor1(1, MOTOR12_1KHZ); 
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);
Servo myservo;

boolean goesForward = false;
int distance = 100;
int speedSet = 0;

int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4,
  NOTE_D4, NOTE_D4, NOTE_C4
};

int durations[] = {
  4, 4, 4, 4,
  4, 4, 2,
  4, 4, 4, 4,
  4, 4, 2
};

enum Mode {
  NONE,
  OBSTACLE,
  FOLLOW,
  SING
};

Mode currentMode = NONE;

void setup() {
  myservo.attach(10);  

  pinMode(BUTTON_OBSTACLE, INPUT_PULLUP);
  pinMode(BUTTON_FOLLOW, INPUT_PULLUP);
  pinMode(BUTTON_SING, INPUT_PULLUP);

  myservo.write(115); 
  delay(2000);

  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
}

void loop() {
  checkButtons();

  switch (currentMode) {
    case OBSTACLE:
      runObstacleAvoid();
      break;

    case FOLLOW:
      followHand();
      break;

    case SING:
      playTwinkle();
      currentMode = NONE;  // Reset to idle after playing
      break;

    default:
      moveStop();  // Idle state
      break;
  }
}

void checkButtons() {
  if (digitalRead(BUTTON_OBSTACLE) == LOW) {
    currentMode = OBSTACLE;
    delay(300); // simple debounce
  } else if (digitalRead(BUTTON_FOLLOW) == LOW) {
    currentMode = FOLLOW;
    delay(300);
  } else if (digitalRead(BUTTON_SING) == LOW) {
    currentMode = SING;
    delay(300);
  }
}

void runObstacleAvoid() {
  int distanceR = 0;
  int distanceL = 0;
  delay(40);

  if (distance <= 15) {
    moveStop();
    delay(100);
    moveBackward();
    delay(300);
    moveStop();
    delay(200);
    distanceR = lookRight();
    delay(200);
    distanceL = lookLeft();
    delay(200);

    if (distanceR >= distanceL) {
      turnRight();
      moveStop();
    } else {
      turnLeft();
      moveStop();
    }
  } else {
    moveForward();
  }
  distance = readPing();
}

void followHand() {
  int d = readPing();
  if (d >= 10 && d <= 20) {
    moveForward();
  } else {
    moveStop();
  }
}

void playTwinkle() {
  for (int i = 0; i < sizeof(melody) / sizeof(int); i++) {
    int noteDuration = 1000 / durations[i];
    tone(8, melody[i], noteDuration);
    delay(noteDuration * 1.3);
    noTone(8);
  }
}

// Movement and Sensor Functions
int readPing() {
  delay(70);
  int cm = sonar.ping_cm();
  if (cm == 0) cm = 250;
  return cm;
}

int lookRight() {
  myservo.write(50); 
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(115); 
  return distance;
}

int lookLeft() {
  myservo.write(170); 
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(115); 
  return distance;
}

void moveStop() {
  motor1.run(RELEASE); 
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void moveForward() {
  if (!goesForward) {
    goesForward = true;
    motor1.run(FORWARD);      
    motor2.run(FORWARD);
    motor3.run(FORWARD); 
    motor4.run(FORWARD);     
    for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) {
      motor1.setSpeed(speedSet);
      motor2.setSpeed(speedSet);
      motor3.setSpeed(speedSet);
      motor4.setSpeed(speedSet);
      delay(5);
    }
  }
}

void moveBackward() {
  goesForward = false;
  motor1.run(BACKWARD);      
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);  
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) {
    motor1.setSpeed(speedSet);
    motor2.setSpeed(speedSet);
    motor3.setSpeed(speedSet);
    motor4.setSpeed(speedSet);
    delay(5);
  }
}

void turnRight() {
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);     
  delay(500);
}

void turnLeft() {
  motor1.run(BACKWARD);     
  motor2.run(BACKWARD);  
  motor3.run(FORWARD);
  motor4.run(FORWARD);   
  delay(500);
}
