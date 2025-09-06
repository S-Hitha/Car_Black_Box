#include "main.h"

unsigned char ext_eeprom_24C02_read(unsigned char addr)
{
    unsigned char data;
    i2c_start();
    i2c_write(SLAVE_WRITE_EEPROM);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(SLAVE_READ_EEPROM);
    data = i2c_read(0);
    
    i2c_stop();
    
    return data;
}

void ext_eeprom_24C02_byte_write(unsigned char addr, char data)
{
    i2c_start();
    int ack = i2c_write(SLAVE_WRITE_EEPROM);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
}

void ext_eeprom_24C02_str_write(unsigned char addr, char *data)
{
    while(*data != 0)
    {
        ext_eeprom_24C02_byte_write(addr, *data);
        data++;
        addr++;
    }
}

void ext_eeprom_24C02_str_read(unsigned char addr, char *data, unsigned char len)
{
    for (unsigned char i = 0; i < len; i++)
    {
        data[i] = ext_eeprom_24C02_read(addr + i);
    }
    data[len] = '\0'; // Null terminate the string
}