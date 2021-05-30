#include "header.h"

/* ОСНОВНЫЕ ФУНКЦИИ */


// 0. Выход из программы.
void exitTable(Table *tmp) {
    int i;

    // Проверка таблицы на наличие элементов.
    if (checkTable(*tmp))
        return;

    // Удаление первого пространства ключей.
    for (i = 0; i < tmp->csize1; i++) {
        free(tmp->ks1[i].item->info->string1);
        free(tmp->ks1[i].item->info->string2);
        free(tmp->ks1[i].item->info);
        free(tmp->ks1[i].item->str);
        free(tmp->ks1[i].item);
        tmp->ks1[i].key = 0;
        tmp->ks1[i].par = 0;
    }

    // Удаление второго пространства ключей.
    for (i = 0; i < tmp->msize2; i++) {
        KeySpace2 *ptr = tmp->ks2[i];
        while (ptr) {
            KeySpace2 *del = ptr;
            ptr = ptr->next;
            free(del);
        }
    }

    // Удаление таблицы.
    free(tmp->ks1);
    free(tmp->ks2);
    tmp->csize1 = 0;
    tmp->msize1 = 0;
    tmp->msize2 = 0;

    printf("Exit!\n");
}


// 1. Ввод элемента в таблицу.
void inputItem(Table *tmp) {
    int key1, par, ind2;
    char *key2;
    InfoType *info;

    // Проверка таблица на переполнение.
    if (tmp->csize1 == tmp->msize1) {
        printf("ERROR! The table is full.\n");
        return;
    }

    // Ввод ключей и информации.
    printf("Enter the first key: ");
    getInt(&key1);
    printf("Enter the key of the parent element: ");
    getInt(&par);
    printf("Enter the second key: ");
    key2 = getString();
    ind2 = hash(*tmp, key2);
    inputInformation(&info);

    // Проверка первого ключа.
    if (findFirstKey(*tmp, key1)) {
        printf("ERROR! The entered first key is already in the table.\n");
        return;
    }

    // Проверка родительского ключа.
    if (((findFirstKey(*tmp, par) == NULL) || (tmp->csize1 == 0)) && (par != 0)) {
        printf("ERROR! The entered parent key isn't in the table yet.\n");
        return;
    }

    // Проверка второго ключа.
    if (findSecondKey(*tmp, key2, ind2)) {
        printf("ERROR! The entered second key is already in the table.\n");
        return;
    }

    // Выделение памяти под первое пространство ключей.
    tmp->ks1[tmp->csize1].item = (Item*)calloc(1, sizeof(Item));
    tmp->ks1[tmp->csize1].item->info = (InfoType*)calloc(1, sizeof(InfoType));
    tmp->ks1[tmp->csize1].item->str = (char*)calloc(strlen(key2) + 1, sizeof(char));
    tmp->ks1[tmp->csize1].item->info->string1 = (char*)calloc(strlen(info->string1) + 1, sizeof(char));
    tmp->ks1[tmp->csize1].item->info->string2 = (char*)calloc(strlen(info->string2) + 1, sizeof(char));

    // Инициализация первого пространства ключей.
    tmp->ks1[tmp->csize1].key = key1;
    tmp->ks1[tmp->csize1].par = par;
    memcpy(tmp->ks1[tmp->csize1].item->str, key2, strlen(key2) + 1);
    memcpy(tmp->ks1[tmp->csize1].item->info->string1, info->string1, strlen(info->string1) + 1);
    memcpy(tmp->ks1[tmp->csize1].item->info->string2, info->string2, strlen(info->string2) + 1);

    // Выделение памяти под второе пространтсво ключей и его инициализация.
    if (tmp->ks2[ind2] == NULL) {
        tmp->ks2[ind2] = (KeySpace2*)calloc(1, sizeof(KeySpace2));
        tmp->ks2[ind2]->key = tmp->ks1[tmp->csize1].item->str;
        tmp->ks2[ind2]->item = tmp->ks1[tmp->csize1].item;
        tmp->ks2[ind2]->next = NULL;
    }
    else {
        KeySpace2 *ptr_next = tmp->ks2[ind2];
        KeySpace2 *ptr = (KeySpace2*)calloc(1, sizeof(KeySpace2));
        ptr->key = tmp->ks1[tmp->csize1].item->str;
        ptr->item = tmp->ks1[tmp->csize1].item;
        tmp->ks2[ind2] = ptr;
        ptr->next = ptr_next;
    }

    // Сортировка первого пространства ключей.
    tmp->csize1 += 1;
    sortFirstKey(&tmp);

    // Удаление введённой информации.
    free(info->string1);
    free(info->string2);
    free(info);
    free(key2);
}


