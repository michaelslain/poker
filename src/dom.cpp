#include "dom.hpp"
#include "raylib.h"

// Initialize static member
DOM* DOM::globalInstance = nullptr;

DOM::DOM() {
}

DOM::~DOM() {
    Cleanup();
}

void DOM::AddObject(Object* obj) {
    if (obj == nullptr) {
        TraceLog(LOG_ERROR, "DOM::AddObject: obj is nullptr!");
        return;
    }
    
    objects.push_back(obj);
}

void DOM::RemoveObject(Object* obj) {
    for (size_t i = 0; i < objects.size(); i++) {
        if (objects[i] == obj) {
            objects.erase(objects.begin() + i);
            return;
        }
    }
}

void DOM::RemoveAndDelete(Object* obj) {
    RemoveObject(obj);
    delete obj;
}

Object* DOM::FindObjectByID(int id) {
    for (Object* obj : objects) {
        if (obj && obj->GetID() == id) {
            return obj;
        }
    }
    return nullptr;
}

void DOM::Cleanup() {
    objects.clear();
}

void DOM::SetGlobal(DOM* dom) {
    globalInstance = dom;
}

DOM* DOM::GetGlobal() {
    return globalInstance;
}
