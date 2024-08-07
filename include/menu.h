#ifndef MENU_H
#define MENU_H

int menu_initialise(void);
void check_button_event();
void draw_main(struct menu_state *state);
void to_right(struct menu_state *state);
void to_left(struct menu_state *state);
uint8_t towards_int16(int16_t *current, int16_t dest);
uint8_t towards(struct menu_state *current, struct menu_state *destination);
void run_menu(void);
void navigate_menu(void);
void run_submenu(void);
void navigate_submenu(void);
void O2_calibrate_run_display(void);
void dist_calibrate_run_display(void);
void splash_screen(void);
void load_calibration_values(void);
void submenu_draw(void);
void submenu_cases(void);
int mui_active(void);
void formatdate(char const *date, char *buff);

#endif