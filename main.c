#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define IrType int
#include "IrParser.h"

#include <stdio.h>
#include <stdlib.h>

int E_E_PLUS_T(IrParseStackItem *items);
int E_T(IrParseStackItem *items);
int F_id(IrParseStackItem *items);
int F_LPAREN_E_RPAREN(IrParseStackItem *items);
int T_T_INTO_F(IrParseStackItem *items);
int T_F(IrParseStackItem *items);

void errorHandler(void);

int main(void) { 
    IrParser *irParser = createIrParser("parse-table.txt", errorHandler, E_E_PLUS_T, E_T, F_id, F_LPAREN_E_RPAREN, T_T_INTO_F, T_F);
    int finalResult = irParse(irParser);

    printf("%d\n", finalResult);

    destroyIrParser(irParser);
    return 0;
}

int E_E_PLUS_T(IrParseStackItem *items) {
    return items[0].value + items[2].value;
}

int E_T(IrParseStackItem *items) {
    return items[0].value;
}

int F_id(IrParseStackItem *items) {
    return atoi(items[0].lexVal);
}

int F_LPAREN_E_RPAREN(IrParseStackItem *items) {
    return items[1].value;
}

int T_T_INTO_F(IrParseStackItem *items) {
    return items[0].value * items[2].value;
}

int T_F(IrParseStackItem *items) {
    return items[0].value;
}

void errorHandler(void) {
    puts("Synatx error");
}
