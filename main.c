#include "ads1115.h"
#include "gpio.h"
#include "joystick.h"

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

// GPIO layout:
// ---------------------
//  SELECT  6 | 12
//   START 13 | gnd
//      UP 19 | 16 RIGHT
// (taken) 26 | 20 DOWN
//        gnd | 21 LEFT

#define GPIO_ACTION_LEFT   21
#define GPIO_ACTION_DOWN   20
#define GPIO_ACTION_RIGHT  16
#define GPIO_ACTION_UP     19

#define GPIO_ACTION_SELECT  6
#define GPIO_ACTION_START  13

#define UPDATE_FREQ 50000 // ms (200Hz)

int main(void)
{
    int i2c = open("/dev/i2c-1", O_RDWR);
    ADS1115_State ads = ADS1115_Init(i2c, 0x48);
    GPIO_State gpio = GPIO_Create();
    Joystick js = JS_CreateJoystickEx("RPI I2C Custom Joystick",
                                      27, 26528, 500,
                                      25, 26527, 500);
    
    GPIO_SetInput(&gpio, GPIO_ACTION_LEFT);
    GPIO_SetInput(&gpio, GPIO_ACTION_DOWN);
    GPIO_SetInput(&gpio, GPIO_ACTION_RIGHT);
    GPIO_SetInput(&gpio, GPIO_ACTION_UP);
    GPIO_SetInput(&gpio, GPIO_ACTION_SELECT);
    GPIO_SetInput(&gpio, GPIO_ACTION_START);
    
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
        
        int action_left = !GPIO_Read(&gpio, GPIO_ACTION_LEFT);
        int action_down = !GPIO_Read(&gpio, GPIO_ACTION_DOWN);
        int action_right = !GPIO_Read(&gpio, GPIO_ACTION_RIGHT);
        int action_up = !GPIO_Read(&gpio, GPIO_ACTION_UP);
        int action_select = !GPIO_Read(&gpio, GPIO_ACTION_SELECT);
        int action_start = !GPIO_Read(&gpio, GPIO_ACTION_START);
        
        JS_SendButtonEvent(&js, JoystickButton_X, action_left);
        JS_SendButtonEvent(&js, JoystickButton_A, action_down);
        JS_SendButtonEvent(&js, JoystickButton_B, action_right);
        JS_SendButtonEvent(&js, JoystickButton_Y, action_up);
        JS_SendButtonEvent(&js, JoystickButton_Select, action_select);
        JS_SendButtonEvent(&js, JoystickButton_Start, action_start);
    }
    
    // ioctl(uinput, UI_DEV_DESTROY);
    // close(i2c);
}