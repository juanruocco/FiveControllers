//#include <Arduino.h>

#define DEVICE_2_ENABLED   

//#define DEVICE_2_ENABLED //18 Port
//#define DEVICE_3_ENABLED //17 Port
//#define DEVICE_4_ENABLED //16 Port


#ifdef DEVICE_2_ENABLED
#define DEVICE_ID 2
#endif

#ifdef DEVICE_3_ENABLED
#define DEVICE_ID 3
#endif

#ifdef DEVICE_4_ENABLED
#define DEVICE_ID 4
#endif

uint8_t MAC_P2[] = {0x64, 0xE8, 0x33, 0x7E, 0x04, 0x3C};  
uint8_t MAC_P3[] = {0xA0, 0x85, 0xE3, 0xE7, 0x44, 0x28};  
uint8_t MAC_P4[] = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC}; 
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const int WIFI_CHANNEL = 4;
bool incomingLED_status;

//ESP LIBRARIES
#include <esp_now.h>
#include <WiFi.h>

#include <BleConnectionStatus.h>
#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

#include "JoystickButton.h"

int lastTimeCheckMillis = 0;

XboxGamepadDevice *gamepad;

bool buttonJoystickLeftDownPressed = false;

#ifdef DEVICE_2_ENABLED
BleCompositeHID compositeHID("P2f", "P2f", 28);
#endif

#ifdef DEVICE_3_ENABLED
BleCompositeHID compositeHID("P3f", "P3f", 28);
#endif

#ifdef DEVICE_4_ENABLED
BleCompositeHID compositeHID("P4f", "P4f", 28);
#endif

//JOYSTICK VARIABLES

JoystickButton joystickP2Button = JoystickButton( 4,  5, false, false, 0);
JoystickButton joystickP3Button = JoystickButton(15, 16, false, false, 0);
JoystickButton joystickP4Button = JoystickButton( 8,  3, false, false, 0);

#ifdef DEVICE_4_ENABLED
JoystickSensor  joystickP2Direction = JoystickSensor(6,   7, true, false);
JoystickSensor  joystickP3Direction = JoystickSensor(17, 18, true, false);
JoystickSensor  joystickP4Direction = JoystickSensor( 9, 10, true, false);
#else
JoystickSensor joystickP2Direction = JoystickSensor(6,   7, true, true);
JoystickSensor joystickP3Direction = JoystickSensor(17, 18, true, true);
JoystickSensor joystickP4Direction = JoystickSensor( 9, 10, true, true);
#endif

typedef struct JoystickType {
    uint8_t idPlayer;       // ID del dispositivo que envía (ej: 1, 2, 3)
    boolean isLeftSide;  // Algún valor de ejemplo    
    
    boolean isPressUp;  
    boolean isPressDown;
    
    uint16_t direction;

    int posX;
    int posY;
  };

  int countMessage = 0;
  typedef struct struct_message {
    uint16_t num_message;
    JoystickType joystickButtons[5];
  } struct_message;


struct_message myData;
struct_message incomingReadings;
esp_now_peer_info_t peerInfo;

//Print Message
void printMessage(struct_message incomingReadings, int len){
  
  //Serial.print("count: ");
  //Serial.print(incomingReadings.num_message);      

  for(int i = 1; i<1 ; i++){
    Serial.print("\t,id: ");
    Serial.print(incomingReadings.joystickButtons[i].idPlayer);
    Serial.print("\t,direc: ");
    Serial.print(incomingReadings.joystickButtons[i].direction);
    Serial.print("\t,posX: ");
    Serial.print(incomingReadings.joystickButtons[i].posX);
    Serial.print("\t,posY: ");
    Serial.print(incomingReadings.joystickButtons[i].posY);
    
    /*
    Serial.print("\t,isLeftSide ");
    Serial.print(incomingReadings.joystickButtons[i].isLeftSide);
    Serial.print("\t,isPressUp: ");
    Serial.print(incomingReadings.joystickButtons[i].isPressUp);
    Serial.print("\t,isPressDown: ");
    Serial.print(incomingReadings.joystickButtons[i].isPressDown);
    */
  }
  
  
  //Serial.print("\t,Bytes received: ");
  //Serial.println(len);

}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const esp_now_recv_info * info, const uint8_t *incomingData, int len) {
  const uint8_t * mac_addr = info->des_addr; // O info->des_addr
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  
  printMessage(incomingReadings, len);
  for(int i = 0; i<5; i++){
    if(incomingReadings.joystickButtons[i].idPlayer == DEVICE_ID){
        int posX = incomingReadings.joystickButtons[i].posX;
        int posY = incomingReadings.joystickButtons[i].posY;
        int direction = incomingReadings.joystickButtons[i].direction;
        boolean isLeftSide = incomingReadings.joystickButtons[i].isLeftSide;
        joystickP2Direction.justPress(posX, posY, isLeftSide);
        joystickP2Button.pressButton(direction, isLeftSide, false);
    }
  }
}

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

  delay(100);
  gamepad->setLeftThumb(1, 1);
  gamepad->setRightThumb(1, 1);
  gamepad->sendGamepadReport();

  joystickP2Direction.init(gamepad);
  //joystickP2Direction.setCallback(callbackDetectMovementJoystick);
  joystickP3Direction.init(gamepad);
  joystickP4Direction.init(gamepad);

  joystickP2Button.init(gamepad);
  //joystickP2Button.setCallbackDirection(callbackDetectButtonsJoystick);
  joystickP3Button.init(gamepad);
  joystickP4Button.init(gamepad);

  myData = {};
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  WiFi.channel(WIFI_CHANNEL);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Send Callback Function
  esp_now_register_send_cb(OnDataSent);

  // Receive Callback Function
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  //memset(peerInfo.lmk, 0, ESP_NOW_KEY_LEN);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("Broadcast registrado.");


  #ifndef DEVICE_2_ENABLED
    memcpy(peerInfo.peer_addr, MAC_P2, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Fallo al añadir peer P2");
      return;
    }
    Serial.println("Peer P2 registrado.");
  #endif

  #ifndef DEVICE_3_ENABLED
    memcpy(peerInfo.peer_addr, MAC_P3, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Fallo al añadir peer P3");
      return;
    }
    Serial.println("Peer P3 registrado.");
  #endif

  //DATA INIT
  /*myData.joystickButtons[1].idPlayer = 3;
  myData.joystickButtons[1].isLeftSide  = true;
  myData.joystickButtons[1].isPressUp   = false;
  myData.joystickButtons[1].isPressDown = false;
  myData.joystickButtons[1].direction = 0;
  myData.joystickButtons[1].posX = 0;
  myData.joystickButtons[1].posY = 0;*/
  
}

