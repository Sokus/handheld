#include "ads1115.h"
#include "joystick.h"

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define UPDATE_FREQ 50000 // ms (200Hz)
#define USE_ANALOG_DPAD 0

// x: 27 - 13445 - 26528
// y: 25 - 13350 - 26527

int main(void)
{
    int i2c = open("/dev/i2c-1", O_RDWR);
    ADS1115_State ads = ADS1115_Init(i2c, 0x48);
    
    Joystick js = JS_CreateJoystickEx("RPI I2C Custom Joystick",
                                      27, 26528, 500,
                                      25, 26527, 500);
    
    while(1)
    {
        ADS1115_SetActiveInput(&ads, 0);
        usleep(UPDATE_FREQ/2);
        short v0 = ADS1115_ReadValue(&ads);
        
        ADS1115_SetActiveInput(&ads, 1);
        usleep(UPDATE_FREQ/2);
        short v1 = ADS1115_ReadValue(&ads);
        
        JS_SendAxisEvent(&js, JoystickAxis_X, v0);
        JS_SendAxisEvent(&js, JoystickAxis_Y, v1);
        
    }
    
    // ioctl(uinput, UI_DEV_DESTROY);
    // close(i2c);
}