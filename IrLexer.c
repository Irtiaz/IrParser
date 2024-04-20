#include "IrLexer.h"
#include "stb_ds.h"

#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_REGEX_CHARS 40

#define SKIP_TOKEN "SKIP"

typedef struct {
    char *token;
    regex_t *regex;
} LexRule;

struct IrLexerStruct {
    FILE *inputFile;
    LexRule **rules;
    char buffer[BUFFER_SIZE];
    char lastReadCharacter;
};

LexRule *createLexRule(const char *tokenName, const char *regexString);
int matchRegex(const regex_t *regex, const char *string);
regex_t *prepareRegex(const char *regexString);
LexRule *getMatchingLexRule(LexRule **rules, const char *buffer);
void freeLexRule(LexRule *rule);

IrLexer *createIrLexer(FILE *ruleFile, FILE *inputFile) {
    IrLexer *irLexer = (IrLexer *)malloc(sizeof(IrLexer));

    irLexer->rules = NULL;
    memset(irLexer->buffer, 0, BUFFER_SIZE);

    while (1) {
        char tokenName[MAX_TOKEN_CHARS];
        char regexString[MAX_REGEX_CHARS];
        int readFlag = fscanf(ruleFile, "%s %s", tokenName, regexString);
        if (readFlag == EOF) break;
        arrput(irLexer->rules, createLexRule(tokenName, regexString));
    }

    irLexer->inputFile = inputFile;
    irLexer->lastReadCharacter = fgetc(inputFile);

    return irLexer;
}

void getNextToken(IrLexer *irLexer, char token[MAX_TOKEN_CHARS], char lexeme[BUFFER_SIZE]) {
    int matchFlag = 0;
    int bufferIndex = 0;

    strcpy(token, "garbage");
    strcpy(lexeme, "garbage");

    if (irLexer->lastReadCharacter == EOF) {
        strcpy(token, "$");
        strcpy(lexeme, "$");
        return;
    }

    while (irLexer->lastReadCharacter != EOF) {
        int currentlyMatching;

        irLexer->buffer[bufferIndex] = irLexer->lastReadCharacter;
        currentlyMatching = getMatchingLexRule(irLexer->rules, irLexer->buffer) != NULL;

        if (matchFlag && !currentlyMatching) {
            char *matchingToken;
            irLexer->buffer[bufferIndex] = '\0';
            matchingToken = getMatchingLexRule(irLexer->rules, irLexer->buffer)->token;
            if (strcmp(matchingToken, SKIP_TOKEN)) {
                strcpy(token, matchingToken);
                strcpy(lexeme, irLexer->buffer);
                memset(irLexer->buffer, '\0', BUFFER_SIZE);
                return;
            }

            memset(irLexer->buffer, '\0', BUFFER_SIZE);
            bufferIndex = 0;
        }
        else {
            ++bufferIndex;
            irLexer->lastReadCharacter = fgetc(irLexer->inputFile);
        }

        matchFlag = currentlyMatching;
    }

    {
        LexRule *lastRule = getMatchingLexRule(irLexer->rules, irLexer->buffer);
        if (lastRule && strcmp(lastRule->token, SKIP_TOKEN)) {
            strcpy(token, lastRule->token);
            strcpy(lexeme, irLexer->buffer);
        }
        else {
            strcpy(token, "$");
            strcpy(lexeme, "$");
        }
        return;
    }
}

void destroyIrLexer(IrLexer *irLexer) {
    {
        int i;
        for (i = 0; i < arrlen(irLexer->rules); ++i) {
            freeLexRule(irLexer->rules[i]);
        }
    }
    arrfree(irLexer->rules);
    free(irLexer);
}

int matchRegex(const regex_t *regex, const char *string) {
    int matched = regexec(regex, string, 0, NULL, 0) != REG_NOMATCH;
    return matched;
}

regex_t *prepareRegex(const char *regexString) {
    char modifiedRegexString[MAX_REGEX_CHARS];
    regex_t *regex = (regex_t *)malloc(sizeof(regex_t));

    modifiedRegexString[0] = '^';

    {
        int i;
        for (i = 0; i < MAX_REGEX_CHARS && regexString[i]; ++i) {
            modifiedRegexString[i + 1] = regexString[i];
        }
        modifiedRegexString[++i] = '$';
        modifiedRegexString[++i] = '\0';
    }

    if (regcomp(regex, modifiedRegexString, REG_EXTENDED)) {
        printf("Could not compile regex: %s\n", regexString); 
        exit(1);
    }

    return regex;
}

LexRule *createLexRule(const char *tokenName, const char *regexString) {
    LexRule *rule = (LexRule *)malloc(sizeof(LexRule));
    rule->token = (char *)malloc(strlen(tokenName) + 1);
    strcpy(rule->token, tokenName);
    rule->regex = prepareRegex(regexString);
    return rule;
}

LexRule *getMatchingLexRule(LexRule **rules, const char *buffer) {
    int i;
    for (i = 0; i < arrlen(rules); ++i) {
        if (matchRegex(rules[i]->regex, buffer)) return rules[i];
    }
    return NULL;
}


void freeLexRule(LexRule *rule) {
    regfree(rule->regex);
    free(rule->regex);
    free(rule->token);
    free(rule);
}
