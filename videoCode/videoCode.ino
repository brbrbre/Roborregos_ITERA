#include <AFMotor.h>  
#include <NewPing.h>
#include <Servo.h> 

#define TRIG_PIN A0 
#define ECHO_PIN A1 
#define MAX_DISTANCE 200 
#define MAX_SPEED 190 
#define BUZZER_PIN A4

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); 

AF_DCMotor motor1(1, MOTOR12_1KHZ); 
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);

Servo myservo;   

boolean goesForward = false;
int distance = 100;
int speedSet = 0;
int modo = 0;

void setup() {
  Serial.begin(9600);
  myservo.attach(10);  
  myservo.write(115);
  pinMode(BUZZER_PIN, OUTPUT); 
  delay(2000);
  distance = readPing();
}

void loop() {
  int bottom1 = analogRead(A5);
  int bottom2 = analogRead(A3);
  int bottom3 = analogRead(A2);

  if (modo == 0) {
    if (bottom1 >= 500) {
      modo = 1;
      Serial.println("Modo evitar obstáculos activado");
      delay(500);
    } else if (bottom2 >= 500) {
      modo = 2;
      Serial.println("Buzzer activado");
      delay(500);
    } else if (bottom3 >= 500) {
      modo = 3;
      Serial.println("Modo seguir mano activado");
      delay(500);
    }
  } else if (modo == 1) {
    modeAvoidObstacles();
    modo = 0;
  } else if (modo == 2) {
    playMelody();
    modo = 0;
  } else if (modo == 3) {
    modeFollow();
    modo = 0;
  } else {
    moveStop();
  }
}

void modeAvoidObstacles() {
  while (true) {
    int newBottom1 = analogRead(A5);
    int newBottom2 = analogRead(A3);
    int newBottom3 = analogRead(A2);

    if (newBottom1 >= 500 || newBottom2 >= 500 || newBottom3 >= 500) {
      moveStop();
      break;
    }

    distance = readPing();
    Serial.print("Distancia: ");
    Serial.println(distance);

    if (distance <= 15) {
      moveStop();
      delay(100);
      moveBackward();
      delay(300);
      moveStop();
      delay(200);

      int distanceR = lookRight();
      delay(200);
      int distanceL = lookLeft();
      delay(200);

      if (distanceR >= distanceL) {
        turnRight();
      } else {
        turnLeft();
      }

      moveStop();
    } else {
      moveForward();
    }

    delay(100);
  }
}

void modeFollow() {
  while (true) {
    int newBottom1 = analogRead(A5);
    int newBottom2 = analogRead(A3);
    int newBottom3 = analogRead(A2);

    if (newBottom1 >= 500 || newBottom2 >= 500 || newBottom3 >= 500) {
      moveStop();
      break;
    }

    distance = readPing();
    Serial.print("Distancia a mano: ");
    Serial.println(distance);

    if (distance > 3 && distance < 30) {
      moveForward();
    } else {
      moveStop();
    }

    delay(100);
  }
}

int lookRight() {
  myservo.write(50); 
  delay(500);
  int dist = readPing();
  delay(100);
  myservo.write(115); 
  return dist;
}

int lookLeft() {
  myservo.write(170); 
  delay(500);
  int dist = readPing();
  delay(100);
  myservo.write(115); 
  return dist;
}

int readPing() { 
  delay(70);
  int cm = sonar.ping_cm();
  if (cm == 0) cm = 250;
  return cm;
}

void moveStop() {
  motor1.run(RELEASE); 
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
  goesForward = false;
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

void beep(int frequency, int duration) {
  long delayTime = 1000000L / frequency / 2; 
  long cycles = (long)frequency * duration / 1000;

  for (long i = 0; i < cycles; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(delayTime);
  }
}
void playMelody() {
  int melody[] = {
    262, 262, 392, 392, 440, 440, 392,
    349, 349, 330, 330, 294, 294, 262
  };

  int noteDurations[] = {
    400, 400, 400, 400, 400, 400, 600,
    400, 400, 400, 400, 400, 400, 600
  };

  int notesCount = sizeof(melody) / sizeof(melody[0]);

  for (int i = 0; i < notesCount; i++) {
    int note = melody[i];
    int duration = noteDurations[i];
    if (note == 0) {
      delay(duration);
    } else {
      beep(note, duration);
    }
    delay(100);
  }
}