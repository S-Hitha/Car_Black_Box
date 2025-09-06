#ifndef CAR_BLACK_BOX_DEF_H
#define CAR_BLACK_BOX_DEF_H

void display_dashboard(unsigned char event[], unsigned char speed);
void log_event(unsigned char event[], unsigned char speed);
//void store_event();
unsigned char login(unsigned char key, unsigned char reset_flag);
unsigned char menu_screen(unsigned char key, unsigned char reset_flag);
void clear_screen();
unsigned char view_log(unsigned char key, unsigned char reset_flag);
void display_log_time(unsigned char log[]);
char clear_log(unsigned char reset_flag);
char change_password(unsigned char key, unsigned char reset_flag);
void set_time(unsigned char key);
void download_log(void);
unsigned char check_long_press(unsigned char key);
#endif