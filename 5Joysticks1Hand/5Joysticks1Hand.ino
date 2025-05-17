#include <BleConnectionStatus.h>
#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

#include "JoystickSensor.h"
#include "MultipleButton.h"
#include "GamepadManager.h"

//I2C
#include <Wire.h>

const int I2C_SDA_ESP = 21;
const int I2C_SCL_ESP = 22;
const int I2C_SDA_ESPS3 = 21;
const int I2C_SCL_ESPS3 = 47;

//#define DEVICE_1_ENABLED   // 4 port esp32
//#define DEVICE_2_ENABLED // 16 Port 
//#define DEVICE_3_ENABLED // 4 port esp32
#define DEVICE_4_ENABLED // 18 Port
//#define DEVICE_5_ENABLED // 4 port esp32

#ifdef DEVICE_1_ENABLED 
#define DEVICE_ID 1
const int I2C_SCL = I2C_SCL_ESP;
const int I2C_SDA = I2C_SDA_ESP;
#endif

#ifdef DEVICE_2_ENABLED
#define DEVICE_ID 2
const int I2C_SCL = I2C_SCL_ESPS3;
const int I2C_SDA = I2C_SDA_ESPS3;
#endif

#ifdef DEVICE_3_ENABLED
#define DEVICE_ID 3
const int I2C_SCL = I2C_SCL_ESP;//20;//22;
const int I2C_SDA = I2C_SDA_ESP;//19;//21;
#endif

#ifdef DEVICE_4_ENABLED
#define DEVICE_ID 4
const int I2C_SCL = I2C_SCL_ESPS3;
const int I2C_SDA = I2C_SDA_ESPS3;
#endif

#ifdef DEVICE_5_ENABLED
#define DEVICE_ID 5
const int I2C_SCL = I2C_SCL_ESP;
const int I2C_SDA = I2C_SDA_ESP;
#endif


volatile bool newDataReceived = false; 

#define SLAVE_ADDRESS_P1 0x08
#define SLAVE_ADDRESS_P3 0x10
#define SLAVE_ADDRESS_P4 0x11
#define SLAVE_ADDRESS_P5 0x12

int lastTimeCheckMillis = 0;

XboxGamepadDevice *gamepad;

bool buttonJoystickLeftDownPressed = false;

#ifdef DEVICE_1_ENABLED
BleCompositeHID compositeHID("P1i", "P1i", 30);
#endif

#ifdef DEVICE_2_ENABLED
BleCompositeHID compositeHID("P2i", "P2i", 30);
#endif

#ifdef DEVICE_3_ENABLED
BleCompositeHID compositeHID("P3i", "P3i", 30);
#endif

#ifdef DEVICE_4_ENABLED
BleCompositeHID compositeHID("P4i", "P4i", 30);
#endif

#ifdef DEVICE_5_ENABLED
BleCompositeHID compositeHID("P5i", "P5i", 30);
#endif


//JOYSTICK VARIABLES
int pressDownPinPlayer1 = 42;
int pressDownPinPlayer2 = 41;
int pressDownPinPlayer3 = 40;
int pressDownPinPlayer4 = 39;
int pressDownPinPlayer5 = 38;

#ifdef DEVICE_2_ENABLED
boolean xInverseJoystickDirectionFinger1 = true;
boolean yInverseJoystickDirectionFinger1 = false;
boolean xInverseJoystickDirectionFinger2 = true;
boolean yInverseJoystickDirectionFinger2 = false;
boolean xInverseJoystickDirectionFinger3 = true;
boolean yInverseJoystickDirectionFinger3 = false;
boolean xInverseJoystickDirectionFinger4 = true;
boolean yInverseJoystickDirectionFinger4 = false;
boolean xInverseJoystickDirectionFinger5 = true;
boolean yInverseJoystickDirectionFinger5 = false;

#else

boolean xInverseJoystickDirectionFinger1 = true;
boolean yInverseJoystickDirectionFinger1 = true;
boolean xInverseJoystickDirectionFinger2 = false;
boolean yInverseJoystickDirectionFinger2 = true;//false;
boolean xInverseJoystickDirectionFinger3 = false;
boolean yInverseJoystickDirectionFinger3 = true;//false;
boolean xInverseJoystickDirectionFinger4 = false;
boolean yInverseJoystickDirectionFinger4 = true;//false;
boolean xInverseJoystickDirectionFinger5 = true;
boolean yInverseJoystickDirectionFinger5 = true;

#endif

  //Directions

