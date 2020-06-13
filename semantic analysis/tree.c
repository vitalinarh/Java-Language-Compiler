#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tree.h"

struct token * create_token(char * value, int line, int column) {

	token * new_token = (token*)malloc(sizeof(token));

	if (value) {
		new_token->value = (char*)strdup(value);
	}
	else {
		new_token->value = NULL;
	}

	new_token->line = line;
	new_token->column = column;

	return new_token;
}

node* create_node(char* type, char* value, int line, int column){

	node* no  = (node*)malloc(sizeof(node));

	no->type  = (char*)strdup(type);
	no->line = line;
	no->column = column;
	no->anotation = NULL;

	if(value != NULL) {
		no->value = (char*)strdup(value);
	}
	else
		no->value = NULL;
	
	no->child = NULL;
	no->next_brother = NULL;
	return no;
}

void add_children(node* father, node* children){
	
	if(father == NULL || children == NULL){
		return;
	}

	if(father->child == NULL) {

		father->child = children;
	}
}

void add_brother(node* original, node* brother){
	
	if(original == NULL || brother == NULL)
		return;

	node* temp = original;

	while(temp->next_brother != NULL){

		temp = temp->next_brother;
	}


	temp->next_brother = brother;
}

int count_brother(node* original){

	int nChild = 1;
	node* temp = NULL;
	if (original == NULL)
		return 0;
	else{
		temp = original;
		while(temp->next_brother != NULL && strcmp(temp->next_brother->type, "NULL") != 0){	
			nChild += 1;
			temp = temp->next_brother;
		}
	}

	return nChild;
}

void add_type(node* original, node* type){

	node* temp = original;
	node* new_node = NULL;
	node* aux = NULL;

	while(temp != NULL){
		new_node = create_node(type->type, NULL, 0, 0);
		aux =  temp->child;
		temp->child = new_node;
		add_brother(new_node, aux);
		temp = temp->next_brother;
	}
}

void printTree(int depth, node* node){

	int i;

	if (node == NULL){
		return;
	}
	if(node != NULL){
		if (strcmp(node->type, "NULL") != 0) {
			for( i = 0; i < depth; i++ ){
				printf("..");
			}

			if( node->value == NULL)
				printf("%s\n", node->type);
			else
				printf("%s(%s)\n",node->type, node->value);
		}
		printTree(depth + 1, node->child);
		printTree(depth, node->next_brother);
	}
}