#ifndef IR_PARSER_543547
#define IR_PARSER_543547

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define TOKEN_NAME_MAX_LENGTH 30

typedef struct {
    char *key;
    int value;
} IrParseStringHashMapItem;

typedef struct {
    int variableColumnIndex;
    int popCount;
} IrParseRuleDescriptor;


typedef struct {
    IrType value;
    char *lexVal;
} IrParseStackItem;

typedef struct {
    IrParseStringHashMapItem *tokenMap;
    IrParseRuleDescriptor *descriptors;
    int **table;
    int rowCount;
    int columnCount;
    
    void (**ruleHandlers)(void);

} IrParser;

char tokens[][TOKEN_NAME_MAX_LENGTH] = {
   "id", "+", "id", "*", "id", "$" 
};
int tokenIndex = 0;
char *getNextToken(void);

IrParser *createIrParser(const char *parseTableFileName, ...);
void irParse(IrParser *irParser);
void destroyIrParser(IrParser *irParser);

IrParser *createIrParser(const char *parseTableFileName, ...) {
    IrParser *irParser;

    FILE *parseTableFile;
    IrParseStringHashMapItem *tokenMap;
    int rowCount, columnCount;
    IrParseRuleDescriptor *descriptors;
    int **table;
    void (**ruleHandlers)(void);

    parseTableFile = fopen(parseTableFileName, "r");
    if (!parseTableFile) {
        fprintf(stderr, "Could not find parse table file: %s\n", parseTableFileName);
        exit(1);
    }
    
    sh_new_strdup(tokenMap);
    {
        int numberOfTokens, tokenStart;
        fscanf(parseTableFile, "%d %d", &numberOfTokens, &tokenStart);

        {
            int i;
            for (i = 0; i < numberOfTokens; ++i) {
                char tokenName[TOKEN_NAME_MAX_LENGTH];
                fscanf(parseTableFile, "%s", tokenName);
                
                shput(tokenMap, tokenName, tokenStart++);
            }
        }

        shput(tokenMap, "#", tokenStart++);
        shput(tokenMap, "$", tokenStart++);

        columnCount = tokenStart;
    }
    
    {
        int ruleDescriptorCount;
        fscanf(parseTableFile, "%d", &ruleDescriptorCount);

        descriptors = (IrParseRuleDescriptor *)malloc(sizeof(IrParseRuleDescriptor) * ruleDescriptorCount);
        
        {
            int i;
            for (i = 0; i < ruleDescriptorCount; ++i) {
                fscanf(parseTableFile, "%d %d", &descriptors[i].variableColumnIndex, &descriptors[i].popCount);
            }
        }

        {
            va_list rulesArgumentList;
            int i;
            
            ruleHandlers = (void(**)(void))malloc(sizeof(void(**)(void)) * ruleDescriptorCount);

            va_start(rulesArgumentList, ruleDescriptorCount);
            
            for (i = 0; i < ruleDescriptorCount; ++i) {
                ruleHandlers[i] = va_arg(rulesArgumentList, void(*)(void));
            }
            
            va_end(rulesArgumentList);
        }
    }

    {
        fscanf(parseTableFile, "%d", &rowCount); 
        
        table = (int **)malloc(rowCount * sizeof(int *));
        {
            int row;
            for (row = 0; row < rowCount; ++row) {
                table[row] = (int *)malloc(columnCount * sizeof(int));
            }
        }

        {
            int row;
            for (row = 0; row < rowCount; ++row) {
                int column;
                for (column = 0; column < columnCount; ++column) {
                    fscanf(parseTableFile, "%d", &table[row][column]);
                }
            }
        }
    }
    fclose(parseTableFile);

    irParser = (IrParser *)malloc(sizeof(IrParser));
    irParser->tokenMap = tokenMap;
    irParser->descriptors = descriptors;
    irParser->table = table;
    irParser->rowCount = rowCount;
    irParser->columnCount = columnCount;
    irParser->ruleHandlers = ruleHandlers;

    return irParser;
}

void irParse(IrParser *irParser) {
    IrParseStringHashMapItem *tokenMap = irParser->tokenMap;
    IrParseRuleDescriptor *descriptors = irParser->descriptors;
    int **table = irParser->table;
    void (**ruleHandlers)(void) = irParser->ruleHandlers;

    int *stateStack = NULL;
    int lookahead = shget(tokenMap, getNextToken());
    int inputRightEndMarker = shget(tokenMap, "$");

    arrput(stateStack, 0);

    while (1) {
       int currentState = stateStack[arrlen(stateStack) - 1]; 
       int tableValue = table[currentState][lookahead];

       if (currentState == 1 && lookahead == inputRightEndMarker) {
           puts("Successfully parsed");
           break;
       }

       if (tableValue) {

           if (tableValue > 0) {
               arrput(stateStack, tableValue);
               lookahead = shget(tokenMap, getNextToken());
           }
           else {
               int ruleIndex = -tableValue - 1;
               IrParseRuleDescriptor descriptor = descriptors[ruleIndex];
               int i;

               if (ruleHandlers[ruleIndex]) {
                   (*ruleHandlers[ruleIndex])();
               }

               for (i = 0; i < descriptor.popCount; ++i) {
                   arrdel(stateStack, arrlen(stateStack) - 1);
               }

               currentState = stateStack[arrlen(stateStack) - 1]; 
               tableValue = table[currentState][descriptor.variableColumnIndex];

               if (tableValue) arrput(stateStack, tableValue);
               else {
                   puts("Syntax error");
                   break;
               }
           }

       }

       else {
           puts("Syntax error");
           break;
       }
    }

    arrfree(stateStack);
}

void destroyIrParser(IrParser *irParser) {
    int row;
    for (row = 0; row < irParser->rowCount; ++row) {
        free(irParser->table[row]);
    }
    free(irParser->table);
    free(irParser->descriptors);
    shfree(irParser->tokenMap);
    free(irParser->ruleHandlers);

    free(irParser);
}


char *getNextToken(void) {
    return tokens[tokenIndex++];
}

#endif
