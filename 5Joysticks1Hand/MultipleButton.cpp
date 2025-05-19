#include "MultipleButton.h"

//void MultipleButton::MultipleButton(){

//}

void MultipleButton::init(XboxGamepadDevice *gamepad){
  
  pinMode(buttonIn[0], INPUT_PULLUP);
  pinMode(buttonIn[1], INPUT_PULLUP);
  pinMode(buttonIn[2], INPUT_PULLUP);
  pinMode(buttonIn[3], INPUT_PULLUP);
  pinMode(buttonIn[4], INPUT_PULLUP);
  
  pinMode(buttonOut[0], OUTPUT);
  pinMode(buttonOut[1], OUTPUT);
  pinMode(buttonOut[2], OUTPUT);
  pinMode(buttonOut[3], OUTPUT);
  pinMode(buttonOut[4], OUTPUT);

  digitalWrite(buttonOut[0], HIGH);
  digitalWrite(buttonOut[1], HIGH);
  digitalWrite(buttonOut[2], HIGH);
  digitalWrite(buttonOut[3], HIGH);
  digitalWrite(buttonOut[4], HIGH);

  this-> gamepad = gamepad;
  //lastTimeCheck = millis();
}


void MultipleButton::detectDirectionAndCenterButtons(){

  //Serial.print("direccion: ");
  
  for(int out = 0; out < 5; out++){
    direction[out] = 0;
    centerButton[out] = false;
    if(out < 3){//To inverse the direction
        if(buttonPressed[out][0]){
          direction[out] = 1;
        }
        //if(buttonPressed[out][0]  && buttonPressed[out][1] ){
        //  direction[out] = 2;
        //}
        if(buttonPressed[out][1] ){
          direction[out] = 3;
        }
        //if(buttonPressed[out][1]  && buttonPressed[out][2] ){
        //  direction[out] = 4;
        //}
        if(buttonPressed[out][2] ){
          direction[out] = 5;
        }
        //if(buttonPressed[out][2] && buttonPressed[out][3] ){
        //  direction[out] = 6;
        //}
        if(buttonPressed[out][3] ){
          direction[out] = 7;
        }
        //if(buttonPressed[out][3] && buttonPressed[out][0]){
        //  direction[out] = 8;
        //}

    }else{
        if(buttonPressed[out][0]){
          direction[out] = 5;
        }
        //if(buttonPressed[out][0]  && buttonPressed[out][1] ){
        //  direction[out] = 6;
        //}
        if(buttonPressed[out][1] ){
          direction[out] = 7;
        }
        //if(buttonPressed[out][1]  && buttonPressed[out][2] ){
        //  direction[out] = 8;
        //}
        if(buttonPressed[out][2] ){
          direction[out] = 1;
        }
        //if(buttonPressed[out][2] && buttonPressed[out][3] ){
        //  direction[out] = 2;
        //}
        if(buttonPressed[out][3] ){
          direction[out] = 3;
        }
        //if(buttonPressed[out][3] && buttonPressed[out][0]){
        //  direction[out] = 4;
        //}

        
    }
    
    if(buttonPressed[out][4]){
      centerButton[out] = true;
    }
    //Serial.print(direction[out]);
    //Serial.print("  ");

  }

  //Serial.println("  ");
  

}

void MultipleButton::scanKeys(){

  //Serial.print("start keyboard ");
  //Serial.print(millis());
  //Keyboard Detection
  for (int out = 0; out < 5; out++) {
    digitalWrite(buttonOut[out], LOW);
    delay(1);
    for (int in = 0; in < 5; in++) {
      int buttonState = digitalRead(buttonIn[in]);//Row in, col out
      if (buttonState == LOW) {
        if (buttonPressed[out][in] == false) {
          buttonPressed[out][in] = true; 
          detectDirectionAndCenterButtons();       
        }
      } else {
        if (buttonPressed[out][in] == true) {
          buttonPressed[out][in] = false;
          detectDirectionAndCenterButtons();
        }        
      }
    }
    
  
    digitalWrite(buttonOut[out], HIGH);
    
    delay(1);
  }
  //Serial.print(" ");
  //Serial.print(millis());
  //Serial.println(" finish keyboard");
  
}