#ifndef __COMI2C_H__
#define __COMI2C_H__

#include <Arduino.h>
#include <Wire.h>
#include <JoystickSensor.h>
#include <MultipleButton.h>
#include <GlobalVars.h>

#pragma pack(1)
typedef struct sctruct_gamepad {
  uint8_t idPlayer;
  
  boolean isPressUpLeft;  
  boolean isPressUpRigth; 

  boolean isPressDownLeft;
  boolean isPressDownRigth;
  
  uint16_t directionLeft;
  uint16_t directionRigth;

  int posXLeft;
  int posXRigth;

  int posYLeft;
  int posYRigth;
};
#pragma pack()

typedef struct JoystickType {
  uint8_t idPlayer;       // ID del dispositivo que envía (ej: 1, 2, 3)
  boolean isLeftSide;  // Algún valor de ejemplo    
  
  boolean isPressUp;  
  boolean isPressDown;
  
  uint16_t direction;

  int posX;
  int posY;
};



typedef struct struct_message {
  uint16_t num_message;
  JoystickType joystickButtons[5];
} struct_message;




class ComI2C {

  private:
  
  public:

    volatile bool newDataReceived = false; 
    struct_message receiveData;
    struct_message sendData;
    sctruct_gamepad gamepadData;
    int countMessage = 0;

    void init();
    void receiveEvent(int howMany);
    void receiveEventOnlyListeners(int howMany);
    void requestEvent();
    struct_message requestMessageSlave(int address);
    void sendDataToListenersOnly(int address);
    void setSendDataOfSensors(JoystickSensor  joystickP1Direction, JoystickSensor  joystickP2Direction, JoystickSensor  joystickP3Direction, JoystickSensor  joystickP4Direction, JoystickSensor  joystickP5Direction, MultipleButton multipleButton, boolean isLeftSide );
    void setGamepadData(struct_message messageIncomeP2, struct_message messageIncomeP4, int playerId);

    void printGamepadData(sctruct_gamepad data);
    void printMessage(struct_message data);
};


extern ComI2C comI2CManager;

#endif