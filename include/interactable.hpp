#ifndef INTERACTABLE_HPP
#define INTERACTABLE_HPP

#include "raylib.h"
#include "object.hpp"
#include <functional>

class Interactable : public Object {
public:
    float interactRange;
    std::function<void(Interactable*)> onInteract;

    Interactable(Vector3 pos = {0.0f, 0.0f, 0.0f});
    virtual ~Interactable() = default;

    virtual void Interact();
    void DrawPrompt(Camera3D camera);
    const char* GetType() const override;
};

#endif
