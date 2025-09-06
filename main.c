//To select from memory use sw6
//To return from View log use sw2
//To return from rest use sw6


#include "main.h"

#pragma config WDTE = OFF

void init_config()
{
    init_i2c(100000);
    init_ds1307();
    init_adc();
    init_clcd();
    init_digital_keypad();
    init_timer2();
    init_uart(9600);
    PEIE = 1;
    GIE = 1;
}

unsigned char clock_reg[3];
char time[7];  
char log[11]; 
char log_pos = -1;
unsigned char sec;
unsigned char return_time = 5;  // Added initialization
unsigned char *menu[] = { "Log", "Clear log", "Download log", "Set time", "Change password"};
unsigned char menu_pos;
int count  = 0;

void main(void) {
    unsigned delay = 0;
    unsigned char cntrl_flag = DASHBOARD_SCREEN, key;
    unsigned char reset_flag, menu_pos;
    unsigned char event[3] = "ON";
    unsigned char speed = 0;
    unsigned char *gear[] = {"GN", "GR", "G1", "G2", "G3"};
    unsigned char gr = 0;
    init_config();
    log_event(event, speed);
    ext_eeprom_24C02_str_write(0x00, "1000");
    
    while(1)
    {
        speed = read_adc()/10.3;
        key = read_digital_keypad(STATE);
        
        if(key == SW1)
        {
            strcpy(event, "C0");
            log_event(event, speed);
        }
        else if(key == SW2 && gr<6)
        {
            strcpy(event, gear[gr]);
            gr++;
            log_event(event, speed);
        }
        else if(key == SW3 && gr>0)
        {
            gr--;
            strcpy(event, gear[gr]);
            log_event(event, speed);
        }
        else if((key == SW4 || key == SW5) && cntrl_flag == DASHBOARD_SCREEN)
        {
            delay++;
            if(delay > 0 && delay < 200)
            {
                cntrl_flag = LOGIN_SCREEN;
                clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                __delay_us(500);
                clcd_print("Enter password", LINE1(1));
               
                clcd_write(LINE2(4), INST_MODE);
               
                clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
                __delay_us(100);
                
                reset_flag = RESET_PASSWORD;
                TMR2ON = 1;
            }
        }
        else if(cntrl_flag == MAIN_MENU_SCREEN && key == SW6)
        {
            /*unsigned char press_type = check_long_press(key);
            if(press_type == 1 && key == SW4 && menu_pos > 0)
            {
                clear_screen();
                menu_pos--;
                return_time = 5;
            }
            else if(press_type == 1 && key == SW5 && menu_pos < 4)
            {
                clear_screen();
                menu_pos++;
                return_time = 5;
            }
            else if(press_type == 2 && key == SW4)
            {*/
                switch(menu_pos)
                {
                    case 0:
                        clear_screen();
                        clcd_print("# TIME     E  SP",LINE1(0));
                        cntrl_flag = VIEW_LOG_SCREEN;
                        reset_flag = VIEW_LOG_RESET;
                        break;
                        
                    case 1:
                        clear_screen();
                        reset_flag = RESET_MEMORY;
                        cntrl_flag = CLEAR_LOG_SCREEN;
                        break;
                        
                    case 2:
                        clear_screen();
                        cntrl_flag = DOWNLOAD_LOG_SCREEN;
                        break;
                    case 3:
                        clear_screen();
                        cntrl_flag = SET_TIME_SCREEN;
                        break;
                    case 4:
                        clear_screen();
                        cntrl_flag = CHANGE_PASSWORD_SCREEN;
                        reset_flag = RESET_PASSWORD;
                        break;
                }
            }
        
        switch(cntrl_flag)
        {
            case DASHBOARD_SCREEN:
                display_dashboard(event, speed);
                break;
             
            case LOGIN_SCREEN:
                switch(login(key,reset_flag))
                {
                    case RETURN_BACK:
                        clear_screen();
                        cntrl_flag = DASHBOARD_SCREEN;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        TMR2ON = 0;
                        break;
                    
                    case LOGIN_SUCCESS:
                        clear_screen();
                        cntrl_flag = MAIN_MENU_SCREEN;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        reset_flag = RESET_MENU;
                        continue;
                }
                break;
                
            case MAIN_MENU_SCREEN:
                menu_pos = menu_screen(key, reset_flag);
                switch(menu_pos)
                {
                    case RETURN_BACK:
                        clear_screen();
                        cntrl_flag = DASHBOARD_SCREEN;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        TMR2ON = 0;
                        break;
                }
                break;
                
            case VIEW_LOG_SCREEN:
                if(view_log(key, reset_flag) == SUCCESS)
                {
                    clear_screen();
                    cntrl_flag = MAIN_MENU_SCREEN;
                }
                break;
                
            case CLEAR_LOG_SCREEN:
                clear_log(reset_flag);
                clear_screen();
                cntrl_flag = MAIN_MENU_SCREEN;
                break;
                
            case CHANGE_PASSWORD_SCREEN:
                switch(change_password(key, reset_flag))
                {
                    case SUCCESS:
                            clear_screen();
                            clcd_write(DISP_ON_AND_CURSOR_OFF,INST_MODE);
                            __delay_us(100);
                            cntrl_flag=LOGIN_SCREEN;
                            reset_flag=RESET_PASSWORD;
                            continue;
                            break;
                }
                break;
                
            case SET_TIME_SCREEN:
                set_time(key);
                cntrl_flag = DASHBOARD_SCREEN;
                break;
                
            case DOWNLOAD_LOG_SCREEN:
                download_log();
                clear_screen();
                cntrl_flag = DASHBOARD_SCREEN;
                break;
        }
        reset_flag = RESET_NOTHING;
    }
}