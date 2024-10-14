#include <ZumoShield.h>
#include <Wire.h>


ZumoMotors motors;
ZumoBuzzer buzzer;
ZumoIMU imu;
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
long accValue;

bool checkForSensorErrors();
bool avoidObstacles();
bool followLight();
void Drive();
void SensorValue();
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

  Wire.begin();

  if (!imu.init())
  {
    while(1)
    {
      Serial.println(F("Failed to initialize IMU sensors."));
      delay(100);
    }
  }

  imu.enableDefault();

  delay(500);

  imu.read();

  accValue = imu.a.x;

  delay(500);


}

void loop() {
  if (checkForSensorErrors()) {
    return; 
  }

  SensorValue();

  if (avoidObstacles()) {
    return;
  }

  if (followLight()) {
    return;
  }

  Drive();
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
  long distance1 = ultraLError ? -1 : readUltrasonicDistance(trigPin1, echoPin1);
  long distance2 = ultraRError ? -1 : readUltrasonicDistance(trigPin2, echoPin2);

  // Serial.print("distance1: ");
  // Serial.println(distance1);
  // Serial.print("distance2: ");
  // Serial.println(distance2);


  if (distance1 == -1 && distance2 == -1) {
    return false;
  }

  if ((distance1 != -1 && distance1 < obstacleDistanceThreshold) || 
      (distance2 != -1 && distance2 < obstacleDistanceThreshold)) {


    if (distance1 != -1 && (distance2 == -1 || distance1 < distance2)) {
      motors.setSpeeds(200, -200); 
      delay(500);
    } else if (distance2 != -1 && (distance1 == -1 || distance2 < distance1)) {
      motors.setSpeeds(-200, 200); 
      delay(500);
    }
    return true;  
  }

  return false; 
}


bool followLight() {
  int ldrLeft = ldrLError ? -1 : analogRead(ldrLeftPin);  
  int ldrRight = ldrRError ? -1 : analogRead(ldrRightPin);  

  Serial.print("ldrLeft: ");
  Serial.println(ldrLeft);
  Serial.print("ldrRight: ");
  Serial.println(ldrRight);
  delay(500);

  if (ldrLeft == -1 && ldrRight == -1) {
    return false;
  }

  if (ldrRight == -1 && ldrLeft > lightThreshold) {
    motors.setSpeeds(50, 200);  
    return true;
  }

  if (ldrLeft == -1 && ldrRight > lightThreshold) {
    motors.setSpeeds(200, 50);  
    return true;
  }

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
  if(ultraLError == true || ultraRError == true){
    motors.setSpeeds(200, 200);
    delay(500);
    motors.setSpeeds(-200, 200);
    delay(150);
  }else{
    motors.setSpeeds(100, 100); 
  }
  
}

void SensorValue(){
    imu.read();

    if(abs(accValue - imu.a.x) <= 5){
      motors.setSpeeds(-200, -200);
      delay(300);
      motors.setSpeeds(200, -200);
      delay(500);
    }

    accValue = imu.a.x;

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
