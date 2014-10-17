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

#include "op-stack.h"
#include "command-stack.h"
#include "command.h"
#include "command-internals.h"

#include "stdio.h"
#include "string.h"
#include <error.h>
#include <stdlib.h>
#include <ctype.h>

int legitCharacters(char token)
{
	if(isalpha(token))
	{
		return 1;
	}
	switch (token)
	{
		case '!':
		case '%':
		case '+':
		case ',':
		case '-':
		case '.':
		case '/':
		case ':':
		case '@':
		case '^':
		case '_':
		return 1;
		default:
		return 0;
	}
}

operator_type specialKeywords(char* string)
{
	if(strcmp(string, "if")==0)
		return IF;
	if(strcmp(string, "then")==0)
		return THEN;
	if(strcmp(string, "else")==0)
		return ELSE;
	if(strcmp(string, "fi")==0)
		return FI;
	if(strcmp(string, "while")==0)
		return WHILE;
	if(strcmp(string, "do")==0)
		return DO;
	if(strcmp(string, "until")==0) 
		return UNTIL;
	if(strcmp(string, "done")==0)
		return DONE;
	else
	return EMPTY;
}

operator_type specialTokens(char token)
{
	if(token=='\n')
	  return NEWLINE;
	else if(token==';')
		return SEMICOLON;
	else if(token=='|')
		return PIPE;
	else if(token=='(')
		return LPAREN;
	else if(token==')')
		return RPAREN;
	else if(token=='<')
		return LARROW;
	else if(token=='>')
		return RARROW;
	else
		return EMPTY;
}

int precedence(enum operator_type op) {
    switch(op) {
    case LPAREN:
         return 6;
         break;
    case RPAREN:
         return 1;
         break;
    case PIPE:
         return 4;
         break;
    case LARROW:
         return 5;
         break;
    case RARROW: 
         return 5;
         break;
    case SEMICOLON:
         return 3;
    case NEWLINE:
         return 2;
         break;
    default:
         return 0;
         break;
    }
}

typedef struct someStream {
       int amISpecial;
       command_t command;
       enum operator_type op;
}someStream;

command_t combine(command_t first, command_t second, operator_type op) { //op cannot be LPAREN or RPAREN
     command_t combinedCommand = checked_malloc(sizeof(struct command));
     switch(op) {
         case NEWLINE:
         case SEMICOLON: {
             combinedCommand->type = SEQUENCE_COMMAND;
             combinedCommand->status = -1;
             combinedCommand->input = NULL;
             combinedCommand->output = NULL;
             combinedCommand->u.word = NULL;
             combinedCommand->u.command[0] = first;
             combinedCommand->u.command[1] = second;
             break;   
          }
         case PIPE: {
             combinedCommand->type = PIPE_COMMAND;
             combinedCommand->status = -1;
             combinedCommand->input = NULL;
             combinedCommand->output = NULL;
             combinedCommand->u.word = NULL;
             combinedCommand->u.command[0] = first;
             combinedCommand->u.command[1] = second;
             break;
          }
         case LARROW: {
             combinedCommand->type = SIMPLE_COMMAND;
             combinedCommand->status = -1;
             if(second->type == SIMPLE_COMMAND)
             combinedCommand->input = second->u.word[0];
             combinedCommand->output = NULL;
             combinedCommand->u.word = first->u.word;
             break;
          }
         case RARROW: {
         // (a b < c) > (d)
             combinedCommand->type = SIMPLE_COMMAND;
             combinedCommand->status = -1;
             combinedCommand->input = first->input;
             combinedCommand->output = second->u.word[0];
             combinedCommand->u.word = first->u.word;
             break;
          }
         default:
           error(1,0,"an error has occured at combine()");
     }
   return combinedCommand;
}

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
   
/*struct commandNode {
      command_t command;
      //struct CommandNode* next;
};*/

struct command_stream {
       command_t command;
       struct command_stream* prev;
       struct command_stream* next;
};

command_t makeSimpleCommand(char** buffer) {
     command_t newCommand = checked_malloc(sizeof(struct command));
           newCommand -> type = SIMPLE_COMMAND;
           newCommand -> status = -1;
           newCommand -> input = NULL;
           newCommand -> output = NULL;
           newCommand -> u.word = buffer;
     return newCommand;
}

