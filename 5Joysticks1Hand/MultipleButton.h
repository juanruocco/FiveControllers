#ifndef __MULTIPLEBUTTON_H__
#define __MULTIPLEBUTTON_H__

#include <Arduino.h>
#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>


//uint16_t gamepadButtons[8] =   { NORTH_DIRE  , XBOX_BUTTON_LS  , XBOX_BUTTON_Y  , XBOX_BUTTON_RS, NORTH_DIRE  , XBOX_BUTTON_LS  , XBOX_BUTTON_Y  , XBOX_BUTTON_RS  };

class MultipleButton {

  private:

    int buttonOut[5] = {  4,  5,   6,  7,  15 };  //Output COL
    int buttonIn[5]  = { 16, 17,  18,  8,   3 };  //Input  ROW//{ 17, 18, 4, 5 , 16 }; //{ 4, 16, 17, 5, 18 };//ROW    
    //boolean inverseXDirection;
    //boolean inverseYDirection;
    //CallbackFunctionWithArgs _callback = NULL;
    boolean buttonPressed[5][5] = { false };    
  
  public:
    //int numPlayer;
    int direction[5] = { 0 };
    bool centerButton[5] = {false};

    XboxGamepadDevice *gamepad;
    
    //MultipleButton();
    void init(XboxGamepadDevice *gamepad);
    void detectDirectionAndCenterButtons();
    //void setCallback(CallbackFunctionWithArgs callback);

    void scanKeys();

};


#endif