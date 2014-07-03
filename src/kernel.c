#include "../include/kasm.h"
#include "../include/kc.h"
#include "../include/defs.h"
#include "../include/kernel.h"
#include "../include/video.h"
#include "../include/file_descriptor.h"
#include "../include/circular_buffer.h"
#include "../include/keyboard.h"

DESCR_INT idt[0x77];			/* IDT de 0x77 entradas*/
IDTR idtr;				/* IDTR */

CircularBuffer keyboard_buffer = {0};

byte color[6] = {0x17, 0x27, 0x37, 0x47, 0x57, 0x67};
unsigned long int color_time = 30;
byte current_color;
byte prev_color;
int prev_hour, prev_minute, prev_seconds;
int current_hour, current_minute, current_seconds;
int enable_ss = false;
unsigned int ss_time = 10;
unsigned int countdown = 10;
byte mouse_x, mouse_y;
byte left_button = 0; 
byte right_button = 0;
byte tmp, mouse;
int times = 0;

void rtc_handler(){
  _rtc_hand_read();
}

/*
 * Save time. It uses when the savescreen time is set.
 * Then, this previous time is compared with the current time.
 */
void rtc_handler_save_prev_time(){
 _rtc_hand_save_prev_time();
}

void rtc_handler_save_current_time(){
 _rtc_hand_save_current_time();
}

int rtc_hand_write(byte hour, byte minutes, byte seconds){
  _rtc_hand_write(hour, 0x04);
  _rtc_hand_write(minutes, 0x02);
  _rtc_hand_write(seconds, 0x00);
  return true;
}

size_t __read(int fd, void* buffer, int count){
  if (cbIsEmpty(&keyboard_buffer))
  {
    return 0; // 0 elements found.
  } else {
    cbRead(&keyboard_buffer, buffer); // ToDo: implement iteration for count > 0
    return 1; // should return how many elements it has read.
  }
}

int _write_new_line(int fd){
  if(fd == STDOUT)
    _video_new_line();
  return 1;
}

int _write_delete_char(int fd){
  if(fd == STDOUT)
    video_erase_write();
  return 1;
}

size_t __write(int fd, const char *buf, int n){
	int i;
	if(fd == STDOUT){
		for( i = 0; i < n; i++){
           video_write(buf[i]);          
 		}
	}
  return i;
}

void enable_savescreen(){
  prev_color = -1;
  current_color = 0;
  enable_ss = true;
}

void disable_savescreen(){
  enable_ss = false;
}

int ssIsEnabled(){
  return enable_ss;
}

void set_time_savescreen(int value){
  ss_time = value;
  countdown = ss_time;
  rtc_handler_save_prev_time();
}

/*
 * Run sacescreen. It paints the whole screen with a color every 'color_time'
 */
void run_savescreen(){
  if(prev_color != current_color){
    prev_color = current_color;
   paint_screen(color[current_color]);
  }

  color_time--;
  if(color_time == 0){
    color_time = 30;
    if(current_color == 5){
      current_color = 0;
    }else{
      current_color++;
    }
  }
}

/*
 * Check if it is time to show the savescreen. 
 * If not, 'countdown' value is decreased.
 */
int show_savescreen(){
  _rtc_hand_save_current_time(&current_hour, &current_minute, &current_seconds);
  if(current_hour != prev_hour || current_minute != prev_minute || current_seconds != prev_seconds){
    _rtc_hand_save_prev_time();
    countdown--;
    if(countdown == 0)
      return true;
  }
  return false;
}

/*
 * Check enable_ss is true (ssIsEnabled). If so, this method backs up the screen,
 * and hides the cursor. Next time, the method runs the savescreen.
 */
void int_08() {
  if(!ssIsEnabled()){
   if(show_savescreen()){
       backup_screen();
       hide_cursor();
       enable_savescreen();
   }
 }else{
   run_savescreen();
 }
}

/*
 * Thid method receives sacencode and checks if is MAKE CODE
 * or BREAK CODE. 
 * If the savescreen is running, this method restores
 * the status of the screen before savescreen appears.
 */
void keyboard_handler(unsigned char scancode){
    if(ssIsEnabled()){
      disable_savescreen();
      restore_screen();
    }
    countdown = ss_time;

    if((scancode & 0x80) == 0x80){
      key_release(scancode); // key release (BREAK CODE)
    }else{
      key_press(scancode); // key press (MAKE CODE)
    }
}

/*
 * This method receives the position x and y of the mouse.
 * Then, It is printed on the screen.
 * Also, if the savescreen is running, this method restores
 * the status of the screen before savescreen appears.
 */
void _mouse_handler(){
  __mouse_handler(&mouse_x, &mouse_y);
  times++;
  
  if(times == 3){
    if(ssIsEnabled()){
      disable_savescreen();
      restore_screen();
    }
    countdown = ss_time;

    video_update_mouse(mouse_x,mouse_y);
    if(any_button_press())
      help_mouse();
    times = 0;
  }
}

/**********************************************
kmain() 
Punto de entrada de cóo C.
*************************************************/

kmain() 
{
  
/* CARGA DE IDT CON LA RUTINA DE ATENCION DE IRQ0    */

  setup_IDT_entry (&idt[0x08], 0x08, (dword)&_int_08_hand, ACS_INT, 0); // Timer Tick
  setup_IDT_entry (&idt[0x09], 0x08, (dword)&_int_09_hand, ACS_INT, 0); // Keyboard
  setup_IDT_entry (&idt[0x74], 0x08, (dword)&_int_74_hand, ACS_INT, 0); // mouse 

/* Carga de IDTR    */

	idtr.base = 0;  
	idtr.base +=(dword) &idt;
	idtr.limit = sizeof(idt)-1;

	_lidt (&idtr);	

  cbInit(&keyboard_buffer);
  initialize_video();
  clean_screen();
  _setmouse();
  _Cli();
  
  _mascaraPIC1(0xF8);  //11111000 Habilito interrupcion de timer tick(irq0), keyboard (irq1) e irq2(pic slave) 
  _mascaraPIC2(0xEE);  //11101110 E:habilito RTC y Mouse 

  
  _Sti();

  update_cursor(0,0);
  video_update_mouse(0,0);
  rtc_handler_save_prev_time();
  

  run_shell();
}

