#ifndef _COMMAND_H_
#define _COMMAND_H_



typedef struct {
	char  *name_command;
	void  (*function)();
}Command;

#endif