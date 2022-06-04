#include "gpio.h"

GPIO_State GPIO_Create(void)
{
    GPIO_State result = {0};
    
    int memory_fd = open("/dev/mem", O_RDWR|O_SYNC);
    if(memory_fd < 0)
    {
        fprintf(stderr, "ERROR: Couldn't open /dev/mem, try checking permissions.\n");
        return result;
    }
    
    void *gpio = mmap(0, GPIO_BLOCK_SIZE, PROT_READ | PROT_WRITE,
                      MAP_SHARED, memory_fd, GPIO_BASE_ADDRESS);
    
    if(gpio == MAP_FAILED)
    {
        fprintf(stderr, "ERROR: GPIO memory map.\n");
        return result;;
    }
    
    result.memory_fd = memory_fd;
    result.gpio = (uint32_t *)gpio;
    
    return result;
}

void GPIO_Rpi0_Destroy(GPIO_State *state)
{
    munmap(state->gpio, GPIO_BLOCK_SIZE);
    close(state->memory_fd);
    
    state->gpio = 0;
    state->memory_fd = 0;
}

void GPIO_SetInput(GPIO_State *state, int pin)
{
    uint32_t *pin_ptr = state->gpio + (pin / 10);
    *pin_ptr &= ~(7 << ((pin % 10) * 3));
}

void GPIO_SetOutput(GPIO_State *state, int pin)
{
    uint32_t *pin_ptr = state->gpio + (pin / 10);
    *pin_ptr |= (1 << ((pin % 10) * 3));
}

int GPIO_Read(GPIO_State *state, int pin)
{
    int bit_value = (*state->gpio + 13) & (1 << pin);
    int result = *(state->gpio + 13) & (1 << pin);
}

unsigned int GPIO_Write(GPIO_State *state, int pin, int value)
{
    if(value != 0)
        *(state->gpio + 7) = (1 << pin);
    else
        *(state->gpio + 10) = (1 << pin);
}
