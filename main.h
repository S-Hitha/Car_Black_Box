#ifndef MAIN_H
#define MAIN_H

#include "adc.h"
#include "clcd.h"
#include "digital_keypad.h"
#include "ds1307.h"
#include "i2c.h"
#include "car_black_box_def.h"
#include <xc.h>
#include <string.h>
#include "EEprom.h"
#include "timers.h"
#include "uart.h"
#include <stdio.h>

#define LONG_PRESS_THRESHOLD        1000  
#define DASHBOARD_SCREEN            0x00
#define LOGIN_SCREEN                0x01
#define MAIN_MENU_SCREEN            0x02
#define RESET_PASSWORD              0x11
#define RESET_NOTHING               0x22
#define RETURN_BACK                 0x33
#define LOGIN_SUCCESS               0x44
#define RESET_MENU                  0x55
#define VIEW_LOG_SCREEN             0x66
#define CLEAR_LOG_SCREEN            0x77
#define DOWNLOAD_LOG_SCREEN         0x88
#define SET_TIME_SCREEN             0x99
#define CHANGE_PASSWORD_SCREEN      0xAA
#define VIEW_LOG_RESET              0xBB
#define RESET_MEMORY                0xCC
#define SUCCESS                     0x0A
#define FAILURE                     0x0B

#endif