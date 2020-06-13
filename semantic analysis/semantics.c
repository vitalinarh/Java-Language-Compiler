#include "semantics.h"
#include "symbol_table.h"
#include <stdio.h>
#include "tree.h"

extern int columns;
extern int line;
extern int onlyErrors;

void buildTable(node* root) {

    struct table_element* globalTable = NULL;
    struct fieldsAndMethods* field_declarations = NULL;
    struct fieldsAndMethods* aux_field_declarations = NULL;
    struct table_element* auxTable = NULL;
    struct table_element* auxTable2 = NULL;
    struct table_element * method;
    struct fieldsAndMethods * newSymbol = NULL;
    struct node* temp = NULL;
    struct node* temp2 = NULL;
    struct node* temp3 = NULL;
    struct node* local = NULL;
    struct node* methodBody = NULL;
    struct node* currentMethod = NULL;
    struct node* auxParams = NULL;
    char * type, * methodId;
    int nParams = 0;
    int i = 0;

    if(root != NULL){
        if (strcmp(root->type, "NULL") != 0) {
            // CREATE GLOBAL TABLE
            if( strcmp(root->type, "Program") == 0){

                temp = root->child->next_brother; //FIRST CHILD IS PROGRAM ID, THEN WE EITHER HAVE FIELD DECL OF METHOD DECL
                local = root->child->next_brother;

                while(temp != NULL){

                    // IF FIELD DECL
                    if(strcmp(temp->type, "FieldDecl") == 0){

                        newSymbol = (struct fieldsAndMethods*) malloc(sizeof( struct fieldsAndMethods));
                        newSymbol->next = NULL;
                        newSymbol->isField = 1;
                        newSymbol->method = NULL;
                        strcpy(newSymbol->name, temp->child->next_brother->value);

                        if (strcmp(temp->child->next_brother->value, "_") == 0) {
                            printf("Line %d, col %d: Symbol _ is reserved\n", temp->child->next_brother->line, temp->child->next_brother->column);
                        }
                        else {
                            type = searchVar(NULL, field_declarations, temp->child->next_brother->value);

                            if(strcmp(type, "undef") == 0) {

                                if (strcmp(temp->child->type, "Int") == 0) {
                                    strcpy(newSymbol->type, "int");
                                }
                                if (strcmp(temp->child->type, "Double") == 0) {
                                    strcpy(newSymbol->type, "double");
                                }
                                if (strcmp(temp->child->type, "Bool") == 0) {
                                    strcpy(newSymbol->type, "boolean");
                                }
                                if (strcmp(temp->child->type, "Void") == 0) {
                                    strcpy(newSymbol->type, "void");
                                }

                                if (field_declarations == NULL) {
                                    field_declarations = newSymbol;
                                    aux_field_declarations = field_declarations;
                                }

                                else {
                                    while (aux_field_declarations->next != NULL) {

                                        aux_field_declarations = aux_field_declarations->next;
                                    }
                                    aux_field_declarations->next = newSymbol;
                                }
                            }

                            else {
                                printf("Line %d, col %d: Symbol %s already defined\n", temp->child->next_brother->line, temp->child->next_brother->column, temp->child->next_brother->value);
                            }
                        }
                    }
                    // IF METHOD DECL
                    if(strcmp(temp->type, "MethodDecl") == 0){

                        newSymbol = (struct fieldsAndMethods*) malloc(sizeof(struct fieldsAndMethods));
                        newSymbol->next = NULL;
                        newSymbol->method = NULL;
                        newSymbol->isField = 0;
                        temp2 = temp->child; // SKIP TO METHOD HEADER
                        methodId = (char*)strdup(temp2->child->next_brother->value);
                        currentMethod = temp2->child->next_brother;

                        // SAVE TYPE AND ID OF METHOD
                        if (strcmp(temp2->child->type, "Int") == 0)
                            globalTable = insert_el(globalTable, methodId, "int");

                        if (strcmp(temp2->child->type, "Void") == 0)
                            globalTable = insert_el(globalTable, methodId, "void");

                        if (strcmp(temp2->child->type, "Bool") == 0)
                            globalTable = insert_el(globalTable, methodId, "boolean");

                        if (strcmp(temp2->child->type, "Double") == 0)
                            globalTable = insert_el(globalTable, methodId, "double");

                        // IF IS FIRST OF GLOBAL TABLE SAVE IN AUX, ELSE GO TO NEXT METHOD SLOT
                        if(auxTable == NULL){
                            auxTable = globalTable;
                        }
                        else {
                            auxTable = auxTable->next;
                        }
                        auxTable->paramDecl = NULL;
                        auxTable->next = NULL;
                        auxTable->varDecl = NULL;

                        temp2 = temp2->child->next_brother->next_brother->child;  // SKIP TO FIRST PARAM DECL
                        auxParams = temp2;
                        nParams = 0;
                        i = 0;

                        while(auxParams) {
                            nParams++;
                            auxParams = auxParams->next_brother;
                        }

                        // CREATE PARAM ARRAY WITH ITS TYPES
                        char paramTypes[nParams][500];

                        auxParams = temp2;

                        while(auxParams) {
                            if (strcmp(auxParams->child->type, "Int") == 0)
                                strcpy(paramTypes[i], "int");
                            if (strcmp(auxParams->child->type, "StringArray") == 0)
                                strcpy(paramTypes[i], "String[]");
                            if (strcmp(auxParams->child->type, "Bool") == 0)
                                strcpy(paramTypes[i], "boolean");
                            if (strcmp(auxParams->child->type, "Double") == 0)
                                strcpy(paramTypes[i], "double");

                            auxParams = auxParams->next_brother;
                            i++;
                        }   

                        method = searchMethod(NULL, field_declarations, methodId, paramTypes, nParams, 0);

                        while(temp2 != NULL) {  // RUN TRHOUGH ALL PARAM DECL

                            if (strcmp(temp2->child->next_brother->value, "_") == 0) {

                                printf("Line %d, col %d: Symbol _ is reserved\n", temp2->child->next_brother->line, temp2->child->next_brother->column);
                            }

                            type = searchVar(auxTable, NULL, temp2->child->next_brother->value);

                            if (strcmp(type, "undef") == 0) {

                                if(strcmp(temp2->child->type, "Int") == 0){
                                    auxTable->paramDecl = insert_el(auxTable->paramDecl,  temp2->child->next_brother->value, "int");
                                }
                                if(strcmp(temp2->child->type,"StringArray") == 0){
                                    auxTable->paramDecl = insert_el(auxTable->paramDecl,  temp2->child->next_brother->value, "String[]");
                                }
                                if(strcmp(temp2->child->type, "Bool") == 0){
                                    auxTable->paramDecl = insert_el(auxTable->paramDecl,  temp2->child->next_brother->value, "boolean");
                                }
                                 if(strcmp(temp2->child->type,"Double") == 0){
                                    auxTable->paramDecl = insert_el(auxTable->paramDecl,  temp2->child->next_brother->value, "double");
                                }
                            }

                            else {

                                if(strcmp(temp2->child->type, "Int") == 0){
                                    auxTable->paramDecl = insert_el(auxTable->paramDecl,  "NULL", "int");
                                }
                                if(strcmp(temp2->child->type,"StringArray") == 0){
                                    auxTable->paramDecl = insert_el(auxTable->paramDecl,  "NULL", "String[]");
                                }
                                if(strcmp(temp2->child->type, "Bool") == 0){
                                    auxTable->paramDecl = insert_el(auxTable->paramDecl,  "NULL", "boolean");
                                }
                                 if(strcmp(temp2->child->type,"Double") == 0){
                                    auxTable->paramDecl = insert_el(auxTable->paramDecl,  "NULL", "double");
                                }

                                printf("Line %d, col %d: Symbol %s already defined\n", temp2->child->next_brother->line, temp2->child->next_brother->column, temp2->child->next_brother->value);

                            }

                            temp2 = temp2->next_brother; // NEXT PARAM DECLARATION
                        }

                        if(method) {
                            printf("Line %d, col %d: Symbol %s(", currentMethod->line, currentMethod->column, currentMethod->value);
                            for (i = 0; i < nParams; i++) {
                                if (i == nParams - 1)
                                    printf("%s", paramTypes[i]);
                                else {
                                    printf("%s,", paramTypes[i]);
                                }
                            }
                            printf(") already defined\n");
                            strcpy(auxTable->name, "NULL");
                        }

                        // ADD METHOD TO FIELD AND METHOD DECLARATIONS LIST
                        newSymbol->method = auxTable;

                        if (field_declarations == NULL) {
                            field_declarations = newSymbol;
                            aux_field_declarations = field_declarations;
                        }
                        else{
                            while (aux_field_declarations->next != NULL) {
                                aux_field_declarations = aux_field_declarations->next;
                            }
                            aux_field_declarations->next = newSymbol;
                        }

                    }

                    temp = temp->next_brother;
                   
                } // end while: temp!=null


                // ONLY START ANOTATION OF LOCAL VARIABLES WHEN CLASS TABLE IS FINISHED
                while (local != NULL) {

                    if(strcmp(local->type, "MethodDecl") == 0){

                        // IF IS FIRST OF GLOBAL TABLE SAVE IN AUX, ELSE GO TO NEXT METHOD SLOT
                        if(auxTable2 == NULL){
                            auxTable2 = globalTable;
                        }
                        else {
                            auxTable2 = auxTable2->next;
                        }

                        methodBody = local->child->next_brother; // SAVE METHOD BODY OF CURRENT METHOD

                        // GET ALL LOCAL VARIABLES OF METHOD BODY AND STORE THEM
                        if (methodBody != NULL) {
                            if (strcmp(methodBody->type, "MethodBody") == 0) {
                                temp3 = methodBody->child;
                                while (temp3 != NULL) {

                                    if (strcmp(temp3->type, "VarDecl") == 0) {

                                        if (strcmp(temp3->child->next_brother->value, "_") == 0) {

                                            printf("Line %d, col %d: Symbol _ is reserved\n", temp3->child->next_brother->line, temp3->child->next_brother->column);
                                        }

                                        else {
                                            type = searchVar(auxTable2, NULL, temp3->child->next_brother->value);

                                            if (strcmp(type, "undef") == 0) {

                                                if (temp3->child != NULL && temp3->child->next_brother) {

                                                    if (strcmp(temp3->child->type, "Int") == 0)
                                                        auxTable2->varDecl = insert_el(auxTable2->varDecl, temp3->child->next_brother->value, "int");

                                                    if (strcmp(temp3->child->type, "Bool") == 0)
                                                        auxTable2->varDecl = insert_el(auxTable2->varDecl, temp3->child->next_brother->value, "boolean");

                                                    if (strcmp(temp3->child->type, "Double") == 0)
                                                        auxTable2->varDecl = insert_el(auxTable2->varDecl, temp3->child->next_brother->value, "double");
                                                }
                                            }

                                            else {
                                                printf("Line %d, col %d: Symbol %s already defined\n", temp3->child->next_brother->line, temp3->child->next_brother->column, temp3->child->next_brother->value);
                                            }
                                        }
                                    }

                                    else {
                                        // MAKE ANOTATIONS FOR THE CURRENT NODE WITH THE CURRENT STATE OF THE TABLE
                                        if(temp3->type != NULL) {
                                            anotateMethod(temp3, auxTable2, field_declarations);
                                        }
                                    }
                                    temp3 = temp3->next_brother;
                                }
                            }
                        }
                    }

                    local = local->next_brother;
                }
            }
        }

        if (onlyErrors == 0)
            show_table(globalTable, field_declarations, root);
    }

    if (onlyErrors == 0) {
        printf("\n");
        printAnotadedTree(0, root);
    }
}

