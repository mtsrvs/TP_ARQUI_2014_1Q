#include "../include/defs.h"
#include "../include/stdio.h"
#include "../include/file_descriptor.h"
#include "../include/command.h"

char *prompt = "arqui@kernel:~$ ";
Command commands[MAX_COMMANDS];

void write_prompt(){
	printf(prompt);
}

void set_savescreen(){
  int var_time;
  scanf("\nset savescreen time: %d", &var_time);
  set_time_savescreen(var_time);
  printf("savescreen configurated\n");
  return;
}

void hello_world(){
  printf("\nHello World!\n");
}

void view_time(){
  rtc_handler();
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

void initialize_commands(){
  commands[0].name_command = "savescreen";
  commands[0].function = &set_savescreen;

  commands[1].name_command = "hello world";
  commands[1].function = &hello_world;

  commands[2].name_command = "time";
  commands[2].function = &view_time;

  commands[3].name_command = "help";
  commands[3].function = &help;
}

int run_command(char *aCommand){
  int i;

  for (i = 0; i < MAX_COMMANDS; ++i)
  {
    // printf("\nvalue i: %d, name_command: %s, aCommand: %s",i, commands[i].name_command, aCommand );
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
	int shell_buffer_pos = 0;

  initialize_commands();
  initialize_video();
	clean_screen();
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
          video_erase_write();
        }
        break;
      default:
        shell_buffer[shell_buffer_pos++] = c;
        putchar(c);
    }
  }
}

