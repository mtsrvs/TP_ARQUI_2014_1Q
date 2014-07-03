#include "../include/kc.h"
#include "../include/file_descriptor.h"
#include "../include/video.h"

char *video = (char *) 0xb8000;
char prev_screen[2000] = {0};
int prev_line_offset;
int prev_char_offset;
int pos = 0;
int mouse_pos_x = 39;
int mouse_pos_y = 12;

Screen screen;

// ***************************************************************************

/*
 * Text screen video memory is located at 0xb8000
 * The first 10 lines should be the debug screen
 * and the 15 remaining lines for the shell.
 *
 * To calculate the shell screen starting position:
 * Each character on screen takes up two bytes.
 * And each line consits of 80 characters.
 * So, each line should use 160 bytes.
 * We multiply 10d * 160d, and the result we
 * add it to the real starting position.
 * (of course, taking the precautions to use
 * the correct units).
 *
 */

void restart_screen_offsets()
{
  screen.char_offset = 0;
  screen.line_offset = 0;
}

void initialize_video() {
  screen.start_position = (char *) 0xb8000;
  screen.line_offset_limit = SCREEN_HEIGHT;
  screen.char_offset_limit = SCREEN_WIDTH;
  screen.bytes_per_char = 2;
  screen.current_txt_background_color = WHITE_TXT;
  restart_screen_offsets();
}

// ***************************************************************************

/*
 * Set a white character on given File Descriptor, on given
 * offset.
 * (Only for File Descriptors associated with a screen segment).
 *
 */
void video_set(int char_offset, int line_offset, char value){
  int position = (line_offset * screen.char_offset_limit + char_offset) * screen.bytes_per_char;
  
	screen.start_position[position] = value;
	screen.start_position[position + 1] = screen.current_txt_background_color;
}

/*
 *  Cleans a screen segment and starts offset from 0
 */
void clean_screen()
{
  
  int line;
  int char_pos;
  for (line = 0; line < screen.line_offset_limit; line++)
  {
    for (char_pos = 0; char_pos < screen.char_offset_limit; char_pos++)
    {
      video_set(char_pos, line, ' ');
    }
  }
  restart_screen_offsets();
}

void paint_screen(byte color){
  screen.current_txt_background_color = color;
  clean_screen();
}

void _video_new_line()
{
  screen.line_offset++;
  screen.char_offset = 0;

  if (screen.line_offset == screen.line_offset_limit)
  {
    screen.line_offset = 0;
    clean_screen();
  }
}

/*
 * source: http://wiki.osdev.org/Text_Mode_Cursor
 * 
 * Set cursor position (text mode 80x25)
 * 0x3D4: VGA port
 * 0x3D5: VGA port 
 *
 */ 
 void update_cursor(int row, int col)
 {
    unsigned short position=(row*80) + col;
    // cursor LOW port to vga INDEX register
    _update_cursor_position(0x3D4, 0x0F);
    _update_cursor_position(0x3D5, (unsigned char)(position&0xFF));
    // cursor HIGH port to vga INDEX register
    _update_cursor_position(0x3D4, 0x0E);
    _update_cursor_position(0x3D5, (unsigned char )((position>>8)&0xFF));

    return;
 }

/*
 * Idea of how to hide cursor taken from 
 * http://forum.osdev.org/viewtopic.php?f=1&t=15669&start=0
 */
void hide_cursor(){
  update_cursor(0xFF, 0xFF);
  return;
}

void video_write(char ascii){
  video_set(screen.char_offset, screen.line_offset, ascii);
  update_cursor(screen.line_offset, screen.char_offset + 1);

  /*
   * We increment one positions since we only
   * count logic characters, not bytes on screen.
   */
  screen.char_offset++;

  if (screen.char_offset == screen.char_offset_limit)
  {
    _video_new_line();
  }

}

 void _video_erase_mouse(int mouse_pos_x, int mouse_pos_y){
  video[2*(mouse_pos_y*80 + mouse_pos_x) + 1] = WHITE_TXT;
 }

 void _video_write_mouse(int mouse_pos_x, int mouse_pos_y, int color){
  video[2*(mouse_pos_y*80 + mouse_pos_x) + 1] = color;
 }

void video_update_mouse(byte mouse_x, byte mouse_y){
  int delta_x = 0;
  int delta_y = 0;


    if(mouse_x == 0xFF){
      delta_x = -1;
    }else if(mouse_x == 1){
      delta_x = 1;
    }    


    if(mouse_y == 0xFF){
      delta_y = 1;
    }else if(mouse_y == 1){
      delta_y = -1;
    }

  _video_erase_mouse(mouse_pos_x, mouse_pos_y);
  if((mouse_pos_x + delta_x) >= 0 && (mouse_pos_x + delta_x) <= 79){
      mouse_pos_x += delta_x;    
  }
  if((mouse_pos_y + delta_y) >= 0 && (mouse_pos_y + delta_y) <= 24){
      mouse_pos_y += delta_y;
  }
  _video_write_mouse(mouse_pos_x, mouse_pos_y, 0x38);
}

/*
 * Erase previous written position.
 * Does not support multi lines erasing.
 */
void video_erase_write()
{
  if (screen.char_offset > 0)
  {
    screen.char_offset--;
    video_set(screen.char_offset, screen.line_offset, ' ');
    update_cursor(screen.line_offset, screen.char_offset);
  }
}

/*
 * Back up every character on screen.
 */
void backup_screen(){
  int pos_char_on_screen = 0;
  int i = 0;
  char *video = (char *) 0xb8000;
  
  while(i < SCREEN_WIDTH*SCREEN_WIDTH){
    prev_screen[i] = video[pos_char_on_screen];
    i++;
    pos_char_on_screen += 2;
  }

  prev_line_offset = screen.line_offset;
  prev_char_offset = screen.char_offset;
}

void restore_screen(){
  int aChar;
  int pos_char_on_screen = 0;
  int i = 0;

  restart_screen_offsets();

  screen.current_txt_background_color = WHITE_TXT;
  
  while(i < SCREEN_WIDTH*SCREEN_WIDTH){
    video[pos_char_on_screen++] = prev_screen[i];
    video[pos_char_on_screen++] = screen.current_txt_background_color;
    i++;
  }

  screen.line_offset = prev_line_offset;
  screen.char_offset = prev_char_offset;
  update_cursor(screen.line_offset, screen.char_offset + 1);
  return;
}