#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct InfoType {
    char *string1;
    char *string2;
} InfoType;


typedef struct Item {
    InfoType *info;
    char *str;
} Item;


typedef struct KeySpace1 {
    int key;
    int par;
    Item *item;
} KeySpace1;


typedef struct KeySpace2 {
    char *key;
    Item *item;
    struct KeySpace2 *next;
} KeySpace2;


typedef struct Table {
    KeySpace1 *ks1;
    KeySpace2 **ks2;
    int csize1;
    int msize1;
    int msize2;
} Table;


// Основные функции.
void exitTable(Table *tmp);
void inputItem(Table *tmp);
void outputItem(Table tmp);
void deleteItem(Table *tmp);
void outputTable(Table tmp);
void createTable(Table tmp);


// Вспомогательные функции.
int checkTable(Table tmp);
void inputInformation(InfoType **tmp);
int inputKeySpace();
int hash(Table tmp, char *key);
KeySpace1* findFirstKey(Table tmp, int key);
KeySpace2* findSecondKey(Table tmp, char *key, int ind);
int findIndexFirstKey(Table tmp, int key);
void sortFirstKey(Table **tmp);
void getInt(int *integer);
char* getString();

#endif

