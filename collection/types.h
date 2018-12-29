typedef enum typeValue {
    typeList,
    typeLong,
    typeDict,
} typeValue;

typedef struct record {
    char* name;
    typeValue type;
} record;




record* newRecord(char*, typeValue);
int recordComparator(record*, record*);
int recordNameComparator(record*, const char*);
void printRecord(record*);