command_stream_t fixOrder(someStream* stream, int size, int lineStart) {
  command_stack* myStack = create_stack(); //Command Stack
  op_stack* myOpStack = create_stackOp(); //Operator Stack
int parenCount = 0, index = 0, semiFlag = 0, leftArrowFlag = 0, pipeFlag = 0,
      rightArrowFlag = 0, wasLastOp = 0, lineNumber = lineStart; //Keeps track of lineNumber
  //command_stream_t headPtr = checked_malloc(sizeof(struct command_stream));
  command_stream_t returnStream = checked_malloc(sizeof(struct command_stream));
  printf("number of tokens %d \n", size);
  
  if(size == 0)
     return NULL;
  
  while((stream[size-1].amISpecial) && ((stream[size-1].op == NEWLINE) || (stream[size-1].op == SEMICOLON)))
        size--;
     
  for(index = 0; index<size; index++) {  
       if(!stream[index].amISpecial){ //Just a simple command
          /*int index2 = 0;
          while(stream[index].command->u.word[index2] != "\0"){
              printf("My words are: %s\n", stream[index].command->u.word[index2]);
              index2++;
              }*/
          push(myStack, stream[index].command);
          wasLastOp = 0;
       }
       else { //is an Op
              if(stream[index].op == NEWLINE){ //Checks for multiple newLines in a row
                 lineNumber++;
                 if(wasLastOp)
                    continue;
                 } 
              if(stream[index].op == SEMICOLON){ //Checks for multiple semicolons in a row
                   if(semiFlag && stream[index-1].op == SEMICOLON) {
                        fprintf(stderr,"%d: Multiple semicolons in a row\n ",lineNumber);
   	                    exit(1);
                     }
                   else if (semiFlag && stream[index].op != SEMICOLON)
                      semiFlag = 0;
                   else
                      semiFlag = 1;
                 }
              if(stream[index].op == PIPE){ //Checks for multiple pipes in a row
                   if(pipeFlag && stream[index-1].op == PIPE) {
                        fprintf(stderr,"%d: Multiple piepes in a row\n ",lineNumber);
   	                    exit(1);
                     }
                   else if (pipeFlag && stream[index].op != PIPE)
                      pipeFlag = 0;
                   else
                      pipeFlag = 1;
                 }
              if(stream[index].op == LARROW){ //Checks for multiple <s in a row
                   if(leftArrowFlag && stream[index-1].op == LARROW) {
                        fprintf(stderr,"%d: Multiple left arrows in a row\n ",lineNumber);
   	                    exit(1);
                     }
                   else if (leftArrowFlag && stream[index].op != LARROW)
                      leftArrowFlag = 0;
                   else
                      leftArrowFlag = 1;
                 }
              if(stream[index].op == RARROW){ //Checks for multiple <s in a row
                   if(rightArrowFlag && stream[index-1].op == RARROW) {
                        fprintf(stderr,"%d: Multiple right arrows in a row\n ",lineNumber);
   	                    exit(1);
                     }
                   else if (rightArrowFlag && stream[index].op != RARROW)
                      rightArrowFlag = 0;
                   else
                      rightArrowFlag = 1;
                 }
              if(stream[index].op == LPAREN)
                 parenCount++;
              if(stream[index].op == RPAREN)
                 parenCount--;
              if(parenCount < 0) {
                    fprintf(stderr,"%d: Too many closing parentheses\n ",lineNumber);
                    exit(1);
                 }
                 wasLastOp = 1;
           if(myOpStack->numItems == 0) //If Op stack is empty
                pushOp(myOpStack, stream[index].op);                
           else {
                if(precedence(stream[index].op) > precedence(peekOp(myOpStack)))
                   pushOp(myOpStack, stream[index].op);
                else {
                    while((peekOp(myOpStack) != LPAREN) 
                          && (precedence(stream[index].op) <= precedence(peekOp(myOpStack)))) {
                          /*if((myOpStack->numItems) != (myStack->numItems - 1)){
                              error(1, 0, "%d: Too many/few operators detected\n", lineNumber);
                            }*/
                          operator_type theOp = popOp(myOpStack);
                          command_t secondCommand = pop(myStack);
                          command_t firstCommand = pop(myStack);
                          push(myStack, combine(firstCommand, secondCommand, theOp));
                          if(myOpStack->numItems == 0)
                             break;
                     }
                     pushOp(myOpStack, stream[index].op);      
                }
                if(stream[index].op == RPAREN){
                  /*if((myOpStack->numItems) != (myStack->numItems - 1)){
                     error(1, 0, "%d: Too many/few operators detected\n", lineNumber);
                     }*/
                     popOp(myOpStack);
                     popOp(myOpStack);
                     command_t subShellCommand = checked_malloc(sizeof(struct command));
                     subShellCommand -> type = SUBSHELL_COMMAND;
                     subShellCommand -> status = -1;
                     subShellCommand -> input = NULL;
                     subShellCommand -> output = NULL;
                     subShellCommand -> u.command[0] = pop(myStack);
                     push(myStack, subShellCommand);
                     wasLastOp = 0;
                  }
           }
       }
  }
  if((myOpStack->numItems) != (myStack->numItems - 1)) {    
        fprintf(stderr,"%d: Too many/few operators detected\n ",lineNumber);
        exit(1);
     }   

  while(myOpStack->numItems  != 0) {  
       command_t secondCommand = pop(myStack);
       command_t firstCommand = pop(myStack);       
       operator_type op = popOp(myOpStack);
       push(myStack, combine(firstCommand, secondCommand, op));
   }
    returnStream -> command = pop(myStack);
 
  return returnStream;
}
//Checks for invalid letters, removes extra spaces, comments, etc
char** parseAndRemove(int (*get_next_byte) (void *),
       void *get_next_byte_argument, int* noOfTokens) {
int TOKEN_SIZE = 10;
	int WORD_SIZE = 10;
	int noOfChars = 0;
	int lineNumber = 1;
	int wordFlag = 0;
	//int noOfTokens = 0;
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
				tokens[*noOfTokens]= word;
				(*noOfTokens)++;
				if(*noOfTokens==TOKEN_SIZE)
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
				fprintf(stderr,"Invalid Comment Syntax at line number: %d \n ",lineNumber );
				exit(1);
			}
			commentFlag = 1;
		}
		else
		if(tempChar != ' ' && tempChar != '\t')
		{
			if(specialTokens(tempChar)!=EMPTY)
			{
				if(wordFlag==1)
				{
					word[noOfChars] = '\0';
					tokens[*noOfTokens]= word;
					(*noOfTokens)++;
					if(*noOfTokens==TOKEN_SIZE)
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
				tokens[*noOfTokens]= word;
				(*noOfTokens)++;
				if(*noOfTokens==TOKEN_SIZE)
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
				if(!legitCharacters(tempChar))
				{
					fprintf(stderr,"Invalid character at line number: %d \n ",lineNumber );
					exit(1);
				}
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
				tokens[*noOfTokens]= word;
				(*noOfTokens)++;
				if(*noOfTokens==TOKEN_SIZE)
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
		tokens[*noOfTokens]= word;
		(*noOfTokens)++;
	}
  return tokens;                
}

command_stream_t
make_command_stream (int (*get_next_byte) (void *),		// Parse texts
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
/* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
 int lineNumber = 1, i=0, bufferFlag=0, noOfStreams=0, specialTokenFlag = 0,reasonableCommandSize = 100, numWordsInCurrentCommand = 0, noOfTokens = 0, streamSizeLimit=100;
  int ifFlag = 0,thenFlag, closingFlag=0, untilWhileFlag=0, doFlag=0, start = 0, newLineSemiColonFlag=0, syntaxFlag =0;
  char** tokens = parseAndRemove((*get_next_byte), get_next_byte_argument, &noOfTokens);
  char** buffer = checked_malloc(sizeof(char*)*reasonableCommandSize);
  command_stream_t the_stream_head = checked_malloc(sizeof(struct command_stream));
  someStream* stream = checked_malloc(sizeof(someStream)*streamSizeLimit);
  the_stream_head -> command = NULL;
  the_stream_head -> prev = NULL;
  the_stream_head -> next = NULL;
  //command_stream_t streamPtr = the_stream_head;
  //command_stack* myStack = create_stack();
  //op_stack* myOpStack = create_stackOp();
  
  while(specialTokens(tokens[i][0])==NEWLINE)
  {
	i++;
  }
  start = i;
  
	for(i=start; i<noOfTokens; i++)
	{
		
		if(specialTokens(tokens[i][0])!=EMPTY)
		{
			bufferFlag = 0;
			
			if(syntaxFlag && specialTokens(tokens[i][0])== NEWLINE && specialTokenFlag)
			{
				fprintf(stderr,"Syntax mismatch\n ");
				exit(1);
			}
			else if(newLineSemiColonFlag && specialTokens(tokens[i][0])== RPAREN && specialTokenFlag)
			{
				stream[noOfStreams-1].op = specialTokens(tokens[i][0]);
			}
			else if(newLineSemiColonFlag && (specialTokens(tokens[i][0])== RARROW || specialTokens(tokens[i][0])== LARROW || specialTokens(tokens[i][0])== PIPE  || specialTokens(tokens[i][0])== SEMICOLON ) && specialTokenFlag)
			{
				fprintf(stderr,"Syntax mismatch\n ");
				exit(1);
			}
			else if(specialTokenFlag || i == start)
			{
				stream[noOfStreams].op = specialTokens(tokens[i][0]);
				stream[noOfStreams].amISpecial = 1;
				noOfStreams++;
				if(noOfStreams==streamSizeLimit)
				{
					streamSizeLimit *=2;
					stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
				}
				specialTokenFlag =1;
			}
			else
			{
				stream[noOfStreams].amISpecial = 0;
				stream[noOfStreams].command=makeSimpleCommand(buffer);
				noOfStreams++;
				reasonableCommandSize = 100;
				numWordsInCurrentCommand=0;
				buffer = checked_malloc(sizeof(char*)*reasonableCommandSize);
				if(noOfStreams+1==streamSizeLimit)
				{
					streamSizeLimit *=2;
					stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
				}
				stream[noOfStreams].op = specialTokens(tokens[i][0]);
				stream[noOfStreams].amISpecial = 1;
				noOfStreams++;
				specialTokenFlag =1;
			}
			if(specialTokens(tokens[i][0])== NEWLINE || specialTokens(tokens[i][0])== SEMICOLON)
				newLineSemiColonFlag=1;
			else
				newLineSemiColonFlag=0;
			if(specialTokens(tokens[i][0])== RARROW || specialTokens(tokens[i][0])== LARROW )
			{
				syntaxFlag=1;
			}
			else
				syntaxFlag=0;
			
		}
		else if((specialTokenFlag || i==start) && specialKeywords(tokens[i]) != EMPTY) //Here
		{
			specialTokenFlag=1;
			if(specialKeywords(tokens[i])==IF)
			{
				closingFlag++;
				ifFlag++;
				stream[noOfStreams].op = specialKeywords(tokens[i]);
				stream[noOfStreams].amISpecial = 1;
				noOfStreams++;
				if(noOfStreams==streamSizeLimit)
				{
					streamSizeLimit *=2;
					stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
				}
			}
			if((specialKeywords(tokens[i])==THEN))
			{
				if(ifFlag)
				{
					thenFlag++;
					ifFlag--;
					stream[noOfStreams].op = specialKeywords(tokens[i]);
					stream[noOfStreams].amISpecial = 1;
					noOfStreams++;
					if(noOfStreams==streamSizeLimit)
					{
						streamSizeLimit *=2;
						stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
					}
				}
				else
				{
					fprintf(stderr,"If statements mismatch\n ");
					exit(1);
				}
			}
			if((specialKeywords(tokens[i])==ELSE))
			{
				if(thenFlag)
				{
					thenFlag--;
					stream[noOfStreams].op = specialKeywords(tokens[i]);
					stream[noOfStreams].amISpecial = 1;
					noOfStreams++;
					if(noOfStreams==streamSizeLimit)
					{
						streamSizeLimit *=2;
						stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
					}
				}
				else
				{
					fprintf(stderr,"If statements mismatch\n ");
					exit(1);
				}
			}
			if((specialKeywords(tokens[i])==FI))
			{
				if(closingFlag)
				{
					closingFlag--;
					stream[noOfStreams].op = specialKeywords(tokens[i]);
					stream[noOfStreams].amISpecial = 1;
					noOfStreams++;
					if(noOfStreams==streamSizeLimit)
					{
						streamSizeLimit *=2;
						stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
					}
				}
				else
				{
					fprintf(stderr,"If statements mismatch\n ");
					exit(1);
				}
			}
			if((specialKeywords(tokens[i])==WHILE)||(specialKeywords(tokens[i])==UNTIL))
			{
				untilWhileFlag++;
				stream[noOfStreams].op = specialKeywords(tokens[i]);
				stream[noOfStreams].amISpecial = 1;
				noOfStreams++;
				if(noOfStreams==streamSizeLimit)
				{
					streamSizeLimit *=2;
					stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
				}
			}
			if((specialKeywords(tokens[i])==DO))
			{
				if(untilWhileFlag)
				{
					doFlag++;
					untilWhileFlag--;
					stream[noOfStreams].op = specialKeywords(tokens[i]);
					stream[noOfStreams].amISpecial = 1;
					noOfStreams++;
					if(noOfStreams==streamSizeLimit)
					{
						streamSizeLimit *=2;
						stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
					}
				}
				else
				{
					fprintf(stderr,"While/Until statements mismatch\n ");
					exit(1);
				}
			}
			if((specialKeywords(tokens[i])==DONE))
			{
				if(doFlag)
				{
					doFlag--;
					stream[noOfStreams].op = specialKeywords(tokens[i]);
					stream[noOfStreams].amISpecial = 1;
					noOfStreams++;
					if(noOfStreams==streamSizeLimit)
					{
						streamSizeLimit *=2;
						stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
					}
				}
				else
				{
					fprintf(stderr,"While/Until statements mismatch\n ");
					exit(1);
				}
			}
		}
		else
		{
			bufferFlag =1;
			specialTokenFlag = 0;
			buffer[numWordsInCurrentCommand] = tokens[i];
			numWordsInCurrentCommand++;
			if(numWordsInCurrentCommand==reasonableCommandSize)
			{
				reasonableCommandSize*=2;
				buffer = checked_realloc(buffer,sizeof(char*)*reasonableCommandSize);
			}
		}
	}
	if(bufferFlag)
	{
		stream[noOfStreams].amISpecial = 0;
		stream[noOfStreams].command=makeSimpleCommand(buffer);
		noOfStreams++;
	}
	i=0;
	while(i<noOfStreams)
	{
		printf("Stream special : %u ", stream[i].amISpecial);
		if(stream[i].amISpecial)
		{
			printf("Type :%u", stream[i].op);
		}
		printf("\n");
		i++;
	}
	   the_stream_head=fixOrder(stream,noOfStreams,1);
   return the_stream_head;


}

command_t	
read_command_stream (command_stream_t s)		// Creates a stack with commands
{
  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");
         /*if(s->command->u.command[0]->type == SIMPLE_COMMAND)
         printf("First command is of type Simple\n");
         printf("%s \n", s -> command -> u.command[0] -> u.word[0]);
         printf("%s \n", s -> command -> u.command[0] -> u.word[1]);*/
         
         /*if(s->command->u.command[0]->type == SIMPLE_COMMAND)
         printf("Second command is of type Simple\n");
         printf("%s \n", s -> command -> u.command[1] -> u.word[0]);
         printf("%s \n", s -> command -> u.command[1] -> u.word[1]);*/

if(s->command)
	{
		command_t temp = s->command;
		if(s->next)
		{
			s->command = s->next->command;
			s->next = s->next->next;
		}
		else
		s->command = NULL;
		//printf("command's 2nd word %s \n", temp->u.command[0]->u.word[1]);

		return temp;
	}
	else return NULL;
}