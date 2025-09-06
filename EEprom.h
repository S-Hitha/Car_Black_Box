#ifndef EEPROM_H
#define EEPROM_H

#define SLAVE_WRITE_EEPROM      0xA0
#define SLAVE_READ_EEPROM       0xA1


unsigned char ext_eeprom_24C02_read(unsigned char addr);
void ext_eeprom_24C02_byte_write(unsigned char addr, char data);
void ext_eeprom_24C02_str_write(unsigned char addr, char *data);
void ext_eeprom_24C02_str_read(unsigned char addr, char *data, unsigned char len);


#endif