#include <BleConnectionStatus.h>
#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

#include "JoystickSensor.h"
#include "MultipleButton.h"
#include "GamepadManager.h"
#include "ComI2C.h"
#include "GlobalVars.h"

int lastTimeCheckMillis = 0;

XboxGamepadDevice *gamepad;

bool buttonJoystickLeftDownPressed = false;
bool buttonJoystickRigthDownPressed = false;

#ifdef DEVICE_1_ENABLED
BleCompositeHID compositeHID("P1i", "P1i", 30);
#endif

#ifdef DEVICE_2_ENABLED
BleCompositeHID compositeHID("P2i", "P2i", 30);
#endif

#ifdef DEVICE_3_ENABLED
BleCompositeHID compositeHID("P3i", "P3i", 30);
#endif

#ifdef DEVICE_4_ENABLED
BleCompositeHID compositeHID("P4i", "P4i", 30);
#endif

#ifdef DEVICE_5_ENABLED
BleCompositeHID compositeHID("P5i", "P5i", 30);
#endif



#ifdef DEVICE_2_ENABLED
boolean xInverseJoystickDirectionFinger1 = true;
boolean yInverseJoystickDirectionFinger1 = false;
boolean xInverseJoystickDirectionFinger2 = true;
boolean yInverseJoystickDirectionFinger2 = false;
boolean xInverseJoystickDirectionFinger3 = true;
boolean yInverseJoystickDirectionFinger3 = false;
boolean xInverseJoystickDirectionFinger4 = true;
boolean yInverseJoystickDirectionFinger4 = false;
boolean xInverseJoystickDirectionFinger5 = true;
boolean yInverseJoystickDirectionFinger5 = false;

#else

boolean xInverseJoystickDirectionFinger1 = true;
boolean yInverseJoystickDirectionFinger1 = true;
boolean xInverseJoystickDirectionFinger2 = false;
boolean yInverseJoystickDirectionFinger2 = true;//false;
boolean xInverseJoystickDirectionFinger3 = false;
boolean yInverseJoystickDirectionFinger3 = true;//false;
boolean xInverseJoystickDirectionFinger4 = false;
boolean yInverseJoystickDirectionFinger4 = true;//false;
boolean xInverseJoystickDirectionFinger5 = true;
boolean yInverseJoystickDirectionFinger5 = true;

#endif

  //Directions
JoystickSensor joystickP1Direction = JoystickSensor(  9,  10, xInverseJoystickDirectionFinger1, yInverseJoystickDirectionFinger1);
JoystickSensor joystickP2Direction = JoystickSensor( 11,  12, xInverseJoystickDirectionFinger2, yInverseJoystickDirectionFinger2);
JoystickSensor joystickP3Direction = JoystickSensor( 13,  14, xInverseJoystickDirectionFinger3, yInverseJoystickDirectionFinger3);
JoystickSensor joystickP4Direction = JoystickSensor(  1,   2, xInverseJoystickDirectionFinger4, yInverseJoystickDirectionFinger4);
JoystickSensor joystickP5Direction = JoystickSensor( 20,  19, xInverseJoystickDirectionFinger5, yInverseJoystickDirectionFinger5);

MultipleButton multipleButton = MultipleButton();

GamepadManager gamepadManager = GamepadManager();
//ComI2C comI2C = ComI2C();
ComI2C comI2CManager;
  //Joysticks Buttons
//JoystickButton joystickP1Button = JoystickButton(  4,   5, xInverseJoystickButtonFinger1, yInverseJoystickButtonFinger1, 0);
//JoystickButton joystickP2Button = JoystickButton( 15,  16, xInverseJoystickButtonFinger2, yInverseJoystickButtonFinger2, 0);
//JoystickButton joystickP3Button = JoystickButton(  8,   3, xInverseJoystickButtonFinger3, yInverseJoystickButtonFinger3, 0);
//JoystickButton joystickP4Button = JoystickButton( 11,  12, xInverseJoystickButtonFinger4, yInverseJoystickButtonFinger4, 0);
//JoystickButton joystickP5Button = JoystickButton(  1,   2, xInverseJoystickButtonFinger5, yInverseJoystickButtonFinger5, 0);





