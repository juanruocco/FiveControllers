#define threshold  300//TODO: check if reduce this threshold to more precicion but errors int the button detection

#define range  32767  // output range of X or Y movement
#define minRange  0
#define maxRange  32767

#define xInvertJoyLD  false
#define yInvertJoyLD  false
#define xInvertJoyRD  true
#define yInvertJoyRD  false

#define xInvertJoyLU  true
#define yInvertJoyLU  true
#define xInvertJoyRU  false
#define yInvertJoyRU  false

#define xCenterCalibrationLeftUp     2000
#define xCenterCalibrationLeftDown   2000
#define xCenterCalibrationRigthUp    2000
#define xCenterCalibrationRigthDown  2000

#define yCenterCalibrationLeftUp     2000
#define yCenterCalibrationLeftDown   2000
#define yCenterCalibrationRigthUp    2000
#define yCenterCalibrationRigthDown  2000

//JOYSTICK VARIABLES
//LEFT  
#define xJoystickLeftDownPin     12  //13;//12;
#define yJoystickLeftDownPin     13  //14;//13;
#define xJoystickLeftUpPin       27    //11 ;//27;
#define yJoystickLeftUpPin       14    //12;//14;
#define buttonJoystickLeftDown   25//19;//15;

//RIGTH
#define xJoystickRigthDownPin  34 //18;//34;
#define yJoystickRigthDownPin  35 //8;//35;
#define xJoystickRigthUpPin    36   //16;//36;
#define yJoystickRigthUpPin    39   //17;//39;
#define triggerButtons4         4        //20;//4;


#define NORTH_DIRE  135
#define SOUTH_DIRE  136
#define WEST_DIRE  137
#define EAST_DIRE  138
#define TRIGER_LEFT   141
#define TRIGER_RIGHT  142



int readAxis(int thisAxis, boolean inverse, int centerCalibration) {// output: -4095 to 4095, 0 in  calibration choose default

  int reading = analogRead(thisAxis);
  //Serial.print("analogue: ");
  //Serial.println(reading);
  int distance = reading - centerCalibration;

  if (abs(distance) < threshold) {
    distance = 0;
  }

  int defaultCalibration = 4096/2 - centerCalibration;
  //Serial.print(" , calibra: ");
  //Serial.print(defaultCalibration);

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

  //Serial.print(" , output: ");
  //Serial.print(output);

  reading = map(output, 0, 4096, -range, range);
  //Serial.print(", convert: ");
  //Serial.print(reading);
  //Serial.print(" , ");
  
  return reading;
}


void joystickDirection(int xJoyPinLeft, int yJoyPinLeft, int xJoyPinRight, int yJoyPinRight){

  int xReading = readAxis(xJoyPinLeft, xInvertJoyLD, xCenterCalibrationLeftDown);
  int yReading = readAxis(yJoyPinLeft, yInvertJoyLD, yCenterCalibrationLeftDown);
  int xReadingRigth = readAxis(xJoyPinRight, xInvertJoyRD, xCenterCalibrationRigthDown);
  int yReadingRigth = readAxis(yJoyPinRight, yInvertJoyRD, yCenterCalibrationRigthDown);

 /*
  Serial.print("xl: ");
  Serial.print(xReading);
  Serial.print(", yl: ");
  Serial.print(yReading);
  Serial.print(", xr: ");
  Serial.print(xReadingRigth);
  Serial.print(", yr: ");
  Serial.println(yReadingRigth);
  */

  if(compositeHID.isConnected()){
    //Serial.print(", connected ");
    gamepad->setLeftThumb (xReading, yReading);
    gamepad->setRightThumb(xReadingRigth, yReadingRigth);
    gamepad->sendGamepadReport();
  }
  
  delay(1);
}

void joystickButtons(int xReading, int yReading, int row){
  //TODO: improve: cancel other buttons when one button is pressed
        
  int thresholdMax = 25000; //31327, 32767
  int thresholdMedium = 15000; //31327, 32767
  int thresholdMin = 1000; 
  if(compositeHID.isConnected()){
    
    // X = 0
    if(xReading < thresholdMin && xReading > -thresholdMin){
        if(yReading < thresholdMin && yReading > -thresholdMin){
            unpressJoystickButtonsRow(row);
        }

        //Up Button
        if(yReading > thresholdMax){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row,0);
        }
      
        //Down Button
        if(yReading < -thresholdMax){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row,4);
        }

    }

    //X Rigth Button
    if(xReading > thresholdMax && yReading < thresholdMin && yReading > -thresholdMin ){
        //unpressJoystickButtonsRow(row);
        pressButtonsJoystickManager(row, 2);
    }

    // X > 0
    if(xReading > thresholdMedium ){

        if(yReading > thresholdMedium){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row, 1);
        }

        if(yReading < -thresholdMedium){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row, 3);
        }
    }

    //X Left Button
    if(xReading < -thresholdMax && yReading < thresholdMin && yReading > -thresholdMin){
        //unpressJoystickButtonsRow(row);
        pressButtonsJoystickManager(row, 6);
    }

    // X < 0
    if(xReading < -thresholdMedium ){

        if(yReading < -thresholdMedium){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row, 5);
        }
        
        if(yReading > thresholdMedium){
            //unpressJoystickButtonsRow(row);
            pressButtonsJoystickManager(row, 7);
        }        
    }
    //gamepad->sendGamepadReport();
  }
}

