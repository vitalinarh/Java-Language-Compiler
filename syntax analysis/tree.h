#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct node{
	char* type;
	char* value;
	struct node* child;
	struct node* next_brother;
}node;

node* create_node(char* type, char* value);
void add_children(node* father, node* children);
void printTree(int depth, node* node);
void add_brother(node* original, node* brother);
int count_brother(node* node);
void add_type(node* original, node* type);
