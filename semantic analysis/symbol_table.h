#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "tree.h"

typedef struct table_element{
	char name[500];
	char type[500];
	struct table_element *paramDecl;
	struct table_element *varDecl;
	struct table_element *next;
} table_element;

typedef struct fieldsAndMethods {
	char name[500];
	char type[500];
	int isField;
	struct fieldsAndMethods *next;
	struct table_element *method;
} fieldsAndMethods;


table_element *insert_el(table_element* table, char *str, char* t);
void show_table(table_element* table, fieldsAndMethods * fieldsAndMethods, node * node);
table_element *search_el(char *str);
void printAnotadedTree(int depth, node* node);

#endif
