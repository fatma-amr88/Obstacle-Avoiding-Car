#include <Servo.h>
Servo smotor;

// Motor Pins
#define left_motorF 13
#define left_motorB 12
#define enable_L 5
#define right_motorF 11
#define right_motorB 10
#define enable_R 6

// Ultrasonic Pins
#define trig 2
#define echo 3

// Servo Pin
#define servoPin 9

// Distance Variables
int duration, distance, distance_R, distance_L;

// Movement Speed (0–255)
const int MOVE_SPEED = 90;      // Much slower forward speed
const int TURN_SPEED = 80;      // Slightly slower for turning

void setup() {
  // Motor Setup
  pinMode(left_motorF, OUTPUT);
  pinMode(left_motorB, OUTPUT);
  pinMode(right_motorF, OUTPUT);
  pinMode(right_motorB, OUTPUT);
  pinMode(enable_L, OUTPUT);
  pinMode(enable_R, OUTPUT);

  // Ultrasonic Setup
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  // Servo Setup
  smotor.attach(servoPin);

  // Debugging
  Serial.begin(9600);
}

// Measure distance with ultrasonic
int measure() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH, 20000); // max 20ms
  distance = 0.034 * duration / 2;
  return constrain(distance, 1, 300);
}

// Get average of 5 measurements
int getAccurateDistance() {
  int total = 0;
  for (int i = 0; i < 5; i++) {
    total += measure();
    delay(15);
  }
  return total / 5;
}

// Stop all movement
void stop() {
  analogWrite(enable_L, 0);
  analogWrite(enable_R, 0);
  digitalWrite(left_motorF, LOW);
  digitalWrite(left_motorB, LOW);
  digitalWrite(right_motorF, LOW);
  digitalWrite(right_motorB, LOW);
  delay(250);
}

// Move Forward
void moveForward() {
  analogWrite(enable_L, MOVE_SPEED);
  analogWrite(enable_R, MOVE_SPEED);
  digitalWrite(left_motorF, HIGH);
  digitalWrite(left_motorB, LOW);
  digitalWrite(right_motorF, HIGH);
  digitalWrite(right_motorB, LOW);
}

// Turn Left
void turnLeft() {
  analogWrite(enable_L, 0);
  analogWrite(enable_R, TURN_SPEED);
  digitalWrite(left_motorF, LOW);
  digitalWrite(left_motorB, LOW);
  digitalWrite(right_motorF, HIGH);
  digitalWrite(right_motorB, LOW);
  delay(750);
  stop();
}

// Turn Right
void turnRight() {
  analogWrite(enable_L, TURN_SPEED);
  analogWrite(enable_R, 0);
  digitalWrite(left_motorF, HIGH);
  digitalWrite(left_motorB, LOW);
  digitalWrite(right_motorF, LOW);
  digitalWrite(right_motorB, LOW);
  delay(750);
  stop();
}

void loop() {
  smotor.write(70); // Center
  int frontDistance = getAccurateDistance();
  Serial.print("Front Distance: ");
  Serial.println(frontDistance);

  if (frontDistance < 40) {  // Increased obstacle threshold
    stop();
    Serial.println("Obstacle ahead!");

    // Check Left
    smotor.write(140);
    delay(600);
    distance_L = getAccurateDistance();

    // Check Right
    smotor.write(0);
    delay(600);
    distance_R = getAccurateDistance();

    // Return to center
    smotor.write(70);
    delay(300);

    // Decide direction
    if (distance_L < distance_R) {
      Serial.println("Turning right...");
      turnRight();
    } else {
      Serial.println("Turning left...");
      turnLeft();
    }
  } else {
    moveForward();
  }

  delay(100);
}