#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

record* newRecord(char* name, typeValue typeVal) {
    record* rec = malloc(sizeof(record));
    rec->name = name;
    rec->type = typeVal;
    return rec;
}

int recordComparator(record* record1, record* record2) {
    int nameNotEquals = strcmp(record1->name, record2->name);

    if (!nameNotEquals) {
        return record1->type - record2->type;
    }

    return nameNotEquals;
}

int recordNameComparator(record* record1, const char* key) {
    return strcmp(record1->name, key);
}

void printRecord(record* rec) {
    printf("%s:%d\n", rec->name, rec->type);
}



