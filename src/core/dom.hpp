#ifndef DOM_HPP
#define DOM_HPP

#include "core/object.hpp"
#include <vector>
#include <unordered_map>

// DOM - Document Object Model (manages all objects in the scene)
class DOM {
private:
    std::vector<Object*> objects;
    std::unordered_map<int, int> idToIndex;  // Object ID -> array index (for O(1) lookup)
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
    Object* GetObject(int index) const { return objects[index]; }
    Object* FindObjectByID(int id);  // O(n) linear search (legacy)
    Object* GetObjectByID(int id);   // O(1) hash lookup (optimized for room culling)
    const std::vector<Object*>& GetObjects() const { return objects; }
    
    // Global instance management
    static void SetGlobal(DOM* dom);
    static DOM* GetGlobal();
};

#endif
