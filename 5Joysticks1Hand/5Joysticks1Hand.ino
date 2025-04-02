//#include <Arduino.h>
//#include <BleGamepad.h>

#include <BleConnectionStatus.h>
#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

//BleGamepad bleGamepad;
//BleGamepadConfiguration bleGamepadConfig;  

XboxGamepadDevice *gamepad;
//BleCompositeHID compositeHID("Multi Left", "Jota", 100);
//BleCompositeHID compositeHID("Multi Der", "Jota", 100);
BleCompositeHID compositeHID("Multi Test", "Jota", 100);

//BleKeyboard Keyboard("FiveInOne", "Bluetooth Device Manufacturer", 100);

//BleGamepad bleGamepad("RO1", "jota", 100);

const int NORTH_DIRE = 135;
const int SOUTH_DIRE = 136;
const int WEST_DIRE = 137;
const int EAST_DIRE = 138;
const int TRIGER_LEFT  = 141;
const int TRIGER_RIGHT = 142;

//JOYSTICK VARIABLES
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

/*
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
*/

int range = 32767;  // output range of X or Y movement
int minRange = 0;
int maxRange = 32767;
int threshold = 100;//range / 6;  // resting threshold
int centerCalibration = 1700;


//BUTTONS VARIABLES
int buttonRow[5] = { 17, 18,  4,  5, 16 };  //Input //{ 17, 18, 4, 5 , 16 }; //{ 4, 16, 17, 5, 18 };
int buttonCol[4] = { 21, 23, 22,  19     };  //Output

boolean keyboardLettersPressed[5][4] = { false };

uint16_t gamepadButtons[5][4] =   { 
                                    { NORTH_DIRE      , XBOX_BUTTON_LS          , XBOX_BUTTON_Y               , XBOX_BUTTON_RS          },                                 
                                    { EAST_DIRE       , XBOX_BUTTON_SELECT      , XBOX_BUTTON_B               , XBOX_BUTTON_SHARE       },     
                                    { SOUTH_DIRE      , XBOX_BUTTON_LB          , XBOX_BUTTON_A               , XBOX_BUTTON_RB          },        
                                    { WEST_DIRE       , 1                       , XBOX_BUTTON_X               , XBOX_BUTTON_START       },    
                                    { TRIGER_LEFT     , XBOX_BUTTON_HOME        , TRIGER_RIGHT                , 1                       }                             
                                  };

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  //Keyboard.begin();
  //bleGamepadConfig.setControllerType(CONTROLLER_TYPE_JOYSTICK); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
  //bleGamepad.begin(&bleGamepadConfig);


  XboxSeriesXControllerDeviceConfiguration* config = new XboxSeriesXControllerDeviceConfiguration();

  // The composite HID device pretends to be a valid Xbox controller via vendor and product IDs (VID/PID).
  // Platforms like windows/linux need this in order to pick an XInput driver over the generic BLE GATT HID driver. 
  BLEHostConfiguration hostConfig = config->getIdealHostConfiguration();
  Serial.println("Using VID source: " + String(hostConfig.getVidSource(), HEX));
  Serial.println("Using VID: " + String(hostConfig.getVid(), HEX));
  Serial.println("Using PID: " + String(hostConfig.getPid(), HEX));
  Serial.println("Using GUID version: " + String(hostConfig.getGuidVersion(), HEX));
  Serial.println("Using serial number: " + String(hostConfig.getSerialNumber()));
    
  // Set up gamepad
  gamepad = new XboxGamepadDevice(config);

  FunctionSlot<XboxGamepadOutputReportData> vibrationSlot(OnVibrateEvent);
  gamepad->onVibrate.attach(vibrationSlot);
  
  compositeHID.addDevice(gamepad);
  Serial.println("Starting composite HID device...");
  compositeHID.begin(hostConfig);



  //Setup Switch
  pinMode(buttonRow[0], INPUT_PULLUP);
  pinMode(buttonRow[1], INPUT_PULLUP);
  pinMode(buttonRow[2], INPUT_PULLUP);
  pinMode(buttonRow[3], INPUT_PULLUP);
  pinMode(buttonRow[4], INPUT_PULLUP);
  
  pinMode(buttonCol[0], OUTPUT);
  pinMode(buttonCol[1], OUTPUT);
  pinMode(buttonCol[2], OUTPUT);
  pinMode(buttonCol[3], OUTPUT);

  digitalWrite(buttonCol[0], HIGH);
  digitalWrite(buttonCol[1], HIGH);
  digitalWrite(buttonCol[2], HIGH);
  

  delay(300);
  gamepad->setLeftThumb(1, 1);
  gamepad->setRightThumb(1, 1);
  gamepad->sendGamepadReport();
  centerCalibration = analogRead(xJoystick1);
  
}

void OnVibrateEvent(XboxGamepadOutputReportData data)
{
    if(data.weakMotorMagnitude > 0 || data.strongMotorMagnitude > 0){
        //digitalWrite(ledPin, LOW);
    } else {
        //digitalWrite(ledPin, HIGH);
    }
    Serial.println("Vibration event. Weak motor: " + String(data.weakMotorMagnitude) + " Strong motor: " + String(data.strongMotorMagnitude));
}

