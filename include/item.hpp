#ifndef ITEM_HPP
#define ITEM_HPP

#include "interactable.hpp"

class Item : public Interactable {
public:
    bool usable;  // Can this item be used via left click? (weapons=true, substances=true, cards/chips=false)

    Item(Vector3 pos = {0.0f, 0.0f, 0.0f});
    virtual ~Item() = default;

    virtual void DrawIcon(Rectangle destRect);
    std::string GetType() const override;

    // Virtual method to determine if this item type can stack
    // Default: true (stackable). Override in derived classes for unique items.
    virtual bool CanStack() const { return true; }

    // Virtual method to get display count text for inventory UI
    // Default: show stack count if > 1. Override for custom display (e.g., ammo count).
    virtual const char* GetDisplayCount(int stackCount) const;

    // Virtual method for using the item (weapons shoot, substances consume)
    // Default: does nothing. Override in Weapon and Substance.
    virtual void Use() {}

    // Virtual method for drawing the item in first-person (when held by player)
    // Default: does nothing. Override in items that should be visible when held.
    virtual void DrawHeld(Camera3D camera) { (void)camera; }
};

#endif
