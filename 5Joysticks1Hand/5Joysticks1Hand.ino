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

bool buttonJoystickLeftDownPressed = false;

BleCompositeHID compositeHID("P2f", "P2f", 25);
/*
//ESP32 P2
BleCompositeHID compositeHID("P2c", "P2c", 22);
BleCompositeHID compositeHID("P3c", "P3c", 22);
*/

//JOYSTICK VARIABLES

//JoystickSensor joystickP1Direction = JoystickSensor(1, 2, false, false)
JoystickSensor joystickP2Direction   = JoystickSensor(6, 7, true, true);
//JoystickSensor joystickP3Direction = JoystickSensor(17, 18, false, false)
//JoystickSensor joystickP4Direction = JoystickSensor( 9, 10, false, false)
//JoystickSensor joystickP5Direction = JoystickSensor(20, 19, false, false)


JoystickButton joystickP2Button = JoystickButton(4, 5, false, false, 0);


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

  delay(300);
  gamepad->setLeftThumb(1, 1);
  gamepad->setRightThumb(1, 1);
  gamepad->sendGamepadReport();

  joystickP2Direction.init(gamepad);
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

*/


void loop() {
  
  //joystickDirection1();
  int startLoop = millis();
  //keyboardDetection();
  joystickDirectionDetect();
  joysticksButtonsDetect();
  
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