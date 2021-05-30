#include "header.h"


int main() {
    int signal;
    Table table = {NULL, NULL, 0, 0, 0};

    // Ввод MSIZE1 и MSIZE2.
    printf("Enter MSIZE1: ");
    getInt(&table.msize1);
    printf("Enter MSIZE2: ");
    getInt(&table.msize2);

    // Выделение памяти под ks1 и ks2.
    table.ks1 = (KeySpace1*)calloc(table.msize1, sizeof(KeySpace1));
    table.ks2 = (KeySpace2**)calloc(table.msize2, sizeof(KeySpace2*));

    // Вызов функций.
    do {
        printf("0. Exit\n");
        printf("1. Add Element\n");
        printf("2. Find Element\n");
        printf("3. Delete Element\n");
        printf("4. Show Table\n");
        printf("5. Create Table\n");
        printf("Enter the command: ");
        getInt(&signal);
        switch (signal) {
            case 0:
                exitTable(&table);
                break;
            case 1:
                inputItem(&table);
                break;
            case 2:
                outputItem(table);
                break;
            case 3:
                deleteItem(&table);
                break;
            case 4:
                outputTable(table);
                break;
            case 5:
                createTable(table);
                break;
            default:
                printf("ERROR! There is no such command.\n");
        }
    } while (signal != 0);
    return 0;
}