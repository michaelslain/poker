#ifndef PERSON_HPP
#define PERSON_HPP

#include "raylib.h"
#include "object.hpp"
#include "inventory.hpp"
#include <string>

class Person : public Object {
protected:
    Inventory inventory;
    std::string name;
    float height;  // Height multiplier for the mesh (1.0 = normal, >1.0 = taller)

    // Seating
    bool isSeated;          // Whether person is seated at a table
    Vector3 seatPosition;   // Position where person is seated

public:
    Person(Vector3 pos, const std::string& personName, float personHeight = 1.0f);
    virtual ~Person() = default;

    // Override Draw to render pitch black (unaffected by lighting)
    void Draw(Camera3D camera) override;

    // Seating methods
    virtual void SitDown(Vector3 seatPos);
    virtual void StandUp();
    bool IsSeated() const { return isSeated; }

    // Accessors
    Inventory* GetInventory() { return &inventory; }
    const std::string& GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }
    float GetHeight() const { return height; }
    void SetHeight(float newHeight) { height = newHeight; }

    const char* GetType() const override;
};

#endif
