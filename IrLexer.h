#ifndef IRLEX_H_325346
#define IRLEX_H_325346

#include <stdio.h>

#define MAX_TOKEN_CHARS 20
#define BUFFER_SIZE 100

typedef struct IrLexerStruct IrLexer;

IrLexer *createIrLexer(FILE *ruleFile, FILE *inputFile);
void getNextToken(IrLexer *irLexer, char token[MAX_TOKEN_CHARS], char lexeme[BUFFER_SIZE]);
void destroyIrLexer(IrLexer *irLexer);

#endif
