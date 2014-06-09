#ifndef _VIDEO_H_
#define _VIDEO_H_

/* Screen descriptor  */
typedef struct {
  char* start_position;
  int line_offset;
  int char_offset;
  int char_offset_limit;
  int line_offset_limit;
  int bytes_per_char;
  char current_txt_background_color;
} Screen;

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#endif