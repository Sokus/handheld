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

// x: 27 - 13445 - 26528
// y: 25 - 13350 - 26527

typedef enum JoystickAxisType
{
    JoystickAxis_X,
    JoystickAxis_Y,
    
    JoystickAxis_Count
} JoystickAxisType;

typedef enum JoystickButtonType
{
    JoystickButton_A,
    JoystickButton_B,
    JoystickButton_X,
    JoystickButton_Y,
    JoystickButton_Select,
    JoystickButton_Start,
    JoystickButton_DPadLeft,
    JoystickButton_DPadRight,
    JoystickButton_DPadUp,
    JoystickButton_DPadDown,
    JoystickButton_BumperLeft,
    JoystickButton_BumperRight,
    JoystickButton_TriggerLeft,
    JoystickButton_TriggerRight,
    JoystickButton_ThumbLeft,
    JoystickButton_ThumbRight,
    
    JoystickButton_Count
} JoystickButtonType;

typedef struct JoystickAxis
{
    int neg_f;
    int neg_c;
    
    int pos_f;
    int pos_c;
} JoystickAxis;

typedef struct Joystick
{
    int uinput;
    JoystickAxis axis[JoystickAxis_Count];
} Joystick;

#define JS_ABS(x) ((x) >= 0 ? (x) : -(x))
#define JS_SIGN(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))
#define JS_BETWEEN(a, x, b) (((a) <= (x) && (x) <= (b)) || ((b) <= (x) && (x) <= (a)))
#define JS_ABS_MAX 32767
#define JS_ABS_MIN -32768

// API
Joystick JS_CreateJoystickEx(char *name, int x_min, int x_max, int x_dzone, int y_min, int y_max, int y_dzone);
void JS_SendAxisEvent(Joystick *js, JoystickAxisType axis, int value);
void JS_SendButtonEvent(Joystick *js, JoystickButtonType button, int value);

// Internal
static int CreateUInputDevice(char *name);
static JoystickAxis CreateJoystickAxis(int min, int max, int dzone);

static int ProcessAxisValue(JoystickAxis *axis, int value);

static uint16_t GetAxisCode(JoystickAxisType axis);
static uint16_t GetButtonCode(JoystickButtonType button);

static void SendJoystickEvent(int fd, uint16_t type, uint16_t code, int value);

#endif //JOYSTICK_H
