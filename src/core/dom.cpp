#include "core/dom.hpp"
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

    int index = objects.size();
    objects.push_back(obj);
    idToIndex[obj->GetID()] = index;  // Maintain ID->index mapping
}

void DOM::RemoveObject(Object* obj) {
    for (size_t i = 0; i < objects.size(); i++) {
        if (objects[i] == obj) {
            objects.erase(objects.begin() + i);

            // Rebuild ID-to-index map after removal (indices shift)
            idToIndex.clear();
            for (size_t j = 0; j < objects.size(); j++) {
                idToIndex[objects[j]->GetID()] = j;
            }
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

Object* DOM::GetObjectByID(int id) {
    auto it = idToIndex.find(id);
    if (it != idToIndex.end()) {
        int index = it->second;
        if (index >= 0 && index < (int)objects.size()) {
            return objects[index];
        }
    }
    return nullptr;
}

void DOM::Cleanup() {
    objects.clear();
    idToIndex.clear();
}

void DOM::SetGlobal(DOM* dom) {
    globalInstance = dom;
}

DOM* DOM::GetGlobal() {
    return globalInstance;
}