// 2. Вывод элемента из таблицы.
void outputItem(Table tmp) {
    int key1, ind2, command;
    char *key2;

    // Проверка таблицы на наличие элементов.
    if (checkTable(tmp))
        return;

    // Выбор ключевого пространства.
    command = inputKeySpace();

    if (command == 1) {
        KeySpace1 *ptr1;

        // Нахождение элемента в таблице по первому ключу.
        printf("Enter the first key: ");
        getInt(&key1);
        ptr1 = findFirstKey(tmp, key1);
        if (ptr1 == NULL) {
            printf("ERROR! The entered key isn't in the table.\n");
            return;
        }

        // Вывод элемента.
        printf("String #1: %s\n", ptr1->item->info->string1);
        printf("String #2: %s\n", ptr1->item->info->string2);
    }
    else {
        KeySpace2 *ptr2;

        // Нахождение элемента в таблице по второму ключу.
        printf("Enter the second key: ");
        key2 = getString();
        ind2 = hash(tmp, key2);
        ptr2 = findSecondKey(tmp, key2, ind2);
        if (ptr2 == NULL) {
            printf("ERROR! The entered key isn't in the table.\n");
            free(key2);
            return;
        }

        // Вывод элемента.
        printf("String #1: %s\n", ptr2->item->info->string1);
        printf("String #2: %s\n", ptr2->item->info->string2);

        // Удаление выделенной памяти.
        free(key2);
    }
}


// 3. Удаление элемента из таблицы.
void deleteItem(Table *tmp) {
    int key1, ind1, ind2, command, i, signal = 0;
    char *key2;
    KeySpace1 ptr1;
    KeySpace2 *ptr2;

    // Проверка таблицы на наличие элементов.
    if (checkTable(*tmp))
        return;

    // Выбор ключевого пространства.
    command = inputKeySpace();

    if (command == 1) {
        // Нахождение элемента в таблице по первому ключу.
        printf("Enter the first key: ");
        getInt(&key1);
        ind1 = findIndexFirstKey(*tmp, key1);
        if (ind1 == -1) {
            printf("ERROR! The entered key isn't in the table.\n");
            return;
        }
        key2 = tmp->ks1[ind1].item->str;
        ind2 = hash(*tmp, key2);
    }
    else {
        // Нахождение элемента в таблице по второму ключу.
        printf("Enter the second key: ");
        key2 = getString();
        ind2 = hash(*tmp, key2);
        ptr2 = findSecondKey(*tmp, key2, ind2);
        if (ptr2 == NULL) {
            printf("ERROR! The entered key isn't in the table.\n");
            free(key2);
            return;
        }

        for (i = 0; i < tmp->csize1; i++) {
            if (strcmp(key2, tmp->ks1[i].item->str) == 0)
                ind1 = i;
        }
        key1 = tmp->ks1[ind1].key;
    }

    // Зануление родительских ключей удалемого элемента.
    ptr1 = tmp->ks1[ind1];
    tmp->ks1[ind1] = tmp->ks1[tmp->csize1 - 1];
    tmp->ks1[tmp->csize1 - 1] = ptr1;
    for (i = 0; i < tmp->csize1; i++)
        if (key1 == tmp->ks1[i].par)
            tmp->ks1[i].par = 0;

    // Удаление элемента во втором пространстве.
    KeySpace2 *ptr = tmp->ks2[ind2];
    while (ptr != NULL) {
        if (strcmp(key2, ptr->key) == 0 && tmp->ks2[ind2] != ptr) {
            KeySpace2 *head = tmp->ks2[ind2];
            KeySpace2 *cur = tmp->ks2[ind2];
            while (cur->next != ptr) {
                cur = cur->next;
            }
            cur->next = ptr->next;
            free(ptr);
            tmp->ks2[ind2] = head;
            break;
        }
        else if (strcmp(key2, ptr->key) == 0 && tmp->ks2[ind2] == ptr) {
            KeySpace2 *cur = ptr->next;
            free(ptr);
            tmp->ks2[ind2] = cur;
            break;
        }
        else {
            ptr = ptr->next;
        }
    }


    // Удаление элемента в первом пространстве.
    free(tmp->ks1[tmp->csize1 - 1].item->info->string1);
    free(tmp->ks1[tmp->csize1 - 1].item->info->string2);
    free(tmp->ks1[tmp->csize1 - 1].item->info);
    free(tmp->ks1[tmp->csize1 - 1].item->str);
    free(tmp->ks1[tmp->csize1 - 1].item);
    tmp->ks1[tmp->csize1 - 1].key = 0;
    tmp->ks1[tmp->csize1 - 1].par = 0;
    tmp->csize1 -= 1;

    // Удаление выделенной памяти.
    if (command == 2)
        free(key2);
}


// 4. Вывод таблицы.
void outputTable(Table tmp) {
    int i;

    // Проверка таблицы на наличие элементов.
    if (checkTable(tmp))
        return;

    // Вывод элементов таблицы.
    for (i = 0; i < tmp.csize1; i++) {
        printf("Element #%d\n", i + 1);
        printf("First key: %d\n", tmp.ks1[i].key);
        printf("Key of the parent element: %d\n", tmp.ks1[i].par);
        printf("Second key: %s\n", tmp.ks1[i].item->str);
        printf("String #1: %s\n", tmp.ks1[i].item->info->string1);
        printf("String #2: %s\n", tmp.ks1[i].item->info->string2);
    }
}


