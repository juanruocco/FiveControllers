#ifndef __JOYSTICKSENSOR_H__
#define __JOYSTICKSENSOR_H__

#include <Arduino.h>


const int thresholdError = 400;//TODO: check if reduce this threshold to more precicion but errors int the button detection
const int range = 32767;  // output range of X or Y movement


class JoystickSensor {
  private:
    int pinX;
    int pinY;
    int centerXCalibration;
    int centerYCalibration;
    boolean inverseXDirection;
    boolean inverseYDirection;

  public:
    JoystickSensor(int pinX, int pinY, boolean inverseXDirection, boolean inverseYDirection);
    void init();
    int readX();
    int readY();    
};


#endif



