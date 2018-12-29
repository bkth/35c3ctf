#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "types.h"



list* listCreate() {
    list* list  = malloc(sizeof(list));
    list->head  = NULL;
    list->end   = NULL;
    list->count = 0;
    return list;
}


node* createNode(void* elem) {
    node* node  = malloc(sizeof(node));
    node->value = elem;
    node->next  = NULL;
    return node;
}
void listAppend(list* list, void* elem) {
    node* node = createNode(elem);
    if (!list->count) {
        list->head = node;
    } else {
        list->end->next = node;
    }
    list->end = node;
    list->count++;
}

void listAddOrdered(list* list, void* elem, comparator comp) {
    node* add = createNode(elem);
    if (!list->count) {
        // empty list
        list->head = add;
        list->end = add;
    } else if (list->count == 1) {
        // 1 elem list
        if (comp(list->head->value, elem) < 0) {
            // add to front
            add->next = list->head;
            list->head = add;
        } else {
            list->end->next = add;
            list->end = add;
        }
    } else {
        // multi element list
        node* previous = NULL;
        node* it = list->head;
        while (it) {
            if (comp(it->value, elem) < 0) {
                add->next = it;
                if (previous) {
                    previous->next = add;
                }
                if (it == list->head) {
                    list->head = add;
                }
                break;

            } else if (it == list->end) {
                list->end->next = add;
                list->end = add;
                break;
            }
            previous = it;
            it = it->next;
        }
    }
    list->count++;
}

void listIterate(list* list, callback cb) {

    if (!list->head) {
        return;
    }
    node* it = list->head;
    while (it) {
        cb(it->value);
        it = it->next;
    }

}

void printString(char* s) {
    printf("%s\n", s);
}

list* listSort(list* l, comparator comp) {
    list* sorted = listCreate();
    if (!l->head) {
        return sorted;
    }
    node* it = l->head;
    while (it) {
        listAddOrdered(sorted, it->value, comp);
        it = it->next;
    }
    return sorted;
}

bool listEqual(list* l1, list* l2, comparator comp) {
    /*printf("list equal\n");*/
    /*listIterate(l1, printRecord);*/
    /*listIterate(l2, printRecord);*/

    if (l1->count != l2->count) {
        return false;
    }
    node* it1 = l1->head;
    node* it2 = l2->head;

    while (it1) {

        if (comp(it1->value, it2->value)) {
            return false;
        }

        it1 = it1->next;
        it2 = it2->next;
    }
    return true;
}

int listIndexOf(list* list, void* elem, comparator comp) {

    if (!list->head) {
        return -1;
    }
    int index = 0;
    node* it = list->head;
    while (it) {

        if (!comp(it->value, elem)) {
            return index;
        }
        it = it->next;
        index++;
    }
    return -1;
}

bool listIsEquivalent(list* l1, list* l2, comparator comp) {
    if (l1->count != l2->count) {
        return false;
    }
    // don't care about memory leaks
    return listEqual(listSort(l1, comp), listSort(l2, comp), comp);
}

/*int main() {*/

    /*[>list* list1 = listCreate();<]*/
    /*[>listAppend(list1, "a");<]*/
    /*[>listAppend(list1, "b");<]*/
    /*[>listAppend(list1, "a");<]*/
    /*[>listAppend(list1, "c");<]*/
    /*[>listIterate(list1, printString);<]*/
    /*[>list* list2 = listCreate();<]*/
    /*[>listAppend(list2, "a");<]*/
    /*[>listAppend(list2, "b");<]*/
    /*[>listAppend(list2, "a");<]*/
    /*[>listAppend(list2, "c");<]*/
    /*[>listIterate(list2, printString);<]*/
    /*[>printf("%d\n", listEqual(list1, list2, strcmp));<]*/
    /*[>list* list3 = listCreate();<]*/
    /*[>listAppend(list3, "a");<]*/
    /*[>listAppend(list3, "a");<]*/
    /*[>listAppend(list3, "b");<]*/
    /*[>listAppend(list3, "c");<]*/
    /*[>listIterate(list3, printString);<]*/
    /*[>printf("%d\n", listIsEquivalent(list1, list3, strcmp));<]*/


    /*list* l1 = listCreate();*/
    /*listAppend(l1, newRecord("a", typeLong));*/
    /*listAppend(l1, newRecord("b", typeObject));*/
    /*listIterate(l1, printRecord);*/
    /*list* l2 = listCreate();*/
    /*listAppend(l2, newRecord("b", typeLong));*/
    /*listAppend(l2, newRecord("a", typeLong));*/
    /*listIterate(l2, printRecord);*/

    /*printf("%d\n", listIsEquivalent(l1, l2, recordComparator));*/

    /*return 1;*/
/*}*/



