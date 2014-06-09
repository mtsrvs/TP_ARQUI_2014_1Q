#ifndef _COMMAND_H_
#define _COMMAND_H_

#define MAX_COMMANDS 4

typedef struct {
	char  *name_command;
	void  (*function)();
}Command;

#endif