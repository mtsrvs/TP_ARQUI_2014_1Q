#include "../include/kc.h"
#include "../include/file_descriptor.h"
#include "../include/video.h"

char *video = (char *) 0xb8000;
char prev_screen[2000];
int prev_line_offset;
int prev_char_offset;
int pos = 0;

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
}

void paint_screen(char color){
  screen.current_txt_background_color = color;
  clean_screen();
}

// void video_move_type_cursor(int fd)
// {
//   ScreenSegment* ss = &screen_segment_table[fd];
//   // TODO: move this magic number to a constant
//   set_cursor((10 + ss->line_offset) * ss->char_offset_limit + ss->char_offset);
// }

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

// void video_write_new_line(int fd)
// {
//   _video_new_line(fd);
//   video_move_type_cursor(fd);
// }

void video_write(char ascii){
  video_set(screen.char_offset, screen.line_offset, ascii);
  /*
   * We increment one positions since we only
   * count logic characters, not bytes on screen.
   */
  screen.char_offset++;

  if (screen.char_offset == screen.char_offset_limit)
  {
    _video_new_line();
  }

  //video_move_type_cursor(fd);
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
    //video_move_type_cursor(fd);
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
}