#include <BleConnectionStatus.h>
#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

#include "JoystickButton.h"
//I2C
#include <Wire.h>

   

//#define DEVICE_2_ENABLED //18 Port
#define DEVICE_3_ENABLED //17 Port
//#define DEVICE_4_ENABLED //16 Port


#ifdef DEVICE_2_ENABLED
#define DEVICE_ID 2
#endif

#ifdef DEVICE_3_ENABLED
#define DEVICE_ID 3
#endif

#ifdef DEVICE_4_ENABLED
#define DEVICE_ID 4
#endif

const int I2C_SDA = 19;
const int I2C_SCL = 20;
volatile bool newDataReceived = false; 

#define SLAVE_ADDRESS_P3 0x10
#define SLAVE_ADDRESS_P4 0x11

int lastTimeCheckMillis = 0;

XboxGamepadDevice *gamepad;

bool buttonJoystickLeftDownPressed = false;

#ifdef DEVICE_2_ENABLED
BleCompositeHID compositeHID("P2g", "P2g", 29);
#endif

#ifdef DEVICE_3_ENABLED
BleCompositeHID compositeHID("P3g", "P3g", 29);
#endif

#ifdef DEVICE_4_ENABLED
BleCompositeHID compositeHID("P4g", "P4g", 29);
#endif

//JOYSTICK VARIABLES

JoystickButton joystickP2Button = JoystickButton( 4,  5, false, false, 0);
JoystickButton joystickP3Button = JoystickButton(15, 16, false, false, 0);
JoystickButton joystickP4Button = JoystickButton( 8,  3, false, false, 0);

#ifdef DEVICE_4_ENABLED
JoystickSensor  joystickP2Direction = JoystickSensor(6,   7, true, false);
JoystickSensor  joystickP3Direction = JoystickSensor(17, 18, true, false);
JoystickSensor  joystickP4Direction = JoystickSensor( 9, 10, true, false);
#else
JoystickSensor joystickP2Direction = JoystickSensor(6,   7, true, true);
JoystickSensor joystickP3Direction = JoystickSensor(17, 18, true, true);
JoystickSensor joystickP4Direction = JoystickSensor( 9, 10, true, true);
#endif


