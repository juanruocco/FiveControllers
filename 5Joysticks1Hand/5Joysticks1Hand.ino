//#include <Arduino.h>
//#include <BleGamepad.h>

#include <BleConnectionStatus.h>
#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

//BleGamepad bleGamepad;
//BleGamepadConfiguration bleGamepadConfig;  
  
XboxGamepadDevice *gamepad;
//
/*
BleCompositeHID compositeHID("Multi Left2", "Jota", 100);
bool xInvertJoyLD = false;
bool yInvertJoyLD = false;
bool xInvertJoyRD = true;
bool yInvertJoyRD = false;

bool xInvertJoyLU = true;
bool yInvertJoyLU = true;
bool xInvertJoyRU = false;
bool yInvertJoyRU = false;
*/

BleCompositeHID compositeHID("Multi C1", "Jota", 100);
//BleCompositeHID compositeHID("Multi C2", "Jota", 100);

bool xInvertJoyLD = false;
bool yInvertJoyLD = false;
bool xInvertJoyRD = true;
bool yInvertJoyRD = false;

bool xInvertJoyLU = true;
bool yInvertJoyLU = true;
bool xInvertJoyRU = false;
bool yInvertJoyRU = false;


//BleCompositeHID compositeHID("Multi Test", "Jota", 100);

//BleKeyboard Keyboard("FiveInOne", "Bluetooth Device Manufacturer", 100);

//BleGamepad bleGamepad("RO1", "jota", 100);

const int NORTH_DIRE = 135;
const int SOUTH_DIRE = 136;
const int WEST_DIRE = 137;
const int EAST_DIRE = 138;
const int TRIGER_LEFT  = 141;
const int TRIGER_RIGHT = 142;

//JOYSTICK VARIABLES

//LEFT  
const int xJoystickLeftDownPin = 12;//13;//12;
const int yJoystickLeftDownPin = 13;//14;//13;
const int xJoystickLeftUpPin = 27;//11 ;//27;
const int yJoystickLeftUpPin = 14;//12;//14;
const int triggerButtons2 = 15;//19;//15;

//RIGTH
const int xJoystickRigthDownPin = 34;//18;//34;
const int yJoystickRigthDownPin = 35;//8;//35;
const int xJoystickRigthUpPin = 36;//16;//36;
const int yJoystickRigthUpPin = 39;//17;//39;
const int triggerButtons4 = 4;//20;//4;


int range = 32767;  // output range of X or Y movement
int minRange = 0;
int maxRange = 32767;
int threshold = 300;//TODO: check if reduce this threshold to more precicion but errors int the button detection

int centerCalibrationLeftUp = 2000;
int centerCalibrationLeftDown = 2000;
int centerCalibrationRigthUp = 2000;
int centerCalibrationRigthDown = 2000;



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

boolean gamepadButtonsJoystickPressed[2][9] = { false };                                  
uint16_t gamepadButtonsJoystick[2][9] =   { 
                                              { NORTH_DIRE      ,XBOX_BUTTON_SELECT   ,EAST_DIRE        ,XBOX_BUTTON_LS   ,SOUTH_DIRE     ,XBOX_BUTTON_LB   ,WEST_DIRE       ,TRIGER_LEFT          , XBOX_BUTTON_HOME  },                                 
                                              { XBOX_BUTTON_Y   ,TRIGER_RIGHT         ,XBOX_BUTTON_B    ,XBOX_BUTTON_RB  , XBOX_BUTTON_A  ,XBOX_BUTTON_RS   ,XBOX_BUTTON_X   ,XBOX_BUTTON_START    ,XBOX_BUTTON_SHARE }     
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
  //gamepad = new XboxGamepadDevice(config);
  gamepad = new XboxGamepadDevice(config);

  FunctionSlot<XboxGamepadOutputReportData> vibrationSlot(OnVibrateEvent);
  gamepad->onVibrate.attach(vibrationSlot);
  
  compositeHID.addDevice(gamepad);
  Serial.println("Starting composite HID device...");
  compositeHID.begin(hostConfig);



  //Setup Switch
  pinMode(triggerButtons2 , INPUT_PULLDOWN);
  pinMode(triggerButtons4, INPUT_PULLDOWN);

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

  centerCalibrationLeftUp = analogRead(xJoystickLeftUpPin);
  centerCalibrationLeftDown = analogRead(xJoystickLeftDownPin);
  centerCalibrationRigthUp = analogRead(xJoystickRigthUpPin);
  centerCalibrationRigthDown = analogRead(xJoystickRigthDownPin);
  
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

int readAxis(int thisAxis, boolean inverse, int centerCalibration) {// output: -4095 to 4095, 0 in  calibration choose default

  int reading = analogRead(thisAxis);
  //Serial.print("analogue: ");
  //Serial.println(reading);
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

  int xReading = readAxis(xJoyPinLeft, xInvertJoyLD, centerCalibrationLeftDown);
  int yReading = readAxis(yJoyPinLeft, yInvertJoyLD, centerCalibrationLeftDown);
  int xReadingRigth = readAxis(xJoyPinRight, xInvertJoyRD, centerCalibrationRigthDown);
  int yReadingRigth = readAxis(yJoyPinRight, yInvertJoyRD, centerCalibrationRigthDown);

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
  
  delay(1);
}

void joystickButtons(int xReading, int yReading, int row){
  //TODO: improve: cancel other buttons when one button is pressed
        
  int thresholdMax = 28000; //31327, 32767
  int thresholdMedium = 15000; //31327, 32767
  int thresholdMin = 1000; 
  if(compositeHID.isConnected()){
    
    // X = 0
    if(xReading < thresholdMin && xReading > -thresholdMin){
        if(yReading < thresholdMin && yReading > -thresholdMin){
            unpressJoystickButtonsRow(row);
        }

        //Up Button
        if(yReading > thresholdMax){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row,0);
        }
      
        //Down Button
        if(yReading < -thresholdMax){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row,4);
        }

    }

    //X Rigth Button
    if(xReading > thresholdMax && yReading < thresholdMin && yReading > -thresholdMin ){
        //unpressJoystickButtonsRow(row);
        pressButtonsJoystickManager(row, 2);
    }

    // X > 0
    if(xReading > thresholdMedium ){

        if(yReading > thresholdMedium){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row, 1);
        }

        if(yReading < -thresholdMedium){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row, 3);
        }
    }

    //X Left Button
    if(xReading < -thresholdMax && yReading < thresholdMin && yReading > -thresholdMin){
        //unpressJoystickButtonsRow(row);
        pressButtonsJoystickManager(row, 6);
    }

    // X < 0
    if(xReading < -thresholdMedium ){

        if(yReading < -thresholdMedium){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row, 5);
        }
        
        if(yReading > thresholdMedium){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row, 7);
        }        
    }
    //gamepad->sendGamepadReport();
  }
}

