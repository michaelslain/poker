#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <string>

// Forward declaration
class Object;

// Scene class - contains a list of initial objects to spawn
class Scene {
private:
    std::vector<Object*> initialObjects;
    std::string name;
    
public:
    Scene(const std::string& sceneName, std::vector<Object*> objects);
    ~Scene() = default;
    
    // Get the initial objects for this scene
    std::vector<Object*>& GetInitialObjects() { return initialObjects; }
    const std::string& GetName() const { return name; }
};

#endif