JoystickSensor joystickP1Direction = JoystickSensor(  9,  10, xInverseJoystickDirectionFinger1, yInverseJoystickDirectionFinger1);
JoystickSensor joystickP2Direction = JoystickSensor( 11,  12, xInverseJoystickDirectionFinger2, yInverseJoystickDirectionFinger2);
JoystickSensor joystickP3Direction = JoystickSensor( 13,  14, xInverseJoystickDirectionFinger3, yInverseJoystickDirectionFinger3);
JoystickSensor joystickP4Direction = JoystickSensor(  1,   2, xInverseJoystickDirectionFinger4, yInverseJoystickDirectionFinger4);
JoystickSensor joystickP5Direction = JoystickSensor( 20,  19, xInverseJoystickDirectionFinger5, yInverseJoystickDirectionFinger5);

MultipleButton multipleButton = MultipleButton();

GamepadManager gamepadManager = GamepadManager();

  //Joysticks Buttons
//JoystickButton joystickP1Button = JoystickButton(  4,   5, xInverseJoystickButtonFinger1, yInverseJoystickButtonFinger1, 0);
//JoystickButton joystickP2Button = JoystickButton( 15,  16, xInverseJoystickButtonFinger2, yInverseJoystickButtonFinger2, 0);
//JoystickButton joystickP3Button = JoystickButton(  8,   3, xInverseJoystickButtonFinger3, yInverseJoystickButtonFinger3, 0);
//JoystickButton joystickP4Button = JoystickButton( 11,  12, xInverseJoystickButtonFinger4, yInverseJoystickButtonFinger4, 0);
//JoystickButton joystickP5Button = JoystickButton(  1,   2, xInverseJoystickButtonFinger5, yInverseJoystickButtonFinger5, 0);



#pragma pack(1)
typedef struct sctruct_gamepad {
  uint8_t idPlayer;
  
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
#pragma pack()

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

//Print Message
void printMessage(struct_message data){
  
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
    Serial.print("\t,isPressDown: ");
    Serial.print(data.joystickButtons[i].isPressDown);
    */
  }

  //Serial.print("\t,Bytes received: ");
  //Serial.println(len);

  Serial.print(",count: ");
  Serial.println(data.num_message);      
}

void printGamepadData(sctruct_gamepad data){

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
  if(DEVICE_ID == 2){
    pinMode(pressDownPinPlayer1, INPUT_PULLUP);
    pinMode(pressDownPinPlayer2, INPUT_PULLUP);
    pinMode(pressDownPinPlayer3, INPUT_PULLUP);
    pinMode(pressDownPinPlayer4, INPUT_PULLUP);
    pinMode(pressDownPinPlayer5, INPUT_PULLUP); 
  }else{
    pinMode(pressDownPinPlayer1, INPUT_PULLUP);
    pinMode(pressDownPinPlayer2, INPUT_PULLDOWN);
    pinMode(pressDownPinPlayer3, INPUT_PULLDOWN);
    pinMode(pressDownPinPlayer4, INPUT_PULLDOWN);
    pinMode(pressDownPinPlayer5, INPUT_PULLUP);
  }
  
  
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

  joystickP1Direction.init(gamepad);
  joystickP2Direction.init(gamepad);
  //joystickP2Direction.setCallback(callbackDetectMovementJoystick);
  joystickP3Direction.init(gamepad);
  
  joystickP4Direction.init(gamepad);
  joystickP5Direction.init(gamepad);
  

  //joystickP1Button.init(gamepad);
  //joystickP2Button.init(gamepad);
  //joystickP2Button.setCallbackDirection(callbackDetectButtonsJoystick);
  //joystickP3Button.init(gamepad);
  //joystickP4Button.init(gamepad);
  //joystickP5Button.init(gamepad);

  multipleButton.init(gamepad);
  gamepadManager.init(gamepad);

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
      Wire.onReceive(receiveEvent); // Llama a receiveEvent cuando reciba datos del maestro
      Wire.onRequest(requestEvent);   // Llama a requestEvent cuando el maestro pida datos a este esclavo
      
    }else if(DEVICE_ID == 3){
      Wire.begin(SLAVE_ADDRESS_P3);
      Wire.setClock(100000);
      //Wire.begin(I2C_SDA, I2C_SCL);
      Wire.onReceive(receiveEventOnlyListeners); // Llama a receiveEvent cuando reciba datos del maestro
      //Wire.onRequest(requestEvent);   // Llama a requestEvent cuando el maestro pida datos a este esclavo
    } else if(DEVICE_ID == 1){

      Wire.begin(SLAVE_ADDRESS_P1);
      Wire.setClock(100000);
      Wire.onReceive(receiveEventOnlyListeners); // Llama a receiveEvent cuando reciba datos del maestro
      //Wire.onRequest(requestEvent);   // Llama a requestEvent cuando el maestro pida datos a este esclavo
    
    } else if(DEVICE_ID == 5){

      Wire.begin(SLAVE_ADDRESS_P5);
      Wire.setClock(100000);
      Wire.onReceive(receiveEventOnlyListeners); // Llama a receiveEvent cuando reciba datos del maestro
      //Wire.onRequest(requestEvent);   // Llama a requestEvent cuando el maestro pida datos a este esclavo
    } 
    // Registrar las funciones de evento
    
  }
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

