#include "symbol_table.h"
#include "tree.h"

void buildTable(node* p);
table_element * buildMethod(table_element* methodTable, char* name, char* type);
int checkExpr(char* type);
void printAnotadedTree(int depth, node* node);
void anotateMethod(node* node,table_element* currentTable, fieldsAndMethods * fieldsAndMethods);
char * returnType(char * type);
char * searchVar(table_element * method, fieldsAndMethods * fieldsAndMethods, char * varName);
struct table_element * searchMethod(node * method, fieldsAndMethods * fieldsAndMethods, char * varName, char params[][500], int nParams, int searchTwice);