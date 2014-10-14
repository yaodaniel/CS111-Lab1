#include "command-stack.h"

command_stack* create_stack() {
     command_stack* stack = checked_malloc(sizeof(command_stack));
     stack->commandObjects = checked_malloc(sizeof(command_t)*STACK_SIZE);
     stack->stackSize = STACK_SIZE;
     stack->numItems = 0;
     return stack;
}
void push(command_stack* stack, command_t arg) {
     int items = stack->numItems, sSize = stack->stackSize;
     if(items >= sSize) {
             stack->commandObjects = checked_realloc(stack->commandObjects, (sizeof(command_t)*(sSize+STACK_SIZE)));
             stack->stackSize += STACK_SIZE;
         }
     stack->commandObjects[items] = arg;
     stack->numItems += 1;  
}
command_t pop(command_stack* stack) {
     if(stack->numItems == 0)
          return NULL;
     stack->numItems -= 1;
          return stack->commandObjects[stack->numItems];
}
command_t peek(command_stack* stack){
     if(stack->numItems == 0)
          return NULL;
     return stack->commandObjects[stack->numItems - 1];
}