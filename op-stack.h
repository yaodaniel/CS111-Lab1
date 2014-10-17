#ifndef _OP_STACK_H
#define _OP_STACK_H
#define OP_STACK_SIZE 2

#include "alloc.h"

typedef enum operator_type
  {
    NEWLINE,
    SEMICOLON,
    PIPE,
    LPAREN,
    RPAREN,
    LARROW,
    RARROW,
    IF,
    THEN,
    ELSE,
    FI,
    WHILE,
    DO,
    UNTIL,
    DONE,
    EMPTY //Empt, no operators
  }operator_type;

/*struct Operator{
       enum operator_type op;
}Operator;*/

typedef struct op_stack {
        int stackSize, numItems;
        enum operator_type* op;
        
}op_stack;

op_stack* create_stackOp();
void pushOp(op_stack* stack, enum operator_type arg);
enum operator_type popOp(op_stack* stack);
enum operator_type peekOp(op_stack* stack);

#endif