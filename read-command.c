// This is to satisfy S
// UCLA CS 111 Lab 1 command reading

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

#include "command.h"
#include "stdio.h"
#include <stdlib.h>
#include <ctype.h>
#include "command-stack.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
   
//Command linked lists begin
struct commandNode {
      command_t command;
      struct CommandNode *next;
};

struct command_stream {
       struct commandNode *head;
};
//Command Linked lists end

int isWord (char c) {
    return isalpha(c) | isdigit(c) | (c=='!') | (c=='%') | (c=='+') | (c==',') | (c=='-') | (c=='.') |
           (c=='/') | (c==':') | (c=='@') | (c=='^') | (c=='_');
}

int isSpecialToken (char c) {
    return (c==';') | (c=='|') | (c=='(') | (c==')') | (c=='<') | (c=='>');
}

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
     
     char currentCharacter = 'D'; //Random Character Value
     while(currentCharacter != EOF){
          currentCharacter = get_next_byte(get_next_byte_argument);
          if(isWord(currentCharacter)) {
               //DO SOMETHING...
          }      
          if(isSpecialToken(currentCharacter)) {
               //Do SOMETHING...
          }
}
  //printf ("%s\n", "blah");
  //error (1, 0, "command reading not yet implemented");
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");
  command_t returnObject = NULL;
  if(s == NULL || s->head == NULL)
       return NULL;
       
  returnObject = s->head->command;
  s->head = s->head->next;
       return returnObject;
}
