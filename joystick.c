#include "joystick.h"

int CreateJoystickEx(short x_min, short x_max, short y_min, short y_max)
{
    int fd;
    
    fd = open("/dev/uinput", O_WRONLY | O_NDELAY);
    if(fd < 0)
    {
        fprintf(stderr, "Can't open uinput device!\n");
        exit(1);
    }
    
    // device structure
    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    
    // init event
    int ret = 0;
    ret |= ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ret |= ioctl(fd, UI_SET_EVBIT, EV_REL);
    
    // botton
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_A);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_B);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_X);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_Y);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_TL);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_TR);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_SELECT);
    ret |= ioctl(fd, UI_SET_KEYBIT, BTN_START);
    
    // axis
    ret |= ioctl(fd, UI_SET_EVBIT, EV_ABS);
    ret |= ioctl(fd, UI_SET_ABSBIT, ABS_X);
    uidev.absmin[ABS_X] = x_min;
    uidev.absmax[ABS_X] = x_max;
    
    ret |= ioctl(fd, UI_SET_ABSBIT, ABS_Y);
    uidev.absmin[ABS_Y] = y_min;
    uidev.absmax[ABS_Y] = y_max;
    
    ret |= ioctl(fd, UI_SET_ABSBIT, ABS_HAT0X);
    uidev.absmin[ABS_HAT0X] = -1;
    uidev.absmax[ABS_HAT0X] = 1;
    
    ret |= ioctl(fd, UI_SET_ABSBIT, ABS_HAT0Y);
    uidev.absmin[ABS_HAT0Y] = -1;
    uidev.absmax[ABS_HAT0Y] = 1;
    
    if(ret)
    {
        fprintf(stderr, "Error while configuring uinput device!\n");
        exit(1);
    }
    
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "I2C Custom Joystick");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 1;
    uidev.id.product = 1;
    uidev.id.version = 1;
    
    ret = write(fd, &uidev, sizeof(uidev));
    if(ioctl(fd, UI_DEV_CREATE))
    {
        fprintf(stderr, "Error while creating uinput device!\n");
        exit(1);
    }
    
    return fd;
}

void SendJoystickEvent(int fd, uint16_t type, uint16_t code, int32_t value)
{
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.code = code;
    ev.value = value;
    
    if(write(fd, &ev, sizeof(ev)) < 0)
        fprintf(stderr, "Error while sending event to uinput device!\n");
    
    // also send a sync event
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if(write(fd, &ev, sizeof(ev)) < 0)
        fprintf(stderr, "Error while sending sync event to uinput device!\n");
}
