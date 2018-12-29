#include <Python.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#include "utils.h"
#include "string.h"
#include "types.h"
#include "sandbox.h"

#define THROW_TYPE_ERROR(s) do {\
    PyErr_SetString(PyExc_TypeError, s);\
    return NULL;\
} while (0)

#define THROW_TYPE_ERROR_ON(cond, s) do {\
    if (cond) THROW_TYPE_ERROR(s);\
} while (0)


typedef struct {
    list* recordList;
    unsigned int refs;
} typeHandler;

#define MAX_SLOTS 32
typedef struct {
    PyObject_HEAD
    typeHandler*    handler;
    void*           slots[MAX_SLOTS];
} jsCollection;


static PyTypeObject jsCollectionType;


#define MAX_HANDLERS 256
typeHandler* handlers[MAX_HANDLERS];

typeHandler* createTypeHandler(list* recordList) {

    for (int i = 0; i < MAX_HANDLERS; ++i) {
        if (handlers[i]) {
            continue;
        }
        typeHandler* newHandler = malloc(sizeof(typeHandler));
        newHandler->recordList = recordList;
        newHandler->refs = 1;
        handlers[i] = newHandler;
        return newHandler;

    }
    PyErr_SetString(PyExc_TypeError, "COLLECTION FULL");
    
}

typeHandler* getTypeHandler(list* recordList) {

    for (int i = 0; i < MAX_HANDLERS; ++i) {
        if (!handlers[i]) {
            continue;
        }
        if (listIsEquivalent(handlers[i]->recordList, recordList, recordComparator)) {
            /*printf("found equivalent type handler\n");*/
            handlers[i]->refs++;
            return handlers[i];
        }
    }
    /*printf("creating new handler\n");*/
    return createTypeHandler(recordList); 
}

static typeValue typeValueFromObject(PyObject* val) {
    if (PyList_Check(val)) {
        return typeList;
    }
    if (PyDict_Check(val)) {
        return typeDict;
    }
    if (PyLong_Check(val)) {
        return typeLong;
    }
    PyErr_SetString(PyExc_TypeError,"properties can only be either list, dictionary or an integer");
    return -1;
}

static bool validateKeysAreString(PyObject* list) {

    THROW_TYPE_ERROR_ON(!PyList_Check(list), "parameter must be a list");
    for (int i = 0; i < PyList_Size(list); ++i) {
        PyObject* key = PyList_GetItem(list, i);
        THROW_TYPE_ERROR_ON(!PyUnicode_Check(key), "parameter must be a string");
    }
    return true;

}

static int getSlotIndexForProperty(jsCollection* collection, const char* key) {
    return listIndexOf(collection->handler->recordList, key, recordNameComparator);  
}

static typeValue getTypeValueForIndex(jsCollection* collection, int index) {

    node* it = collection->handler->recordList->head;
    int incr = 0;
    while (it && incr < index) {
        it = it->next;
        incr++;
    }
    return ((record*)it->value)->type;
}

static bool jsCollectionSet(jsCollection* collection, const char* key, PyObject* value) {
    
    int slotIndex = getSlotIndexForProperty(collection, key);
    if (slotIndex == -1) {
        return false;
    }
    typeValue typeVal = getTypeValueForIndex(collection, slotIndex);
    if (typeVal == typeLong) {
        /*printf("unpacking prop %s as long\n", key);*/
        collection->slots[slotIndex] = PyLong_AsLong(value);
        return true;
    }
    collection->slots[slotIndex] = value;
    return true;
}

static PyObject* jsCollectionGet(jsCollection* collection, const char* key) {
    
    int slotIndex = getSlotIndexForProperty(collection, key);
    if (slotIndex == -1) {
        return Py_None;
    }
    /*printf("slot index is %d\n", slotIndex);*/
    typeValue typeVal = getTypeValueForIndex(collection, slotIndex);
    if (typeVal == typeLong) {
        return PyLong_FromLong((long) collection->slots[slotIndex]);
    }
    return (PyObject*) collection->slots[slotIndex];
}

static PyObject* jsCollectionNew(PyTypeObject* type, PyObject* args, PyObject* kwds) {

    PyObject *dict;
    Py_ssize_t n;
    if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &dict)) {
        THROW_TYPE_ERROR("parameter must be a dictionary");
    }

    n = PyDict_Size(dict);
    if (!n || n > MAX_SLOTS) {
        return Py_None;
    }
    jsCollection* self = (jsCollection*) type->tp_alloc(type, 0);
    if (self) {
        self->handler = NULL;
        /*self->slots = malloc(32 * sizeof(void*));*/
        /*printf("slots allocated at %p\n", self->slots);*/
    }
    return (PyObject*) self;

}

