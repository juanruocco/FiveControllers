//#include <Arduino.h>

#include "ESP32_NOW.h"
#include "WiFi.h"
#include <esp_mac.h>  // For the MAC2STR and MACSTR macros

#include <BleConnectionStatus.h>
#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

#include "JoystickDirection.h"
#include "JoystickButton.h"

#define ESPNOW_WIFI_CHANNEL 6

/* Classes */

// Creating a new class that inherits from the ESP_NOW_Peer class is required.

class ESP_NOW_Broadcast_Peer : public ESP_NOW_Peer {
public:
  // Constructor of the class using the broadcast address
  ESP_NOW_Broadcast_Peer(uint8_t channel, wifi_interface_t iface, const uint8_t *lmk) : ESP_NOW_Peer(ESP_NOW.BROADCAST_ADDR, channel, iface, lmk) {}

  // Destructor of the class
  ~ESP_NOW_Broadcast_Peer() {
    remove();
  }

  // Function to properly initialize the ESP-NOW and register the broadcast peer
  bool begin() {
    if (!ESP_NOW.begin() || !add()) {
      log_e("Failed to initialize ESP-NOW or register the broadcast peer");
      return false;
    }
    return true;
  }

  // Function to send a message to all devices within the network
  bool send_message(const uint8_t *data, size_t len) {
    if (!send(data, len)) {
      log_e("Failed to broadcast message");
      return false;
    }
    return true;
  }
};


uint32_t msg_count = 0;
// Create a broadcast peer object
ESP_NOW_Broadcast_Peer broadcast_peer(ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);

//BleGamepad bleGamepad;
//BleGamepadConfiguration bleGamepadConfig;  
  
XboxGamepadDevice *gamepad;

bool xInvertJoyLD = false;
bool yInvertJoyLD = false;
bool xInvertJoyRD = true;
bool yInvertJoyRD = false;

bool xInvertJoyLU = true;
bool yInvertJoyLU = true;
bool xInvertJoyRU = false;
bool yInvertJoyRU = false;

bool buttonJoystickLeftDownPressed = false;

BleCompositeHID compositeHID("P2f", "P2f", 25);

//JOYSTICK VARIABLES
//LEFT  
const int xJoystickLeftDownPin   = 12;  //13; //12;
const int yJoystickLeftDownPin   = 13;  //14; //13;
const int xJoystickLeftUpPin     = 27;  //11; //27;
const int yJoystickLeftUpPin     = 14;  //12; //14;
const int buttonJoystickLeftDown = 25;  //19; //15;

//RIGTH
const int xJoystickRigthDownPin = 34; //18; //34;
const int yJoystickRigthDownPin = 35; //8;  //35;
const int xJoystickRigthUpPin   = 36; //16; //36;
const int yJoystickRigthUpPin   = 39; //17; //39;
const int triggerButtons4       =  4; //20; //4;

//JoystickSensor joystickP1Direction = JoystickSensor(1, 2, false, false)
JoystickSensor joystickP2Direction   = JoystickSensor(6, 7, true, true);
//JoystickSensor joystickP3Direction = JoystickSensor(17, 18, false, false)
//JoystickSensor joystickP4Direction = JoystickSensor( 9, 10, false, false)
//JoystickSensor joystickP5Direction = JoystickSensor(20, 19, false, false)


JoystickButton joystickP2Button = JoystickButton(4, 5, false, false, 0);


/*
//ESP32 P2
BleCompositeHID compositeHID("P2c", "P2c", 22);
BleCompositeHID compositeHID("P3c", "P3c", 22);
//JOYSTICK VARIABLES
//LEFT  
const int xJoystickLeftDownPin =  13;//12;
const int yJoystickLeftDownPin =  14;//13;
const int xJoystickLeftUpPin =    11 ;//27;
const int yJoystickLeftUpPin =    12;//14;
const int buttonJoystickLeftDown = 9;//15;

//RIGTH
const int xJoystickRigthDownPin = 18;//34;
const int yJoystickRigthDownPin = 8;//35;
const int xJoystickRigthUpPin = 16;//36;
const int yJoystickRigthUpPin = 17;//39;
const int triggerButtons4 = 20;//4;
*/