void joysticksButtons(int xJoyPinLeft, int yJoyPinLeft, int xJoyPinRight, int yJoyPinRight){




  int xReading = readAxis(xJoyPinLeft, xInvertJoyLU, centerCalibrationLeftUp);
  int yReading = readAxis(yJoyPinLeft, yInvertJoyLU, centerCalibrationLeftUp);
  int xReadingRigth = readAxis(xJoyPinRight, xInvertJoyRU, centerCalibrationRigthUp);
  int yReadingRigth = readAxis(yJoyPinRight, yInvertJoyRU, centerCalibrationRigthUp);
  
  
  Serial.print("x LU: ");
  Serial.print(xReading);
  Serial.print(", y LU: ");
  Serial.print(yReading);
  Serial.print(", x RU: ");
  Serial.print(xReadingRigth);
  Serial.print(", y RU: ");
  Serial.println(yReadingRigth);
  

  joystickButtons(xReading, yReading, 0);
  joystickButtons(xReadingRigth, yReadingRigth, 1);

//> <
  
  
  delay(1);
}

//PRESS GAMEPAD JOYSTICK BUTTONS
void pressButtonsJoystickManager(int row, int col){
  if (gamepadButtonsJoystickPressed[row][col] == false){
    pressButtonsJoystick(row, col);
    gamepadButtonsJoystickPressed[row][col] = true;
  }
}



void pressButtonsJoystick(int row, int col){
  
  Serial.print("PRESS  : ");
  Serial.print(gamepadButtonsJoystick[row][col]);
  Serial.print(", Row: ");
  Serial.print(row);
  Serial.print(" ,Col: ");
  Serial.println(col);

  if( gamepadButtonsJoystick[row][col] == NORTH_DIRE ){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::NORTH);
  }else if(gamepadButtonsJoystick[row][col] == SOUTH_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::SOUTH);
  }else if(gamepadButtonsJoystick[row][col] == WEST_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::WEST);
  }else if(gamepadButtonsJoystick[row][col] == EAST_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::EAST);
  }else if(gamepadButtonsJoystick[row][col] == TRIGER_LEFT){
    gamepad-> setLeftTrigger(30000);
  }else if(gamepadButtonsJoystick[row][col] == TRIGER_RIGHT){
    gamepad-> setRightTrigger(30000);
  }else{
    gamepad->press(gamepadButtonsJoystick[row][col]);
  }
  
  gamepad->sendGamepadReport();

}  

//PRESS GAMEPAD BUTTONS
void pressButtons(int row, int col){
  
  Serial.print("PRESS df  : ");
  Serial.print(gamepadButtons[row][col]);
  Serial.print(", Row: ");
  Serial.print(row);
  Serial.print(" ,Col: ");
  Serial.println(col);

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

}  

//UNPRESS BUTTONS
void unpressJoystickButtonsRow(int row){

  for(int i = 0 ; i < 8; i++){
    unpressButtonsJoystickManager(row,i);
  }
  //gamepad->sendGamepadReport();

}