void setup() {

  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  
  XboxSeriesXControllerDeviceConfiguration* config = new XboxSeriesXControllerDeviceConfiguration();

  // The composite HID device pretends to be a valid Xbox controller via vendor and product IDs (VID/PID).
  // Platforms like windows/linux need this in order to pick an XInput driver over the generic BLE GATT HID driver. 
  BLEHostConfiguration hostConfig = config->getIdealHostConfiguration();
  Serial.println("Using VID source: " + String(hostConfig.getVidSource(), HEX));
  Serial.println("Using VID: " + String(hostConfig.getVid(), HEX));
  Serial.println("Using PID: " + String(hostConfig.getPid(), HEX));
  Serial.println("Using GUID version: " + String(hostConfig.getGuidVersion(), HEX));
  Serial.println("Using serial number: " + String(hostConfig.getSerialNumber()));
  if(DEVICE_ID == 2){
    pinMode(pressDownPinPlayer1, INPUT_PULLUP);
    pinMode(pressDownPinPlayer2, INPUT_PULLUP);
    pinMode(pressDownPinPlayer3, INPUT_PULLUP);
    pinMode(pressDownPinPlayer4, INPUT_PULLUP);
    pinMode(pressDownPinPlayer5, INPUT_PULLUP); 
  }else{
    pinMode(pressDownPinPlayer1, INPUT_PULLUP);
    pinMode(pressDownPinPlayer2, INPUT_PULLDOWN);
    pinMode(pressDownPinPlayer3, INPUT_PULLDOWN);
    pinMode(pressDownPinPlayer4, INPUT_PULLDOWN);
    pinMode(pressDownPinPlayer5, INPUT_PULLUP);
  }
  
  
  // Set up gamepad
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

  joystickP1Direction.init(gamepad);
  joystickP2Direction.init(gamepad);
  //joystickP2Direction.setCallback(callbackDetectMovementJoystick);
  joystickP3Direction.init(gamepad);
  
  joystickP4Direction.init(gamepad);
  joystickP5Direction.init(gamepad);
  
  //joystickP1Button.init(gamepad);
  //joystickP2Button.init(gamepad);
  //joystickP2Button.setCallbackDirection(callbackDetectButtonsJoystick);
  //joystickP3Button.init(gamepad);
  //joystickP4Button.init(gamepad);
  //joystickP5Button.init(gamepad);

  if(DEVICE_ID == 4){
    multipleButton.init(gamepad);
  }
  
  gamepadManager.init(gamepad);
  comI2CManager.init();

  delay(1000);
 
}

/*
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
*/

void OnVibrateEvent(XboxGamepadOutputReportData data)
{
    if(data.weakMotorMagnitude > 0 || data.strongMotorMagnitude > 0){
        //digitalWrite(ledPin, LOW);
    } else {
        //digitalWrite(ledPin, HIGH);
    }
    //Serial.println("Vibration event. Weak motor: " + String(data.weakMotorMagnitude) + " Strong motor: " + String(data.strongMotorMagnitude));
}




void testPressButtonLeftDown(){
  if (buttonJoystickLeftDownPressed == false) {
    buttonJoystickLeftDownPressed = true;
    //gamepad->press(gamepadButtons[0][8]);
    gamepad-> setLeftTrigger(30000);
    gamepad->sendGamepadReport();
  } 
}

void testReleaseButtonLeftDown(){
  if (buttonJoystickLeftDownPressed == true) {
    //gamepad->release(gamepadButtons[0][7]);
    gamepad-> setLeftTrigger(0);
    gamepad->sendGamepadReport();
    buttonJoystickLeftDownPressed = false;
  }
}


void testPressButtonRigthDown(){
  if (buttonJoystickRigthDownPressed == false) {
    buttonJoystickRigthDownPressed = true;
    //gamepad->press(gamepadButtons[0][8]);
    gamepad-> setRightTrigger(30000);
    gamepad->sendGamepadReport();
  } 
}

void testReleaseButtonRigthDown(){
  if (buttonJoystickRigthDownPressed == true) {
    //gamepad->release(gamepadButtons[0][7]);
    gamepad-> setRightTrigger(0);
    gamepad->sendGamepadReport();
    buttonJoystickRigthDownPressed = false;
  }
}


