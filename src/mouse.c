/*
 * source: http://forum.osdev.org/viewtopic.php?t=10247
 *
 * Mouse.inc by SANiK
 * License: Use as you wish, except to cause damage
 */

#include "../include/defs.h"
#include "../include/kasm.h"

byte mouse_cycle=0;     //unsigned char
byte byte0 = 0;
byte mouse_byte[3];    //signed char
int button_pressed = false;
int count = 0;

int any_button_was_pressed_before(){
  return button_pressed == true;
}

inline void mouse_wait(byte a_type) //unsigned char
{
  dword _time_out=200000; //unsigned int
  if(a_type==0)
  {
    while(_time_out--) //Data
    {
      if((inportb(0x64) & 1)==1)
      {
        return;
      }
    }
    return;
  }
  else
  {
    while(_time_out--) //Signal
    {     
      if((inportb(0x64) & 2)==0)
      {
        return;
      }
    }
    return;
  }
}

void __mouse_handler(int *mouse_x, int *mouse_y)
{

   switch(mouse_cycle)
  {
    case 0:
      mouse_byte[0]=inportb(0x60);
      mouse_cycle++;
      break;
    case 1:
      mouse_byte[1]=inportb(0x60);
      mouse_cycle++;
      break;
    case 2:
      mouse_byte[2]=inportb(0x60);
      *mouse_x=mouse_byte[1];
      *mouse_y=mouse_byte[2];
      mouse_cycle=0;
      break;
  }
}

int any_button_press(){
  int button = 0;
        if((mouse_byte[0] & 0x02) == 0x02) //right button
          button = 1;

        if((mouse_byte[0] & 0x01) == 0x01) //left button
          button = 1;
  return button; 
}


inline void mouse_write(byte a_write) //unsigned char
{
  //Wait to be able to send a command
  mouse_wait(1);
  //Tell the mouse we are sending a command
  outportb(0x64, 0xD4);
  //Wait for the final part
  mouse_wait(1);
  //Finally write
  outportb(0x60, a_write);
}

byte mouse_read()
{
  byte res;
  //Get's response from mouse
  mouse_wait(0); 
  res = inportb(0x60);

  return res;
}


void mouse_install()
{
   byte _status;  //unsigned char

  //Enable the auxiliary mouse device
  mouse_wait(1);
  outportb(0x64, 0xA8);
  
  //Enable the interrupts
  mouse_wait(1);
  outportb(0x64, 0x20);
  mouse_wait(0);
  _status=(inportb(0x60) | 2);
  mouse_wait(1);
  outportb(0x64, 0x60);
  mouse_wait(1);
  outportb(0x60, _status);

  //Tell the mouse to use default settings
  mouse_write(0xF6);
  mouse_read();  //Acknowledge
  
  //Enable the mouse
  mouse_write(0xF4);
  mouse_read();  //Acknowledge
}
