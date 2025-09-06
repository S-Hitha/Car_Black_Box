#include "car_black_box_def.h"
#include "main.h"

unsigned char clock_reg[3];
char time[7];  
char log[11]; 
char log_pos = -1;
unsigned char sec;
unsigned char return_time;
unsigned char *menu[] = { "View Log", "Clear log", "Download log", "Set time", "Change password"};
unsigned char menu_pos;
int count  = 0;

static void get_time()
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); 
    clock_reg[1] = read_ds1307(MIN_ADDR); 
    clock_reg[2] = read_ds1307(SEC_ADDR); 
    
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
}

void display_time()
{
    get_time();
    clcd_putch(time[0], LINE2(2));
    clcd_putch(time[1], LINE2(3));
    clcd_putch(':', LINE2(4));
    clcd_putch(time[2], LINE2(5));
    clcd_putch(time[3], LINE2(6));
    clcd_putch(':', LINE2(7));
    clcd_putch(time[4], LINE2(8));
    clcd_putch(time[5], LINE2(9));
}

void display_dashboard(unsigned char event[], unsigned char speed)
{
    clcd_print("TIME     E  SP", LINE1(2));
    display_time();
    
    clcd_print(event,LINE2(11));
    
    clcd_putch(speed/10 + '0', LINE2(14));
    clcd_putch(speed%10 + '0', LINE2(15));
}

void store_event()
{
    char addr;
    log_pos++;
    
    if(log_pos == 10)
        log_pos = 0;
    
    addr = 0x05 + log_pos*10;
    
    for(int i=0; log[i] != '\0'; i++)
    {
        ext_eeprom_24C02_str_write(addr, &log[i]);
        addr++;
    }
    
    if(count<9)
        count++;
}

void log_event(unsigned char event[], unsigned char speed)
{
    get_time();
    strncpy(log, time, 6);
    strncpy(&log[6], event, 2);
    
    log[8] = speed / 10 + '0';
    log[9] = speed % 10 + '0';
    
    log[10] = '\0';
    
    store_event();
}

unsigned char login(unsigned char key, unsigned char reset_flag)
{
    static char user_password[4];
    static unsigned char i;
    static unsigned char attempts_left;
    unsigned char key_2;
    
    key_2 = read_digital_keypad(STATE);
    
    if(reset_flag == RESET_PASSWORD)
    {
        i=0;
        attempts_left = 3;
        user_password[0]='\0';
        user_password[1]='\0';
        user_password[2]='\0';
        user_password[3]='\0';
        key_2 = ALL_RELEASED;
        return_time = 5;
    }
    
    if(return_time == 0)
    {
        return RETURN_BACK;
    }
    
    if(key_2 == SW4 && i<4)
    {
        clcd_putch('*',LINE2(4+i));
        user_password[i] = '1';
        i++;
        return_time = 5;
    }
    else if(key_2 == SW5 && i<4)
    {
        clcd_putch('*',LINE2(4+i));
        user_password[i] = '0';
        i++;
        return_time = 5;
    }
    
    if(i == 4)
    {
        char s_password[4];
        for(int j=0;j<4;j++)
        {
            s_password[j] = ext_eeprom_24C02_read(j);
        }
        if(strncmp(s_password, user_password, 4) == 0)
        {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            clcd_print("Login Success",LINE1(1));
            __delay_ms(3000);
            return LOGIN_SUCCESS;
        }
        else
        {
            attempts_left--;
            if(attempts_left == 0)
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("You are locked",LINE1(0));
                clcd_print("Wait for",LINE2(0));
                sec = 60;
                
                while(sec)
                {
                    clcd_putch(sec/10 + '0', LINE2(9));
                    clcd_putch(sec%10 + '0', LINE2(10));
                }
                
                clcd_print("sec", LINE2(12));
                attempts_left = 3;
                clear_screen();
            }
            else
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("Wrong Password",LINE1(0));
                clcd_print("Attempts left", LINE2(2));
                
                clcd_putch(attempts_left + '0', LINE2(0));
                __delay_ms(3000);
            }
            
            clear_screen();
            clcd_print("Enter password:", LINE1(1));
           
           clcd_write(LINE2(4), INST_MODE);
           
           clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            
            i=0;
        }
    }
}

