#include "ComI2C.h"

void onReceiveI2C_Wrapper(int numBytes) {
    comI2CManager.receiveEvent(numBytes); // Llama al metodo no estatico de la instancia
}

void onReceiveOnlyListenersI2C_Wrapper(int numBytes) {
    comI2CManager.receiveEventOnlyListeners(numBytes); // Llama al metodo no estatico de la instancia
}

void onRequestI2C_Wrapper() {
    comI2CManager.requestEvent(); // Llama al metodo no estatico de la instancia
}



void ComI2C::init(){

  receiveData = {};
  sendData = {};
  gamepadData = {};

  Wire.setPins(I2C_SDA, I2C_SCL);
  if(DEVICE_ID == 2){//MAestro
    Wire.begin();
    Wire.setClock(100000);
  }else{//SLAVES 
    if(DEVICE_ID == 4){//Maestro
      Wire.begin(SLAVE_ADDRESS_P4);
      Wire.setClock(100000);
      Wire.onReceive(onReceiveI2C_Wrapper); // Llama a receiveEvent cuando reciba datos del maestro
      Wire.onRequest(onRequestI2C_Wrapper);   // Llama a requestEvent cuando el maestro pida datos a este esclavo
      
    }else if(DEVICE_ID == 3){
      Wire.begin(SLAVE_ADDRESS_P3);
      Wire.setClock(100000);
      //Wire.begin(I2C_SDA, I2C_SCL);
      Wire.onReceive(onReceiveOnlyListenersI2C_Wrapper); // Llama a receiveEvent cuando reciba datos del maestro
      //Wire.onRequest(onRequestI2C_Wrapper);   // Llama a requestEvent cuando el maestro pida datos a este esclavo
    } else if(DEVICE_ID == 1){

      Wire.begin(SLAVE_ADDRESS_P1);
      Wire.setClock(100000);
      Wire.onReceive(onReceiveOnlyListenersI2C_Wrapper); // Llama a receiveEvent cuando reciba datos del maestro
      //Wire.onRequest(onRequestI2C_Wrapper);   // Llama a requestEvent cuando el maestro pida datos a este esclavo
    
    } else if(DEVICE_ID == 5){

      Wire.begin(SLAVE_ADDRESS_P5);
      Wire.setClock(100000);
      Wire.onReceive(onReceiveOnlyListenersI2C_Wrapper); // Llama a receiveEvent cuando reciba datos del maestro
      //Wire.onRequest(onRequestI2C_Wrapper);   // Llama a requestEvent cuando el maestro pida datos a este esclavo
    } 
    // Registrar las funciones de evento
    
  }

}



// Funcion que se llama automaticamente cuando el maestro ENVIA datos a ESTE esclavo
void ComI2C::receiveEvent(int howMany) {
  // ¡NO HAGAS OPERACIONES LARGAS NI SERIAL.PRINT AQUI!

  // Verificar si el tamaño de los datos recibidos coincide con el tamaño esperado de la estructura
  if (howMany == sizeof(struct_message)) {
    // !!! Leer los bytes recibidos directamente en la variable de la estructura !!!
    // Esto es seguro si howMany coincide exactamente con sizeof(struct_message)
    Wire.readBytes((uint8_t*)&receiveData, howMany);

    newDataReceived = true; // Poner la bandera para procesar en el loop
  } else {
    // Si el tamaño no coincide, es un error o datos inesperados.
    Serial.printf("Esclavo 1: Tamano de datos recibido inesperado: %d bytes (Esperado: %zu)\n", howMany, sizeof(struct_message));
    // Leer y descartar el resto para limpiar el buffer de I2C
    while (Wire.available()) {
      Wire.read();
    }
  }
}

