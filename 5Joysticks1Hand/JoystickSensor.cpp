#include "JoystickSensor.h"

int readAxis(int thisAxis, boolean inverse, int centerCalibration);

JoystickSensor::JoystickSensor(int pinX, int pinY, boolean inverseXDirection, boolean inverseYDirection){
  this->pinX = pinX;
  this->pinY = pinY;
  centerXCalibration = 2000;
  centerYCalibration = 2000;
  this->inverseXDirection  = inverseXDirection;
  this->inverseYDirection  = inverseYDirection;
}


void JoystickSensor::init(XboxGamepadDevice *gamepad){
  centerXCalibration = analogRead(pinX);
  centerYCalibration = analogRead(pinY);
  Serial.print(" center x calibration: ");
  Serial.println(centerXCalibration);
  this-> gamepad = gamepad;
}

int JoystickSensor::readX(){
  return readAxis(pinX, inverseXDirection, centerXCalibration);
}

int JoystickSensor::readY(){
  return readAxis(pinY, inverseYDirection, centerYCalibration);
}

void JoystickSensor::detectAndPress(){

  int xInput = readX();
  int yInput = readY();

  gamepad->setLeftThumb (xInput, yInput);
  gamepad->sendGamepadReport();
}

int readAxis(int axis, boolean inverse, int centerCalibration) {// output: -4095 to 4095, 0 in  calibration choose default

  int reading = analogRead(axis);
  /*Serial.print("center calib: ");
  Serial.print(centerCalibration);
  Serial.print(" ,analogue: ");
  Serial.println(reading);*/
  int distance = reading - centerCalibration;

  if (abs(distance) < thresholdError) {
    distance = 0;
  }

  int defaultCalibration = 4096/2 - centerCalibration;
  //Serial.print(" , calibra: ");
  //Serial.print(defaultCalibration);

  int output =  centerCalibration + distance + defaultCalibration;
  if(inverse == true){
    output = centerCalibration-distance + defaultCalibration;
  }

  if(output>= 4095){
    output = 4095;
  }

  if(output <= 0){
    output = 0;
  }

  //Serial.print(" , output: ");
  //Serial.print(output);

  reading = map(output, 0, 4096, -range, range);
  //Serial.print(", convert: ");
  //Serial.print(reading);
  //Serial.print(" , ");
  delay(1);
  return reading;
}