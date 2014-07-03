/*
 * Keyboard Driver
 */

#include "../include/keyboard.h"
#include "../include/circular_buffer.h" 
#include "../include/defs.h"

// Indicates wether ctrl key is pressed or not
int ctrl_pressed = false;
int caplock_pressed = false;
int shift_pressed = false;
extern CircularBuffer keyboard_buffer;


/*
 * Map each scancode to an ascii value.
 */

char asccode[58][2] =
     {
       {   0,0   } ,
       { 0,0 } ,
       { '1','!' } ,
       { '2','@' } ,
       { '3','#' } ,
       { '4','$' } ,
       { '5','%' } ,
       { '6','^' } ,
       { '7','&' } ,
       { '8','*' } ,
       { '9','(' } ,
       { '0',')' } ,
       { '-','_' } ,
       { '=','+' } ,
       { '\b', '\b' } , // backspace
       {   9,9   } ,
       { 'q','Q' } ,
       { 'w','W' } ,
       { 'e','E' } ,
       { 'r','R' } ,
       { 't','T' } ,
       { 'y','Y' } ,
       { 'u','U' } ,
       { 'i','I' } ,
       { 'o','O' } ,
       { 'p','P' } ,
       { '[','{' } ,
       { ']','}' } ,
       { '\n', '\n' } , // enter
       {   0,0   } ,
       { 'a','A' } ,
       { 's','S' } ,
       { 'd','D' } ,
       { 'f','F' } ,
       { 'g','G' } ,
       { 'h','H' } ,
       { 'j','J' } ,
       { 'k','K' } ,
       { 'l','L' } ,
       { ';',':' } ,
       {  39,34  } ,
       { '`','~' } ,
       {   0,0   } ,
       { '\\','|'} ,
       { 'z','Z' } ,
       { 'x','X' } ,
       { 'c','C' } ,
       { 'v','V' } ,
       { 'b','B' } ,
       { 'n','N' } ,
       { 'm','M' } ,
       { ',','<' } ,
       { '.','>' } ,
       { '/','?' } ,
       {   0,0   } , //RShift
       {   0,0   } ,
       {   0,0   } ,
       { ' ',' ' } , //spacebar

   };

/*
 * Transforms given scancode to its ascii value.
 * Returns its alternative value if shift is pressed.
 * Returns uppdercase if it is a letter and shift is pressed
 * or caplock activated.
 *
 */

char scancode_to_ascii(char scancode) {
  char c = asccode[scancode][0];

  if (shift_pressed || (caplock_pressed && ('a' <= c && c <= 'z')))
  {
    c = asccode[scancode][1];
  }

  return c;
}

int is_ctrl_pressed(){
  return ctrl_pressed;
}

void chance_caplock_state(){
  caplock_pressed = !caplock_pressed;
}

void set_ctrl_pressed(int state){
  ctrl_pressed = state;
}

void set_shift_pressed(int state){
  shift_pressed = state;
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
  //  case 0x1d: set_ctrl_pressed(true);
  //         break;
  //  case 0x3a: chance_caplock_state();
  //         break;
  //  case 0x2a: set_shift_pressed(true);
  //         break;
  //  default:   ascii = scancode_to_ascii(scancode);
  //              video_write(ascii);
  //             cbWrite(&keyboard_buffer, &ascii); 
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