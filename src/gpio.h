/* date = January 22nd 2022 0:23 pm */

#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>
#include <unistd.h>

#include <sys/mman.h> // memory management
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h> // open

// #define MEM_GPIO_ADDRESS 0x20000000
#define MEM_GPIO_ADDRESS 0x3F000000
#define GPIO_BASE_ADDRESS (MEM_GPIO_ADDRESS + 0x00200000)
#define GPIO_BLOCK_SIZE      4096

typedef struct GPIO_State
{
    int memory_fd;
    uint32_t *gpio;
} GPIO_State;

// manage GPIO state
GPIO_State GPIO_Create(void);
void GPIO_Destroy(GPIO_State *state);

// manage pin modes
void GPIO_SetInput(GPIO_State *state, int pin);
void GPIO_SetOutput(GPIO_State *state, int pin);

// read/write inputs
int GPIO_Read(GPIO_State *state, int pin);
unsigned int GPIO_Write(GPIO_State *state, int pin, int value);

#endif //GPIO_H
