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
	if(isalpha(token) || isdigit(token))
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
    case UNTIL:
    case WHILE:
    case IF:
    case LPAREN:
         return 6;
         break;
    case DO:
    case DONE:
    case THEN:
    case ELSE:
    case FI:
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

command_t combine(command_t first, command_t second, operator_type op, int lineNumber) { //op cannot be LPAREN or RPAREN
     command_t combinedCommand = checked_malloc(sizeof(struct command));
     if(first == NULL || second == NULL){
          fprintf(stderr,"%d: Error combining commands: One or more input is null\n",lineNumber);
          exit(1);
     }
     switch(op) {
         case NEWLINE:
         case SEMICOLON: {
             combinedCommand->type = SEQUENCE_COMMAND;
             combinedCommand->status = -1;
             combinedCommand->input = '\0';
             combinedCommand->output = '\0';
             combinedCommand->u.word = '\0';
             combinedCommand->u.command[0] = first;
             combinedCommand->u.command[1] = second;
             break;   
          }
         case PIPE: {
             combinedCommand->type = PIPE_COMMAND;
             combinedCommand->status = -1;
             combinedCommand->input = '\0';
             combinedCommand->output = '\0';
             combinedCommand->u.word = '\0';
             combinedCommand->u.command[0] = first;
             combinedCommand->u.command[1] = second;
             break;
          }
         //(a;b) < d 
         case LARROW: {
             if(second->u.word[1] != NULL){
                fprintf(stderr,"%d: input cannot be more than 1 word\n",lineNumber);
                exit(1);
             }
             combinedCommand->type = first->type;
             combinedCommand->status = -1;
             combinedCommand->input = checked_malloc(sizeof(char*));
             combinedCommand->output = checked_malloc(sizeof(char*));
             combinedCommand->input = second->u.word[0];
             combinedCommand->output = first->output;
             combinedCommand->u = first->u;
             break;
          }
         case RARROW: {
         // (a b < c) > (d)
             if(second->u.word[1] != NULL){
                fprintf(stderr,"%d: output cannot be more than 1 word\n",lineNumber);
                exit(1);
             }
             combinedCommand->type = first->type;
             combinedCommand->status = -1;
             combinedCommand->input = checked_malloc(sizeof(char*));
             combinedCommand->output = checked_malloc(sizeof(char*));
             combinedCommand->input = first->input;
             combinedCommand->output = second->u.word[0];
             combinedCommand->u = first->u;
             break;
          }
         default: {
             fprintf(stderr,"%d: an error has occured at combine()\n",lineNumber);
             exit(1);
           break;
         }
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
     int x = 0, index;
     while(buffer[x] != NULL)
          x++;
          //printf("%d x:", x);
     char** newBuffer = checked_malloc(sizeof(char *)*(x+1));
     for(index = 0; index<x; index++) {
        newBuffer[index] = buffer[index];
        //printf("%s word\n", newBuffer[index]);
     }
     //printf("index: %d", index);
        newBuffer[index] = '\0';
           newCommand -> type = SIMPLE_COMMAND;
           newCommand -> status = -1;
           newCommand -> input = NULL;
           newCommand -> output = NULL;
           newCommand -> u.word = newBuffer;
     return newCommand;
}
command_stream_t fixOrder(someStream* stream, int size, int lineStart) {
  command_stack* myStack = create_stack(); //Command Stack
  op_stack* myOpStack = create_stackOp(); //Operator Stack
  int parenCount = 0, index = 0, wasLastOp = 0, wasLastSemi_Newline = 0, lineNumber = lineStart, elseFlag = 0,
      leadingNewLine = 1, ifFlag = 0, thenFlag = 0, elseFlag2 = 0, whileUntilFlag = 0, doFlag = 0;
  command_stream_t returnStream = checked_malloc(sizeof(struct command_stream));
  
  //printf("number of tokens %d \n", size);
  
  if(size == 0)
     return NULL;
  while((stream[size-1].amISpecial) && ((stream[size-1].op == NEWLINE) || (stream[size-1].op == SEMICOLON)))
        size--;
  for(index = 0; index<size; index++) {
       if(!stream[index].amISpecial){ //Just a simple command
          push(myStack, stream[index].command);
          wasLastOp = 0;
          leadingNewLine = 0;
          wasLastSemi_Newline = 0;
       }
       else { //is an Op
              if(stream[index].op == SEMICOLON || stream[index].op == NEWLINE)
                 wasLastSemi_Newline = 1;
              if(stream[index].op == NEWLINE){ //Checks for multiple newLines in a row              
                 lineNumber++;
                 //printf("lineNumber: %d\n",lineNumber);                 
                 if(wasLastOp || leadingNewLine)
                    continue;
                 }
  //WHILE/UNTIL LOOP
              if(stream[index].op == WHILE || stream[index].op == UNTIL)
                  whileUntilFlag = 1;
              if(stream[index].op == DO) {
                 if(whileUntilFlag) {
                    fprintf(stderr,"%d: Invalid while statement\n",lineNumber);
                    exit(1);
                 } else {
                   whileUntilFlag = 0;
                   doFlag = 1;
                 }
              }
              if(stream[index].op == DONE) {
                if(whileUntilFlag || doFlag) {
                   fprintf(stderr,"%d: Invalid while statement\n", lineNumber);
                   exit(1);    
                }
              }
  //IF Statements            
              if(stream[index].op == IF)
                 ifFlag = 1;
              if(stream[index].op == THEN){
                 if(ifFlag){
                   fprintf(stderr,"%d: Invalid if statement\n",lineNumber);
                   exit(1);
                 } else {
                    ifFlag = 0;
                    thenFlag = 1;
                 }
              }
              if(stream[index].op == ELSE) {
                 if(ifFlag || thenFlag) {
                   fprintf(stderr,"%d: Invalid if statement\n",lineNumber);
                   exit(1);
                 } else {
                     thenFlag = 0;
                     elseFlag2 = 1;
                 }
              }
              if(stream[index].op == FI) {
                 if(ifFlag || thenFlag || elseFlag2) {
                    fprintf(stderr,"%d: Invalid if statement\n",lineNumber);
                    exit(1);
                 } else {
                    elseFlag2 = 0;
                 }
              }
              if(stream[index].op != WHILE && stream[index].op != UNTIL && stream[index].op != DO && stream[index].op != DONE){
                 whileUntilFlag = 0; doFlag = 0;
              }
              
              if(stream[index].op != IF && stream[index].op != THEN && stream[index].op != ELSE && stream[index].op != FI){
                 ifFlag = 0; thenFlag = 0; elseFlag2 = 0;
              }
              /*else if((stream[index].op != IF)&&(stream[index].op != THEN)&&(stream[index].op != ELSE)&&(stream[index].op != FI))
                 wasLastSemi_Newline = 0;*/
              if(((stream[index].op != LPAREN)&&(stream[index].op != IF)&&(stream[index].op != THEN)&&(stream[index].op != ELSE)&&(stream[index].op != FI)
                   &&(stream[index].op != UNTIL)&&(stream[index].op != WHILE)&&(stream[index].op != DO)&&(stream[index].op != DONE)) && wasLastOp){
                   fprintf(stderr,"%d: Invalid operators in a row\n",lineNumber);
                   exit(1);
                 }   
              if(stream[index].op == LPAREN)
                 parenCount++;
              if(stream[index].op == RPAREN)
                 parenCount--;
              if(parenCount < 0) {
                    fprintf(stderr,"%d: Too many closing parentheses\n",lineNumber);
                    exit(1);
                 }
                 wasLastOp = 1;
                 leadingNewLine = 0;
           if(myOpStack->numItems == 0) //If Op stack is empty
                pushOp(myOpStack, stream[index].op);                
           else {
                if(stream[index].op == THEN || stream[index].op == ELSE || stream[index].op == FI || stream[index].op == DO || stream[index].op == DONE) {
                             if(wasLastSemi_Newline && (peekOp(myOpStack) == SEMICOLON || peekOp(myOpStack) == NEWLINE)){
                                popOp(myOpStack);
                                wasLastSemi_Newline = 0;
                             }
                             if(stream[index].op == DO)
                                  doFlag = 1;
                             if(stream[index].op == DONE) {
                                  popOp(myOpStack); //Pop off the DO
                                  operator_type opType = popOp(myOpStack); //Tells us if we should make a While or Until command
                                  command_t newCommand = checked_malloc(sizeof(struct command));
                                  if(opType == WHILE)
                                     newCommand -> type = WHILE_COMMAND;
                                  else
                                     newCommand -> type = UNTIL_COMMAND;
                                  newCommand -> status = -1;
                                  newCommand -> u.command[1] = pop(myStack);
                                  newCommand -> u.command[0] = pop(myStack);
                                  push(myStack, newCommand);
                                  wasLastOp = 0;
                                  continue;
                             }
                                
                             if(stream[index].op == ELSE)
                                elseFlag = 1;
                             if(stream[index].op == FI) {
                                  if(elseFlag){
                                    operator_type three = popOp(myOpStack);
                                    operator_type two = popOp(myOpStack);
                                    operator_type one = popOp(myOpStack);
                                    if(three != ELSE || two != THEN || one != IF){
                                       fprintf(stderr,"%d: Invalid If statement detected\n",lineNumber);
                                       exit(1);
                                     }
                                   }
                                   else {
                                     operator_type two = popOp(myOpStack);
                                     operator_type one = popOp(myOpStack);
                                     if(two != THEN || one != IF) {
                                       fprintf(stderr,"%d: Invalid If statement detected\n",lineNumber);
                                       exit(1);
                                     }
                                   }
                                   command_t ifCommand = checked_malloc(sizeof(struct command));
                                   ifCommand -> type = IF_COMMAND;
                                   ifCommand -> status = -1;
                                if(elseFlag){
                                   ifCommand -> u.command[2] = pop(myStack);
                                   ifCommand -> u.command[1] = pop(myStack);
                                   ifCommand -> u.command[0] = pop(myStack);
                                }
                                else{
                                   ifCommand -> u.command[1] = pop(myStack);
                                   ifCommand -> u.command[0] = pop(myStack);
                                }
                                push(myStack, ifCommand);
                                elseFlag = 0;
                                wasLastOp = 0;
                                continue;
                             }
                    }
                if(precedence(stream[index].op) > precedence(peekOp(myOpStack)))
                   pushOp(myOpStack, stream[index].op);
                else {
                    while(((peekOp(myOpStack) != LPAREN) && (peekOp(myOpStack) != IF) && (peekOp(myOpStack) != THEN) && (peekOp(myOpStack) != ELSE)
                           && (peekOp(myOpStack) != WHILE) && (peekOp(myOpStack) != DO) && (peekOp(myOpStack) != UNTIL)) 
                           && (precedence(stream[index].op) <= precedence(peekOp(myOpStack)))) {
                          operator_type theOp = popOp(myOpStack);
                          command_t secondCommand = pop(myStack);
                          command_t firstCommand = pop(myStack);
                          push(myStack, combine(firstCommand, secondCommand, theOp, lineNumber));
                          if(myOpStack->numItems == 0)
                             break;
                     }
                     pushOp(myOpStack, stream[index].op);      
                }
                if(stream[index].op == RPAREN){
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
  /*if((myOpStack->numItems == 0 && (myStack->numItems == 0)))
        return NULL;*/
  if((myOpStack->numItems) != (myStack->numItems - 1)) {  
        //printf("OpStack %d: \n", myOpStack->numItems);
        //printf("Stack %d: \n", myStack->numItems);
        fprintf(stderr,"%d: Too many/few operators detected\n",lineNumber);
        exit(1);
     }   
  while(myOpStack->numItems  != 0) {  
       command_t secondCommand = pop(myStack);
       command_t firstCommand = pop(myStack);       
       operator_type op = popOp(myOpStack);
       push(myStack, combine(firstCommand, secondCommand, op, lineNumber));
   }
    returnStream -> command = pop(myStack);
   //printf("size: %d", size);
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
				fprintf(stderr,"%d: Invalid Comment Syntax \n",lineNumber);
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
					fprintf(stderr,"%d: Invalid character \n",lineNumber);
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
int lineNumber = 1, i=0, bufferFlag=0, noOfStreams=0, specialTokenFlag = 0,reasonableCommandSize = 100, numWordsInCurrentCommand = 0, noOfTokens = 0, streamSizeLimit=100;
  int start = 0, tempLineNumber=1,ifFlag = 0,thenFlag, closingFlag=0, untilWhileFlag=0, doFlag=0,newLineSemiColonFlag=0,syntaxFlag=0;
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
	lineNumber++;
	tempLineNumber++;
	i++;
  }
	start = i;
	for(i=start; i<noOfTokens; i++)
	{
		if(specialTokens(tokens[i][0])==NEWLINE)
		{
			lineNumber++;
		}
		
		if(specialTokens(tokens[i][0])!=EMPTY)
		{
			bufferFlag = 0;
			
			if(syntaxFlag && specialTokens(tokens[i][0])== NEWLINE && specialTokenFlag)
			{
				fprintf(stderr,"%d: Syntax mismatch\n", lineNumber);
				exit(1);
			}
			else if(newLineSemiColonFlag && specialTokens(tokens[i][0])== RPAREN && specialTokenFlag)
			{
				stream[noOfStreams-1].op = specialTokens(tokens[i][0]);
			}
			else if(newLineSemiColonFlag && (specialTokens(tokens[i][0])== RARROW || specialTokens(tokens[i][0])== LARROW || specialTokens(tokens[i][0])== PIPE  || specialTokens(tokens[i][0])== SEMICOLON ) && specialTokenFlag)
			{
				fprintf(stderr,"%d: Syntax mismatch\n", lineNumber);
				exit(1);
			}
			else if(specialTokenFlag || i == start)
			{
				stream[noOfStreams].op = specialTokens(tokens[i][0]);
				stream[noOfStreams].amISpecial = 1;
				noOfStreams++;
				if(noOfStreams==(streamSizeLimit-10))
				{
					streamSizeLimit *=2;
					stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
				}
				specialTokenFlag =1;
			}
			else
			{
				stream[noOfStreams].amISpecial = 0;
				buffer[numWordsInCurrentCommand]=NULL;
				stream[noOfStreams].command=makeSimpleCommand(buffer);
				noOfStreams++;
				reasonableCommandSize = 100;
				numWordsInCurrentCommand=0;
				buffer = checked_malloc(sizeof(char*)*reasonableCommandSize);
				if(noOfStreams==(streamSizeLimit-10))
				{
					streamSizeLimit *=2;
					stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
				}
				stream[noOfStreams].op = specialTokens(tokens[i][0]);
				stream[noOfStreams].amISpecial = 1;
				noOfStreams++;
				if(noOfStreams==(streamSizeLimit-10))
				{
					streamSizeLimit *=2;
					stream = checked_realloc(stream, sizeof(someStream)*streamSizeLimit);
				}
				specialTokenFlag =1;
			}
			
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
					fprintf(stderr,"%d: If statements mismatch\n", lineNumber);
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
					fprintf(stderr,"%d: If statements mismatch\n", lineNumber);
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
					fprintf(stderr,"%d: If statements mismatch\n", lineNumber);
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
					fprintf(stderr,"%d: While/Until statements mismatch\n", lineNumber);
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
					fprintf(stderr,"%d: While/Until statements mismatch\n", lineNumber);
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
	if(bufferFlag)
	{
		stream[noOfStreams].amISpecial = 0;
		stream[noOfStreams].command=makeSimpleCommand(buffer);
		noOfStreams++;
	}
/*	i=0;
	while(i<noOfStreams)
	{
		printf("Stream special : %u ", stream[i].amISpecial);
		if(stream[i].amISpecial)
		{
			printf("Type :%u", stream[i].op);
		}
		else
		{
			int k =0;
			while(stream[i].command->u.word[k] !=NULL)
			{
				printf("%s",stream[i].command->u.word[k]);
				k++;
			}
		}
		printf("\n");
		i++;
	}*/
	//Code Starts Here
	i=0;
	lineNumber =tempLineNumber;
	int counter=0;
	int numberOfNodes=0;
	int newLineFlag=0;
	int doneExecution=1;
	int skip =0;
	ifFlag=0;
	int whileUntilFlag=0;
	someStream* streamIterator = stream;
	
	
	for(i=0;i<noOfStreams;i++)
	{
		if(stream[i].amISpecial && stream[i].op==NEWLINE)
		{
			tempLineNumber++;
		}
		if(doneExecution)
		{
			if(stream[i].amISpecial && stream[i].op==NEWLINE)
			{
				skip =1;
			}
			else
			{
				lineNumber = tempLineNumber;
				streamIterator = &stream[i];
				doneExecution=0;
			}
		}
		if(skip!=1)
		{
			if(stream[i].amISpecial)
			{
				if(newLineFlag&&stream[i].op==NEWLINE&&ifFlag==0&&whileUntilFlag==0)
				{
					if(numberOfNodes==0)
					{
						the_stream_head = fixOrder(streamIterator,counter,lineNumber);
						doneExecution=1;
						counter=0;
						numberOfNodes++;
					}
					else
					{
						command_stream_t commandNodeIterator=the_stream_head;
						while(commandNodeIterator->next != NULL)
						{
							commandNodeIterator = commandNodeIterator->next;
						}
						commandNodeIterator->next = checked_malloc(sizeof(struct command_stream));
						commandNodeIterator->next = fixOrder(streamIterator,counter,lineNumber);
						doneExecution=1;
						counter=0;
						numberOfNodes++;
					}
				}
				else if(stream[i].op==NEWLINE)
				{
					newLineFlag =1;
					counter++;
				}
				else
				{
					newLineFlag=0;
					counter++;
				}
				if(stream[i].op==IF)
				{
					ifFlag++;
				}
				if(stream[i].op==FI)
				{
					ifFlag--;
				}
				if(stream[i].op==WHILE||stream[i].op==UNTIL)
				{
					whileUntilFlag++;
				}
				if(stream[i].op==DONE)
				{
					whileUntilFlag--;
				}
			}
			else
			{
				newLineFlag=0;
				counter++;
			}
		}
		skip =0;
	}
	if(counter!=0)
	{
		if(numberOfNodes==0)
		{
			the_stream_head = fixOrder(stream,noOfStreams,lineNumber);
			numberOfNodes++;
		}
		else
		{
			command_stream_t commandNodeIterator=the_stream_head;
			while(commandNodeIterator->next != NULL)
			{
				commandNodeIterator = commandNodeIterator->next;
			}
			commandNodeIterator->next = checked_malloc(sizeof(struct command_stream));
			commandNodeIterator->next = fixOrder(streamIterator,counter,lineNumber);
			numberOfNodes++;
		}
	}
   return the_stream_head;
}

command_t	
read_command_stream (command_stream_t s)		// Creates a stack with commands
{
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

		return temp;
	}
	else return NULL;
}