unsigned char menu_screen(unsigned char key, unsigned char reset_flag)
{   
    if(reset_flag == RESET_MENU)
    {
        menu_pos = 0;
        return_time = 5;
    }
    
    if(return_time == 0)
    {
        return RETURN_BACK;
    }
    
    if(key == SW4 && menu_pos > 0)
    {
        clear_screen();
        menu_pos--;
        return_time = 5;
    }
    else if(key == SW5 && menu_pos < 4)
    {
        clear_screen();
        menu_pos++;
        return_time = 5;
    }
    
    if(menu_pos == 4)
    {
        clcd_print("*", LINE2(1));
        clcd_print(menu[menu_pos - 1],LINE1(2));
        clcd_print(menu[menu_pos],LINE2(2));
    }
    else
    {
        clcd_print("*", LINE1(1));
        clcd_print(menu[menu_pos],LINE1(2));
        clcd_print(menu[menu_pos + 1],LINE2(2));
    }
    
    return menu_pos;
}

unsigned char view_log(unsigned char key,unsigned char reset_flag)
{
    char v_log[11];
    unsigned char add;
    static unsigned char pos;
    if(count ==-1)
    {
        clcd_print(" LOG EMPTY",LINE2(0));
    }
    else
    {
        if (reset_flag == VIEW_LOG_RESET)
        {
            pos=0;
        }
        if(key==SW5 && pos< (count-1))
        {
            pos++;
        }
        else if(key==SW4 && pos > 0)
        {
            pos--;
        }
        else if(key == SW2)
        {
            return SUCCESS;
        }
        
        for(int i=0;i<10;i++)
        {
            add=pos*10+5;
            v_log[i]=ext_eeprom_24C02_read(add + i);
        }

        clcd_putch(pos%10 + '0',LINE2(0));

        clcd_putch(v_log[0],LINE2(2));
        clcd_putch(v_log[1],LINE2(3));
        clcd_putch(':',LINE2(4));

        clcd_putch(v_log[2],LINE2(5));
        clcd_putch(v_log[3],LINE2(6));
        clcd_putch(':',LINE2(7));

        clcd_putch(v_log[4],LINE2(8));
        clcd_putch(v_log[5],LINE2(9));
        
        clcd_putch(v_log[6],LINE2(11));
        clcd_putch(v_log[7],LINE2(12));

        clcd_putch(v_log[8],LINE2(14));
        clcd_putch(v_log[9],LINE2(15));   
    }
}

char clear_log(unsigned char reset_flag)
{
    if(reset_flag == RESET_MEMORY)
    {
        count = -1;
        log_pos = -1;
        clcd_print("LOGS CLEARED",LINE1(0));
        clcd_print("SUCCESSFULLY",LINE2(0));
        __delay_ms(3000);
        return SUCCESS;
    }
    return FAILURE;
}

char change_password(unsigned char key, unsigned char reset_flag)
{
    static char password[9];
    static int pos, temp;
    
    if(reset_flag == RESET_PASSWORD)
    {
        pos = 0;
        temp = 1;
    }
    
    if(pos<4 && temp)
    {
        temp=0;
        clcd_print("Enter New Password",LINE1(0));
        clcd_write(LINE2(0), INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON,INST_MODE);
    }
    
    else if(pos>=4 && (temp==0))
    {
        temp=1;
        clear_screen();
        clcd_print("Re-enter Password",LINE1(0));
        clcd_write(LINE2(0), INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON,INST_MODE);
    }
    
    if(key==SW4)
    {
        password[pos]='1';
        clcd_putch('*',LINE2(pos%4));
        pos++;
    }
    
    else if(key==SW5)
    {
        password[pos]='0';
        clcd_putch('*',LINE2(pos%4));
        pos++;
    }
    
    if(pos==8)
    {
        if(strncmp(password,&password[4],4)==0)
        {
            
            for(int i=0;i<4;i++)
            {
                ext_eeprom_24C02_byte_write(i,password[i]);
            }
            clear_screen();
            clcd_print(" Password Changed",LINE1(0));
            clcd_print(" Successfully ",LINE2(0));
            __delay_ms(3000);
            return SUCCESS;
        }
        else
        {
            clear_screen();
            clcd_print(" Password Change",LINE1(0));
            clcd_print(" Failed ",LINE2(0));
            __delay_ms(3000);
            return SUCCESS;
        }
        
    }
    return 0x10;
}

