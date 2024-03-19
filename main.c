#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include <stdio.h>

#define TOKEN_NAME_MAX_LENGTH 30

typedef struct {
    char *key;
    int value;
} StringHashMapItem;

typedef struct {
    int variableColumnIndex;
    int popCount;
} RuleDescriptor;

char tokens[][TOKEN_NAME_MAX_LENGTH] = {
   "id", "+", "id", "*", "id", "$" 
};
int tokenIndex = 0;

char *getNextToken(void);
void parse(StringHashMapItem *tokenMap, RuleDescriptor *descriptors, int **table);

int main(void) {
    FILE *parseTableFile;
    StringHashMapItem *tokenMap;
    int rowCount, columnCount;
    RuleDescriptor *descriptors;
    int **table;

    parseTableFile = fopen("parse-table.txt", "r");
    if (!parseTableFile) {
        fprintf(stderr, "Could not find parse table file\n");
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

        descriptors = (RuleDescriptor *)malloc(sizeof(RuleDescriptor) * ruleDescriptorCount);
        
        {
            int i;
            for (i = 0; i < ruleDescriptorCount; ++i) {
                fscanf(parseTableFile, "%d %d", &descriptors[i].variableColumnIndex, &descriptors[i].popCount);
            }
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

    parse(tokenMap, descriptors, table);



    {
        int row;
        for (row = 0; row < rowCount; ++row) {
            free(table[row]);
        }
        free(table);
    }
    free(descriptors);
    shfree(tokenMap);
    fclose(parseTableFile);

    return 0;
}

char *getNextToken(void) {
    return tokens[tokenIndex++];
}


void parse(StringHashMapItem *tokenMap, RuleDescriptor *descriptors, int **table) {
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
               RuleDescriptor descriptor = descriptors[-tableValue - 1];
               int i;
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
