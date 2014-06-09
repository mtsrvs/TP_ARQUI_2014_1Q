#include "../include/kasm.h"
#include "../include/kc.h"
#include "../include/defs.h"
#include "../include/kernel.h"
#include "../include/video.h"
#include "../include/file_descriptor.h"
#include "../include/circular_buffer.h"
#include "../include/keyboard.h"

extern void _rtc_hand();

int hour, minute, seconds, dayofmonth, month, year;

DESCR_INT idt[0xA];			/* IDT de 10 entradas*/
IDTR idtr;				/* IDTR */

CircularBuffer keyboard_buffer;

char color[6] = {0x17, 0x27, 0x37, 0x47, 0x57, 0x67};
unsigned long int color_time = 30;
int current_color;
int prev_color;

unsigned int ss_time = 60;
unsigned int countdown = 60;

int enable_ss = false;

void rtc_handler(){
  _rtc_hand();
  fprintf(STDOUT,"\nTIME %h:%h:%h  DATE: %h/%h/%h\n",hour, minute, seconds,dayofmonth,month,year);
}

void key_press(byte scancode)
{
  if (scancode == 0x1d) // TODO: implement with a switch, check other ctrl values.
  {
    set_ctrl_pressed(true);
  }
  else if(scancode == 0x3a) /* caplock key */
  {
    chance_caplock_state();
  }
  else if(scancode == 0x2a) /* shift key */
  {
    set_shift_pressed(true);
  }
  else
  {
    char ascii = scancode_to_ascii(scancode);
    // write onto keyboard buffer
    cbWrite(&keyboard_buffer, &ascii);
  }

  // char ascii;

  // switch(scancode){
  // 	case 0x1d: set_ctrl_pressed(true);
  // 			   break;
  // 	case 0x3a: chance_caplock_state();
  // 			   break;
  // 	case 0x2a: set_shift_pressed(true);
  // 			   break;
  // 	default:   ascii = scancode_to_ascii(scancode);
  //              video_write(ascii);
  //       		   cbWrite(&keyboard_buffer, &ascii); 
  // }		
}

void key_release(byte scancode)
{
  switch(scancode){
  	case 0x9d: set_ctrl_pressed(false); 
  			   break;
  	case 0xaa: set_shift_pressed(false); 
  			   break;
  }
}

int _read(int fd, void* buffer, int count){
  if (cbIsEmpty(&keyboard_buffer))
  {
    return 0; // 0 elements found.
  } else {
    cbRead(&keyboard_buffer, buffer); // ToDo: implement iteration for count > 0
    return 1; // should return how many elements it has read.
  }
}

// int _read(int fd, void *buf, int n){
// 	int i;

//   if(fd == STDIN){
// 		for(i = 0; i < n; i++){
// 			if(!cbIsEmpty(&keyboard_buffer)){
// 				cbRead(&keyboard_buffer, buf);
//         return 1;
// 			}
// 		}	
// 	}
// 	return 0;
// }

// int _write(int fd, const void* buffer, int count){
//   char* local_buffer = (char *)buffer;
//   video_write(fd, local_buffer[0]); // to try things out

//   // ToDo: implement count > 1

//   return count;
// }

int _write_new_line(){
  _video_new_line();
  return 1;
}

int _write(int fd, char *buf, int n){
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
  initialize_commands(); //PARCHE!!!!!!!!!!!! funciona!!!!!
}

int ssIsEnabled(){
  return enable_ss;
}

void set_time_savescreen(int value){
  ss_time = value;
}

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

void int_08() {
  if(!ssIsEnabled()){
    countdown--;
    if(countdown == 0){
    backup_screen();
    enable_savescreen();
    }
  }else{
    run_savescreen();
  }
}


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

void

/**********************************************
kmain() 
Punto de entrada de cóo C.
*************************************************/

kmain() 
{

/* CARGA DE IDT CON LA RUTINA DE ATENCION DE IRQ0    */

  setup_IDT_entry (&idt[0x08], 0x08, (dword)&_int_08_hand, ACS_INT, 0); // Timer Tick
  setup_IDT_entry (&idt[0x09], 0x08, (dword)&_int_09_hand, ACS_INT, 0); // Keyboard
  //setup_IDT_entry (&idt[0x0A], 0x08, (dword)&_rtc_hand, ACS_INT, 0); // RTC
	
/* Carga de IDTR    */

	idtr.base = 0;  
	idtr.base +=(dword) &idt;
	idtr.limit = sizeof(idt)-1;
	
	_lidt (&idtr);	

	_Cli();
/* Habilito interrupcion de timer tick*/

  _mascaraPIC1(0xFC);
  _mascaraPIC2(0xFE); // E:habilito RTC
        
	_Sti();

  cbInit(&keyboard_buffer);
  run_shell();
}

