#ifndef __JOYSTICKBUTTON_H__
#define __JOYSTICKBUTTON_H__

#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>
#include <BleCompositeHID.h>

#include "JoystickSensor.h"

const int NORTH_DIRE = 135;
const int SOUTH_DIRE = 136;
const int WEST_DIRE = 137;
const int EAST_DIRE = 138;
const int TRIGER_LEFT  = 141;
const int TRIGER_RIGHT = 142;

const  int thresholdMax    = 25000; //31327, 32767 // limit for horizonal vertical buttons
const  int thresholdMedium = 7000; //31327, 15000 // limit for lateral  buttons
const  int thresholdMin    = 1000; // 1000limit for horizonal vertical buttons

const uint16_t gamepadButtonsJoystick[2][10] =   { 
                                              { 0 ,NORTH_DIRE      ,XBOX_BUTTON_SELECT   ,EAST_DIRE        ,XBOX_BUTTON_LS   ,SOUTH_DIRE     ,XBOX_BUTTON_LB   ,WEST_DIRE       ,XBOX_BUTTON_START          , XBOX_BUTTON_HOME  },                                 
                                              { 0 ,XBOX_BUTTON_Y   ,TRIGER_RIGHT         ,XBOX_BUTTON_B    ,XBOX_BUTTON_RB  , XBOX_BUTTON_A  ,XBOX_BUTTON_RS   ,XBOX_BUTTON_X   ,XBOX_BUTTON_START    , XBOX_BUTTON_SHARE }     
                                          };

typedef void (*CallbackFunctionWithArg)(int direction, boolean isPressed);

class JoystickButton: public JoystickSensor{
  
  private:
    CallbackFunctionWithArg _callbackDirection = NULL;

    boolean directionPressed[2][10] = { false };
    int buttonSide; //0 Left, 1 Rigth
    
    void releaseAllButtons(boolean isLeftSide, boolean callback);
    void pressButtonsJoystickManager(int direction, boolean isLeftSide, boolean callback);
    void directionDetected(int direction, boolean callback);
    void pressButtonsJoystick(int direction, boolean isLeftSide, boolean callback);
    void releaseButtonsJoystickManager(int direction, boolean isLeftSide, boolean callback);
    void releaseButtonsJoystick(int direction, boolean isLeftSide);
    //BleCompositeHID compositeHID;

  public:
    JoystickButton(int pinX, int pinY, boolean inverseXDirection, boolean inverseYDirection, int buttonSide);
    void init(XboxGamepadDevice *gamepad);
    void setCallbackDirection(CallbackFunctionWithArg callbackDirection);

    int detectDirecction();
    void detectAndPress(boolean callback);
    void pressButton(int direction, boolean isLeftSide, boolean callback);
    
};

#endif