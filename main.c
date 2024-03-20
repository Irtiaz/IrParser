#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define IrType int
#include "IrParser.h"

void E_E_PLUS_T(void);
void E_T(void);
void F_id(void);
void F_LPAREN_E_RPAREN(void);
void T_T_INTO_F(void);
void T_F(void);

int main(void) {    
    IrParser *irParser = createIrParser("parse-table.txt", E_E_PLUS_T, E_T, F_id, F_LPAREN_E_RPAREN, T_T_INTO_F, T_F);
    irParse(irParser);

    destroyIrParser(irParser);
    return 0;
}

void E_E_PLUS_T(void) {
    puts("E -> E + T");
}
void E_T(void) {
    puts("E -> T");
}
void T_T_INTO_F(void) {
    puts("T -> T * F");
}
void T_F(void) {
    puts("T -> F");
}
void F_id(void) {
    puts("F -> id");
}
void F_LPAREN_E_RPAREN(void) {
    puts("F -> (E)");
}
