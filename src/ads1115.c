#include "ads1115.h"

ADS1115_State ADS1115_Init(int fd, int address)
{
    ioctl(fd, I2C_SLAVE, address);
    
    unsigned char write_buf[3];
    
    // high threshold
    write_buf[0] = 2;
    write_buf[1] = 0x2E;
    write_buf[2] = 0xEE;
    write(fd, write_buf, 3);
    
    // low threshold
    write_buf[0] = 3;
    write_buf[1] = 0x80;
    write_buf[2] = 0x00;
    write(fd, write_buf, 3);
    
    ADS1115_State result = {0};
    result.is_valid = 1;
    result.fd = fd;
    result.address = address;
    
    return result;
}

int ADS1115_SetActiveInput(ADS1115_State *state, int ain)
{
    if(ain < 0 || ain > 3)
        return -1;
    
    unsigned char write_buf[3];
    write_buf[0] = 1;
    write_buf[1] = (ain == 0 ? 0xC2 :
                    ain == 1 ? 0xD2 :
                    ain == 2 ? 0xE2 :
                    ain == 3 ? 0xF2 : 0xC2);
    // write_buf[2] = 0x07; // 0000 0111
    write_buf[2] = 0x87; // 1000 0111
    
    write(state->fd, write_buf, 3);
    
    write_buf[0] = 0;
    write(state->fd, write_buf, 1);
    
    return 0;
}

short ADS1115_ReadValue(ADS1115_State *state)
{
    if(state == 0 || state->is_valid == 0)
        return 0;
    
    unsigned char read_buffer[2] = { 0, 0 };
    read(state->fd, read_buffer, 2);
    short result = (read_buffer[0] << 8 | read_buffer[1]);
    return result;
}

float ADS1115_ReadVoltage(ADS1115_State *state)
{
    short value = ADS1115_ReadValue(state);
    float result = value*4.096f/32767.0f;
    return result;
}
