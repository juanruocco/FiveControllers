//#include <Arduino.h>
#define PLAYER_2_ENABLED

#define PLAYER_ID 2

uint8_t MAC_P2[] = {0x64, 0xE8, 0x33, 0x7E, 0x04, 0x3C};  
uint8_t MAC_P3[] = {0xA0, 0x85, 0xE3, 0xE7, 0x44, 0x28};  
uint8_t MAC_P4[] = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC}; 

//#include "ESP32_NOW.h"
#include "WiFi.h"
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_mac.h>  // For the MAC2STR and MACSTR macros
#include <esp_system.h>

#include <BleConnectionStatus.h>
#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

#include "JoystickDirection.h"
#include "JoystickButton.h"

#define ESPNOW_WIFI_CHANNEL 6


XboxGamepadDevice *gamepad;

bool buttonJoystickLeftDownPressed = false;

BleCompositeHID compositeHID("P2f", "P2f", 26);
/*
//ESP32 P2
BleCompositeHID compositeHID("P2c", "P2c", 22);
BleCompositeHID compositeHID("P3c", "P3c", 22);
*/

//JOYSTICK VARIABLES

//JoystickSensor joystickP1Direction = JoystickSensor(1, 2, false, false)
JoystickSensor joystickP2Direction   = JoystickSensor(6, 7, true, true);
//JoystickSensor joystickP3Direction = JoystickSensor(17, 18, false, false)
//JoystickSensor joystickP4Direction = JoystickSensor( 9, 10, false, false)
//JoystickSensor joystickP5Direction = JoystickSensor(20, 19, false, false)


JoystickButton joystickP2Button = JoystickButton(4, 5, false, false, 0);



typedef struct struct_message {
    int idPlayer;       // ID del dispositivo que envía (ej: 1, 2, 3)
    boolean isLeftSide;  // Algún valor de ejemplo
    boolean isUpSide;  // Algún valor de ejemplo
    int value1;
    int value2;
    
} struct_message;

struct_message myData;

esp_now_peer_info_t peerInfo;

// ===================================================
// === CALLBACKS DE ESP-NOW ===
// ===================================================

// Callback cuando los datos son enviados
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\nUltimo Paquete Enviado a: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X:", mac_addr[i]);
  }
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? " Enviado Correctamente" : " Fallo al Enviar");
  
  
}

// Callback cuando los datos son recibidos
void OnDataRecv(const esp_now_recv_info * info, const uint8_t *incomingData, int len) {
  // Ahora, para obtener la dirección MAC del emisor, la lees de la estructura 'info'
  Serial.println("llego mensaje");
  const uint8_t * mac_addr = info->des_addr; // Lee la MAC del emisor desde la estructura info

  Serial.print("\nMensaje Recibido de: ");
   for (int i = 0; i < 6; i++) {
    Serial.printf("%02X:", mac_addr[i]); // Usa la MAC obtenida de info
  }

  struct_message receivedData;
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  Serial.printf(" Tamano: %d bytes\n", len);
  Serial.printf("  ID del Emisor: %d \n", receivedData.idPlayer); // El ID viene en los datos, no en la info de recepcion
  //Serial.printf("  Valor: %f \n", receivedData.value);
  //Serial.printf("  Texto: %s \n", receivedData.text);

  // Opcionalmente, puedes acceder a RSSI u otra info:
  // Serial.printf("  RSSI: %d dBm\n", info->rssi);
}

/* Classes */

// Creating a new class that inherits from the ESP_NOW_Peer class is required.
/*
class ESP_NOW_Broadcast_Peer : public ESP_NOW_Peer {
public:
  // Constructor of the class using the broadcast address
  ESP_NOW_Broadcast_Peer(uint8_t channel, wifi_interface_t iface, const uint8_t *lmk) : ESP_NOW_Peer(ESP_NOW.BROADCAST_ADDR, channel, iface, lmk) {}

  // Destructor of the class
  ~ESP_NOW_Broadcast_Peer() {
    remove();
  }

  // Function to properly initialize the ESP-NOW and register the broadcast peer
  bool begin() {
    if (!ESP_NOW.begin() || !add()) {
      log_e("Failed to initialize ESP-NOW or register the broadcast peer");
      return false;
    }
    return true;
  }

  // Function to send a message to all devices within the network
  bool send_message(const uint8_t *data, size_t len) {
    if (!send(data, len)) {
      log_e("Failed to broadcast message");
      return false;
    }
    return true;
  }
};

uint32_t msg_count = 0;
// Create a broadcast peer object
ESP_NOW_Broadcast_Peer broadcast_peer(ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);

//BleGamepad bleGamepad;
//BleGamepadConfiguration bleGamepadConfig;  
 */ 