// PRINT AST TREE WITH THE ANOTATIONS
void printAnotadedTree(int depth, node* node){

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
                printf("%s", node->type);
            else
                printf("%s(%s)",node->type, node->value);

            if(node->anotation != NULL && checkExpr(node->type) == 1) { // ANOTATION
                printf(" - %s\n",node->anotation);
            }
            else{
                printf("\n");
            }
        }
        printAnotadedTree(depth + 1, node->child);
        printAnotadedTree(depth, node->next_brother);
    }
}

// CHECK IF PROVIDED TYPE IS EXPRESSION
int checkExpr(char* type){

    if(strcmp(type,"Assign") == 0    || strcmp(type,"Plus") == 0      || strcmp(type,"Minus") == 0  || 
       strcmp(type,"Mul") == 0       || strcmp(type,"Div") == 0       || strcmp(type,"Mod") == 0    || 
       strcmp(type,"And") == 0       || strcmp(type,"Or") == 0        || strcmp(type,"Xor") == 0    || 
       strcmp(type,"Lshift") == 0    || strcmp(type,"Rshift") == 0    || strcmp(type,"Eq") == 0     || 
       strcmp(type,"Ge") == 0        || strcmp(type,"Gt") == 0        || strcmp(type,"Le") == 0     || 
       strcmp(type,"Lt") == 0        || strcmp(type,"Ne") == 0        || strcmp(type,"Le") == 0     || 
       strcmp(type,"Add") == 0       || strcmp(type,"Not") == 0       || strcmp(type,"Length") == 0 || 
       strcmp(type,"Call") == 0      || strcmp(type,"ParseArgs") == 0 || strcmp(type,"BoolLit") == 0 || 
       strcmp(type,"DecLit") == 0    || strcmp(type,"Id") == 0        || strcmp(type,"RealLit") == 0 || 
       strcmp(type,"IntLit") == 0    || strcmp(type,"StrLit") == 0  || strcmp(type, "Sub") == 0 ){
       return 1; 
    }

    else
        return 0;
}