void set_time(unsigned char key) {
    unsigned char new_time[3] = {0}; 
    unsigned char pos = 0; 
    unsigned char blink = 1; 
    unsigned char update_flag = 0; 
 
    get_time();
    new_time[0] = ((time[0] - '0') * 10) + (time[1] - '0'); 
    new_time[1] = ((time[2] - '0') * 10) + (time[3] - '0'); 
    new_time[2] = ((time[4] - '0') * 10) + (time[5] - '0'); 
 
    clcd_print("SET TIME", LINE1(0));
 
    while(1)
    {
        if(blink)
        {
            clcd_putch(' ', (pos == 0) ? LINE2(2) : (pos == 1) ? LINE2(5) : LINE2(8));
            clcd_putch('_', (pos == 0) ? LINE2(3) : (pos == 1) ? LINE2(6) : LINE2(9));
        }
        else
        {
            clcd_putch((new_time[0] / 10) + '0', LINE2(2)); 
            clcd_putch((new_time[0] % 10) + '0', LINE2(3));
            clcd_putch(':', LINE2(4));
 
            clcd_putch((new_time[1] / 10) + '0', LINE2(5)); 
            clcd_putch((new_time[1] % 10) + '0', LINE2(6)); 
            clcd_putch(':', LINE2(7));
 
            clcd_putch((new_time[2] / 10) + '0', LINE2(8)); 
            clcd_putch((new_time[2] % 10) + '0', LINE2(9)); 
        }
 
        blink = !blink;
        __delay_ms(300); 
 
        key = read_digital_keypad(STATE); 
 
        if(key == SW4) 
        {
            pos = (pos + 1) % 3;
        }
        else if(key == SW5) 
        {
            if(pos == 0)             
                new_time[0] = (new_time[0] + 1) % 24;
            
            else if(pos == 1)             
                new_time[1] = (new_time[1] + 1) % 60;
            
            else if(pos == 2)             
                new_time[2] = (new_time[2] + 1) % 60;            
        }
        else if(key == SW6) 
        {
            write_ds1307(HOUR_ADDR, ((new_time[0] / 10) << 4) | (new_time[0] % 10));
            write_ds1307(MIN_ADDR, ((new_time[1] / 10) << 4) | (new_time[1] % 10));
            write_ds1307(SEC_ADDR, ((new_time[2] / 10) << 4) | (new_time[2] % 10));
        
            clear_screen();
            clcd_print("Updated ", LINE1(0));
            clcd_print("Successfully", LINE2(0));
            __delay_ms(2000);
        
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            clear_screen();
        
            break;
        }
    } 
    return;
}

void download_log(void)
{
    char log_data[11]; 
    unsigned char addr; 
    unsigned char i;

    clear_screen();

    clcd_print("DOWNLOADING", LINE1(0));

    puts("      LOG:        \n\r");
    uart_puts("TIME     E  SP\n\r");

 for (i = 0; i <= count; i++)
    {
        addr = 0x05 + i * 10;
        ext_eeprom_24C02_str_read(addr, log_data, 10);

        log_data[10] = '\0';

        clcd_print(log_data, LINE2(0));
    
        char time[7], event[3], speed[3];
        
        strncpy(time, log_data, 6);
        time[6] = '\0'; 

        strncpy(event, &log_data[6], 2);
        event[2] = '\0'; 

        strncpy(speed, &log_data[8], 2);
        speed[2] = '\0'; 

        uart_puts(time);  
        uart_puts("  "); 
        uart_puts(event); 
        uart_puts("  ");  
        uart_puts(speed); 
        uart_puts("\n\r"); 

        __delay_ms(500);
    }

    clcd_print("DOWNLOAD DONE", LINE1(0));

    uart_puts("     DOWNLOAD COMPLETE       \n\r");
    __delay_ms(2000);
    
    return;
}

void clear_screen()
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}

unsigned char check_long_press(unsigned char key)
{
    static unsigned int press_time = 0;
    static unsigned char prev_key = ALL_RELEASED;
    static unsigned char long_press_detected = 0;
    
    if (key != ALL_RELEASED)
    {
        if (key == prev_key)
        {
            press_time++;
            
            if (press_time >= LONG_PRESS_THRESHOLD && !long_press_detected)
            {
                long_press_detected = 1;
                return 2;
            }
        }
        else
        {
            press_time = 0;
            long_press_detected = 0;
        }
        
        prev_key = key;
        return 1;
    }
    else
    {
        if (prev_key != ALL_RELEASED)
        {
            prev_key = ALL_RELEASED;
            
            if (long_press_detected)
            {
                long_press_detected = 0;
                return 0;
            }
            else if (press_time > 0)
            {
                unsigned char was_short_press = (press_time < LONG_PRESS_THRESHOLD);
                press_time = 0;
                return was_short_press ? 1 : 2;
            }
        }
    }
    
    return 0;
}