// 5. Создание новой таблицы.
void createTable(Table tmp) {
    int par, i, j = 0, msize1 = 0;
    Table new = {NULL, 0, 0};

    // Проверка таблицы на наличие элементов.
    if (checkTable(tmp))
        return;

    // Ввод родительского ключа из старой таблицы.
    printf("Enter the key of the parent element: ");
    getInt(&par);

    // Подсчитывание количества родителських элементов в старой таблице.
    for (i = 0; i < tmp.csize1; i++)
        if (par == tmp.ks1[i].par)
            msize1++;

    // Проверка на наличие родителського элемента в старой таблице.
    if (msize1 == 0) {
        printf("ERROR! The entered parent key isn't in the table.\n");
        return;
    }

    // Выделение памяти под новую таблицу.
    new.csize1 = msize1;
    new.msize1 = msize1;
    new.ks1 = (KeySpace1*)calloc(new.msize1, sizeof(KeySpace1));

    // Добавление элементов из первого пространтсва ключей.
    for (i = 0; i < tmp.csize1; i++) {
        if (par == tmp.ks1[i].par) {
            new.ks1[j].key = tmp.ks1[i].key;
            new.ks1[j].par = par;
            new.ks1[j].item = tmp.ks1[i].item;
            j++;
        }
    }

    // Вывод новой таблицы.
    outputTable(new);

    // Удаление памяти, выделенной под новую таблицу.
    free(new.ks1);
    new.csize1 = 0;
    new.msize1 = 0;
}




/* ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ */


// Проверка таблицы.
int checkTable(Table tmp) {

    // Проверка таблицы на наличие элементов
    if (tmp.csize1 == 0) {
        printf("ERROR! The table is empty.\n");
        return 1;
    }
    return 0;
}


// Ввод информации.
void inputInformation(InfoType **tmp) {
    (*tmp) = (InfoType*)malloc(sizeof(InfoType));
    printf("Enter the string #1: ");
    (*tmp)->string1 = getString();
    printf("Enter the string #2: ");
    (*tmp)->string2 = getString();
}


// Выбор ключевого пространства.
int inputKeySpace() {
    int command, signal = 1;

    do {
        printf("1. First key\n");
        printf("2. Second key\n");
        printf("Enter the KeySpace: ");
        getInt(&command);
        switch (command) {
            case 1:
                return 1;
            case 2:
                return 2;
            default:
                printf("ERROR! There is no such KeySpace.\n");
        }
    } while (signal);
}


// Хэш-функция для второго пространства ключей.
int hash(Table tmp, char *key) {
    int ind, sum = 0, length, i;

    length = strlen(key);
    for (i = 0; i < length; i++)
        sum += key[i];
    ind = sum % tmp.msize2;
    return ind;

}


// Поиск первого ключа.
KeySpace1* findFirstKey(Table tmp, int key) {
    int i;

    for (i = 0; i < tmp.csize1; i++)
        if (key == tmp.ks1[i].key)
            return tmp.ks1 + i;
    return NULL;
}


// Поиск второго ключа.
KeySpace2* findSecondKey(Table tmp, char *key, int ind) {
    KeySpace2 *ptr = tmp.ks2[ind];

    if (ptr) {
        while (ptr) {
            if (strcmp(key, ptr->key) == 0) {
                return ptr;
            }
            ptr = ptr->next;
        }
    }
    return NULL;
}


// Поиск индекса первого ключа.
int findIndexFirstKey(Table tmp, int key) {
    int i;

    for (i = 0; i < tmp.csize1; i++)
        if (key == tmp.ks1[i].key)
            return i;
    return -1;
}


// Сортировка первого пространтсва ключей.
void sortFirstKey(Table **tmp) {
    int i, j;
    KeySpace1 ptr;

    for (i = 0; i < (*tmp)->csize1 - 1; i++) {
        for (j = 0; j < (*tmp)->csize1 - i - 1; j++) {
            if ((*tmp)->ks1[j].par > (*tmp)->ks1[j + 1].par) {
                ptr = (*tmp)->ks1[j];
                (*tmp)->ks1[j] = (*tmp)->ks1[j + 1];
                (*tmp)->ks1[j + 1] = ptr;
            }
        }
    }
}


// Ввод целого числа.
void getInt(int *integer) {
    int n;
    do {
        n = scanf("%d", integer);
        if (n == 0) {
            printf("ERROR! Enter the natural number: ");
            scanf("%*[^\n]");
        }
    } while (n == 0);
    scanf("%*c");
}


// Ввод строки неопределённой длины.
char* getString() {
    char *ptr = (char *)malloc(1);
    char buf[81];
    int n, len = 0;
    *ptr = '\0';
    do {
        n = scanf("%80[^\n]", buf);
        if (n < 0) {
            free(ptr);
            ptr = NULL;
            continue;
        }
        if (n == 0)
            scanf("%*c");
        else {
            len += strlen(buf);
            ptr = (char *)realloc(ptr, len + 1);
            strcat(ptr, buf);
        }
    } while (n > 0);
    return ptr;
}

