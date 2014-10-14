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
#include "command-internals.h"
#include "stdio.h"
#include "string.h"
#include <error.h>

typedef int bool;

int specialTokens(char token)
{
	if(token==';'||token=='|'||token=='('||token==')'||token=='<'||token=='>' || token == '\n')
		return 1;
	else
		return 0;
}

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
struct command_stream {
       struct commandNode* head;
};

struct commandNode {
      command_t command;
      struct CommandNode* next;
};

command_stream_t
make_command_stream (int (*get_next_byte) (void *),		// Parse texts
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  int TOKEN_SIZE = 10;
	int WORD_SIZE = 10;
	int noOfChars = 0;
	int lineNumber = 1;
	int wordFlag = 0;
	int noOfTokens = 0;
	int commentFlag = 0;
	char tempChar = get_next_byte(get_next_byte_argument);
	char **tokens = (char**) checked_malloc(TOKEN_SIZE*sizeof(char*));
	char *word = (char*) checked_malloc(WORD_SIZE* sizeof(char));
	while(tempChar != EOF)
	{
		if(tempChar == '\n')
		{
			lineNumber++;
		}
		if(commentFlag==1)
		{
			if(tempChar == '\n')
			{
				word[noOfChars]=tempChar;
				noOfChars++;
				word[noOfChars]='\0';
				tokens[noOfTokens]= word;
				noOfTokens++;
				if(noOfTokens==TOKEN_SIZE)
				{
					TOKEN_SIZE= TOKEN_SIZE*2;
					tokens = checked_realloc(tokens,TOKEN_SIZE*sizeof(char*));
				}
				WORD_SIZE =10;
				word = (char*) checked_malloc(WORD_SIZE* sizeof(char));
				noOfChars=0;
				wordFlag =0;
				commentFlag = 0;
			}
		}
		else
		if(tempChar == '#')
		{
			if(wordFlag == 1)
			{
				fprintf(stderr,"%d: ",lineNumber );
				exit(1);
			}
			commentFlag = 1;
		}
		else
		if(tempChar != ' ' && tempChar != '\t')
		{
			if(specialTokens(tempChar)==1)
			{
				if(wordFlag==1)
				{
					word[noOfChars] = '\0';
					tokens[noOfTokens]= word;
					noOfTokens++;
					if(noOfTokens==TOKEN_SIZE)
					{
						TOKEN_SIZE= TOKEN_SIZE*2;
						tokens = checked_realloc(tokens,TOKEN_SIZE*sizeof(char*));
					}
					WORD_SIZE =10;
					word = (char*) checked_malloc(WORD_SIZE* sizeof(char));
					noOfChars=0;
					wordFlag =0;
				}
				word[noOfChars]=tempChar;
				noOfChars++;
				word[noOfChars]='\0';
				tokens[noOfTokens]= word;
				noOfTokens++;
				if(noOfTokens==TOKEN_SIZE)
				{
					TOKEN_SIZE= TOKEN_SIZE*2;
					tokens = checked_realloc(tokens,TOKEN_SIZE*sizeof(char*));
				}
				WORD_SIZE =10;
				word = (char*) checked_malloc(WORD_SIZE* sizeof(char));
				noOfChars=0;
				wordFlag =0;
			}
			else
			{
				word[noOfChars] = tempChar;
				noOfChars++;
				if(noOfChars == WORD_SIZE)
				{
					WORD_SIZE = WORD_SIZE*2;
					word = checked_realloc(word,WORD_SIZE*sizeof(char));
				}
				wordFlag =1;
			}
		}
		else
		{
			if(wordFlag==1)
			{
				word[noOfChars] = '\0';
				tokens[noOfTokens]= word;
				noOfTokens++;
				if(noOfTokens==TOKEN_SIZE)
				{
					TOKEN_SIZE= TOKEN_SIZE*2;
					tokens = checked_realloc(tokens,TOKEN_SIZE*sizeof(char*));
				}
				WORD_SIZE =10;
				word = (char*) checked_malloc(WORD_SIZE* sizeof(char));
				noOfChars=0;
				wordFlag =0;
			}
		}
		tempChar = get_next_byte(get_next_byte_argument);
	}
	if(wordFlag==1)
	{
		word[noOfChars] = '\0';
		tokens[noOfTokens]= word;
		noOfTokens++;
	}
	int i=0;
	while(i<noOfTokens)
	{
		printf("%s\n", tokens[i]);
		i++;
	}
	printf("%u", noOfTokens);
}

command_t	
read_command_stream (command_stream_t s)		// Creates a stack with commands
{
  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");
   command_t returnObject = NULL;
  if(s == NULL || s->head == NULL)
       return NULL;
       
  returnObject = s->head->command;
  s->head = (s->head)->next;
       return returnObject;
}