#include <string>

#include "entity.hpp"
#include "entity_manager.hpp"

Entity::Entity(const std::string& id) : id(id) {}
Entity::~Entity() {}

template<typename T>
bool Entity::isA() const
{
    return id.substr(0, entityToString<T>().size()) == entityToString<T>();
}

