#include "../include/defs.h"
#include "../include/stdio.h"
#include "../include/file_descriptor.h"
#include "../include/command.h"

#define MAX_COMMANDS 6

int hour, minute, seconds, dayofmonth, month, year;
int new_hour, new_minute, new_seconds;

int shell_buffer_pos = 0;

char *prompt = "arqui@kernel:~$  ";
Command commands[MAX_COMMANDS];

void write_prompt(){
	printf(prompt);
}

void set_savescreen(){
  int var_time;
  scanf("\nset savescreen time (seconds): %d", &var_time);
  set_time_savescreen(var_time);
  printf("\nsavescreen configurated\n");
  return;
}

void hello_world(){
  printf("\nHello World!\n");
}



void view_time(){
  rtc_handler();
  printf("\nTIME %h:%h:%h  DATE: %h/%h/%h\n",hour, minute, seconds,dayofmonth,month,year);
}



void set_time(){
  int correct = false;
  byte hex_hour, hex_min, hex_sec;

  while (!correct){
  scanf("\nchange hour: %d", &new_hour);
  scanf("\nchange minute: %d", &new_minute);
  scanf("\nchange seconds: %d\n", &new_seconds);

  if(new_hour >= 0 && new_hour <= 23)
    if(new_minute >= 0 && new_minute <= 59)
      if(new_seconds >= 0 && new_seconds <= 59)
          correct = true;

  if(!correct)
    printf("time not valid.\n");
  }
  hex_hour = tohex(new_hour);
  hex_min = tohex(new_minute);
  hex_sec = tohex(new_seconds);

  rtc_hand_write(hex_hour, hex_min, hex_sec);

  printf("time configurated\n");

}

void help(){
  int i;
  printf("\n***command list***");

  for (i = 0; i < MAX_COMMANDS; ++i)
  {
    printf("\n%s",commands[i].name_command);
  }
  printf("\n");
}

void help_mouse(){
  int i;

  printf("\n***command list***");

  for (i = 0; i < MAX_COMMANDS; ++i)
  {
    printf("\n%s",commands[i].name_command);
  }
  printf("\n******************\n");
  write_prompt();
  shell_buffer_pos = 0;
}

void clear(){
  clean_screen();
}

void initialize_commands(){
  commands[0].name_command = "savescreen";
  commands[0].function = &set_savescreen;

  commands[1].name_command = "hello world";
  commands[1].function = &hello_world;

  commands[2].name_command = "time";
  commands[2].function = &view_time;

  commands[3].name_command = "set time";
  commands[3].function = &set_time;

  commands[4].name_command = "help";
  commands[4].function = &help;

  commands[5].name_command = "clear";
  commands[5].function = &clear;

}

int run_command(char *aCommand){
  int i;

  for (i = 0; i < MAX_COMMANDS; ++i)
  {
    if(strcmp(commands[i].name_command, aCommand) == 0){
      (*commands[i].function)();
      return 1;
    }
  }
  return 0;
}

void shell_run_command(char *buffer, int *pos){
  if(*pos == 0){
    printf("\n");
    return;
  }

  buffer[*pos] = '\0';
  if(!run_command(buffer))
    printf("\nCommand not found.\n");
  *pos = 0;
  return;
}

void run_shell(){
  char c;
  char shell_buffer[1000];
  
  initialize_commands();
	write_prompt();

	while(true){

	c = getChar();

    switch(c)
    {
      case '\n':
        shell_run_command(shell_buffer, &shell_buffer_pos);
        write_prompt();
        break;
      case '\b':
        if (shell_buffer_pos > 0)
        {
          shell_buffer_pos--;
          putchar(c);
        }
        break;
      default:
        shell_buffer[shell_buffer_pos++] = c;
        putchar(c);
    }
  }
}