void loop() {

 if(DEVICE_ID == 2){
    
    comI2CManager.setSendDataOfSensors(joystickP1Direction, joystickP2Direction, joystickP3Direction, joystickP4Direction, joystickP5Direction, multipleButton, DEVICE_ID == 2);
    //printMessage(comI2CManager.sendData);
    struct_message sendData = comI2CManager.sendData;
    //RECEIVE THE REST OF INFO OF SENSORS AND SEND TO P$
    struct_message messageIncomeP4 = comI2CManager.requestMessageSlave(SLAVE_ADDRESS_P4);
    comI2CManager.printMessage(messageIncomeP4);

    joystickP2Direction.justPress(sendData.joystickButtons[1].posX, sendData.joystickButtons[1].posY, true);
    joystickP2Direction.justPress(messageIncomeP4.joystickButtons[1].posX, messageIncomeP4.joystickButtons[1].posY, false);
    if(sendData.joystickButtons[1].isPressDown){
      testPressButtonLeftDown();
    }else{
      testReleaseButtonLeftDown();
    }

    //multipleButton
    gamepadManager.pressDirection(sendData.joystickButtons[1].direction, true, false);
    gamepadManager.pressDirection(messageIncomeP4.joystickButtons[1].direction, true, false);

    //joystickP2Button.pressButton(sendData.joystickButtons[1].direction, true, false);
    //joystickP2Button.pressButton(messageIncomeP4.joystickButtons[1].direction, false, false);

    delay(2);    

    //SEND TO OTHERS
    //DEVICE 1
    comI2CManager.setGamepadData(sendData, messageIncomeP4, 1);
    comI2CManager.sendDataToListenersOnly(SLAVE_ADDRESS_P1);
    delay(2);


    //DEVICE 3
    comI2CManager.setGamepadData(sendData, messageIncomeP4, 3);
    comI2CManager.sendDataToListenersOnly(SLAVE_ADDRESS_P3);
    delay(2);

    //DEVICE 5
    comI2CManager.setGamepadData(sendData, messageIncomeP4, 5);
    comI2CManager.sendDataToListenersOnly(SLAVE_ADDRESS_P5);
    delay(2);


 }else if(DEVICE_ID == 4){
    comI2CManager.setSendDataOfSensors(joystickP1Direction, joystickP2Direction, joystickP3Direction, joystickP4Direction, joystickP5Direction, multipleButton, DEVICE_ID == 2);
    struct_message sendData = comI2CManager.sendData;   
    //printMessage(comI2CManager.sendData);

    joystickP4Direction.justPress(sendData.joystickButtons[3].posX, sendData.joystickButtons[3].posY, false);
    gamepadManager.pressDirection(sendData.joystickButtons[3].direction, false, false);
    //joystickP4Button.pressButton(sendData.joystickButtons[3].direction, false, false);
      
    if (comI2CManager.newDataReceived) {

      comI2CManager.newDataReceived = false;
      struct_message receiveData = comI2CManager.receiveData;
      comI2CManager.printMessage(receiveData);

      joystickP4Direction.justPress(receiveData.joystickButtons[3].posX, receiveData.joystickButtons[3].posY, true);
      //joystickP4Button.pressButton(receiveData.joystickButtons[3].direction, true, false);
      gamepadManager.pressDirection(receiveData.joystickButtons[3].direction, true, false);
      
      //countMessage++;
      //sendData.num_message = countMessage; 

      if(receiveData.joystickButtons[3].isPressDown){
        testPressButtonLeftDown();
      }else{
        testReleaseButtonLeftDown();
      }

      if(sendData.joystickButtons[3].isPressDown){
        testPressButtonRigthDown();
      }else{
        testReleaseButtonRigthDown();
      }

    }
    // El loop puede hacer otras tareas
    delay(4);
 
 }else{//Slaves
    if (comI2CManager.newDataReceived) {

      comI2CManager.newDataReceived = false;
      sctruct_gamepad gamepadData = comI2CManager.gamepadData;
      comI2CManager.printGamepadData(gamepadData);
      joystickP3Direction.justPress(gamepadData.posXLeft , gamepadData.posYLeft , true);
      joystickP3Direction.justPress(gamepadData.posXRigth, gamepadData.posYRigth, false);
      
      //joystickP3Button.pressButton(gamepadData.directionLeft, true, false);  
      //joystickP3Button.pressButton(gamepadData.directionRigth, false, false);
      
      gamepadManager.pressDirection(gamepadData.directionLeft, true, false); 
      gamepadManager.pressDirection(gamepadData.directionRigth, false, false);   

      if(gamepadData.isPressDownLeft){
        testPressButtonLeftDown();
      }else{
        testReleaseButtonLeftDown();
      }

      if(gamepadData.isPressDownRigth){
        testPressButtonRigthDown();
      }else{
        testReleaseButtonRigthDown();
      }

      
      //countMessage++;
      //sendData.num_message = countMessage; 
    }
    // El loop puede hacer otras tareas
    delay(4);
    
 }

}