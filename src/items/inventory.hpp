#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <vector>
#include <string>

// Forward declaration to avoid circular dependency
class Item;

class ItemStack {
public:
    Item* item;           // First item in stack
    int count;            // Number of items in stack
    std::string typeString;  // Full type identifier (e.g., "card_spades_ace" or "chip_5")
    
    ItemStack() : item(nullptr), count(0), typeString("") {}
    ItemStack(Item* i, int c, const std::string& type) 
        : item(i), count(c), typeString(type) {}
};

class Inventory {
private:
    std::vector<ItemStack> stacks;

public:
    Inventory();
    ~Inventory();
    
    bool AddItem(Item* item);
    bool RemoveItem(int stackIndex);
    void Cleanup();
    void Sort();  // Sort inventory by type: weapons, cards (by rank), chips (by value)
    
    // Accessors
    int GetStackCount() const { return stacks.size(); }
    ItemStack* GetStack(int index) { 
        if (index < 0 || index >= (int)stacks.size()) return nullptr;
        return &stacks[index]; 
    }
    const std::vector<ItemStack>& GetStacks() const { return stacks; }
    
    // Helper methods to reduce code duplication
    int CountItemsByType(const std::string& typeSubstring) const;
    std::vector<int> GetIndicesByType(const std::string& typeSubstring) const;
    int GetTotalChipValue() const;  // Get total value of all chips in inventory
};

#endif
