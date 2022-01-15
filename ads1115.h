/* date = January 15th 2022 5:38 pm */

#ifndef ADS1115_H
#define ADS1115_H

#include <linux/i2c-dev.h> // I2C bus definitions
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef struct ADS1115_State
{
    int is_valid;
    int fd;
    int address;
} ADS1115_State;

ADS1115_State ADS1115_Init(int fd, int address);
int ADS1115_SetActiveInput(ADS1115_State *state, int ain);
short ADS1115_ReadValue(ADS1115_State *state);
float ADS1115_ReadVoltage(ADS1115_State *state);

#endif //ADS1115_H
