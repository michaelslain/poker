#ifndef DOM_H
#define DOM_H

#include "object.h"

// Forward declaration for use in other headers
typedef struct DOM DOM;

// DOM - Document Object Model (list of all objects in the scene)
struct DOM {
    Object** objects;
    int count;
    int capacity;
};

void DOM_Init(DOM* dom, int initialCapacity);
void DOM_AddObject(DOM* dom, Object* obj);
void DOM_RemoveObject(DOM* dom, Object* obj);
void DOM_Cleanup(DOM* dom);

// Global DOM instance accessors
void DOM_SetGlobal(DOM* dom);
DOM* DOM_GetGlobal(void);

#endif
