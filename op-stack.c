#include "op-stack.h"

op_stack* create_stackOp(){
     op_stack* stack = checked_malloc(sizeof(op_stack));
     stack->op = checked_malloc(sizeof(enum operator_type)*OP_STACK_SIZE);
     stack->stackSize = OP_STACK_SIZE;
     stack->numItems = 0;
     return stack;
}
void pushOp(op_stack* stack, enum operator_type arg){
     int items = stack->numItems, sSize = stack->stackSize;
     if(items >= sSize) {
             stack->op = checked_realloc(stack->op, (sizeof(enum operator_type)*(sSize+OP_STACK_SIZE)));
             stack->stackSize += OP_STACK_SIZE;
         }
     stack->op[items] = arg;
     stack->numItems += 1; 
}
enum operator_type popOp(op_stack* stack){
     if(stack->numItems == 0)
          return EMPTY;
     stack->numItems -= 1;
          return stack->op[stack->numItems];
}
enum operator_type peekOp(op_stack* stack){
     if(stack->numItems == 0)
          return EMPTY;
     return stack->op[stack->numItems - 1];
}