int readAxis(int thisAxis, boolean inverse) {

  int reading = analogRead(thisAxis);
  //Serial.print("analogue: ");
  //Serial.print(reading);
  int distance = reading - centerCalibration;

  if (abs(distance) < threshold) {
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
  
  return reading;
}


void joystickDirection(int xJoyPinLeft, int yJoyPinLeft, int xJoyPinRight, int yJoyPinRight){

  int xReading = readAxis(xJoyPinLeft, true);
  int yReading = readAxis(yJoyPinLeft, false);
  int xReadingRigth = readAxis(xJoyPinRight, true);
  int yReadingRigth = readAxis(yJoyPinRight, false);
 /* 
  Serial.print("xl: ");
  Serial.print(xReading);
  Serial.print(", yl: ");
  Serial.print(yReading);
  Serial.print(", xr: ");
  Serial.print(xReadingRigth);
  Serial.print(", yr: ");
  Serial.println(yReadingRigth);
*/
  if(compositeHID.isConnected()){
    //Serial.print(", connected ");
    gamepad->setLeftThumb (xReading, yReading);
    gamepad->setRightThumb(xReadingRigth, yReadingRigth);
    gamepad->sendGamepadReport();
  }
  
  delay(10);
}


//PRESS GAMEPAD BUTTONS
void pressButtons(int row, int col){
  
  Serial.print("PRESS  : ");
  Serial.print(gamepadButtons[row][col]);
  Serial.print(", Row: ");
  Serial.print(row);
  Serial.print(" ,Col: ");
  Serial.println(col);
  //XBOX_BUTTON_DPAD_NORTH
  if( gamepadButtons[row][col] == NORTH_DIRE ){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::NORTH);
  }else if(gamepadButtons[row][col] == SOUTH_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::SOUTH);
  }else if(gamepadButtons[row][col] == WEST_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::WEST);
  }else if(gamepadButtons[row][col] == EAST_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::EAST);
  }else if(gamepadButtons[row][col] == TRIGER_LEFT){
    gamepad-> setLeftTrigger(30000);
  }else if(gamepadButtons[row][col] == TRIGER_RIGHT){
    gamepad-> setRightTrigger(30000);
  }else{
    gamepad->press(gamepadButtons[row][col]);
  }
  
  gamepad->sendGamepadReport();

  /*
  if( keyboardLettersPressed[0][0]  ){  // XBOX_BUTTON_DPAD_NORTH 
      gamepad->pressDPadDirectionFlag(XboxDpadFlags::NORTH);
    }else if(gamepadButtons[1][0] ){  // XBOX_BUTTON_DPAD_SOUTH
      gamepad->pressDPadDirectionFlag(XboxDpadFlags::SOUTH);
    }else if(gamepadButtons[2][0] ){  // == XBOX_BUTTON_DPAD_WEST
      gamepad->pressDPadDirectionFlag(XboxDpadFlags::WEST);
    }else if(gamepadButtons[3][0] ){  // == XBOX_BUTTON_DPAD_EAST
      gamepad->pressDPadDirectionFlag(XboxDpadFlags::EAST);
    }else{
      gamepad->press(gamepadButtons[row][col]);
    }
  */
}  

void unpressButtons(int row, int col){
  
  Serial.print("UNPRESS: ");
  Serial.print(gamepadButtons[row][col]);
  Serial.print(", Row: ");
  Serial.print(row);
  Serial.print(" ,Col: ");
  Serial.println(col);

  if( keyboardLettersPressed[0][0]  && gamepadButtons[row][col] == NORTH_DIRE){ 
      gamepad  -> releaseDPad();
  }else if(gamepadButtons[1][0] && gamepadButtons[row][col] == SOUTH_DIRE){  
      gamepad  -> releaseDPad();
  }else if(gamepadButtons[2][0] && gamepadButtons[row][col] == WEST_DIRE){  
      gamepad  -> releaseDPad();
  }else if(gamepadButtons[3][0] && gamepadButtons[row][col] == EAST_DIRE){  
      gamepad  -> releaseDPad();
  }else if(gamepadButtons[0][1] && gamepadButtons[row][col] == TRIGER_LEFT){  
      gamepad  -> setLeftTrigger(0);
      gamepad  -> setRightTrigger(0);
  }else if(gamepadButtons[0][3] && gamepadButtons[row][col] == TRIGER_RIGHT){  
      gamepad  -> setLeftTrigger(0);
      gamepad  -> setRightTrigger(0);
  }else{//Release button
      gamepad->release(gamepadButtons[row][col]);
  }

  gamepad->sendGamepadReport();
}


/*void joystickDirection(int xJoyPin, int yJoyPin){

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
}*/

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

void keyboardDetection(){

  //Keyboard Detection
  for (int col = 0; col < 4; col++) {
    digitalWrite(buttonCol[col], LOW);
    delay(1);
    for (int row = 0; row < 5; row++) {
      int buttonState = digitalRead(buttonRow[row]);
      if (buttonState == LOW) {
        if (keyboardLettersPressed[row][col] == false) {
          keyboardLettersPressed[row][col] = true;
          pressButtons(row, col);
        }
      } else {
        if (keyboardLettersPressed[row][col] == true) {
          unpressButtons(row, col);
        }
        keyboardLettersPressed[row][col] = false;
      }
    }
    
    //delay(5000);
    digitalWrite(buttonCol[col], HIGH);
    delay(1);
  }
}

void loop() {
  
  //joystickDirection1();
  keyboardDetection();
  joystickDirection(xJoystick1, yJoystick1, xJoystick2, yJoystick2);
  delay(2);


  //joystickDirection(xJoystick2, yJoystick2, 'f', 'h', 't', 'g', leftJoystickPressed2, rigthJoystickPressed2, upJoystickPressed2, downJoystickPressed2);
  //delay(5);
  //joystickDirection(xJoystick3, yJoystick3, 'j', 'l', 'i', 'k', leftJoystickPressed3, rigthJoystickPressed3, upJoystickPressed3, downJoystickPressed3);
  //delay(20);
  //joystickDirection(xJoystick4, yJoystick4, KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, leftJoystickPressed4, rigthJoystickPressed4, upJoystickPressed4, downJoystickPressed4);
  //delay(20);

}