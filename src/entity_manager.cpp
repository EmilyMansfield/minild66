#include <string>
#include <map>
#include <JsonBox.h>

#include "entity_manager.hpp"
#include "entity.hpp"

EntityManager::EntityManager() {}
EntityManager::~EntityManager()
{
    for(auto& entity : mData)
    {
        delete entity.second;
    }
}

template<typename T>
void EntityManager::load(const std::string& filename)
{
    JsonBox::Value v;
    v.loadFromFile(filename);

    JsonBox::Object o = v.getObject();

    for(auto entity : o)
    {
        std::string key = entity.first;
        mData[key] = dynamic_cast<Entity*>(new T(key, entity.second, this));
    }
}

template<typename T>
T* EntityManager::getEntity(const std::string& id) const
{
    return dynamic_cast<T*>(mData.at(id));
}
