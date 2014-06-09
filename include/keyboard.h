/*
 * Keyboard Driver
 */

char scancode_to_ascii(char scancode);

int is_ctrl_pressed();

void chance_caplock_state();

void set_ctrl_pressed(int state);

void set_shift_pressed(int state);