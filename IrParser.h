#ifndef IR_PARSER_543547
#define IR_PARSER_543547

typedef struct IrParserStruct IrParser;

IrParser *createIrParser(const char *parseTableFileName);
void irParse(IrParser *irParser);
void destroyIrParser(IrParser *irParser);

#endif
