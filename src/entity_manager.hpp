#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <string>
#include <map>

#include "entity.hpp"

class EntityManager
{
private:
    std::map<std::string, Entity*> mData;

public:
    EntityManager();
    ~EntityManager();

    template<typename T>
    void load(const std::string& filename);

    template<typename T>
    T* getEntity(const std::string& id) const;
};

template<typename T>
std::string entityToString();

#endif /* ENTITY_MANAGER_HPP */
