#define true 1
#define false 0
#define bool int

typedef struct node {
    void* value;
    struct node* next;
} node;

typedef struct list {
    node* head;
    node* end;
    unsigned int count;
} list;

typedef int (*comparator)(void*, void*);
typedef void (*callback)(void*);


list* listCreate(void);
void listAppend(list* list, void* elem);
int listIndexOf(list* list, void* elem, comparator comp);
void listIterate(list* list, callback callback);
bool listIsEquivalent(list* list1, list* list2, comparator comp);
bool listEqual(list* l1, list* l2, comparator comp);