// SEARCH FOR METHOD BY NAME AND PARAM TYPES
struct table_element * searchMethod(node * method, fieldsAndMethods * fieldsAndMethods, char * varName, char params[][500], int nParams, int searchTwice) {

    struct fieldsAndMethods * aux = fieldsAndMethods;
    struct table_element * paramsAux = NULL;
    struct table_element * methodFound = NULL;
    int i = 0;
    int count = 0;
    int methodParams = 0;
    int nfound = 0;

    // SEARCH METHOD DECL
    while (aux) {

        if (aux->isField == 0) {

            if (strcmp(aux->method->name, varName) == 0) {


                paramsAux = aux->method->paramDecl;
                count = 0;
                i = 0;
                methodParams = 0;

                while (paramsAux != NULL) {
                    methodParams++;
                    paramsAux = paramsAux->next;
                }

                paramsAux = aux->method->paramDecl;

                while (paramsAux != NULL && i < nParams) {

                    if (strcmp(paramsAux->type, params[i]) == 0) {
                        count++;
                    }
                    i++;
                    paramsAux = paramsAux->next;
                }
                if (count == nParams && nParams == methodParams) {
                    return aux->method;
                }
            }
        }
        aux = aux->next;
    } 

    aux = fieldsAndMethods;

    if (searchTwice == 0) {
        return NULL;
    }

