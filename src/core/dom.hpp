#ifndef DOM_HPP
#define DOM_HPP

#include "core/object.hpp"
#include <vector>

// DOM - Document Object Model (manages all objects in the scene)
class DOM {
private:
    std::vector<Object*> objects;
    static DOM* globalInstance;

public:
    DOM();
    ~DOM();

    void AddObject(Object* obj);
    void RemoveObject(Object* obj);
    void RemoveAndDelete(Object* obj);  // Helper: removes from DOM and deletes
    void Cleanup();
    
    // Accessors
    int GetCount() const { return objects.size(); }
    Object* GetObject(int index) { return objects[index]; }
    Object* FindObjectByID(int id);
    const std::vector<Object*>& GetObjects() const { return objects; }
    
    // Global instance management
    static void SetGlobal(DOM* dom);
    static DOM* GetGlobal();
};

#endif