void ComI2C::receiveEventOnlyListeners(int howMany) {
  if (howMany == sizeof(sctruct_gamepad)) {
    Wire.readBytes((uint8_t*)&gamepadData, howMany);
    newDataReceived = true; 
  } else {
    Serial.printf("Esclavo 1: Tamano de datos recibido inesperado: %d bytes (Esperado: %zu)\n", howMany, sizeof(sctruct_gamepad));
    while (Wire.available()) {
      Wire.read();
    }
  }
}

// Funcion que se llama automaticamente cuando el maestro SOLICITA datos a ESTE esclavo
void ComI2C::requestEvent() {
  // ¡Envía los datos que el maestro solicitó!

  // Asegurarse de que messageToSend tenga los datos mas recientes o relevantes
  // Puedes actualizar messageToSend justo antes de este evento si es necesario,
  // pero es mejor tenerla actualizada continuamente en el loop o en otro lugar.

  // !!! Enviar la estructura completa como un bloque de bytes !!!
  Wire.write((uint8_t*)&sendData, sizeof(sendData));
}

struct_message ComI2C::requestMessageSlave(int address){
  // --- Enviar el dato al Esclavo 1 ---
  //Serial.printf("\nMaestro: Enviando struct (%zu bytes) al Esclavo 1 (0x%02X)...\n", sizeof(sendData), address);
  Wire.beginTransmission(address);
  // !!! Envía la estructura completa como un bloque de bytes !!!
  size_t bytesSent = Wire.write((uint8_t*)&sendData, sizeof(sendData));
  byte end_transmission_status = Wire.endTransmission();

  //Serial.printf("Maestro: Se intentaron enviar %zu bytes. Resultado: %d\n", bytesSent, end_transmission_status);
  if (end_transmission_status == 0 && bytesSent == sizeof(sendData)) {
    //Serial.println("Maestro: Envio a Esclavo 1 exitoso.");
  } else {
    Serial.printf("Maestro: Error/Envio parcial a Esclavo 1. Codigo: %d\n", end_transmission_status);
  }
  delay(20);

  // --- Solicitar la estructura completa del Esclavo 1 ---
  //Serial.printf("Maestro: Solicitando struct (%zu bytes) del Esclavo 1...\n", sizeof(struct_message));
  struct_message receivedMessageS1; // Variable para recibir la estructura
  size_t bytes_requested = sizeof(struct_message);
  size_t bytes_received = Wire.requestFrom(address, bytes_requested); // Solicitar el tamaño completo
  //Serial.printf("Maestro: Se solicitaron %zu bytes, se recibieron %zu\n", bytes_requested, bytes_received);

  if (bytes_received == bytes_requested) {
    // !!! Leer los bytes recibidos directamente en la variable de la estructura !!!
    Wire.readBytes((uint8_t*)&receivedMessageS1, bytes_received);
    //Serial.println("Maestro: Struct recibida de Esclavo 1.");
  } else {
    Serial.println("Maestro: Error o recepcion parcial de Esclavo 1.");
    // Leer y descartar el resto si Wire.available() > 0 para limpiar el buffer
    while(Wire.available()) Wire.read();
  }
  return receivedMessageS1;
    
}

void ComI2C::sendDataToListenersOnly(int address){
  // --- Enviar el dato al Esclavo 1 ---
    //Serial.printf("\nMaestro: Enviando struct (%zu bytes) al Esclavo 1 (0x%02X)...\n", sizeof(sendData), address);
    Wire.beginTransmission(address);
    // !!! Envía la estructura completa como un bloque de bytes !!!
    size_t bytesSent = Wire.write((uint8_t*)&gamepadData, sizeof(gamepadData));
    byte end_transmission_status = Wire.endTransmission();

    //Serial.printf("Maestro: Se intentaron enviar %zu bytes. Resultado: %d\n", bytesSent, end_transmission_status);
    if (end_transmission_status == 0 && bytesSent == sizeof(gamepadData)) {
      //Serial.println("Maestro: Envio a Esclavo 1 exitoso.");
    } else {
      Serial.printf("Maestro: Error/Envio parcial a Esclavo 1. Codigo: %d\n", end_transmission_status);
    }
    delay(2);
   
}

