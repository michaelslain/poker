#include "core/scene.hpp"

Scene::Scene(const std::string& sceneName, std::vector<Object*> objects)
    : initialObjects(objects), name(sceneName) {
}
