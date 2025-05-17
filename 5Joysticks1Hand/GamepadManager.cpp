#include "GamepadManager.h"


void GamepadManager::init(XboxGamepadDevice *gamepad){

  this-> gamepad = gamepad;
  //lastTimeCheck = millis();
}

/*void GamepadManager::pressJoystick(int xPosition, int yPosition, boolean isLeftSide, boolean callback){
  
  boolean lastValueIsDiferent = lastPositionX != xPosition || lastPositionY != yPosition;
  boolean positionDiferenceIsBigToCallback = abs(xPosition-lastPositionX)>distanceToAvoidCallback || abs(yPosition-lastPositionY)>distanceToAvoidCallback || (xPosition == 0 && yPosition == 0);
  boolean pastTimeMaximumToCallback = (millis() - lastTimeCheck) >maxTimeToAvoidCallback;
  if( (lastValueIsDiferent && positionDiferenceIsBigToCallback) || pastTimeMaximumToCallback){
    
    if (_callback != NULL && callback) {
      _callback(xPosition, yPosition); // ¡Aquí se ejecuta la función del .ino y recibe los valores!
      //Serial.println("Libreria: El callback del .ino terminó de ejecutarse.");
    } //else { //Serial.println("Libreria: No hay función de callback configurada.");}

    justPressJoystick(xPosition, yPosition, isLeftSide);
    gamepad->setLeftThumb ();
    gamepad->sendGamepadReport();

    lastPositionX = xPosition;
    lastPositionY = yPosition;
    lastTimeCheck = millis();

  }
}*/

void GamepadManager::justPressJoystick(int xPosition, int yPosition, boolean isLeftSide){
  if(isLeftSide){
    gamepad->setLeftThumb (xPosition, yPosition);
  }else{
    gamepad->setRightThumb (xPosition, yPosition);
  }
  gamepad->sendGamepadReport();
}


//BUTTON DIRECTION
void GamepadManager::pressDirection(int direction, boolean isLeftSide, boolean callback){
    if (direction == 0){
        releaseAllButtons(isLeftSide, callback);
    }else{
        pressDirectionsManager(direction, isLeftSide, callback);
    }
}

void GamepadManager::pressDirectionsManager(int direction, boolean isLeftSide, boolean callback){
  int rowDirection = 0;
  if(!isLeftSide) rowDirection = 1;
  
  if (directionPressed[rowDirection][direction] == false){
    pressDirectionsJoystick(direction ,isLeftSide ,callback);
    directionPressed[rowDirection][direction] = true;
  }
}

void GamepadManager::releaseAllButtons(boolean isLeftSide, boolean callback){

  for(int i = 1 ; i < 10; i++){
    releaseButtonsJoystickManager(i, isLeftSide, callback);
  }
  //gamepad->sendGamepadReport();

}

void GamepadManager::releaseButtonsJoystickManager(int direction, boolean isLeftSide, boolean callback){
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


void GamepadManager::pressDirectionsJoystick(int direction, boolean isLeftSide, boolean callback){
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

void GamepadManager::releaseButtonsJoystick(int direction, boolean isLeftSide){

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