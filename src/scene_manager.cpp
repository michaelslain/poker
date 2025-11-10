#include "scene_manager.hpp"
#include <raylib.h>

// Initialize static instance
SceneManager* SceneManager::instance = nullptr;

SceneManager::SceneManager() 
    : currentScene(nullptr), currentSceneName("") {
}

SceneManager* SceneManager::GetInstance() {
    if (instance == nullptr) {
        instance = new SceneManager();
    }
    return instance;
}

void SceneManager::DestroyInstance() {
    if (instance != nullptr) {
        instance->Cleanup();
        delete instance;
        instance = nullptr;
    }
}

void SceneManager::RegisterSceneFactory(const std::string& name, SceneFactory factory) {
    sceneFactories[name] = factory;
}

Scene* SceneManager::CreateScene(const std::string& name, PhysicsWorld* physics) {
    auto it = sceneFactories.find(name);
    if (it != sceneFactories.end()) {
        if (currentScene != nullptr) {
            delete currentScene;
        }
        currentScene = it->second(physics);  // Call factory function
        currentSceneName = name;
        return currentScene;
    } else {
        TraceLog(LOG_ERROR, "SCENE_MANAGER: Scene factory '%s' not found!", name.c_str());
        return nullptr;
    }
}

bool SceneManager::HasScene(const std::string& name) const {
    return sceneFactories.find(name) != sceneFactories.end();
}

void SceneManager::Cleanup() {
    if (currentScene != nullptr) {
        delete currentScene;
        currentScene = nullptr;
    }
    sceneFactories.clear();
}
