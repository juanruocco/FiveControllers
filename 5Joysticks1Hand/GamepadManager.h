#ifndef __GAMEPADMANAGER_H__
#define __GAMEPADMANAGER_H__

#include <Arduino.h>

#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

const int NORTH_DIRE = 135;
const int SOUTH_DIRE = 136;
const int WEST_DIRE = 137;
const int EAST_DIRE = 138;
const int TRIGER_LEFT  = 141;
const int TRIGER_RIGHT = 142;

const uint16_t gamepadButtonsJoystick[2][10] =   {                  //mapa                                  //Montar                          //pa la izqu                      //No impor, nose
                                              { 0 ,NORTH_DIRE      ,XBOX_BUTTON_SELECT   ,EAST_DIRE        ,XBOX_BUTTON_LS   ,SOUTH_DIRE     ,XBOX_BUTTON_LB   ,WEST_DIRE       ,XBOX_BUTTON_START          , XBOX_BUTTON_HOME  },                                 
                                              { 0 ,XBOX_BUTTON_Y   ,TRIGER_RIGHT         ,XBOX_BUTTON_B    ,XBOX_BUTTON_RB  , XBOX_BUTTON_A  ,XBOX_BUTTON_RS   ,XBOX_BUTTON_X   ,XBOX_BUTTON_START    , XBOX_BUTTON_SHARE }     
                                          };                       //escojer opcion RT                                                                                          //inventario

typedef void (*CallbackFunctionWithArg)(int direction, boolean isPressed);

class GamepadManager {

  private:
    CallbackFunctionWithArg _callbackDirection = NULL;

    boolean directionPressed[2][10] = { false };
    int buttonSide; //0 Left, 1 Rigth
    
 
  public:
    XboxGamepadDevice *gamepad;
    
    //GamepadManager();
    void init(XboxGamepadDevice *gamepad);

    void justPressJoystick(int xPosition, int yPosition, boolean isLeftSide);
    void pressDirection(int direction, boolean isLeftSide, boolean callback);
    void pressDirectionsManager(int direction, boolean isLeftSide, boolean callback);
    void releaseAllButtons(boolean isLeftSide, boolean callback);
    void releaseButtonsJoystickManager(int direction, boolean isLeftSide, boolean callback);
    void pressDirectionsJoystick(int direction, boolean isLeftSide, boolean callback);
    void releaseButtonsJoystick(int direction, boolean isLeftSide);

};

#endif