//const int NORTH_DIRE = 135;
//const int SOUTH_DIRE = 136;
///const int WEST_DIRE = 137;
//const int EAST_DIRE = 138;
//const int TRIGER_LEFT  = 141;
//const int TRIGER_RIGHT = 142;


int minRange = 0;
int maxRange = 32767;

int xCenterCalibrationLeftUp = 2000;
int xCenterCalibrationLeftDown = 2000;
int xCenterCalibrationRigthUp = 2000;
int xCenterCalibrationRigthDown = 2000;

int yCenterCalibrationLeftUp = 2000;
int yCenterCalibrationLeftDown = 2000;
int yCenterCalibrationRigthUp = 2000;
int yCenterCalibrationRigthDown = 2000;

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
/*uint16_t gamepadButtonsJoystick[2][9] =   { 
                                              { NORTH_DIRE      ,XBOX_BUTTON_SELECT   ,EAST_DIRE        ,XBOX_BUTTON_LS   ,SOUTH_DIRE     ,XBOX_BUTTON_LB   ,WEST_DIRE       ,TRIGER_LEFT          , XBOX_BUTTON_HOME  },                                 
                                              { XBOX_BUTTON_Y   ,TRIGER_RIGHT         ,XBOX_BUTTON_B    ,XBOX_BUTTON_RB  , XBOX_BUTTON_A  ,XBOX_BUTTON_RS   ,XBOX_BUTTON_X   ,XBOX_BUTTON_START    ,XBOX_BUTTON_SHARE }     
                                          };*/

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
  pinMode(buttonJoystickLeftDown , INPUT_PULLUP);
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

  xCenterCalibrationLeftUp = analogRead(xJoystickLeftUpPin);
  yCenterCalibrationLeftUp = analogRead(yJoystickLeftUpPin);
  xCenterCalibrationLeftDown = analogRead(xJoystickLeftDownPin);
  yCenterCalibrationLeftDown = analogRead(yJoystickLeftDownPin);
  xCenterCalibrationRigthUp = analogRead(xJoystickRigthUpPin);
  yCenterCalibrationRigthUp = analogRead(yJoystickRigthUpPin);
  xCenterCalibrationRigthDown = analogRead(xJoystickRigthDownPin);
  yCenterCalibrationRigthDown = analogRead(yJoystickRigthDownPin);
  joystickP2Direction.init();
  joystickP2Button.init(gamepad);


  // Initialize the Wi-Fi module
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  Serial.println("ESP-NOW Example - Broadcast Master");
  Serial.println("Wi-Fi parameters:");
  Serial.println("  Mode: STA");
  Serial.println("  MAC Address: " + WiFi.macAddress());
  Serial.printf("  Channel: %d\n", ESPNOW_WIFI_CHANNEL);

  // Register the broadcast peer
  if (!broadcast_peer.begin()) {
    Serial.println("Failed to initialize broadcast peer");
    Serial.println("Reebooting in 5 seconds...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Setup complete. Broadcasting messages every 5 seconds.");
  
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




void joystickDirection(int xJoyPinLeft, int yJoyPinLeft, int xJoyPinRight, int yJoyPinRight){

  int xReading = joystickP2Direction.readX();
  int yReading = joystickP2Direction.readY();
  //int xReading = readAxis(xJoyPinLeft, xInvertJoyLD, xCenterCalibrationLeftDown);
  //int yReading = readAxis(yJoyPinLeft, yInvertJoyLD, yCenterCalibrationLeftDown);
  int xReadingRigth = 2000;//readAxis(xJoyPinRight, xInvertJoyRD, xCenterCalibrationRigthDown);
  int yReadingRigth = 2000;//readAxis(yJoyPinRight, yInvertJoyRD, yCenterCalibrationRigthDown);

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
/*
void joystickButtons(int xReading, int yReading, int row){
  //TODO: improve: cancel other buttons when one button is pressed
        
  int thresholdMax = 25000; //31327, 32767
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
*/
  /*
void joysticksButtons(int xJoyPinLeft, int yJoyPinLeft, int xJoyPinRight, int yJoyPinRight){

  int xReading = 0;//readAxis(xJoyPinLeft, xInvertJoyLU, xCenterCalibrationLeftUp);
  int yReading = 0;//readAxis(yJoyPinLeft, yInvertJoyLU, yCenterCalibrationLeftUp);
  int xReadingRigth =0;// readAxis(xJoyPinRight, xInvertJoyRU, xCenterCalibrationRigthUp);
  int yReadingRigth = 0;//readAxis(yJoyPinRight, yInvertJoyRU, yCenterCalibrationRigthUp);
  

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
*/
void joysticksButtons(){
  if(compositeHID.isConnected()){
    joystickP2Button.detectAndPress();
  }
  
  
  /*
  Serial.print("x LU: ");
  Serial.print(xReading);
  Serial.print(", y LU: ");
  Serial.print(yReading);
  Serial.print(", x RU: ");
  Serial.print(xReadingRigth);
  Serial.print(", y RU: ");
  Serial.println(yReadingRigth);
  */

  //joystickButtons(xReading, yReading, 0);
  //joystickButtons(xReadingRigth, yReadingRigth, 1);
//> <
  delay(1);
}



void sendMessage(){
  char data[32];
  snprintf(data, sizeof(data), "Hello, World! #%lu", msg_count++);

  Serial.printf("Broadcasting message: %s\n", data);

  if (!broadcast_peer.send_message((uint8_t *)data, sizeof(data))) {
    Serial.println("Failed to broadcast message");
  }
}
/*
//PRESS GAMEPAD JOYSTICK BUTTONS
void pressButtonsJoystickManager(int row, int col){
  if (gamepadButtonsJoystickPressed[row][col] == false){

    sendMessage();
    
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
*/


/*
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
*/


void loop() {
  
  //joystickDirection1();
  int startLoop = millis();
  //keyboardDetection();
  joystickDirection(xJoystickLeftDownPin, yJoystickLeftDownPin, xJoystickRigthDownPin, yJoystickRigthDownPin);
  //joysticksButtons(xJoystickLeftUpPin, yJoystickLeftUpPin, xJoystickRigthUpPin, yJoystickRigthUpPin);
  joysticksButtons();
  /*
  int delayLoop = millis() - startLoop;

  int buttonLeftDown = digitalRead(buttonJoystickLeftDown);
  int buttonRigthUp= digitalRead(triggerButtons4);

  
  if (buttonLeftDown == LOW) {
    if (buttonJoystickLeftDownPressed == false) {
        buttonJoystickLeftDownPressed = true;
        //gamepad->press(gamepadButtons[0][8]);
        gamepad-> setLeftTrigger(30000);
        gamepad->sendGamepadReport();
    }    
  } else {
    if (buttonJoystickLeftDownPressed == true) {
      //gamepad->release(gamepadButtons[0][7]);
      gamepad-> setRightTrigger(0);
      gamepad-> setLeftTrigger(0);
      gamepad->sendGamepadReport();
      buttonJoystickLeftDownPressed = false;
    }
    //buttonJoystickLeftDownPressed = false;
  }
  */

  /*
  if (buttonRigthUp == HIGH) {
    if (gamepadButtonsJoystickPressed[1][1] == false) {
        gamepadButtonsJoystickPressed[1][1] = true;
        //gamepad->press(gamepadButtons[1][1]);
        gamepad-> setRightTrigger(30000);
        gamepad->sendGamepadReport();
      }    
  } else {
    if (gamepadButtonsJoystickPressed[1][1] == true) {
      //gamepad->release(gamepadButtons[1][1]);
      gamepad-> setRightTrigger(0);
      gamepad-> setLeftTrigger(0);
      gamepad->sendGamepadReport();
      gamepadButtonsJoystickPressed[1][1] = false;
    }
    
    //gamepadButtonsJoystickPressed[0][1] = false;
  }       
  */
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