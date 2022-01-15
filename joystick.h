/* date = January 15th 2022 1:12 pm */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>

#include <linux/input.h>
#include <linux/uinput.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef struct Joystick
{
    int uinput_handle;
} Joystick;

int CreateJoystickEx(short x_min, short x_max, short y_min, short y_max);
void SendJoystickEvent(int fd, uint16_t type, uint16_t code, int32_t value);
#endif //JOYSTICK_H