    // SEARCH METHODS AGAIN BUT NOW DOUBLE AND INT TYPES ARES INTERCHANGEABLE MEANING THAT f(int) CALL CAN BE ATRIBUTED TO A f(double) METHOD AND VICE VERSA
    while(aux) {

        if (aux->isField == 0) {

            if (strcmp(aux->method->name, varName) == 0) {

                paramsAux = aux->method->paramDecl;
                count = 0;
                i = 0;
                methodParams = 0;

                while (paramsAux != NULL) {
                    methodParams++;
                    paramsAux = paramsAux->next;
                }

                paramsAux = aux->method->paramDecl;

                while (paramsAux != NULL) {

                    if (strcmp(paramsAux->type, params[i]) == 0) {
                        count++;
                    }
                    else if (strcmp(paramsAux->type, "double") == 0 && strcmp(params[i], "int") == 0) {
                        count++;
                    }
                    i++;
                    paramsAux = paramsAux->next;
                }

                if (count == nParams && nParams == methodParams) {
                    methodFound = aux->method;
                    nfound++;
                }
            }      
        }
        aux = aux->next;
    }

    if(nfound > 1) {
        // IF METHOD NOT FOUND THAT MEANS THAT IT IS UNDEFINED
        printf("Line %d, col %d: Reference to method %s(", method->line, method->column, method->value);
        for (i = 0; i < nParams; i++) {
            if(i != nParams - 1)
                printf("%s,", params[i]);
            else
                printf("%s) is ambiguous\n", params[i]);
        } 
    }
    else if(methodFound) {
        return methodFound;
    }
    else {
        // IF METHOD NOT FOUND THAT MEANS THAT IT IS UNDEFINED
        printf("Line %d, col %d: Cannot find symbol %s(", method->line, method->column, method->value);
        for (i = 0; i < nParams; i++) {
            if(i != nParams - 1)
                printf("%s,", params[i]);
            else
                printf("%s", params[i]);
        } 
        printf(")\n");
    }
    return NULL;
}

// SEARCH FOR VAR IN FIELD DECL OR IN LOCAL VARIABLES
char * searchVar(table_element * method, fieldsAndMethods * fieldsAndMethods, char * varName) {

    struct fieldsAndMethods * aux = NULL;
    table_element * local = NULL;
    table_element * params = NULL;

    if(method) {
        local = method->varDecl;
        params = method->paramDecl;  
    }
    
    if(fieldsAndMethods) {
        aux = fieldsAndMethods;
    }

    // SEARCH METHOD PARAMS
    while (params) {
        if (strcmp(params->name, varName) == 0) {
            return params->type;
        }
        params = params->next;
    }

    // SEARCH METHOD LOCAL VARS
    while (local) {
        if (strcmp(local->name, varName) == 0) {
            return local->type;
        }
        local = local->next;
    }

    // SEARCH FIELD DECL
    while (aux) {
        if (strcmp(aux->name, varName) == 0 && aux->isField == 1) {
            return aux->type;
        }

        aux = aux->next;
    } 

    return "undef";
}

