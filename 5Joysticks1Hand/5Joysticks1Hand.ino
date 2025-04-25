//#include <Arduino.h>
//#define PLAYER_2_ENABLED //18
#define PLAYER_2_ENABLED //17 
//#define PLAYER_4_ENABLED

//#define PLAYER_ID 3

uint8_t MAC_P2[] = {0x64, 0xE8, 0x33, 0x7E, 0x04, 0x3C};  
uint8_t MAC_P3[] = {0xA0, 0x85, 0xE3, 0xE7, 0x44, 0x28};  
uint8_t MAC_P4[] = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC}; 
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

bool incomingLED_status;

//ESP LIBRARIES
#include <esp_now.h>
#include <WiFi.h>

#include <BleConnectionStatus.h>
#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

#include "JoystickButton.h"

XboxGamepadDevice *gamepad;

bool buttonJoystickLeftDownPressed = false;

#ifdef PLAYER_2_ENABLED
BleCompositeHID compositeHID("P2f", "P2f", 26);
#endif

#ifdef PLAYER_3_ENABLED
BleCompositeHID compositeHID("P3f", "P3f", 26);
#endif

#ifdef PLAYER_4_ENABLED
BleCompositeHID compositeHID("P4f", "P4f", 26);
#endif

//JOYSTICK VARIABLES

//JoystickSensor joystickP1Direction = JoystickSensor(1, 2, false, false)
JoystickSensor joystickP2Direction   = JoystickSensor(6, 7, true, true);
//JoystickSensor joystickP3Direction = JoystickSensor(17, 18, false, false)
//JoystickSensor joystickP4Direction = JoystickSensor( 9, 10, false, false)
//JoystickSensor joystickP5Direction = JoystickSensor(20, 19, false, false)

JoystickButton joystickP2Button = JoystickButton(4, 5, false, false, 0);




int countMessage = 0;
typedef struct struct_message {
    int idPlayer;       // ID del dispositivo que envía (ej: 1, 2, 3)
    boolean isLeftSide;  // Algún valor de ejemplo
    boolean isUpSide;  // Algún valor de ejemplo
    int value1;
    int value2;
    int num_message;
} struct_message;

/*
typedef struct struct_message
{
  int but_status;
} struct_message;
*/
struct_message myData;
struct_message incomingReadings;
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const esp_now_recv_info * info, const uint8_t *incomingData, int len) {
  const uint8_t * mac_addr = info->des_addr; // O info->des_addr
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("count: ");
  Serial.print(incomingReadings.num_message);
  Serial.print(" ,value 1: ");
  Serial.print(incomingReadings.value1);
  Serial.print(" ,value 2: ");
  Serial.print(incomingReadings.value2);
  Serial.print(" ,Bytes received: ");
  Serial.println(len);
  //digitalWrite(LED, incomingLED_status);

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
  joystickP2Direction.setCallback(callbackDetectMovementJoystick);
  joystickP2Button.init(gamepad);
  joystickP2Button.setCallbackDirection(callbackDetectButtonsJoystick);


  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

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


  #ifndef PLAYER_2_ENABLED
    memcpy(peerInfo.peer_addr, MAC_P2, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Fallo al añadir peer P2");
      return;
    }
    Serial.println("Peer P2 registrado.");
  #endif

  #ifndef PLAYER_3_ENABLED
    memcpy(peerInfo.peer_addr, MAC_P3, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Fallo al añadir peer P3");
      return;
    }
    Serial.println("Peer P3 registrado.");
  #endif
  
}

void callbackDetectMovementJoystick(int positionX, int positionY) {
  
  Serial.print("posX:  ");
  Serial.print(positionX);
  Serial.print(" ,pos Y: ");
  Serial.println(positionY); 
  
  //sendDataToPeer(MAC_P3);
  //sendMessage();
  
  //Serial.printf("Heap libre: %d bytes\n", ESP.getFreeHeap());

  myData.idPlayer = 2;
  myData.isLeftSide = true;
  myData.isUpSide = true;
  myData.value1 = positionX;
  myData.value2 = positionY;

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

  myData.idPlayer = 2;
  myData.isLeftSide = true;
  myData.isUpSide = true;
  myData.value1 = direction;
  myData.value2 = isPressed;
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
    joystickP2Direction.detectAndPress();
  }
  delay(1);
}

 
void joysticksButtonsDetect(){
  if(compositeHID.isConnected()){
    joystickP2Button.detectAndPress();
  }
  delay(1);
}

void sendMessage(){
  /*char data[32];
  snprintf(data, sizeof(data), "Hello, World! #%lu", msg_count++);

  Serial.printf("Broadcasting message: %s\n", data);

  if (!broadcast_peer.send_message((uint8_t *)data, sizeof(data))) {
    Serial.println("Failed to broadcast message");
  }*/
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
*/

void sendTestMessage(){
  myData.num_message = countMessage;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData)); // declaration
  if (result == ESP_OK) {
    Serial.println("Proccess sent successfull");
  }else{
    Serial.println("Error sending the data");
  }
  countMessage++;
}

void loop() {
  
  //joystickDirection1();
  //int startLoop = millis();
  //keyboardDetection();
  joystickDirectionDetect();
  delay(1);
  joysticksButtonsDetect();
  delay(1);
  //Serial.print("Peers count: ");
  //Serial.println(countPeers());

  delay(10);
  
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