void unpressButtonsJoystickManager(int row, int col){
  if (gamepadButtonsJoystickPressed[row][col] == true){
    unpressButtonsJoystick( row, col);
    gamepadButtonsJoystickPressed[row][col] = false;
    Serial.print("unpres realease row: ");
    Serial.print(row);
    Serial.print(" ,col: ");
    Serial.println(col);
  }
}

void unpressButtonsJoystick(int row, int col){

  //Serial.print("release: ");
  Serial.println(gamepadButtonsJoystick[row][col]);
  if( gamepadButtonsJoystick[row][col] == NORTH_DIRE || gamepadButtonsJoystick[row][col] == SOUTH_DIRE || gamepadButtonsJoystick[row][col] == WEST_DIRE  || gamepadButtonsJoystick[row][col] == EAST_DIRE){
    gamepad  -> releaseDPad();
    //Serial.print("release DPad ");
  }else if(gamepadButtonsJoystick[row][col] == TRIGER_LEFT){  
      gamepad  -> setLeftTrigger(0);
      gamepad  -> setRightTrigger(0);
  }else if(gamepadButtonsJoystick[row][col] == TRIGER_RIGHT){  
      gamepad  -> setLeftTrigger(0);
      gamepad  -> setRightTrigger(0);
  }else{
    gamepad->release(gamepadButtonsJoystick[row][col]);
    //Serial.println("release gamepad buttons ");
  }
  gamepad->sendGamepadReport();
  //delay(1);
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




void keyboardDetection(){

  //Serial.print("start keyboard ");
  //Serial.print(millis());
  //Keyboard Detection
  for (int col = 0; col < 4; col++) {
    digitalWrite(buttonCol[col], LOW);
    //delay(1);
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
    
  
    digitalWrite(buttonCol[col], HIGH);
    //delay(1);
  }
  //Serial.print(" ");
  //Serial.print(millis());
  //Serial.println(" finish keyboard");
  
}



void loop() {
  
  //joystickDirection1();
  int startLoop = millis();
  //keyboardDetection();
  joystickDirection(xJoystickLeftDownPin, yJoystickLeftDownPin, xJoystickRigthDownPin, yJoystickRigthDownPin);
  joysticksButtons(xJoystickLeftUpPin, yJoystickLeftUpPin, xJoystickRigthUpPin, yJoystickRigthUpPin);
  int delayLoop = millis() - startLoop;

  int buttonLeftUp = digitalRead(triggerButtons2);
  int buttonRigthUp= digitalRead(triggerButtons4);

  
  if (buttonLeftUp == HIGH) {
    if (gamepadButtonsJoystickPressed[0][8] == false) {
        gamepadButtonsJoystickPressed[0][8] = true;
        //gamepad->press(gamepadButtons[0][8]);
        gamepad-> setLeftTrigger(30000);
        gamepad->sendGamepadReport();
    }    
  } else {
    if (gamepadButtonsJoystickPressed[0][8] == true) {
      gamepad->release(gamepadButtons[0][8]);
      gamepad-> setRightTrigger(0);
      gamepad-> setLeftTrigger(0);
      gamepad->sendGamepadReport();
      gamepadButtonsJoystickPressed[0][8] = false;
    }
    //gamepadButtonsJoystickPressed[0][8] = false;
  }

  if (buttonRigthUp == HIGH) {
    if (gamepadButtonsJoystickPressed[1][8] == false) {
        gamepadButtonsJoystickPressed[1][8] = true;
        //gamepad->press(gamepadButtons[1][8]);
        gamepad-> setRightTrigger(30000);
        gamepad->sendGamepadReport();
      }    
  } else {
    if (gamepadButtonsJoystickPressed[1][8] == true) {
      //gamepad->release(gamepadButtons[1][8]);
      gamepad-> setRightTrigger(0);
      gamepad-> setLeftTrigger(0);
      gamepad->sendGamepadReport();
      gamepadButtonsJoystickPressed[1][8] = false;
    }
    //gamepadButtonsJoystickPressed[0][8] = false;
  }       

  //Serial.print("loop elapse: ");
  //Serial.println(delayLoop);
  //delay(2);


  //joystickDirection(xJoystickRigthDownPin, yJoystickRigthDownPin, 'f', 'h', 't', 'g', leftJoystickPressed4, rigthJoystickPressed4, riupJoystickPressed4, downJoystickPressed2);
  //delay(5);
  //joystickDirection(xJoystick3, yJoystick3, 'j', 'l', 'i', 'k', leftJoystickPressed3, rigthJoystickPressed3, upJoystickPressed3, downJoystickPressed3);
  //delay(20);
  //joystickDirection(xJoystickRigthDownPin, yJoystickRigthDownPin, KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, leftJoystickPressed4, rigthJoystickPressed4, upJoystickPressed4, downJoystickPressed4);
  //delay(20);

}