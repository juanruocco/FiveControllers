
#include "JoystickButton.h"

JoystickButton::JoystickButton(int pinX, int pinY, boolean inverseXDirection, boolean inverseYDirection ,int buttonSide ): JoystickSensor(pinX, pinY, inverseXDirection, inverseYDirection){
    this-> buttonSide = buttonSide;
    //this-> compositeHID = compositeHID;
}

void JoystickButton::init(XboxGamepadDevice *gamepad){
    JoystickSensor::init(gamepad);    
}

void JoystickButton::setCallbackDirection(CallbackFunctionWithArg callbackDirection) {
  _callbackDirection = callbackDirection; // Guarda el puntero
}

void JoystickButton::detectAndPress(boolean callback){
    int direction = detectDirecction();
    boolean isLeftSide = false;
    if(buttonSide == 1) isLeftSide = true;
    pressButton(direction, isLeftSide, callback);
}

void JoystickButton::pressButton(int direction, boolean isLeftSide, boolean callback){
    if (direction == 0){
        releaseAllButtons(isLeftSide, callback);
    }else{
        pressButtonsJoystickManager(direction, isLeftSide, callback);
    }
}

void JoystickButton::pressButtonsJoystickManager(int direction, boolean isLeftSide, boolean callback){
  int rowDirection = 0;
  if(!isLeftSide) rowDirection = 1;
  
  if (directionPressed[rowDirection][direction] == false){
    pressButtonsJoystick(direction ,isLeftSide ,callback);
    directionPressed[rowDirection][direction] = true;
  }
}

void JoystickButton::releaseAllButtons(boolean isLeftSide, boolean callback){

  for(int i = 1 ; i < 10; i++){
    releaseButtonsJoystickManager(i, isLeftSide, callback);
  }
  //gamepad->sendGamepadReport();

}

void JoystickButton::releaseButtonsJoystickManager(int direction, boolean isLeftSide, boolean callback){
  int rowDirection = 0;
  if(!isLeftSide) rowDirection = 1;
  if (directionPressed[rowDirection][direction] == true){

    if (_callbackDirection != NULL && callback) {
      _callbackDirection(direction, false); // ¡Aquí se ejecuta la función del .ino y recibe los valores!
    } //else { //Serial.println("Libreria: No hay función de callback configurada.");}

    releaseButtonsJoystick( direction, isLeftSide);
    directionPressed[rowDirection][direction] = false;
    Serial.print("RELEASE  DIREC: ");
    Serial.print(direction);
    Serial.print(", Button Side: ");
    Serial.println(buttonSide);
  }
}


void JoystickButton::pressButtonsJoystick(int direction, boolean isLeftSide, boolean callback){
  int rowDirection = 0;
  if(!isLeftSide) rowDirection = 1;

  Serial.print("PRESS    DIREC: ");
  Serial.print(direction);
  Serial.print(", Button Side: ");
  Serial.print(buttonSide);
  Serial.print(" ,Code Button: ");
  Serial.println(gamepadButtonsJoystick[rowDirection][direction]);
  
  if (_callbackDirection != NULL && callback) {
      _callbackDirection(direction, true); // ¡Aquí se ejecuta la función del .ino y recibe los valores!
  } //else { //Serial.println("Libreria: No hay función de callback configurada.");}

  if( gamepadButtonsJoystick[rowDirection][direction] == NORTH_DIRE ){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::NORTH);
  }else if(gamepadButtonsJoystick[rowDirection][direction] == SOUTH_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::SOUTH);
  }else if(gamepadButtonsJoystick[rowDirection][direction] == WEST_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::WEST);
  }else if(gamepadButtonsJoystick[rowDirection][direction] == EAST_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::EAST);
  }else if(gamepadButtonsJoystick[rowDirection][direction] == TRIGER_LEFT){
    gamepad-> setLeftTrigger(30000);
  //RIGTH SIDE
  }else if(gamepadButtonsJoystick[rowDirection][direction] == TRIGER_RIGHT){
    gamepad-> setRightTrigger(30000);
  }else{
    gamepad->press(gamepadButtonsJoystick[rowDirection][direction]);
  }
  
  gamepad->sendGamepadReport();

}  

void JoystickButton::releaseButtonsJoystick(int direction, boolean isLeftSide){

  //Serial.print("release: ");
  //Serial.println(gamepadButtonsJoystick[buttonSide][direction]);
  int rowDirection = 0;
  if(!isLeftSide) rowDirection = 1;

  if( gamepadButtonsJoystick[rowDirection][direction] == NORTH_DIRE || gamepadButtonsJoystick[rowDirection][direction] == SOUTH_DIRE || gamepadButtonsJoystick[rowDirection][direction] == WEST_DIRE  || gamepadButtonsJoystick[rowDirection][direction] == EAST_DIRE){
    gamepad  -> releaseDPad();
    //Serial.print("release DPad ");
  }else if(gamepadButtonsJoystick[rowDirection][direction] == TRIGER_LEFT){  
      gamepad  -> setLeftTrigger(0);
      gamepad  -> setRightTrigger(0);
  }else if(gamepadButtonsJoystick[rowDirection][direction] == TRIGER_RIGHT){  
      gamepad  -> setLeftTrigger(0);
      gamepad  -> setRightTrigger(0);
  }else{
    gamepad->release(gamepadButtonsJoystick[rowDirection][direction]);
    //Serial.println("release gamepad buttons ");
  }
  gamepad->sendGamepadReport();
  delay(1);
}
    

//0 center, 1 up, 2 up right, 3 right, 4 right down, 5 down, 6 down left, 7 left, 8 left up
int JoystickButton::detectDirecction(){
  //TODO: improve: cancel other buttons when one button is pressed
  int xInput = readX();
  int yInput = readY();
/*
  Serial.print(" x: ");
  Serial.print(xInput);
  Serial.print(" ,y: ");
  Serial.println(yInput);
*/
    
  // X = 0
  if(xInput < thresholdMin && xInput > -thresholdMin){
      if(yInput < thresholdMin && yInput > -thresholdMin){
          return 0; //Center 
      }

      //Up Button
      if(yInput > thresholdMax){
          return 1; // Up
      }
    
      //Down Button
      if(yInput < -thresholdMax){
          return 5; //Down
      }

  }

  //X Rigth Button
  if(xInput > thresholdMax && yInput < thresholdMin && yInput > -thresholdMin ){
      return 3; //rigth
  }

  // X > 0 // Rigth Side
  if(xInput > thresholdMedium ){

      if(yInput > thresholdMedium){
          return 2;//Rigth Up
      }

      if(yInput < -thresholdMedium){
          return 4;// Rigth Down
      }
  }

  //X Left Button
  if(xInput < -thresholdMax && yInput < thresholdMin && yInput > -thresholdMin){
      return 7;//Left
  }

  // X < 0  //Left Side
  if(xInput < -thresholdMedium ){

      if(yInput < -thresholdMedium){
          return 6;//Left Down
      }
      
      if(yInput > thresholdMedium){
          return 8;//Left Up
      }        
  }
  return 0;
  //gamepad->sendGamepadReport();
  
}