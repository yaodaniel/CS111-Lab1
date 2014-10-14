#ifndef _COMMAND_STACK_H
#define _COMMAND_STACK_H
#define STACK_SIZE 256

#include "alloc.h"
#include "command.h"

typedef struct command_stack {
        
        int stackSize, numItems;
        command_t* commandObjects;
        
}command_stack;

command_stack* create_stack();
void push(command_stack* stack, command_t arg);
command_t pop(command_stack* stack);
command_t peek(command_stack* stack);

#endif