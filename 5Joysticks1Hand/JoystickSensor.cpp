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
  lastTimeCheck = millis();
}

void JoystickSensor::setCallback(CallbackFunctionWithArgs callback) {
  _callback = callback; // Guarda el puntero
}


int JoystickSensor::readX(){
  return readAxis(pinX, inverseXDirection, centerXCalibration);
}

int JoystickSensor::readY(){
  return readAxis(pinY, inverseYDirection, centerYCalibration);
}

void JoystickSensor::detectAndPress(boolean callback){

  int xPosition = readX();
  int yPosition = readY();
  press(xPosition, yPosition, callback);

}

void JoystickSensor::press(int xPosition, int yPosition, boolean callback){
  
  boolean lastValueIsDiferent = lastPositionX != xPosition || lastPositionY != yPosition;
  boolean positionDiferenceIsBigToCallback = abs(xPosition-lastPositionX)>distanceToAvoidCallback || abs(yPosition-lastPositionY)>distanceToAvoidCallback || (xPosition == 0 && yPosition == 0);
  boolean pastTimeMaximumToCallback = (millis() - lastTimeCheck) >maxTimeToAvoidCallback;
  if( (lastValueIsDiferent && positionDiferenceIsBigToCallback) || pastTimeMaximumToCallback){
    
    if (_callback != NULL && callback) {
      _callback(xPosition, yPosition); // ¡Aquí se ejecuta la función del .ino y recibe los valores!
      //Serial.println("Libreria: El callback del .ino terminó de ejecutarse.");
    } //else { //Serial.println("Libreria: No hay función de callback configurada.");}

    gamepad->setLeftThumb (xPosition, yPosition);
    gamepad->sendGamepadReport();

    lastPositionX = xPosition;
    lastPositionY = yPosition;
    lastTimeCheck = millis();

  }
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