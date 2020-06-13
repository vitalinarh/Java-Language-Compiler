#include "symbol_table.h"
#include <stdlib.h>
#include<string.h>
#include<stdio.h>

//Insere um novo identificador na cauda de uma lista ligada de simbolos
table_element *insert_el(table_element* table, char *id, char* type)
{
	table_element *newSymbol = (table_element*) malloc(sizeof(table_element));
	table_element *aux = NULL;

	strcpy(newSymbol->name, id);
	strcpy(newSymbol->type, type);
	newSymbol->next = NULL;	
    newSymbol->paramDecl = NULL;
    newSymbol->varDecl = NULL;

	if (table == NULL) {
        table = newSymbol;
    }
    else {
        aux = table;
        while (aux->next != NULL)
            aux = aux->next;
        aux->next = newSymbol;
    }
	return table; 
}

void show_table(table_element* table, fieldsAndMethods* fieldsAndMethods, node* node){

    struct table_element *aux = NULL;
    struct table_element *auxVar = NULL;
    struct table_element *auxLocalVar = NULL;
    struct fieldsAndMethods *aux_field = fieldsAndMethods;

    printf("===== Class %s Symbol Table =====\n", node->child->value);

    while (aux_field != NULL) {

        if(aux_field->isField == 1){
                
            printf("%s\t\t%s\n", aux_field->name, aux_field->type);
        }

        else{

            if(strcmp(aux_field->method->name, "NULL") != 0) {

                printf("%s\t(", aux_field->method->name);

                auxVar = aux_field->method->paramDecl;

                while (auxVar != NULL){

                    if(auxVar->next == NULL)
                        printf("%s", auxVar->type);
                    else
                        printf("%s,", auxVar->type);

                    auxVar = auxVar->next;
                }
                printf(")\t%s\n", aux_field->method->type);

            }

        }

        aux_field = aux_field->next;
    }

    aux = table;

    while (aux != NULL) {

        if (strcmp(aux->name, "NULL") != 0) {
 
            printf("\n===== Method %s(", aux->name);

            auxVar = aux->paramDecl; 

            while (auxVar != NULL) {

                if(auxVar->next == NULL)
                    printf("%s", auxVar->type);
                else
                    printf("%s,", auxVar->type);

                auxVar = auxVar->next;

            }

            printf(") Symbol Table =====\n");

            auxVar = aux->paramDecl;

            printf("%s\t\t%s\n", "return", aux->type);

            while (auxVar != NULL && strcmp(auxVar->name, "NULL") != 0) {
                printf("%s\t\t%s\t%s\n", auxVar->name, auxVar->type, "param");
                auxVar = auxVar->next;
            }

            auxLocalVar = aux->varDecl;

            while (auxLocalVar != NULL) {

                printf("%s\t\t%s\n", auxLocalVar->name, auxLocalVar->type);
                auxLocalVar = auxLocalVar->next;
            }

        }

        aux = aux->next;
    }
}
