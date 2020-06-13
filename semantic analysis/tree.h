#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifndef TREE_H
#define TREE_H

typedef struct node{
	char* type;
	char* value;
	char* anotation;
	struct node* child;
	struct node* next_brother;
	int line;
	int column;
}node;

typedef struct token {
	char * value;
	int line;
	int column;
} token;

struct token * create_token(char * value, int line, int column);
node* create_node(char* type, char* value, int line, int column);
void add_children(node* father, node* children);
void printTree(int depth, node* node);
void add_brother(node* original, node* brother);
int count_brother(node* node);
void add_type(node* original, node* type);

#endif