void ComI2C::setSendDataOfSensors(JoystickSensor  joystickP1Direction, JoystickSensor  joystickP2Direction,JoystickSensor  joystickP3Direction,JoystickSensor  joystickP4Direction,JoystickSensor  joystickP5Direction, MultipleButton multipleButton, boolean isLeftSide ){
  
  countMessage++;
  sendData.num_message = countMessage;

  multipleButton.scanKeys();

  //First Finger  
  int direction1 = multipleButton.direction[0];
  int x1 = joystickP1Direction.readX();
  int y1 = joystickP1Direction.readY();
  int buttonDown1 = digitalRead(pressDownPinPlayer1);
  boolean isPressDown1 = multipleButton.centerButton[0];
  //if(DEVICE_ID == 2){
  //    isPressDown1 = !isPressDown1;
  //}
  //Serial.print("button down: ");
  //Serial.println(buttonDown1);

  sendData.joystickButtons[0].idPlayer = 1;
  sendData.joystickButtons[0].isLeftSide  = isLeftSide;
  sendData.joystickButtons[0].direction = direction1;
  sendData.joystickButtons[0].posX = x1;
  sendData.joystickButtons[0].posY = y1;
  sendData.joystickButtons[0].isPressDown = isPressDown1;
  
  //Second Finger  
  int direction2 = multipleButton.direction[1];
  int x2 = joystickP2Direction.readX();
  int y2 = joystickP2Direction.readY();
  int buttonDown2 = digitalRead(pressDownPinPlayer2);
  boolean isPressDown2 = multipleButton.centerButton[1];
  //if(DEVICE_ID == 2){
  //  isPressDown2 = !isPressDown2;
  //}
  //Serial.print("button down: ");
  //Serial.println(buttonDown2);

  sendData.joystickButtons[1].idPlayer = 2;
  sendData.joystickButtons[1].isLeftSide  = isLeftSide;
  sendData.joystickButtons[1].direction = direction2;
  sendData.joystickButtons[1].posX = x2;
  sendData.joystickButtons[1].posY = y2;
  sendData.joystickButtons[1].isPressDown = isPressDown2;

  
  //Third Finger
  int direction3 = multipleButton.direction[2];
  int x3 = joystickP3Direction.readX();
  int y3 = joystickP3Direction.readY();
  int buttonDown3 = digitalRead(pressDownPinPlayer3);
  boolean isPressDown3 = multipleButton.centerButton[2];
  //if(DEVICE_ID == 2){
  //  isPressDown3 = !isPressDown3;
  //}

  /*
  Serial.print("x3: ");
  Serial.print(x3);
  Serial.print(" ,direccion: ");
  Serial.println(direction3);
  */

  sendData.joystickButtons[2].idPlayer = 3;
  sendData.joystickButtons[2].isLeftSide  = isLeftSide;
  sendData.joystickButtons[2].direction = direction3;
  sendData.joystickButtons[2].posX = x3;
  sendData.joystickButtons[2].posY = y3;
  sendData.joystickButtons[2].isPressDown = isPressDown3;

  //Four Finger
  int direction4 = multipleButton.direction[3];
  int x4 = joystickP4Direction.readX();
  int y4 = joystickP4Direction.readY();
  int buttonDown4 = digitalRead(pressDownPinPlayer4);
  boolean isPressDown4 = multipleButton.centerButton[3];
  //if(DEVICE_ID == 2){
  //  isPressDown4 = !isPressDown4;
  //}
  
  sendData.joystickButtons[3].idPlayer = 4;
  sendData.joystickButtons[3].isLeftSide  = isLeftSide;
  sendData.joystickButtons[3].direction = direction4;
  sendData.joystickButtons[3].posX = x4;
  sendData.joystickButtons[3].posY = y4;
  sendData.joystickButtons[3].isPressDown = isPressDown4;

  //Five Finger
  int direction5 = multipleButton.direction[4];
  int x5 = joystickP5Direction.readX();
  int y5 = joystickP5Direction.readY();
  int buttonDown5 = digitalRead(pressDownPinPlayer5);
  boolean isPressDown5 = multipleButton.centerButton[4];;
  //if(DEVICE_ID == 2){
  //    isPressDown5 = !isPressDown5;
  //}
  
  sendData.joystickButtons[4].idPlayer = 5;
  sendData.joystickButtons[4].isLeftSide  = isLeftSide;
  sendData.joystickButtons[4].direction = direction5;
  sendData.joystickButtons[4].posX = x5;
  sendData.joystickButtons[4].posY = y5;
  sendData.joystickButtons[4].isPressDown = isPressDown5;
  
}

