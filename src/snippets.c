#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/input.h>
#include <linux/uinput.h>

#define MEM_GPIO_ADDRESS 0x3F000000
#define GPIO_BASE_ADDRESS (MEM_GPIO_ADDRESS + 0x00200000)
#define GPIO_BLOCK_SIZE 4096

typedef struct GPIO_State {
    int memory_fd;
    uint32_t *gpio;
} GPIO_State;

GPIO_State GPIO_Create(void) {
    GPIO_State result = {0};
    result.memory_fd = open("/dev/mem", O_RDWR|O_SYNC);
    result.gpio = (uint32_t *)mmap(0, GPIO_BLOCK_SIZE,
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED, memory_fd,
                                   GPIO_BASE_ADDRESS);
    return result;
}

void GPIO_SetInput(GPIO_State *state, int pin) {
    uint32_t *pin_ptr = state->gpio + (pin / 10);
    *pin_ptr &= ~(7 << ((pin % 10) * 3));
}

int GPIO_Read(GPIO_State *state, int pin) {
    int result = *(state->gpio + 13) & (1 << pin);
    return result;
}

// ==============================================================

typedef struct ADS1115_State {
    int is_valid;
    int fd;
    int address;
} ADS1115_State;

ADS1115_State ADS1115_Init(int fd, int address) {
    ioctl(fd, I2C_SLAVE, address);
    unsigned char write_buf[3];
    // ...
    write_buf[0] = 2; // high treshold
    write_buf[1] = 0x2E;
    write_buf[2] = 0xEE;
    write(fd, write_buf, 3);
    // ...
    return result;
}

void ADS1115_SetActiveInput(ADS1115_State *state) {
    unsigned char write_buf[3];
    write_buf[0] = 1;
    write_buf[1] = 0xC2
    write_buf[2] = 0x87;
    write(state->fd, write_buf, 3);
    
    write_buf[0] = 0;
    write(state->fd, write_buf, 1);
}

short ADS1115_ReadValue(ADS1115_State *state) {
    // ...
    unsigned char read_buffer[2] = { 0, 0 };
    read(state->fd, read_buffer, 2);
    short result = (read_buffer[0] << 8 | read_buffer[1]);
    return result;
}

// ==============================================================

typedef struct JoystickAxis {
    int neg_f; int neg_c;
    int pos_f; int pos_c;
} JoystickAxis;

typedef struct Joystick {
    int uinput;
    JoystickAxis axis[JoystickAxis_Count];
} Joystick;

static int CreateUInputDevice(char *name) {
    int fd = open("/dev/uinput", O_WRONLY | O_NDELAY);
    // ...
    ioctl(fd, UI_SET_EVBIT, EV_KEY); // init event
    ioctl(fd, UI_SET_EVBIT, EV_REL); // init event
    ioctl(fd, UI_SET_KEYBIT, BTN_A); // button
    ioctl(fd, UI_SET_EVBIT, EV_ABS); // axis
    ioctl(fd, UI_SET_ABSBIT, ABS_X); // axis
    
    struct uinput_user_dev uidev = {0};
    uidev.absmin[ABS_X] = JS_ABS_MIN;
    uidev.absmax[ABS_X] = JS_ABS_MAX;
    
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, name);
    uidev.id.bustype = BUS_USB;
    // ... set vendor, product and version
    write(fd, &uidev, sizeof(uidev));
    // ...
    return fd;
}

static void SendJoystickEvent(int fd, uint16_t type,
                              uint16_t code, int value) {
    struct input_event ev = {0};
    ev.type = type;
    ev.code = code;
    ev.value = value;
    write(fd, &ev, sizeof(ev));
    // ...
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    write(fd, &ev, sizeof(ev));
    // ...
}

// ==============================================================

#define GPIO_ACTION_LEFT   21
#define UPDATE_DELAY 50000 // ms (200Hz)

int main(void) {
    int i2c = open("/dev/i2c-1", O_RDWR);
    ADS1115_State ads = ADS1115_Init(i2c, 0x48);
    GPIO_State gpio = GPIO_Create();
    Joystick js = JS_CreateJoystickEx("Custom Joystick",
                                      27, 26528, 500,
                                      25, 26527, 500);
    GPIO_SetInput(&gpio, GPIO_ACTION_LEFT);
    // ...
    while(1) {
        ADS1115_SetActiveInput(&ads, 0);
        short v0 = ADS1115_ReadValue(&ads);
        JS_SendAxisEvent(&js, JoystickAxis_X, v0);
        // ...
        int action_left = !GPIO_Read(&gpio, GPIO_ACTION_LEFT);
        JS_SendButtonEvent(&js, JoystickButton_X, action_left);
        // ...
        usleep(UPDATE_DELAY);
    }
}
