//BUTTONS VARIABLES
//int buttonRow[5] = { 17, 18,  4,  5, 16 };  //Input //{ 17, 18, 4, 5 , 16 }; //{ 4, 16, 17, 5, 18 };
//int buttonCol[4] = { 21, 23, 22,  19     };  //Output
//boolean keyboardLettersPressed[5][4] = { false };

/*uint16_t gamepadButtons[5][4] =   { 
                                    { NORTH_DIRE      , XBOX_BUTTON_LS          , XBOX_BUTTON_Y               , XBOX_BUTTON_RS          },                                 
                                    { EAST_DIRE       , XBOX_BUTTON_SELECT      , XBOX_BUTTON_B               , XBOX_BUTTON_SHARE       },     
                                    { SOUTH_DIRE      , XBOX_BUTTON_LB          , XBOX_BUTTON_A               , XBOX_BUTTON_RB          },        
                                    { WEST_DIRE       , 1                       , XBOX_BUTTON_X               , XBOX_BUTTON_START       },    
                                    { TRIGER_LEFT     , XBOX_BUTTON_HOME        , TRIGER_RIGHT                , 1                       }                             
                                  };
*/


/*
  //Setup Switch
  pinMode(buttonJoystickLeftDown , INPUT_PULLUP);
  pinMode(triggerButtons4, INPUT_PULLDOWN);

  pinMode(buttonRow[0], INPUT_PULLUP);
  pinMode(buttonRow[1], INPUT_PULLUP);
  pinMode(buttonRow[2], INPUT_PULLUP);
  pinMode(buttonRow[3], INPUT_PULLUP);
  pinMode(buttonRow[4], INPUT_PULLUP);
  
  pinMode(buttonCol[0], OUTPUT);
  pinMode(buttonCol[1], OUTPUT);
  pinMode(buttonCol[2], OUTPUT);
  pinMode(buttonCol[3], OUTPUT);

  digitalWrite(buttonCol[0], HIGH);
  digitalWrite(buttonCol[1], HIGH);
  digitalWrite(buttonCol[2], HIGH);
*/  




/*
void keyboardDetection(){

  //Serial.print("start keyboard ");
  //Serial.print(millis());
  //Keyboard Detection
  for (int col = 0; col < 4; col++) {
    digitalWrite(buttonCol[col], LOW);
    //delay(1);
    for (int row = 0; row < 5; row++) {
      int buttonState = digitalRead(buttonRow[row]);
      if (buttonState == LOW) {
        if (keyboardLettersPressed[row][col] == false) {
          keyboardLettersPressed[row][col] = true;
          pressButtons(row, col);
        }
      } else {
        if (keyboardLettersPressed[row][col] == true) {
          unpressButtons(row, col);
        }
        keyboardLettersPressed[row][col] = false;
      }
    }
    
  
    digitalWrite(buttonCol[col], HIGH);
    //delay(1);
  }
  //Serial.print(" ");
  //Serial.print(millis());
  //Serial.println(" finish keyboard");
  
}
*/