typedef struct sctruct_gamepad {
  uint8_t idPlayer;       // ID del dispositivo que envía (ej: 1, 2, 3)
    // Algún valor de ejemplo    
  
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


typedef struct JoystickType {
  uint8_t idPlayer;       // ID del dispositivo que envía (ej: 1, 2, 3)
  boolean isLeftSide;  // Algún valor de ejemplo    
  
  boolean isPressUp;  
  boolean isPressDown;
  
  uint16_t direction;

  int posX;
  int posY;
};

int countMessage = 0;
typedef struct struct_message {
  uint16_t num_message;
  JoystickType joystickButtons[5];
} struct_message;


struct_message receiveData;
struct_message sendData;
sctruct_gamepad gamepadData;

//esp_now_peer_info_t peerInfo;

//Print Message
void printMessage(struct_message data){
  
  for(int i = 1; i<4 ; i++){
    Serial.print("id: ");
    Serial.print(data.joystickButtons[i].idPlayer);
    Serial.print("   ,direc: ");
    Serial.print(data.joystickButtons[i].direction);
    Serial.print(" ,posX: ");
    Serial.print(data.joystickButtons[i].posX);
    Serial.print("\t,posY: ");
    Serial.print(data.joystickButtons[i].posY);
    Serial.print("\t");
    
    /*
    Serial.print("\t,isLeftSide ");
    Serial.print(data.joystickButtons[i].isLeftSide);
    Serial.print("\t,isPressUp: ");
    Serial.print(data.joystickButtons[i].isPressUp);
    Serial.print("\t,isPressDown: ");
    Serial.print(data.joystickButtons[i].isPressDown);
    */
  }

  //Serial.print("\t,Bytes received: ");
  //Serial.println(len);

  Serial.print(",count: ");
  Serial.println(data.num_message);      
}



void setup() {

  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  
  XboxSeriesXControllerDeviceConfiguration* config = new XboxSeriesXControllerDeviceConfiguration();

  // The composite HID device pretends to be a valid Xbox controller via vendor and product IDs (VID/PID).
  // Platforms like windows/linux need this in order to pick an XInput driver over the generic BLE GATT HID driver. 
  BLEHostConfiguration hostConfig = config->getIdealHostConfiguration();
  Serial.println("Using VID source: " + String(hostConfig.getVidSource(), HEX));
  Serial.println("Using VID: " + String(hostConfig.getVid(), HEX));
  Serial.println("Using PID: " + String(hostConfig.getPid(), HEX));
  Serial.println("Using GUID version: " + String(hostConfig.getGuidVersion(), HEX));
  Serial.println("Using serial number: " + String(hostConfig.getSerialNumber()));
    
  // Set up gamepad
  gamepad = new XboxGamepadDevice(config);

  FunctionSlot<XboxGamepadOutputReportData> vibrationSlot(OnVibrateEvent);
  gamepad->onVibrate.attach(vibrationSlot);
  
  compositeHID.addDevice(gamepad);
  Serial.println("Starting composite HID device...");
  compositeHID.begin(hostConfig);

  delay(100);
  gamepad->setLeftThumb(1, 1);
  gamepad->setRightThumb(1, 1);
  gamepad->sendGamepadReport();

  joystickP2Direction.init(gamepad);
  //joystickP2Direction.setCallback(callbackDetectMovementJoystick);
  joystickP3Direction.init(gamepad);
  joystickP4Direction.init(gamepad);

  joystickP2Button.init(gamepad);
  //joystickP2Button.setCallbackDirection(callbackDetectButtonsJoystick);
  joystickP3Button.init(gamepad);
  joystickP4Button.init(gamepad);

  receiveData = {};
  sendData = {};
  gamepadData = {};

  Wire.setPins(I2C_SDA, I2C_SCL);
  if(DEVICE_ID == 2){//MAestro
    Wire.begin();
  }else{//SLAVE P3 por ahora
    if(DEVICE_ID == 3){//MAestro
      Wire.begin(SLAVE_ADDRESS_P3);
      Wire.onReceive(receiveEventOnlyListeners); // Llama a receiveEvent cuando reciba datos del maestro
      //Wire.onRequest(requestEvent);   // Llama a requestEvent cuando el maestro pida datos a este esclavo
    }else{
      Wire.begin(SLAVE_ADDRESS_P4);
      Wire.onReceive(receiveEvent); // Llama a receiveEvent cuando reciba datos del maestro
      Wire.onRequest(requestEvent);   // Llama a requestEvent cuando el maestro pida datos a este esclavo
    }    
    // Registrar las funciones de evento
    

  }

  
  
  // Wire.setClock(400000); // Opcional: mayor velocidad
  delay(1000);

  
  
}

/*
void callbackDetectMovementJoystick(int positionX, int positionY) {
  
  Serial.print("posX:  ");
  Serial.print(positionX);
  Serial.print(" ,pos Y: ");
  Serial.println(positionY); 

  //DATA INIT
  //myData.joystickButtons[1].direction = 0;
  myData.joystickButtons[1].posX = positionX;
  myData.joystickButtons[1].posY = positionY;

  sendTestMessage();
  delay(1);//TODO: increase to 10 and put the delay do it with a counter time instead of delay()

}

void callbackDetectButtonsJoystick(int direction, boolean isPressed){
  //Serial.print("direction:  ");
  //Serial.print(direction);
  //Serial.print(" ,isPressed: ");
  //Serial.println(isPressed); 
  
  //sendDataToPeer(MAC_P3);
  //Serial.printf("Heap libre: %d bytes\n", ESP.getFreeHeap());

  myData.joystickButtons[1].direction = direction;

  sendTestMessage();

  delay(1);
}
*/

void OnVibrateEvent(XboxGamepadOutputReportData data)
{
    if(data.weakMotorMagnitude > 0 || data.strongMotorMagnitude > 0){
        //digitalWrite(ledPin, LOW);
    } else {
        //digitalWrite(ledPin, HIGH);
    }
    //Serial.println("Vibration event. Weak motor: " + String(data.weakMotorMagnitude) + " Strong motor: " + String(data.strongMotorMagnitude));
}


void joystickDirectionDetect(){
  if(compositeHID.isConnected()){
    joystickP2Direction.detectAndPress(true, true);
  }
  delay(1);
}

 
void joysticksButtonsDetect(){
  if(compositeHID.isConnected()){
    joystickP2Button.detectAndPress(true);
  }
  delay(1);
}

// Funcion que se llama automaticamente cuando el maestro ENVIA datos a ESTE esclavo
void receiveEvent(int howMany) {
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

void receiveEventOnlyListeners(int howMany) {
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
void requestEvent() {
  // ¡Envía los datos que el maestro solicitó!

  // Asegurarse de que messageToSend tenga los datos mas recientes o relevantes
  // Puedes actualizar messageToSend justo antes de este evento si es necesario,
  // pero es mejor tenerla actualizada continuamente en el loop o en otro lugar.

  // !!! Enviar la estructura completa como un bloque de bytes !!!
  Wire.write((uint8_t*)&sendData, sizeof(sendData));
}

struct_message requestMessageSlave(int address){
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

void sendDataToListenersOnly(int address){
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

void setSendDataOfSensors(boolean isLeftSide){
  
  countMessage++;
  sendData.num_message = countMessage;
  
  //Second Finger  
  int direction2 = joystickP2Button.detectDirecction();
  int x2 = joystickP2Direction.readX();
  int y2 = joystickP2Direction.readY();
  sendData.joystickButtons[1].idPlayer = 2;
  sendData.joystickButtons[1].isLeftSide  = isLeftSide;
  sendData.joystickButtons[1].direction = direction2;
  sendData.joystickButtons[1].posX = x2;
  sendData.joystickButtons[1].posY = y2;
  
  
  //Third Finger
  int direction3 = joystickP3Button.detectDirecction();
  int x3 = joystickP3Direction.readX();
  int y3 = joystickP3Direction.readY();
  sendData.joystickButtons[2].idPlayer = 3;
  sendData.joystickButtons[2].isLeftSide  = isLeftSide;
  sendData.joystickButtons[2].direction = direction3;
  sendData.joystickButtons[2].posX = x3;
  sendData.joystickButtons[2].posY = y3;

  //Four Finger
  int direction4 = joystickP4Button.detectDirecction();
  int x4 = joystickP4Direction.readX();
  int y4 = joystickP4Direction.readY();
  
  sendData.joystickButtons[3].idPlayer = 4;
  sendData.joystickButtons[3].isLeftSide  = isLeftSide;
  sendData.joystickButtons[3].direction = direction4;
  sendData.joystickButtons[3].posX = x4;
  sendData.joystickButtons[3].posY = y4;
  
}

void setGamepadData(struct_message messageIncomeP2, struct_message messageIncomeP4, int playerId){
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


void loop() {

 if(DEVICE_ID == 2){
    
    setSendDataOfSensors(DEVICE_ID == 2);

    //RECEIVE THE REST OF INFO OF SENSORS AND SEND TO P$
    struct_message messageIncomeP4 = requestMessageSlave(SLAVE_ADDRESS_P4);
    printMessage(messageIncomeP4);

    joystickP2Direction.justPress(sendData.joystickButtons[1].posX, sendData.joystickButtons[1].posY, true);
    joystickP2Direction.justPress(messageIncomeP4.joystickButtons[1].posX, messageIncomeP4.joystickButtons[1].posY, false);

    joystickP2Button.pressButton(sendData.joystickButtons[1].direction, true, false);
    joystickP2Button.pressButton(messageIncomeP4.joystickButtons[1].direction, false, false);

    delay(5);
    

    //SEND TO OTHERS
    //DEVICE 3
    setGamepadData(sendData, messageIncomeP4, 3);
    sendDataToListenersOnly(SLAVE_ADDRESS_P3);

 }else if(DEVICE_ID == 4){
    setSendDataOfSensors(DEVICE_ID == 4);   
    
    joystickP4Direction.justPress(sendData.joystickButtons[3].posX, sendData.joystickButtons[3].posY, false);
    joystickP4Button.pressButton(sendData.joystickButtons[3].direction, false, false);
      
    if (newDataReceived) {

      newDataReceived = false;
      printMessage(receiveData);

      joystickP4Direction.justPress(receiveData.joystickButtons[3].posX, receiveData.joystickButtons[3].posY, true);
      joystickP4Button.pressButton(receiveData.joystickButtons[3].direction, true, false);
      //countMessage++;
      //sendData.num_message = countMessage; 
    }
    // El loop puede hacer otras tareas
    delay(4);
 
 }else{//Slaves
    if (newDataReceived) {

      newDataReceived = false;
      //printMessage(receiveData);
      joystickP3Direction.justPress(gamepadData.posXLeft , gamepadData.posYLeft , true);
      joystickP3Direction.justPress(gamepadData.posXRigth, gamepadData.posYRigth, false);
      
      joystickP3Button.pressButton(gamepadData.directionLeft, true, false);  
      joystickP3Button.pressButton(gamepadData.directionRigth, false, false);
      //countMessage++;
      //sendData.num_message = countMessage; 
    }
    // El loop puede hacer otras tareas
    delay(4);
 }
  

  /*
  if (DEVICE_ID == 2|| DEVICE_ID == 4){

    int currentMillis = millis();
    if(  (currentMillis-lastTimeCheckMillis) > 40){

      //joystickDirectionDetect();
      //joysticksButtonsDetect();
      
      //Second Finger
      int direction2 = joystickP2Button.detectDirecction();
      int x2 = joystickP2Direction.readX();
      int y2 = joystickP2Direction.readY();
      if(DEVICE_ID == 2){
        joystickP2Direction.justPress(x2, y2, true);
        delay(1);
        joystickP2Button.pressButton(direction2, true, false);
        delay(1);
      }else{
        myData.joystickButtons[1].idPlayer = 2;
        myData.joystickButtons[1].isLeftSide  = false;
        myData.joystickButtons[1].direction = direction2;
        myData.joystickButtons[1].posX = x2;
        myData.joystickButtons[1].posY = y2;
      }
      
      //Third Finger
      int direction3 = joystickP3Button.detectDirecction();
      int x3 = joystickP3Direction.readX();
      int y3 = joystickP3Direction.readY();
      myData.joystickButtons[2].idPlayer = 3;
      myData.joystickButtons[2].isLeftSide  = true;
      if(DEVICE_ID == 4){
        myData.joystickButtons[2].isLeftSide  = false;
      }
      myData.joystickButtons[2].direction = direction3;
      myData.joystickButtons[2].posX = x3;
      myData.joystickButtons[2].posY = y3;

      //Four Finger
      int direction4 = joystickP4Button.detectDirecction();
      int x4 = joystickP4Direction.readX();
      int y4 = joystickP4Direction.readY();
      if(DEVICE_ID == 2){
        myData.joystickButtons[3].idPlayer = 4;
        myData.joystickButtons[3].isLeftSide  = true;
        myData.joystickButtons[3].direction = direction4;
        myData.joystickButtons[3].posX = x4;
        myData.joystickButtons[3].posY = y4;
      }else{//DEVICE_ID == 4
        joystickP4Direction.justPress(x4, y4, false);
        delay(1);
        joystickP4Button.pressButton(direction4, false, false);
        delay(1);
      }
      

      sendTestMessage();
      
      //Serial.print("Current Millis: ");
      //Serial.println(lastTimeCheckMillis); 
      lastTimeCheckMillis = currentMillis;
    }
  }
  */

  //Serial.print("Peers count: ");
  //Serial.println(countPeers());

  /*
  int delayLoop = millis() - startLoop;

  int buttonLeftDown = digitalRead(buttonJoystickLeftDown);
  int buttonRigthUp = digitalRead(triggerButtons4);

  
  if (buttonLeftDown == LOW) {
    if (buttonJoystickLeftDownPressed == false) {
        buttonJoystickLeftDownPressed = true;
        //gamepad->press(gamepadButtons[0][8]);
        gamepad-> setLeftTrigger(30000);
        gamepad->sendGamepadReport();
    }    
  } else {
    if (buttonJoystickLeftDownPressed == true) {
      //gamepad->release(gamepadButtons[0][7]);
      gamepad-> setRightTrigger(0);
      gamepad-> setLeftTrigger(0);
      gamepad->sendGamepadReport();
      buttonJoystickLeftDownPressed = false;
    }
    //buttonJoystickLeftDownPressed = false;
  }
  */

  //Serial.print("loop elapse: ");
  //Serial.println(delayLoop);
  //delay(2);

}