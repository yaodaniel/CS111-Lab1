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
#include "alloc.h"
#include <error.h>
#define true 1
#define false 0
typedef int bool;

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
   
//Command linked lists begin
struct commandNode {
      command_t command;
      struct CommandNode* next;
};

struct command_stream {
       struct commandNode* head;
};
//Command Linked lists end

/////////////////////////////////Helper Functions Begin///////////////////////////////////////
int getFirstWord(int index, char** words)
{
	while(strcmp(words[index],"\n") == 0)
		index++;
	return index;
}

int getCommand(int index, char** words)
{
	while(strcmp(words[index],"\n") == 0)
		index++;
	return index;
}

int specialTokens(char token)
{
	if(token==';'||token=='|'||token=='('||token==')'||token=='<'||token=='>' || token == '\n')
		return 1;
	else
		return 0;
}

int searchNextTail(int Index, char* text)
{
	while(text[Index]== ' ')
	{
		if(text[Index] == '\0')
			break;
		Index++;
	}
	return Index;
}

int searchHead(int tailIndex, char* text)
{
	if(specialTokens(text[tailIndex]) == 1)
		return tailIndex+1;
	else
		while(1)
		{
			if(specialTokens(text[tailIndex]) == 1 || text[tailIndex] == ' ' || text[tailIndex] == '\0')
				return tailIndex;
			tailIndex++;
		}
}

void removeComments (int (*get_next_byte) (void *), void *get_next_byte_argument)
{
	char temp = get_next_byte(get_next_byte_argument);
	while(temp!='\n')
	{
		temp = get_next_byte(get_next_byte_argument);
	}
}

///////////////////////////////Helper Functions End////////////////////////////////////////////////

command_stream_t
make_command_stream (int (*get_next_byte) (void *),		// Parse texts
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
//	char temp = get_next_byte(get_next_byte_argument);
//	char result[50000000000];
//	while( temp != EOF)
//	{
//			printf("%c",temp);
//			temp = get_next_byte(get_next_byte_argument);
//	}
  int wordLimit = 1000, textLimit = 1000; //Initial size limit for number of tokens in input and length of each token
	int totalCharacters = 0;
	//int wordMemBlock = 1;
	//int textMemBlock =1;
	int removedCommentsFlag = 0;
	char previousChar;
	char **wordArray = checked_malloc(wordLimit * sizeof(char *));
	char *text = checked_malloc(textLimit * sizeof(char));
	char head = get_next_byte(get_next_byte_argument);
	while(head != EOF)
	{
		if(head == '#')
		{
			removeComments(get_next_byte, get_next_byte_argument);
			text[totalCharacters] = '\n';
			totalCharacters++;
			previousChar = '\n';
		}
		else
		{
			if(!(previousChar == '\n' && head == '\n'))
			{
				text[totalCharacters] = head;
				totalCharacters++;
				while(totalCharacters >= textLimit-1)
					{
						//textMemBlock++;
            textLimit *= 2;
						text = checked_realloc(text, textLimit * sizeof(char));
					}
			}
			previousChar = head;
		}
		head = get_next_byte(get_next_byte_argument);
	}
	text = checked_realloc(text, (textLimit+1) * sizeof(char));
	text[totalCharacters+1]='\0';
	totalCharacters++;
	int noOfWords = 0;
	int cursor_head;
	int cursor_tail;
	int iterator=0;
	while(iterator < totalCharacters)
	{
		cursor_tail = searchNextTail(iterator, text);
		cursor_head = searchHead(cursor_tail,text);
		iterator = cursor_head;
		if(cursor_head == cursor_tail)
			break;
		iterator = cursor_head;
		int string_length = cursor_head - cursor_tail +1;
		char *tempStringPointer = checked_malloc(string_length * sizeof(char));
		int tempStringPointerCursor = 0;
		while(cursor_tail != cursor_head)
		{
			tempStringPointer[tempStringPointerCursor] = text[cursor_tail];
			cursor_tail++;
			tempStringPointerCursor++;
		}
		tempStringPointer[tempStringPointerCursor+1] ='\0';
		wordArray[noOfWords] = tempStringPointer;
		noOfWords++;
		while(noOfWords >= wordLimit-1)
		{
			//wordMemBlock++;
      wordLimit *= 2;
			wordArray = checked_realloc(wordArray, wordLimit * sizeof(char *));
		}
	}
	int i = 0;
	while(i<noOfWords)
	{
		printf("%s\n", wordArray[i]);
		i++;
	}
//	for(iterator = 0; iterator< totalCharacters; iterator++)
//	{
//		cursor_tail = searchNextTail(iterator, text);
//		cursor_head = searchHead(cursor_tail,text);
//		iterator = cursor_head;
//		printf("%u",cursor_tail);
//		iterator = cursor_head;
//		int string_length = cursor_head - cursor_tail +1;
//		char *tempStringPointer = (char *) malloc(string_length * sizeof(char));
//		int tempStringPointerCursor = 0;
//		while(cursor_tail != cursor_head)
//		{
//			tempStringPointer[tempStringPointerCursor] = text[cursor_tail];
//			cursor_tail++;
//			tempStringPointerCursor++;
//		}
//		tempStringPointer[tempStringPointerCursor+1] ='\0';
//		wordArray[noOfWords] = tempStringPointer;
//		noOfWords++;
//		if(noOfWords/10000 != (wordMemBlock-1))
//		{
//			wordMemBlock++;
//			wordArray = (char *) realloc(wordArray, wordMemBlock*10000 * sizeof(char));
//		}
//	}

//	for(iterator =0; iterator < noOfWords; iterator++)
//	{
//		printf("%s", wordArray[iterator]);
//	}
//	while(cursor_tail != cursor_head)
//	{
//		printf("%c\n", text[cursor_tail]);
//		cursor_tail++;
//	}
	return 0;
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