// ANOTATE CURRENT NODE OF THE CURRENT METHOD AT THE CURRENT POINT
void anotateMethod(node* node, table_element* currentTable, fieldsAndMethods * fieldsAndMethods){ 

    struct node * aux = node;
    struct node * aux2, * aux3;
    struct table_element * method;
    struct table_element * params;
    char * type, * operator;
    char auxParams[500];

    int nParams, i;

    if(aux != NULL){

        // DONE??
        if (strcmp(aux->type, "Assign") == 0) {

            aux2 = aux->child;

            while (aux2 != NULL) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;

            }

            aux2 = aux->child;
            aux3 = aux2->next_brother;
            aux->anotation = aux->child->anotation;

            // CHECK IF ASSIGNMENT IS VALID FOR THE CORRESPONDING TYPE
            if (aux2->anotation && aux3->anotation) {
                if (strcmp(aux2->anotation, aux3->anotation) == 0) {
                    if (strcmp(aux2->anotation, "undef") != 0 && strcmp(aux2->anotation, "String[]") != 0) {
                        return;
                    }
                }
                if (strcmp(aux2->anotation, "double") == 0 && strcmp(aux3->anotation, "int") == 0) {
                    return;
                }
                else {
                    printf("Line %d, col %d: Operator = cannot be applied to types %s, %s\n", aux->line, aux->column, aux2->anotation, aux3->anotation);   
                }
            }

        }

        // DONE
        else if (strcmp(aux->type, "Return") == 0) {

            aux2 = aux->child;

            while (aux2 != NULL) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }

            aux2 = aux->child;

            if (aux2) {
                if (strcmp(currentTable->type, "void") == 0) {
                    printf("Line %d, col %d: Incompatible type %s in return statement\n",  aux2->line, aux2->column, aux2->anotation);
                }
                else if (strcmp(currentTable->type, aux2->anotation) == 0) {
                    return;
                }
                else if (strcmp(currentTable->type, "double") == 0) {
                    if (strcmp(aux2->anotation, "int") != 0  && strcmp(aux2->anotation, "double") != 0) {
                        printf("Line %d, col %d: Incompatible type %s in return statement\n",  aux2->line, aux2->column, aux2->anotation);
                    }
                }
                else {
                    printf("Line %d, col %d: Incompatible type %s in return statement\n",  aux2->line, aux2->column, aux2->anotation);
                }
            }
            else if (strcmp(currentTable->type, "void") != 0) {
                printf("Line %d, col %d: Incompatible type void in return statement\n",  aux->line, aux->column);
            }

            // [ERRORS] CHECK FOR ERRORS IN RETURN VALUE FOR TYPE COMPATIBLITY
        }

        else if (strcmp(aux->type, "Id") == 0) {

            // FIND VARIABLE WITH THE NAME, IF NOT FOUND RETURN STRING WITH UNDEF
            type = searchVar(currentTable, fieldsAndMethods, aux->value);

            // [ERRORS] SYMBOL NOT FOUND, PRINT FOR ERROR
            if (strcmp(type, "undef") == 0) {
                printf("Line %d, col %d: Cannot find symbol %s\n", aux->line, aux->column, aux->value);
            }

            aux->anotation = type;
        }

        // DONE
        else if (strcmp(aux->type, "If") == 0){

            aux2 = aux->child;
            anotateMethod(aux2, currentTable, fieldsAndMethods);
            aux2 = aux2->next_brother;
            aux3 = aux->child;

            aux3 = aux->child;

            if(strcmp(aux3->anotation, "boolean") != 0) {
                    printf("Line %d, col %d: Incompatible type %s in if statement\n", aux3->line, aux3->column, aux3->anotation);
            }

            while (aux2 != NULL) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }
        }

        // DONE
        else if (strcmp(aux->type, "While") == 0) {
            aux2 = aux->child;
            while(aux2 != NULL) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }

            aux2 = aux->child;
            // CHECK IF CHILD IS ANOTED WITH BOOLEAN TYPE 
            if (strcmp(aux2->anotation, "boolean") != 0) {
                printf("Line %d, col %d: Incompatible type %s in while statement\n", aux2->line, aux2->column, aux2->anotation);
                aux->anotation = (char*)strdup("undef");
            }
            else {
                aux->anotation = (char*)strdup("boolean");
            }
        }

        // DONE
        else if (strcmp(aux->type, "Block") == 0) {
            aux2 = aux->child;
            while(aux2 != NULL) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }
        }

        // DONE
        else if (strcmp (aux->type, "Eq") == 0 || strcmp (aux->type, "Gt") == 0 || strcmp (aux->type, "Ge") == 0 ||
                 strcmp (aux->type, "Le") == 0 || strcmp (aux->type, "Lt") == 0 || strcmp (aux->type, "Ne") == 0) {
            aux2 = aux->child;
            while (aux2) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }

            aux2 = aux->child;
            aux3 = aux2->next_brother;

            // ANOTATE AUX AS BOOLEAN
            aux->anotation = (char*)strdup("boolean");

            // SAVE OPERATOR IN CASE THERE'S AN ERROR
            if (strcmp(aux->type, "Le") == 0) {
                operator = (char*)strdup("<=");
            }
            else if (strcmp(aux->type, "Eq") == 0) {
                operator = (char*)strdup("==");    
            }
            else if (strcmp(aux->type, "Ge") == 0) {
                operator = (char*)strdup(">=");    
            }
            else if (strcmp(aux->type, "Gt") == 0) {
                operator = (char*)strdup(">");    
            }
            else if (strcmp(aux->type, "Lt") == 0) {
                operator = (char*)strdup("<");    
            }
            else if (strcmp(aux->type, "Ne") == 0) {
                operator = (char*)strdup("!=");    
            }

            // THIS OPERATIONS CAN BE APPLIED TO BOOLEAN TYPES ALSO (NOT EQUAL AND EQUAL, HENCE THE DIFERENT TREATMENT)
            if(strcmp(aux->type, "Eq") == 0 || strcmp(aux->type, "Ne") == 0){
                if(strcmp(aux2->anotation, "boolean") == 0 && strcmp(aux3->anotation, "boolean") == 0){
                    return;
                }
            }
            
            // CHECK IF TYPE OF BOTH CHILDREN OF AUX ARE VALID TO MAKE THE OPERATION, IN THIS CASE IT ONLY APPLIES TO TYPES OF DOUBLE OR INT, BOTH ARE INTERCHANGEABLE
            if (strcmp(aux2->anotation, "int") == 0 && strcmp(aux3->anotation, "double") == 0) { // VALID
                return;
            }
            else if (strcmp(aux2->anotation, "double") == 0 && strcmp(aux3->anotation, "int") == 0) { // VALID
                return;
            }
            else if (strcmp(aux2->anotation, "double") == 0 && strcmp(aux3->anotation, "double") == 0) { // VALID
                return;
            }
            else if (strcmp(aux2->anotation, "int") == 0 && strcmp(aux3->anotation, "int") == 0) { // VALID
                return;
            }
            else {
                if (aux2->anotation == NULL) {
                    printf("Line %d, col %d: Operator %s cannot be applied to types none, %s\n", aux->line, aux->column, operator, aux3->anotation);
                    printf("asdasd\n");
                }
                else if (aux3->anotation == NULL) 
                    printf("Line %d, col %d: Operator %s cannot be applied to types %s, none\n", aux->line, aux->column, operator, aux2->anotation);
                else
                    printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", aux->line, aux->column, operator, aux2->anotation, aux3->anotation);
            }

        }

        // CALL TREATMENT, HAS CHILD NAME ID, THEN FOLLOWED BY PARAMS
        else if (strcmp (aux->type, "Call") == 0) {

            if (aux->child) {
                // ANOTATE CHILDREN (ITS PARAMS BUT NOT ID)
                aux2 = aux->child->next_brother;
                while (aux2 != NULL) {
                    // ANOTATE CHILDREN OF AUX RECURSIVELY
                    anotateMethod(aux2, currentTable, fieldsAndMethods);
                    aux2 = aux2->next_brother;
                }
            }

            // AUX3 WILL SERVE AS AN AUXILIARY TO GET NUMBER OF PARAMS AND ITS TYPES
            // START BY FIRST PARAM
            aux3 = aux->child->next_brother;

            // TO COUNT PARAMS
            nParams = 0;
            // TO ITERATE 
            i = 0;

            // COUNTS PARAMS
            while (aux3) {
                nParams += 1;
                aux3 = aux3->next_brother;
            }

            // CREATE PARAM ARRAY WITH ITS TYPES
            char paramTypes[nParams][500];

            aux3 = aux->child->next_brother;

            // GET PARAM TYPES AND STORY THEM IN ARRAY 
            while (aux3) {
                strcpy(paramTypes[i], aux3->anotation);
                aux3 = aux3->next_brother;
                i++;
            }

            // GET METHOD BY NAME, PARAM TYPES AND NUMBER OF PARAMS
            method = searchMethod(aux->child, fieldsAndMethods, aux->child->value, paramTypes, nParams, 1);

            // ANOTATE ID OF METHOD WITH THE CORRESPONDING PARAM TYPES
            aux3 = aux->child;
            aux3->anotation = (char*)strdup("");

            // ANOTATES THE ID OF THE CALL WITH THE PARAM TYPES OF THE RETURNED METHOD
            if(method) {

                params = method->paramDecl;
                strcpy(auxParams, "(");
                for(i = 0; i < nParams; i++) {
                    if (i == nParams - 1) {

                        strcat(auxParams, params->type);
                    }
                    else {
                        strcat(auxParams, params->type);
                        strcat(auxParams, ",");
                    }
                    params = params->next;
                }
                strcat(auxParams, ")");
                aux3->anotation = (char*)strdup(auxParams);
                aux->anotation = method->type;
            }
            else {    
                aux3->anotation = "undef";
                aux->anotation = "undef";
            }
        }

        // DONE
        else if (strcmp(aux->type, "Print") == 0) {
            aux2 = aux->child;
            while (aux2 != NULL) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }

            aux2 = aux->child;

            if(strcmp(aux2->anotation, "String[]") == 0 || strcmp(aux2->anotation, "void") == 0 || strcmp(aux2->anotation, "undef") == 0) {
                   printf("Line %d, col %d: Incompatible type %s in System.out.print statement\n", aux2->line, aux2->column, aux2->anotation);
            }
        }

        // DONE
        else if (strcmp(aux->type, "ParseArgs") == 0) {
            aux2 = aux->child;
            while (aux2 != NULL) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }

            aux2 = aux->child;
            aux3 = aux2->next_brother;

            if (strcmp(aux2->anotation, "String[]") != 0) {
                   printf("Line %d, col %d: Operator Integer.parseInt cannot be applied to types %s, %s\n", aux->line, aux->column, aux2->anotation, aux3->anotation);
            }
            else if (strcmp(aux3->anotation, "int") != 0) {
                printf("Line %d, col %d: Operator Integer.parseInt cannot be applied to types %s, %s\n", aux->line, aux->column, aux2->anotation, aux3->anotation);
            }

            aux->anotation = (char*)strdup("int");
        }


        // DONE
        else if (strcmp(aux->type, "Length") == 0) {
            aux2 = aux->child;
            while(aux2 != NULL) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }

            aux2 = aux->child;
            if (strcmp(aux2->anotation, "String[]") != 0) {
                    printf("Line %d, col %d: Operator .length cannot be applied to type %s\n", aux->line, aux->column, aux2->anotation);
            }

            aux->anotation = (char*)strdup("int");
        }


        // DONE
        else if (strcmp(aux->type, "Mul") == 0 || strcmp(aux->type, "Sub") == 0 || strcmp(aux->type, "Div") == 0 ||
                 strcmp(aux->type, "Add") == 0 || strcmp(aux->type, "Mod") == 0 ) {

            aux2 = aux->child;

            while (aux2 != NULL) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }

            if (strcmp(aux->type, "Mul") == 0) {
                operator = (char*)strdup("*");
            }
            else if (strcmp(aux->type, "Sub") == 0) {
                operator = (char*)strdup("-");
            }
            else if (strcmp(aux->type, "Div") == 0) {
                operator = (char*)strdup("/");
            }
            else if (strcmp(aux->type, "Add") == 0) {
                operator = (char*)strdup("+");
            }
            else if (strcmp(aux->type, "Mod") == 0) {
                operator = (char*)strdup("%");
            }

            aux2 = aux->child;
            aux3 = aux2->next_brother;
            // CHECK FOR TYPES OF CHILDREN TO SEE WHICH TO ANOTE TO THE FATHER
            if (aux2->anotation) {
                if (strcmp(aux2->anotation, "int") == 0) {
                    if (aux2->next_brother->anotation == NULL) {
                        printf("asdd");
                    }
                    if (aux2->next_brother->anotation && strcmp(aux2->next_brother->anotation, "int") == 0) {
                        aux->anotation = (char*)strdup("int");
                    }
                    else if (aux2->next_brother->anotation && strcmp(aux2->next_brother->anotation, "double") == 0) {
                        aux->anotation = (char*)strdup("double");  
                    }
                    else {
                        if (aux3->anotation == NULL)
                            printf("Line %d, col %d: Operator %s cannot be applied to types %s, none\n", aux->line, aux->column, operator, aux2->anotation);
                        else
                            printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", aux->line, aux->column, operator, aux2->anotation, aux3->anotation);
                        aux->anotation = (char*)strdup("undef");   
                    }
                }
                else if (strcmp(aux2->anotation, "double") == 0) {
                    if (aux2->next_brother->anotation && strcmp(aux2->next_brother->anotation, "int") == 0) {
                        aux->anotation = (char*)strdup("double");
                    }
                    else if (aux2->next_brother->anotation && strcmp(aux2->next_brother->anotation, "double") == 0) {
                        aux->anotation = (char*)strdup("double");  
                    }
                    else {
                        printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", aux->line, aux->column, operator, aux2->anotation, aux3->anotation);
                        aux->anotation = (char*)strdup("undef");   
                    }
                }
                else {
                    printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", aux->line, aux->column, operator, aux2->anotation, aux3->anotation);
                    aux->anotation = (char*)strdup("undef");
                }
            }
            else {
                if (aux2->anotation == NULL) {
                    printf("Line %d, col %d: Operator %s cannot be applied to types none, %s\n", aux->line, aux->column, operator, aux3->anotation);
                }
                else if (aux3->anotation == NULL)
                    printf("Line %d, col %d: Operator %s cannot be applied to types %s, none\n", aux->line, aux->column, operator, aux2->anotation);
                else
                    printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", aux->line, aux->column, operator, aux2->anotation, aux3->anotation);
                aux->anotation = (char*)strdup("undef");
            }
        }

        // NODE
        else if (strcmp(aux->type, "And") == 0 || strcmp(aux->type, "Or") == 0 || strcmp(aux->type, "Xor") == 0) {
            aux2 = aux->child;
            while (aux2 != NULL) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }
            aux->anotation = (char*)strdup("boolean");

            aux2 = aux->child;
            aux3 = aux2->next_brother;

            if (strcmp(aux->type, "And") == 0) {
                operator = "&&";
            }
            else if (strcmp(aux->type, "Or") == 0) {
                operator = "||";
            }
            else if (strcmp(aux->type, "Xor") == 0) {
                operator = "^";
            }

            if(strcmp(aux2->anotation, "boolean") != 0) {
                printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", aux->line, aux->column, operator, aux2->anotation, aux3->anotation);
            }
            else if(strcmp(aux3->anotation, "boolean") != 0) {
                printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", aux->line, aux->column, operator, aux2->anotation, aux3->anotation);   
            }
        }

        // MISSING ERROR TREATMENT FOR TYPE INCONSISTENCIES
        else if (strcmp(aux->type, "Plus") == 0 || strcmp(aux->type, "Minus") == 0) {
            aux2 = aux->child;
            // // ANOTATE CHILDREN OF AUX RECURSIVELY
            while (aux2 != NULL) {
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }

            aux2 = aux->child;

            // DONE
            if (strcmp(aux->child->anotation, "int") == 0 || strcmp(aux->child->anotation, "double") == 0)
                aux->anotation = aux->child->anotation;
            else {
                aux->anotation = (char*)strdup("undef"); 

                if(strcmp(aux->type, "Plus") == 0){
                printf("Line %d, col %d: Operator + cannot be applied to type %s\n", aux->line, aux->column, aux2->anotation);
            }
            else{
                printf("Line %d, col %d: Operator - cannot be applied to type %s\n", aux->line, aux->column, aux2->anotation);
            }
            }
        }

        // DONE
        else if (strcmp(aux->type, "Not") == 0) {
            aux2 = aux->child;
            while (aux2 != NULL) {
                // ANOTATE CHILDREN OF AUX RECURSIVELY
                anotateMethod(aux2, currentTable, fieldsAndMethods);
                aux2 = aux2->next_brother;
            }

            aux->anotation = (char*)strdup("boolean");

            aux2 = aux->child;

            if(strcmp(aux2->anotation, "boolean") != 0) {
                printf("Line %d, col %d: Operator ! cannot be applied to type %s\n", aux->line, aux->column, aux2->anotation);
            }
        }

        // MISSING ERROR TREATMENT FOR NUMBER BOUNDS
        else if (strcmp(aux->type, "DecLit") == 0) {
            aux->anotation = (char*)strdup("int");

            long number;

            number = strtol(aux->value, NULL, 10);

            if (number >= 2147483648) {
                printf("Line %d, col %d: Number %s out of bounds\n", aux->line, aux->column, aux->value);
            }
        }

        // MISSING ERROR TREATMENT FOR NUMBER BOUNDS
        else if (strcmp(aux->type, "RealLit") == 0) {
            aux->anotation = (char*)strdup("double");
        }

        // DONE
        else if (strcmp(aux->type, "BoolLit") == 0) {
            aux->anotation = (char*)strdup("boolean");
        }

        // DONE
        else if (strcmp(aux->type, "StrLit") == 0) {
            aux->anotation = (char*)strdup("String");
        }

        // DONE
        else if (strcmp(aux->type, "StringArray") == 0) {
            aux->anotation = (char*)strdup("String[]");
        }
        
    }
}