void callbackDetectMovementJoystick(int positionX, int positionY) {
  
  Serial.print("posX:  ");
  Serial.print(positionX);
  Serial.print(" ,pos Y: ");
  Serial.println(positionY); 

  //DATA INIT
  //myData.joystickButtons[1].direction = 0;
  myData.joystickButtons[1].posX = positionX;
  myData.joystickButtons[1].posY = positionY;

  sendTestMessage();
  delay(1);//TODO: increase to 10 and put the delay do it with a counter time instead of delay()

}

void callbackDetectButtonsJoystick(int direction, boolean isPressed){
  //Serial.print("direction:  ");
  //Serial.print(direction);
  //Serial.print(" ,isPressed: ");
  //Serial.println(isPressed); 
  
  //sendDataToPeer(MAC_P3);
  //Serial.printf("Heap libre: %d bytes\n", ESP.getFreeHeap());

  myData.joystickButtons[1].direction = direction;

  sendTestMessage();

  delay(1);
}


void OnVibrateEvent(XboxGamepadOutputReportData data)
{
    if(data.weakMotorMagnitude > 0 || data.strongMotorMagnitude > 0){
        //digitalWrite(ledPin, LOW);
    } else {
        //digitalWrite(ledPin, HIGH);
    }
    //Serial.println("Vibration event. Weak motor: " + String(data.weakMotorMagnitude) + " Strong motor: " + String(data.strongMotorMagnitude));
}


void joystickDirectionDetect(){
  if(compositeHID.isConnected()){
    joystickP2Direction.detectAndPress(true, true);
  }
  delay(1);
}

 
void joysticksButtonsDetect(){
  if(compositeHID.isConnected()){
    joystickP2Button.detectAndPress(true);
  }
  delay(1);
}

void sendTestMessage(){
  myData.num_message = countMessage;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData)); // declaration
  if (result == ESP_OK) {
    //Serial.println("Proccess sent successfull");
  }else{
    Serial.println("Error sending the data");
  }
  countMessage++;
}

void loop() {


  if (DEVICE_ID == 2|| DEVICE_ID == 4){

    int currentMillis = millis();
    if(  (currentMillis-lastTimeCheckMillis) > 40){

      //joystickDirectionDetect();
      //joysticksButtonsDetect();
      
      //Second Finger
      int direction2 = joystickP2Button.detectDirecction();
      int x2 = joystickP2Direction.readX();
      int y2 = joystickP2Direction.readY();
      if(DEVICE_ID == 2){
        joystickP2Direction.justPress(x2, y2, true);
        delay(1);
        joystickP2Button.pressButton(direction2, true, false);
        delay(1);
      }else{
        myData.joystickButtons[1].idPlayer = 2;
        myData.joystickButtons[1].isLeftSide  = false;
        myData.joystickButtons[1].direction = direction2;
        myData.joystickButtons[1].posX = x2;
        myData.joystickButtons[1].posY = y2;
      }
      
      //Third Finger
      int direction3 = joystickP3Button.detectDirecction();
      int x3 = joystickP3Direction.readX();
      int y3 = joystickP3Direction.readY();
      myData.joystickButtons[2].idPlayer = 3;
      myData.joystickButtons[2].isLeftSide  = true;
      if(DEVICE_ID == 4){
        myData.joystickButtons[2].isLeftSide  = false;
      }
      myData.joystickButtons[2].direction = direction3;
      myData.joystickButtons[2].posX = x3;
      myData.joystickButtons[2].posY = y3;

      //Four Finger
      int direction4 = joystickP4Button.detectDirecction();
      int x4 = joystickP4Direction.readX();
      int y4 = joystickP4Direction.readY();
      if(DEVICE_ID == 2){
        myData.joystickButtons[3].idPlayer = 4;
        myData.joystickButtons[3].isLeftSide  = true;
        myData.joystickButtons[3].direction = direction4;
        myData.joystickButtons[3].posX = x4;
        myData.joystickButtons[3].posY = y4;
      }else{//DEVICE_ID == 4
        joystickP4Direction.justPress(x4, y4, false);
        delay(1);
        joystickP4Button.pressButton(direction4, false, false);
        delay(1);
      }
      

      sendTestMessage();
      
      //Serial.print("Current Millis: ");
      //Serial.println(lastTimeCheckMillis); 
      lastTimeCheckMillis = currentMillis;
    }
  }
  
  //Serial.print("Peers count: ");
  //Serial.println(countPeers());

  /*
  int delayLoop = millis() - startLoop;

  int buttonLeftDown = digitalRead(buttonJoystickLeftDown);
  int buttonRigthUp = digitalRead(triggerButtons4);

  
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

  //Serial.print("loop elapse: ");
  //Serial.println(delayLoop);
  //delay(2);

}