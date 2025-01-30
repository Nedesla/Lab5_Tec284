#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *rightMotor = AFMS.getMotor(4);
Adafruit_DCMotor *leftMotor = AFMS.getMotor(3);

const int trigPin = 7;
const int echoPin = 8;

long duration, distance;

// Function to get distance from ultrasonic sensor
long distanceTest() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH, 20000);  // Timeout at 20ms
    if (duration == 0) return 1000;  // If no reading, assume very far object
    return (duration / 2) / 29.1;
}

// Function to get stable distance by averaging multiple readings
long averageDistance() {
    long total = 0;
    for (int i = 0; i < 5; i++) {  // Take 5 readings
        total += distanceTest();
        delay(50);  // Small delay to prevent interference
    }
    return total / 5;  // Return average distance
}

// Motor control functions
void stopCar() {
    rightMotor->setSpeed(0);
    leftMotor->setSpeed(0);
}

void forwards() {
    rightMotor->setSpeed(210);
    leftMotor->setSpeed(200);
    rightMotor->run(FORWARD);
    leftMotor->run(FORWARD);
}

void leftTurn() {
    rightMotor->setSpeed(200);
    leftMotor->setSpeed(100);
    rightMotor->run(FORWARD);
    leftMotor->run(BACKWARD);  // Makes a sharper left turn
}

void rightTurn() {
    rightMotor->setSpeed(100);
    leftMotor->setSpeed(200);
    rightMotor->run(BACKWARD);
    leftMotor->run(FORWARD);  // Makes a sharper right turn
}

void backwards() {
    rightMotor->setSpeed(150);
    leftMotor->setSpeed(150);
    rightMotor->run(BACKWARD);
    leftMotor->run(BACKWARD);
}

void setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    Serial.begin(9600);
    AFMS.begin();
}

void loop() {
    long distance = averageDistance();
    Serial.print("Filtered Distance: ");
    Serial.println(distance);
    
    if (distance > 20 || distance < 5) {  // Move forward if no obstacle or too close reading
        forwards();
    } else {  // If obstacle detected
        stopCar();
        delay(500);
        backwards();
        delay(500);
        
        long leftDist, rightDist;

        leftTurn();
        delay(500);
        leftDist = averageDistance();

        rightTurn();
        delay(500);
        rightDist = averageDistance();

        stopCar();
        delay(500);

        if (rightDist > leftDist) {
            Serial.println("Turning Right");
            rightTurn();
            delay(1000);
        } else {
            Serial.println("Turning Left");
            leftTurn();
            delay(1000);
        }
    }
    delay(100);  // Small delay between sensor readings
}