// ===================================================
// === FUNCION DE ENVIO AUXILIAR ===
// ===================================================
// Esta función ayuda a enviar datos a un peer especifico
void sendDataToPeer(uint8_t *peer_mac) {

    myData.idPlayer = 2;
    myData.isLeftSide = true;
    myData.isUpSide = false;
    myData.value1 = 321;
    myData.value2 = 123;
    
    esp_err_t result = esp_now_send(peer_mac, (uint8_t *) &myData, sizeof(myData));

    if (result == ESP_OK) {
        Serial.print("Enviando datos a MAC: ");
        for (int i = 0; i < 6; i++) {
            Serial.printf("%02X:", peer_mac[i]);
        }
        Serial.println(" - en proceso..."); // El resultado final se ve en OnDataSent
    } else {
        Serial.print("Error al enviar datos a MAC: ");
         for (int i = 0; i < 6; i++) {
            Serial.printf("%02X:", peer_mac[i]);
        }
        Serial.println(" - Fallo.");
    }
}

void setup() {

  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  //Keyboard.begin();
  //bleGamepadConfig.setControllerType(CONTROLLER_TYPE_JOYSTICK); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
  //bleGamepad.begin(&bleGamepadConfig);


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
  //gamepad = new XboxGamepadDevice(config);
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
  joystickP2Direction.setCallback(callbackDetectMovementJoystick);
  joystickP2Button.init(gamepad);
  joystickP2Button.setCallbackDirection(callbackDetectButtonsJoystick);

  // Initialize the Wi-Fi module
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    ESP.restart(); // Reinicia si falla la inicialización
    return;
  }
  Serial.println("ESP-NOW inicializado correctamente.");

  Serial.println("  MAC Address: " + WiFi.macAddress());
  Serial.printf("  Channel: %d\n", ESPNOW_WIFI_CHANNEL);

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Callbacks registrados.");

  // ===================================================
  // === REGISTRAR PEERS (CAMBIAR SEGUN EL DISPOSITIVO) ===
  // ===================================================
  // La configuración de peers cambia para cada dispositivo.
  // Cada dispositivo debe registrar las MACs de los *otros dos*.
  // --- REGISTRAR PEERS PARA D1 ---

  // Configuración general de peerInfo
  //memcpy(peerInfo.lmk, NULL, ESP_NOW_KEY_LEN); // Clave LMK (0 por defecto si no usas encriptacion)
  memset(peerInfo.lmk, 0, ESP_NOW_KEY_LEN);
  peerInfo.channel = 0; // Canal WiFi (0 = usa el canal actual, mejor si lo fijas explicitamente)
  peerInfo.encrypt = false; // Deshabilitar encriptacion por ahora

  if(PLAYER_ID != 2){
    // Registrar D2
    memcpy(peerInfo.peer_addr, MAC_P2, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Fallo al añadir peer P2");
      return;
    }
    Serial.println("Peer P2 registrado.");
  }
  
  if(PLAYER_ID != 3){
    // Registrar D3
    memcpy(peerInfo.peer_addr, MAC_P3, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Fallo al añadir peer P3");
      return;
    }
    Serial.println("Peer P3 registrado.");
  }

  if(PLAYER_ID != 4){
    // Registrar D4
    memcpy(peerInfo.peer_addr, MAC_P4, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Fallo al añadir peer P4");
      return;
    }
    Serial.println("Peer P4 registrado.");
  }
  
  Serial.println("Peers registrados correctamente");
  
}

void callbackDetectMovementJoystick(int positionX, int positionY) {
  
  Serial.print("posX:  ");
  Serial.print(positionX);
  Serial.print(" ,pos Y: ");
  Serial.println(positionY); 
  //sendDataToPeer(MAC_P3);
  //sendMessage();
  Serial.printf("Heap libre: %d bytes\n", ESP.getFreeHeap());
  delay(10);//TODO: increase to 10 and put the delay do it with a counter time instead of delay()
}

void callbackDetectButtonsJoystick(int direction, boolean isPressed){
  Serial.print("direction:  ");
  Serial.print(direction);
  Serial.print(" ,isPressed: ");
  Serial.println(isPressed); 
  sendDataToPeer(MAC_P3);
  Serial.printf("Heap libre: %d bytes\n", ESP.getFreeHeap());
  delay(10);
}


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
    joystickP2Direction.detectAndPress();
  }
  delay(1);
}

 
void joysticksButtonsDetect(){
  if(compositeHID.isConnected()){
    joystickP2Button.detectAndPress();
  }
  delay(1);
}

void sendMessage(){
  /*char data[32];
  snprintf(data, sizeof(data), "Hello, World! #%lu", msg_count++);

  Serial.printf("Broadcasting message: %s\n", data);

  if (!broadcast_peer.send_message((uint8_t *)data, sizeof(data))) {
    Serial.println("Failed to broadcast message");
  }*/
}

/*
//PRESS GAMEPAD JOYSTICK BUTTONS
void pressButtonsJoystickManager(int row, int col){
  if (gamepadButtonsJoystickPressed[row][col] == false){

    sendMessage();
    
    pressButtonsJoystick(row, col);
    gamepadButtonsJoystickPressed[row][col] = true;
  }
}

*/


void loop() {
  
  //joystickDirection1();
  int startLoop = millis();
  //keyboardDetection();
  joystickDirectionDetect();
  joysticksButtonsDetect();

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