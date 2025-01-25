//#include <Arduino.h>
//#include <BleGamepad.h>

#include <BleConnectionStatus.h>
#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

//BleGamepad bleGamepad;
//BleGamepadConfiguration bleGamepadConfig;  

XboxGamepadDevice *gamepad;
BleCompositeHID compositeHID("ESP32 SeriesX Controller", "Mystfit", 100);

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


  delay(300);
  centerCalibration = analogRead(xJoystick1);
  gamepad->setRightThumb(0, 0);
  gamepad->sendGamepadReport();
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

  if(output <= 0){
    output = 0;
  }

  

  Serial.print(" , output: ");
  Serial.print(output);


  reading = map(output, 0, 4096, -range, range);
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
  if(compositeHID.isConnected()){
    Serial.print(", connected ");
    gamepad->setLeftThumb (xReading, yReading);
    gamepad->setRightThumb(1, 1);
    gamepad->sendGamepadReport();
  }
  
  delay(20);
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