void joysticksButtons(int xJoyPinLeft, int yJoyPinLeft, int xJoyPinRight, int yJoyPinRight){

  int xReading = readAxis(xJoyPinLeft, xInvertJoyLU, xCenterCalibrationLeftUp);
  int yReading = readAxis(yJoyPinLeft, yInvertJoyLU, yCenterCalibrationLeftUp);
  int xReadingRigth = readAxis(xJoyPinRight, xInvertJoyRU, xCenterCalibrationRigthUp);
  int yReadingRigth = readAxis(yJoyPinRight, yInvertJoyRU, yCenterCalibrationRigthUp);
  
  /*
  Serial.print("x LU: ");
  Serial.print(xReading);
  Serial.print(", y LU: ");
  Serial.print(yReading);
  Serial.print(", x RU: ");
  Serial.print(xReadingRigth);
  Serial.print(", y RU: ");
  Serial.println(yReadingRigth);
  */

  joystickButtons(xReading, yReading, 0);
  joystickButtons(xReadingRigth, yReadingRigth, 1);

//> <
  
  
  delay(1);
}


//PRESS GAMEPAD JOYSTICK BUTTONS
void pressButtonsJoystickManager(int row, int col){
  if (gamepadButtonsJoystickPressed[row][col] == false){

    sendMessage();
    
    pressButtonsJoystick(row, col);
    gamepadButtonsJoystickPressed[row][col] = true;
  }
}



void pressButtonsJoystick(int row, int col){
  
  Serial.print("PRESS  : ");
  Serial.print(gamepadButtonsJoystick[row][col]);
  Serial.print(", Row: ");
  Serial.print(row);
  Serial.print(" ,Col: ");
  Serial.println(col);

  if( gamepadButtonsJoystick[row][col] == NORTH_DIRE ){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::NORTH);
  }else if(gamepadButtonsJoystick[row][col] == SOUTH_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::SOUTH);
  }else if(gamepadButtonsJoystick[row][col] == WEST_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::WEST);
  }else if(gamepadButtonsJoystick[row][col] == EAST_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::EAST);
  }else if(gamepadButtonsJoystick[row][col] == TRIGER_LEFT){
    gamepad-> setLeftTrigger(30000);
  }else if(gamepadButtonsJoystick[row][col] == TRIGER_RIGHT){
    gamepad-> setRightTrigger(30000);
  }else{
    gamepad->press(gamepadButtonsJoystick[row][col]);
  }
  
  gamepad->sendGamepadReport();

}  

//PRESS GAMEPAD BUTTONS
void pressButtons(int row, int col){
  
  Serial.print("PRESS df  : ");
  Serial.print(gamepadButtons[row][col]);
  Serial.print(", Row: ");
  Serial.print(row);
  Serial.print(" ,Col: ");
  Serial.println(col);

  if( gamepadButtons[row][col] == NORTH_DIRE ){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::NORTH);
  }else if(gamepadButtons[row][col] == SOUTH_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::SOUTH);
  }else if(gamepadButtons[row][col] == WEST_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::WEST);
  }else if(gamepadButtons[row][col] == EAST_DIRE){
    gamepad->pressDPadDirectionFlag(XboxDpadFlags::EAST);
  }else if(gamepadButtons[row][col] == TRIGER_LEFT){
    gamepad-> setLeftTrigger(30000);
  }else if(gamepadButtons[row][col] == TRIGER_RIGHT){
    gamepad-> setRightTrigger(30000);
  }else{
    gamepad->press(gamepadButtons[row][col]);
  }
  
  gamepad->sendGamepadReport();

}  

//UNPRESS BUTTONS
void unpressJoystickButtonsRow(int row){

  for(int i = 0 ; i < 8; i++){
    unpressButtonsJoystickManager(row,i);
  }
  //gamepad->sendGamepadReport();

}

void unpressButtonsJoystickManager(int row, int col){
  if (gamepadButtonsJoystickPressed[row][col] == true){
    unpressButtonsJoystick( row, col);
    gamepadButtonsJoystickPressed[row][col] = false;
    Serial.print("unpres realease row: ");
    Serial.print(row);
    Serial.print(" ,col: ");
    Serial.println(col);
  }
}

void unpressButtonsJoystick(int row, int col){

  //Serial.print("release: ");
  Serial.println(gamepadButtonsJoystick[row][col]);
  if( gamepadButtonsJoystick[row][col] == NORTH_DIRE || gamepadButtonsJoystick[row][col] == SOUTH_DIRE || gamepadButtonsJoystick[row][col] == WEST_DIRE  || gamepadButtonsJoystick[row][col] == EAST_DIRE){
    gamepad  -> releaseDPad();
    //Serial.print("release DPad ");
  }else if(gamepadButtonsJoystick[row][col] == TRIGER_LEFT){  
      gamepad  -> setLeftTrigger(0);
      gamepad  -> setRightTrigger(0);
  }else if(gamepadButtonsJoystick[row][col] == TRIGER_RIGHT){  
      gamepad  -> setLeftTrigger(0);
      gamepad  -> setRightTrigger(0);
  }else{
    gamepad->release(gamepadButtonsJoystick[row][col]);
    //Serial.println("release gamepad buttons ");
  }
  gamepad->sendGamepadReport();
  //delay(1);
}