#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else
#include "USB.h"
#include "USBHIDGamepad.h"
USBHIDGamepad Gamepad;

const int buttonPin = 0;
int previousButtonState = HIGH;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Gamepad.begin();
  USB.begin();
  Serial.begin(115200);
  Serial.println("\n==================\nUSB Gamepad Testing\n==================\n");
  Serial.println("Press BOOT Button to activate the USB gamepad.");
  Serial.println("Longer press will change the affected button and controls.");
  Serial.println("Shorter press/release just activates the button and controls.");
}

void loop() {
  static uint8_t padID = 0;
  static long lastPress = 0;

  Gamepad.pressButton(padID);                     // Buttons 1 to 32
  Gamepad.pressButton(1);
  Gamepad.pressButton(2); 
  Gamepad.pressButton(3); 
  Gamepad.pressButton(4); 
  Gamepad.pressButton(5);    
  Gamepad.pressButton(6); 
  Gamepad.pressButton(7); 
  Gamepad.leftStick(padID << 3, padID << 3);      // X Axis, Y Axis
  Gamepad.rightStick(-(padID << 2), padID << 2);  // Z Axis, Z Rotation
  Gamepad.leftTrigger(padID << 4);                // X Rotation
  Gamepad.rightTrigger(-(padID << 4));            // Y Rotation
  Gamepad.hat((padID & 0x7) + 1);                 // Point of View Hat
  log_d("Pressed PadID [%d]", padID);
  delay(1000);

  Gamepad.releaseButton(padID);
  Gamepad.releaseButton(1);
  Gamepad.releaseButton(2);
  Gamepad.releaseButton(3);
  Gamepad.releaseButton(4);
  Gamepad.releaseButton(5);
  Gamepad.releaseButton(6);
  Gamepad.releaseButton(7);
  Gamepad.leftStick(20000, 20000);
  Gamepad.rightStick(0, 0);
  Gamepad.leftTrigger(0);
  Gamepad.rightTrigger(0);
  Gamepad.hat(HAT_CENTER);
  log_d("Released PadID [%d]\n", padID);
  delay(1000);

 
}
#endif /* ARDUINO_USB_MODE */