/*
void joysticksButtonsDetect(){
  if(compositeHID.isConnected()){
    joystickP2Button.detectAndPress(true);
  }
  delay(1);
}
*/

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

void testPressButtonDown(){
  if (buttonJoystickLeftDownPressed == false) {
    buttonJoystickLeftDownPressed = true;
    //gamepad->press(gamepadButtons[0][8]);
    gamepad-> setLeftTrigger(30000);
    gamepad->sendGamepadReport();
  } 
}

void testReleaseButtonDown(){
  if (buttonJoystickLeftDownPressed == true) {
    //gamepad->release(gamepadButtons[0][7]);
    gamepad-> setLeftTrigger(0);
    gamepad->sendGamepadReport();
    buttonJoystickLeftDownPressed = false;
  }
}


void loop() {

 if(DEVICE_ID == 2){
    
    setSendDataOfSensors(DEVICE_ID == 2);

    //RECEIVE THE REST OF INFO OF SENSORS AND SEND TO P$
    struct_message messageIncomeP4 = requestMessageSlave(SLAVE_ADDRESS_P4);
    printMessage(messageIncomeP4);

    joystickP2Direction.justPress(sendData.joystickButtons[1].posX, sendData.joystickButtons[1].posY, true);
    joystickP2Direction.justPress(messageIncomeP4.joystickButtons[1].posX, messageIncomeP4.joystickButtons[1].posY, false);
    if(sendData.joystickButtons[1].isPressDown){
      testPressButtonDown();
    }else{
      testReleaseButtonDown();
    } 

    //multipleButton
    gamepadManager.pressDirection(sendData.joystickButtons[1].direction, true, false);
    gamepadManager.pressDirection(messageIncomeP4.joystickButtons[1].direction, true, false);

    //joystickP2Button.pressButton(sendData.joystickButtons[1].direction, true, false);
    //joystickP2Button.pressButton(messageIncomeP4.joystickButtons[1].direction, false, false);

    delay(2);    

    //SEND TO OTHERS
    //DEVICE 1
    setGamepadData(sendData, messageIncomeP4, 1);
    sendDataToListenersOnly(SLAVE_ADDRESS_P1);
    delay(2);


    //DEVICE 3
    setGamepadData(sendData, messageIncomeP4, 3);
    sendDataToListenersOnly(SLAVE_ADDRESS_P3);
    delay(2);

    //DEVICE 5
    setGamepadData(sendData, messageIncomeP4, 5);
    sendDataToListenersOnly(SLAVE_ADDRESS_P5);
    delay(2);


 }else if(DEVICE_ID == 4){
    setSendDataOfSensors(DEVICE_ID == 4);   
    //printMessage(sendData);

    joystickP4Direction.justPress(sendData.joystickButtons[3].posX, sendData.joystickButtons[3].posY, false);
    gamepadManager.pressDirection(sendData.joystickButtons[3].direction, false, false);
    //joystickP4Button.pressButton(sendData.joystickButtons[3].direction, false, false);
      
    if (newDataReceived) {

      newDataReceived = false;
      printMessage(receiveData);

      joystickP4Direction.justPress(receiveData.joystickButtons[3].posX, receiveData.joystickButtons[3].posY, true);
      //joystickP4Button.pressButton(receiveData.joystickButtons[3].direction, true, false);
      gamepadManager.pressDirection(receiveData.joystickButtons[3].direction, true, false);
      
      //countMessage++;
      //sendData.num_message = countMessage; 

      if(receiveData.joystickButtons[3].isPressDown){
        testPressButtonDown();
      }else{
        testReleaseButtonDown();
      }

    }
    // El loop puede hacer otras tareas
    delay(4);
 
 }else{//Slaves
    if (newDataReceived) {

      newDataReceived = false;
      printGamepadData(gamepadData);
      joystickP3Direction.justPress(gamepadData.posXLeft , gamepadData.posYLeft , true);
      joystickP3Direction.justPress(gamepadData.posXRigth, gamepadData.posYRigth, false);
      
      //joystickP3Button.pressButton(gamepadData.directionLeft, true, false);  
      //joystickP3Button.pressButton(gamepadData.directionRigth, false, false);
      gamepadManager.pressDirection(gamepadData.directionLeft, true, false); 
      gamepadManager.pressDirection(gamepadData.directionRigth, false, false);   

      if(gamepadData.isPressDownLeft){
        testPressButtonDown();
      }else{
        testReleaseButtonDown();
      }
      //countMessage++;
      //sendData.num_message = countMessage; 
    }
    // El loop puede hacer otras tareas
    delay(4);
 }

}