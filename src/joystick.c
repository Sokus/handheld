#include "joystick.h"

Joystick JS_CreateJoystickEx(char *name,
                             int x_min, int x_max, int x_dzone,
                             int y_min, int y_max, int y_dzone)
{
    int uinput = CreateUInputDevice(name);
    
    Joystick js = {0};
    
    js.uinput = uinput;
    js.axis[0] = CreateJoystickAxis(x_min, x_max, x_dzone);
    js.axis[1] = CreateJoystickAxis(y_min, y_max, y_dzone);
    
    return js;
}

void JS_SendAxisEvent(Joystick *js, JoystickAxisType axis, int value)
{
    if(js != 0 && axis >= 0 && axis < JoystickAxis_Count)
    {
        JoystickAxis *js_axis = js->axis + axis;
        int value_processed = ProcessAxisValue(js_axis, value);
        uint16_t axis_code = GetAxisCode(axis);
        SendJoystickEvent(js->uinput, EV_ABS, axis_code, value_processed);
    }
}

void JS_SendButtonEvent(Joystick *js, JoystickButtonType button, int value)
{
    if(js != 0 && button >= 0 && button < JoystickButton_Count)
    {
        uint16_t button_code = GetButtonCode(button);
        SendJoystickEvent(js->uinput, EV_KEY, button_code, value);
    }
}

static int CreateUInputDevice(char *name)
{
    int fd = open("/dev/uinput", O_WRONLY | O_NDELAY);
    if(fd < 0)
    {
        fprintf(stderr, "ERROR: Couldn't open uinput!\n");
        return -1;
    }
    
    // device structure
    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    
    // init event
    int ret = 0;
    ret |= ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ret |= ioctl(fd, UI_SET_EVBIT, EV_REL);
    
    // bottons
    
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_A);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_B);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_X);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_Y);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_SELECT);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_START);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_LEFT);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_RIGHT);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_UP);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_DOWN);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_TL);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_TR);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_TL2);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_TR2);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_THUMBL);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_THUMBR);
    
    // axis
    ret |= ioctl(fd, UI_SET_EVBIT, EV_ABS);
    ret |= ioctl(fd, UI_SET_ABSBIT, ABS_X);
    uidev.absmin[ABS_X] = JS_ABS_MIN;
    uidev.absmax[ABS_X] = JS_ABS_MAX;
    
    ret |= ioctl(fd, UI_SET_ABSBIT, ABS_Y);
    uidev.absmin[ABS_Y] = JS_ABS_MIN;
    uidev.absmax[ABS_Y] = JS_ABS_MAX;
    
#if 0
    // TODO(sokus): Figure out what ABS_HAT is
    ret |= ioctl(fd, UI_SET_ABSBIT, ABS_HAT0X);
    uidev.absmin[ABS_HAT0X] = -1;
    uidev.absmax[ABS_HAT0X] = 1;
    
    ret |= ioctl(fd, UI_SET_ABSBIT, ABS_HAT0Y);
    uidev.absmin[ABS_HAT0Y] = -1;
    uidev.absmax[ABS_HAT0Y] = 1;
#endif
    
    if(ret)
    {
        fprintf(stderr, "ERROR: Couldn't configure uinput device!\n");
        close(fd);
        return -1;
    }
    
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, name);
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 1;
    uidev.id.product = 1;
    uidev.id.version = 1;
    
    ret = write(fd, &uidev, sizeof(uidev));
    if(ioctl(fd, UI_DEV_CREATE))
    {
        fprintf(stderr, "ERROR: Couldn't configure uinput device!\n");
        close(fd);
        return -1;
    }
    
    return fd;
}

static JoystickAxis CreateJoystickAxis(int min, int max, int dzone)
{
    int mid = (min + max) / 2;
    int neg_dist = min - mid;
    int pos_dist = max - mid;
    
    JoystickAxis result;
    result.neg_f = min;
    result.neg_c = mid + JS_SIGN(neg_dist) * dzone;
    result.pos_f = max;
    result.pos_c = mid + JS_SIGN(pos_dist) * dzone;
    
    return result;
}

static int ProcessAxisValue(JoystickAxis *axis, int value)
{
    int result = 0;
    
    if(!JS_BETWEEN(axis->neg_c, value, axis->pos_c))
    {
        int mid = (axis->neg_c + axis->pos_c) / 2;
        int dir = JS_SIGN(value - mid) * JS_SIGN(axis->pos_c - mid);
        
        if(dir < 0)
        {
            int src_amp = JS_ABS(axis->neg_f - axis->neg_c);
            int dst_amp = JS_ABS(JS_ABS_MIN);
            int value_abs = JS_ABS(value - axis->neg_c);
            
            float ratio = (float)dst_amp / (float)src_amp;
            float value_scaled = (float)value_abs * ratio;
            result = (int)(-value_scaled);
        }
        else
        {
            int src_amp = JS_ABS(axis->pos_f - axis->pos_c);
            int dst_amp = JS_ABS(JS_ABS_MAX);
            int value_abs = JS_ABS(value - axis->pos_c);
            
            float ratio = (float)dst_amp / (float)src_amp;
            float value_scaled = (float)value_abs * ratio;
            result = (int)value_scaled;
        }
    }
    
    return result;
}

static uint16_t GetAxisCode(JoystickAxisType axis)
{
    switch(axis)
    {
        case JoystickAxis_X: return ABS_X;
        case JoystickAxis_Y: return ABS_Y;
        default:             return ABS_X;
    }
}

static uint16_t GetButtonCode(JoystickButtonType button)
{
    switch(button)
    {
        case JoystickButton_DPadLeft:     return BTN_DPAD_LEFT;
        case JoystickButton_DPadRight:    return BTN_DPAD_RIGHT;
        case JoystickButton_DPadUp:       return BTN_DPAD_UP;
        case JoystickButton_DPadDown:     return BTN_DPAD_DOWN;
        case JoystickButton_A:            return BTN_A;
        case JoystickButton_B:            return BTN_B;
        case JoystickButton_X:            return BTN_X;
        case JoystickButton_Y:            return BTN_Y;
        case JoystickButton_Select:       return BTN_SELECT;
        case JoystickButton_Start:        return BTN_START;
        case JoystickButton_BumperLeft:   return BTN_TL;
        case JoystickButton_BumperRight:  return BTN_TR;
        case JoystickButton_TriggerLeft:  return BTN_TL2;
        case JoystickButton_TriggerRight: return BTN_TR2;
        case JoystickButton_ThumbLeft:    return BTN_THUMBL;
        case JoystickButton_ThumbRight:   return BTN_THUMBR;
        default:                          return BTN_A;
    }
}

static void SendJoystickEvent(int fd, uint16_t type, uint16_t code, int value)
{
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.code = code;
    ev.value = value;
    
    if(write(fd, &ev, sizeof(ev)) < 0)
        fprintf(stderr, "ERROR: Couldn't send joystick event to uinput device!\n");
    
    // also send a sync event
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if(write(fd, &ev, sizeof(ev)) < 0)
        fprintf(stderr, "ERROR: Couldn't send sync event to uinput device!\n");
}
