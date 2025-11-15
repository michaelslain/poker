#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "core/scene.hpp"
#include <map>
#include <string>
#include <functional>

class PhysicsWorld;

// Type alias for scene factory functions
using SceneFactory = std::function<Scene*(PhysicsWorld*)>;

// Singleton scene manager - manages scene switching
class SceneManager {
private:
    static SceneManager* instance;
    
    std::map<std::string, SceneFactory> sceneFactories;
    Scene* currentScene;
    std::string currentSceneName;
    
    // Private constructor for singleton
    SceneManager();
    
public:
    // Singleton access
    static SceneManager* GetInstance();
    static void DestroyInstance();
    
    // Delete copy constructor and assignment operator
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    
    // Scene management
    void RegisterSceneFactory(const std::string& name, SceneFactory factory);
    Scene* CreateScene(const std::string& name, PhysicsWorld* physics);
    void SetCurrentScene(const std::string& name) { currentSceneName = name; }
    
    // Accessors
    Scene* GetCurrentScene() const { return currentScene; }
    const std::string& GetCurrentSceneName() const { return currentSceneName; }
    bool HasScene(const std::string& name) const;
    
    // Cleanup
    void Cleanup();
};

#endif
