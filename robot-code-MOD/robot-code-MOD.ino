#include <ZumoShield.h>

ZumoMotors motors;
ZumoBuzzer buzzer;
Pushbutton button(ZUMO_BUTTON);


const int trigPin1 = 12;
const int echoPin1 = 11;
const int trigPin2 = 2;
const int echoPin2 = 4;

const int ldrLeftPin = A0;
const int ldrRightPin = A1;


int obstacleDistanceThreshold = 20; 
int lightThreshold = 400; 

bool ldrLError;
bool ldrRError;
bool ultraLError;
bool ultraRError;

bool checkForSensorErrors();
bool avoidObstacles();
bool followLight();
void Drive();
long readUltrasonicDistance(int trigPin, int echoPin);

void setup() {
  Serial.begin(9600);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  
  pinMode(ldrLeftPin, INPUT);
  pinMode(ldrRightPin, INPUT);
  motors.setSpeeds(0, 0);

  delay(500);

}

void loop() {
  if (checkForSensorErrors()) {
    Serial.println("Sensor Error Detected");
    return; 
  }

  if (avoidObstacles()) {
    Serial.println("Avoiding");
    return;
  }

  if (followLight()) {
    Serial.println("Found light");
    return;
  }

  Drive();
  Serial.println("Driveing stright");
}

bool checkForSensorErrors() {
  int ldrLeft = analogRead(ldrLeftPin);
  int ldrRight = analogRead(ldrRightPin);
  long distance1 = readUltrasonicDistance(trigPin1, echoPin1);
  long distance2 = readUltrasonicDistance(trigPin2, echoPin2);

  if(ldrLeft == 0){
    ldrLError = true;
  }else {
      ldrLError = false;
    }

  if(ldrRight == 0){
    ldrRError = true;
  }else {
      ldrRError = false;
    }

  if(distance1 == 0){
    ultraLError = true;
    obstacleDistanceThreshold = 40;
  }else {
      ultraLError = false;
    }

  if(distance2 == 0){
    ultraRError = true;
    obstacleDistanceThreshold = 40;
  }else {
      ultraRError = false;
    }

  return false; 
}

bool avoidObstacles() {
  long distance1 = readUltrasonicDistance(trigPin1, echoPin1);
  long distance2 = readUltrasonicDistance(trigPin2, echoPin2);

        Serial.print("distance1: ");
  Serial.println(distance1);
    Serial.print("distance2 2: ");
  Serial.println(distance2);



  if (distance1 < obstacleDistanceThreshold || distance2 < obstacleDistanceThreshold) {
    if (distance1 < distance2) {
      motors.setSpeeds(200, -200);
      delay(500);
    } else {
      motors.setSpeeds(-200, 200);
      delay(500);

    }
    return true;
  }
  return false;
}

bool followLight() {
  int ldrLeft = analogRead(ldrLeftPin);
  int ldrRight = analogRead(ldrRightPin);

      Serial.print("ldrLeft: ");
  Serial.println(ldrLeft);
    Serial.print("ldrRight 2: ");
  Serial.println(ldrRight);

  if (ldrLeft > lightThreshold || ldrRight > lightThreshold) {
    if (ldrLeft > ldrRight) {
      motors.setSpeeds(50, 200);
    } else {
      motors.setSpeeds(200, 50);
    }
    return true;
  }
  return false;
}

void Drive() {
  motors.setSpeeds(100, 100); 
}

long readUltrasonicDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);   
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long  distance = (duration*.0343)/2;

  return distance;
}
