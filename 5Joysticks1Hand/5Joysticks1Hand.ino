#include <Arduino.h>
//#include <BleKeyboard.h> 
//#include <Wire.h>

#include <BleGamepad.h>

BleGamepad bleGamepad;
BleGamepadConfiguration bleGamepadConfig;  

//BleKeyboard Keyboard("FiveInOne", "Bluetooth Device Manufacturer", 100);

//BleGamepad bleGamepad("RO1", "jota", 100);


const int xJoystick1 = 25;
const int yJoystick1 = 26;
bool leftJoystickPressed1 = false;
bool rigthJoystickPressed1 = false;
bool upJoystickPressed1 = false;
bool downJoystickPressed1 = false;

const int xJoystick2 = 27;
const int yJoystick2 = 14;
bool leftJoystickPressed2 = false;
bool rigthJoystickPressed2 = false;
bool upJoystickPressed2 = false;
bool downJoystickPressed2 = false;

const int xJoystick3 = 25;
const int yJoystick3 = 26;
bool leftJoystickPressed3 = false;
bool rigthJoystickPressed3 = false;
bool upJoystickPressed3 = false;
bool downJoystickPressed3 = false;

const int xJoystick4 = 27;
const int yJoystick4 = 14;
bool leftJoystickPressed4 = false;
bool rigthJoystickPressed4 = false;
bool upJoystickPressed4 = false;
bool downJoystickPressed4 = false;

const int xJoystick5 = 12;
const int yJoystick5 = 13;
bool leftJoystickPressed5 = false;
bool rigthJoystickPressed5 = false;
bool upJoystickPressed5 = false;
bool downJoystickPressed5 = false;

int range = 32767;  // output range of X or Y movement
int minRange = 0;
int maxRange = 32767;
int threshold = 100;//range / 6;  // resting threshold
int centerCalibration = 1700;

void setup() {
  //Serial.begin(115200);
  Serial.println("Starting BLE work!");
  //Keyboard.begin();
  bleGamepadConfig.setControllerType(CONTROLLER_TYPE_JOYSTICK); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
  bleGamepad.begin(&bleGamepadConfig);

  delay(300);
    
  centerCalibration = analogRead(xJoystick1);
}

int readAxis(int thisAxis, boolean inverse) {

  int reading = analogRead(thisAxis);
  Serial.print("analogue: ");
  Serial.print(reading);
  int distance = reading - centerCalibration;

  if (abs(distance) < threshold) {
    distance = 0;
  }

  int defaultCalibration = 4096/2 - centerCalibration;
  Serial.print(" , calibra: ");
  Serial.print(defaultCalibration);

  int output =  centerCalibration + distance + defaultCalibration;
  if(inverse == true){
    output = centerCalibration-distance + defaultCalibration;
  }

  if(output>= 4095){
    output = 4095;
  }
  

  Serial.print(" , output: ");
  Serial.print(output);


  reading = map(output, 0, 4096, 0, range);
  Serial.print(", convert: ");
  Serial.print(reading);
  Serial.print(" , ");
  

  

  return reading;
}


void joystickDirection(int xJoyPin, int yJoyPin){

  int xReading = readAxis(xJoyPin, true);
  int yReading = readAxis(yJoyPin, false);
  //int yReading = 0;
  Serial.print("x: ");
  Serial.print(xReading);
  Serial.print(", y: ");
  Serial.println(yReading);
  if (bleGamepad.isConnected())
  {
    bleGamepad.setLeftThumb(xReading, yReading);
  }
  
}


/*void joystickToKeyboardDirection(int xJoyPin, int yJoyPin, int leftKey, int rightKey, int upKey, int downKey, bool &leftPressed, bool &rightPressed, bool &upPressed, bool &downPressed){

  int xReading = readAxis(xJoyPin, true);
  int yReading = readAxis(yJoyPin, true);
  Serial.print("x: ");
  Serial.print(xReading);
  Serial.print(", y: ");
  Serial.println(yReading);
  
  if (xReading >=15){
    rightPressed = true;
    Keyboard.press(rightKey);
  }else if(xReading <= -15){
    leftPressed = true;
    Keyboard.press(leftKey);
  }else{
    if(rightPressed == true){
      rightPressed = false;
      Keyboard.release(rightKey);
    }
    if(leftPressed == true){
      leftPressed = false;
      Keyboard.release(leftKey);
    }  
  }

  if (yReading >=15){
    upPressed = true;
    Keyboard.press(upKey);
  }else if(yReading <= -15){
    downPressed = true;
    Keyboard.press(downKey);
  }else{
    if(upPressed == true){
      upPressed = false;
      Keyboard.release(upKey);
    }
    if(downPressed == true){
      downPressed = false;
      Keyboard.release(downKey);
    }
  }
}*/

void loop() {
  
  //joystickDirection1();
  joystickDirection(xJoystick1, yJoystick1);
  delay(5);
  //joystickDirection(xJoystick2, yJoystick2, 'f', 'h', 't', 'g', leftJoystickPressed2, rigthJoystickPressed2, upJoystickPressed2, downJoystickPressed2);
  //delay(5);
  //joystickDirection(xJoystick3, yJoystick3, 'j', 'l', 'i', 'k', leftJoystickPressed3, rigthJoystickPressed3, upJoystickPressed3, downJoystickPressed3);
  //delay(20);
  //joystickDirection(xJoystick4, yJoystick4, KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, leftJoystickPressed4, rigthJoystickPressed4, upJoystickPressed4, downJoystickPressed4);
  //delay(20);

}