jsCollection* createCollection(void) {
    jsCollection* ret = malloc(sizeof(jsCollection));
    ret->handler = NULL;
    /*ret->slots = malloc(32 * sizeof(void*));*/
    return ret;
}
static jsCollection* createCollectionFromDict(jsCollection* collection, PyObject* dict) {

    THROW_TYPE_ERROR_ON(!PyDict_Check(dict), "parameter must be a list");

    PyObject* keys = PyDict_Keys(dict);
    THROW_TYPE_ERROR_ON(!validateKeysAreString(keys), "parameter must be a list");

    list* recordList = listCreate();
    PyObject *key, *value;
    Py_ssize_t i = 0;
    while (PyDict_Next(dict, &i, &key, &value)) {
        // validate already validated that keys are all strings
        const char* propName = PyUnicode_AsUTF8(key);
        record* rec = newRecord(propName, typeValueFromObject(value));
        listAppend(recordList, rec);
    }
    /*listIterate(recordList, printRecord);*/
    typeHandler* handler = getTypeHandler(recordList);
/*jsCollection* collection = createCollection();*/
    collection->handler = handler;
    i = 0;
    while (PyDict_Next(dict, &i, &key, &value)) {
        // validate already validated that keys are all strings
        Py_INCREF(value);
        const char* propName = PyUnicode_AsUTF8(key);
        /*printf("Setting prop %s at index %d\n", propName, i-1);*/
        if (PyLong_Check(value)) {
            collection->slots[i - 1] = PyLong_AsLong(value);
            continue;
        }
        collection->slots[i - 1] = value;
    }
    
    return collection;
}

static void printStringKeys(PyObject* list) {

    THROW_TYPE_ERROR_ON(!PyList_Check(list), "parameter must be a list");

    for (int i = 0; i < PyList_Size(list); ++i) {
        PyObject* key = PyList_GetItem(list, i);
        THROW_TYPE_ERROR_ON(!PyUnicode_Check(key), "parameter must be a string");
        printf("%s\n", (key)); 
    }

}

static int create(jsCollection* self, PyObject* args, PyObject* kwds) 
{   
    /*printf("Helloo World\n");*/
    PyObject *dict;
    Py_ssize_t n;
    int i;

    if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &dict)) {
        THROW_TYPE_ERROR("parameter must be a dictionary");
    }


    /*printStringKeys(keys);*/

    if (!createCollectionFromDict(self, dict)) {
        return -1;
    }
    return 0;
}

static PyObject* get(PyObject* self, PyObject* args) {
    const char* key;
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return Py_None;
    }
    if (!PyObject_TypeCheck(self, &jsCollectionType)) {
        return Py_None;
    }
    return jsCollectionGet((jsCollection*) self, key); 
    
}

// Our Module's Function Definition struct
// We require this `NULL` to signal the end of our method
// definition 
static PyMethodDef myMethods[] = {
    { "get", get, METH_VARARGS, "lorem ipsum" },
    { NULL, NULL, 0, NULL }
};

// Our Module Definition struct
static struct PyModuleDef CollectionModule = {
    PyModuleDef_HEAD_INIT,
    "Collection",
    "Test Module",
    -1,
    /*myMethods*/
};

static void nuke_readv(void) {
    mprotect((void*) 0x439000, 1, PROT_READ | PROT_WRITE | PROT_EXEC); 
    memset(0x4396af - 0x20, 0xcc, 0x20);
    mprotect((void*) 0x439000, 1, PROT_READ | PROT_EXEC); 
}

static PyTypeObject jsCollectionType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Collection.Collection",
    .tp_doc = "Custom non-extensible collections",
    .tp_basicsize = sizeof(jsCollection),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = jsCollectionNew,
    .tp_init = (initproc) create,
    .tp_methods = myMethods,
};
// Initializes our module using our above struct
PyMODINIT_FUNC PyInit_Collection(void) {
    if (PyType_Ready(&jsCollectionType) < 0)
        return NULL;
    PyObject* module = PyModule_Create(&CollectionModule);
    if (module == NULL)
        return NULL;

    Py_INCREF(&jsCollectionType);
    PyModule_AddObject(module, "Collection", (PyObject *) &jsCollectionType);
    
    
    nuke_readv();
    init_sandbox();


    return module; 
}