void ComI2C::setGamepadData(struct_message messageIncomeP2, struct_message messageIncomeP4, int playerId){
  gamepadData.directionLeft  = messageIncomeP2.joystickButtons[playerId-1].direction;
  gamepadData.directionRigth = messageIncomeP4.joystickButtons[playerId-1].direction;

  gamepadData.posXLeft = messageIncomeP2.joystickButtons[playerId-1].posX;
  gamepadData.posXRigth = messageIncomeP4.joystickButtons[playerId-1].posX;

  gamepadData.posYLeft = messageIncomeP2.joystickButtons[playerId-1].posY;
  gamepadData.posYRigth = messageIncomeP4.joystickButtons[playerId-1].posY;

  gamepadData.isPressDownLeft  = messageIncomeP2.joystickButtons[playerId-1].isPressDown;
  gamepadData.isPressDownRigth = messageIncomeP4.joystickButtons[playerId-1].isPressDown;

  gamepadData.isPressUpLeft  = messageIncomeP2.joystickButtons[playerId-1].isPressUp;
  gamepadData.isPressUpRigth = messageIncomeP4.joystickButtons[playerId-1].isPressUp;

}


//Print Message
void ComI2C::printMessage(struct_message data){
  
  for(int i = 0; i<5 ; i++){
    Serial.print("id: ");
    Serial.print(data.joystickButtons[i].idPlayer);
    Serial.print("   ,direc: ");
    Serial.print(data.joystickButtons[i].direction);
    Serial.print(" ,posX: ");
    Serial.print(data.joystickButtons[i].posX);
    Serial.print("\t,posY: ");
    Serial.print(data.joystickButtons[i].posY);
    Serial.print("\t,isPressDown: ");
    Serial.print(data.joystickButtons[i].isPressDown);
    Serial.print("\t");
    
    /*
    Serial.print("\t,isLeftSide ");
    Serial.print(data.joystickButtons[i].isLeftSide);
    Serial.print("\t,isPressUp: ");
    Serial.print(data.joystickButtons[i].isPressUp);
    */
  }

  //Serial.print("\t,Bytes received: ");
  //Serial.println(len);

  Serial.print(",count: ");
  Serial.println(data.num_message);      
}

void ComI2C::printGamepadData(sctruct_gamepad data){

  Serial.print("id: ");
  Serial.print(data.idPlayer);
  Serial.print(" LEFT:  ,direct: ");
  Serial.print(data.directionLeft);
  Serial.print("\t,posX : ");
  Serial.print(data.posXLeft);
  Serial.print("\t,posY : ");
  Serial.print(data.posYLeft);
  Serial.print("\t,isPressDown: ");
  Serial.print(data.isPressDownLeft);
  Serial.print("\t");

  Serial.print(" RIGTH:  ,direc: ");
  Serial.print(data.directionRigth);
  Serial.print("\t,posX : ");
  Serial.print(data.posXRigth);
  Serial.print("\t,posY : ");
  Serial.print(data.posYRigth);
  Serial.print("\t,isPressDown: ");
  Serial.print(data.isPressDownRigth);
  Serial.println("\t");

}