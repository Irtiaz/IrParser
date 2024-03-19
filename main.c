#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define IrType int
#include "IrParser.h"

int main(void) {    
    IrParser *irParser = createIrParser("parse-table.txt");
    irParse(irParser);

    destroyIrParser(irParser);
    return 0;
}
