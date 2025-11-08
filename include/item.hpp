#ifndef ITEM_HPP
#define ITEM_HPP

#include "interactable.hpp"

class Item : public Interactable {
public:
    Item(Vector3 pos = {0.0f, 0.0f, 0.0f});
    virtual ~Item() = default;

    virtual void DrawIcon(Rectangle destRect);
    std::string GetType() const override;
};

#endif
