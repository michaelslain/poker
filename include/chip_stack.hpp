#ifndef CHIP_STACK_HPP
#define CHIP_STACK_HPP

#include "object.hpp"
#include "chip.hpp"
#include <vector>
#include <map>

class ChipStack : public Object {
private:
    std::vector<Chip*> chips;  // All chips in the stack
    std::map<int, std::vector<Chip*>> chipsByValue;  // Organized by denomination
    
    void OrganizeChips();  // Reorganize chip positions based on value

public:
    ChipStack(Vector3 pos);
    ~ChipStack();
    
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    std::string GetType() const override;
    
    // Chip management
    void AddChip(Chip* chip);
    void AddChips(const std::vector<Chip*>& newChips);
    void Clear();  // Remove all chips (doesn't delete them)
    std::vector<Chip*> RemoveAll();  // Remove and return all chips
    
    int GetTotalValue() const;
    int GetChipCount() const { return chips.size(); }
    bool IsEmpty() const { return chips.empty(); }
};

#endif
