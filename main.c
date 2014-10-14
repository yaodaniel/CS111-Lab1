// UCLA CS 111 Lab 1 main program

// Copyright 2012-2014 Paul Eggert.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "op-stack.h"
#include "command-stack.h"
#include "command-internals.h"

#include "command.h"

static char const *program_name;
static char const *script_name;

static void
usage (void)
{
  error (1, 0, "usage: %s [-p PROF-FILE | -t] SCRIPT-FILE", program_name);
}

static int
get_next_byte (void *stream)
{
  return getc (stream);
}

int
main (int argc, char **argv)
{
  /*enum operator_type token1 = SEMICOLON;
  enum operator_type token2 = PIPE;
  enum operator_type token3 = LARROW;
  
  op_stack* myOpStack = create_stackOp();
  pushOp(myOpStack, token1);
  printf("Good! token1 is being pushed to the stack\n");
  pushOp(myOpStack, token2);
  printf("Good! token2 is being pushed to the stack\n");
  pushOp(myOpStack, token3);
  printf("Good! token3 is being pushed to the stack\n");
  if(peekOp(myOpStack) == token3)
  printf("Good! token3 is at the top of the stack\n");
  printf("The current stack size is: %d \n", myOpStack->stackSize);
  
  printf("The current number of items on the stack is: %d \n", myOpStack->numItems);
    if(popOp(myOpStack) == LARROW)
    printf("Good popping works!\n");
  popOp(myOpStack);
  popOp(myOpStack);
  printf("The current number of items on the stack is: %d \n", myOpStack->numItems);
  printf("The current stack size is: %d \n", myOpStack->stackSize);
  if(peekOp(myOpStack) == EMPTY)
     printf("Good! the stack is now empty\n");*/

  /*char c[] = "abcdefg";

  command_stack* myStack = create_stack();
  command_t commandOne = checked_malloc(sizeof(struct command));
  command_t commandTwo = checked_malloc(sizeof(struct command));
  
  commandOne -> status = 42;
  commandOne -> type = SIMPLE_COMMAND;
  commandOne -> input = c;
  
  push(myStack, commandOne);
  printf("Good! commandOne is being pushed to the stack\n");
  if(peek(myStack) == commandOne)
     printf("Good! commandOne is on the stack\n");
  if(peek(myStack) != commandTwo)
     printf("Good! commandTwo is not on the stack\n");
     
     printf("The current stack size is: %d \n", myStack->stackSize);
  
     printf("The current number of items on the stack is: %d \n", myStack->numItems);
     
     printf("commandOne on the stack has the status: %d \n", peek(myStack) -> status);
     printf("commandOne on the stack being poped has the input: %s \n", pop(myStack) -> input); 
  if(peek(myStack) == NULL)
     printf("Good! the stack is now empty\n");
  
  return 1;*/
  int command_number = 1;
  bool print_tree = false;
  char const *profile_name = 0;
  program_name = argv[0];

  for (;;)
    switch (getopt (argc, argv, "p:t"))
      {
      case 'p': profile_name = optarg; break;
      case 't': print_tree = true; break;
      default: usage (); break;
      case -1: goto options_exhausted;
      }
 options_exhausted:;

  // There must be exactly one file argument.
  if (optind != argc - 1)
    usage ();

  script_name = argv[optind];
  FILE *script_stream = fopen (script_name, "r");
  if (! script_stream)
    error (1, errno, "%s: cannot open", script_name);
  command_stream_t command_stream =
    make_command_stream (get_next_byte, script_stream);
  int profiling = -1;
  if (profile_name)
    {
      profiling = prepare_profiling (profile_name);
      if (profiling < 0)
	error (1, errno, "%s: cannot open", profile_name);
    }

  command_t last_command = NULL;
  command_t command;
  while ((command = read_command_stream (command_stream)))
    {
      if (print_tree)
	{
	  printf ("# %d\n", command_number++);
	  print_command (command);
	}
      else
	{
	  last_command = command;
	  execute_command (command, profiling);
	}
    }

  return print_tree || !last_command ? 0 : command_status (last_command);
}
