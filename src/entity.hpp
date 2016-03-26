#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <JsonBox.h>
#include <string>

class EntityManager;

class Entity
{
public:

    std::string id;

    explicit Entity(const std::string& id);
    virtual ~Entity();

    virtual void load(const JsonBox::Value& v, EntityManager* mgr) = 0;

    template<typename T>
    bool isA() const;
};

#endif /* ENTITY_HPP */
