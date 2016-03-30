#ifndef GAME_CONTAINER_HPP
#define GAME_CONTAINER_HPP

#include <map>
#include <memory>
#include <SFML/System.hpp>
#include <vector>
#include <cstdlib>

#include "game_map.hpp"
#include "character.hpp"
#include "entity_manager.hpp"

class TargetAttack;

class GameContainer
{
public:

    enum class Team
    {
        None,
        One,
        Two,
        Any
    };

    const static sf::Uint8 playersPerTeam = 5;

    // Wraps a Character, containing properties relevant only
    // to this game and not to characters in general
    class CharWrapper
    {
    public:
        Character c;

        sf::Uint32 gold;
        sf::Time respawnTimer;
        sf::Uint32 kills;
        sf::Uint32 assists;
        sf::Uint32 deaths;
        Team team;

        bool isPlayer;

        CharWrapper(const std::string& characterId, Team team, EntityManager* mgr) :
            c(*mgr->getEntity<Character>(characterId)),
            gold(0),
            kills(0),
            assists(0),
            deaths(0),
            team(team)
        {
        }

        CharWrapper() {}
    };

    GameMap* map;
    std::map<sf::Uint8, CharWrapper> characters;
    sf::Uint16 gameId;
    sf::Uint8 client;

    // Attacks being processed
    std::vector<std::shared_ptr<TargetAttack>> targetAttacks;

    GameContainer() {}
    GameContainer(GameMap* map, sf::Uint16 gameId, sf::Uint8 client) :
        map(map),
        gameId(gameId),
        client(client)
    {}

    // Return a pointer to the client's character
    CharWrapper* getClient()
    {
        return &characters.at(client);
    }

    // Add a character to the given team, if possible
    // Automatically allocate to the team with the fewest players if
    // team is set to Any. charId should be set to 255 on the server,
    // otherwise it should be set to what the server told us
    bool add(const std::string& characterId,
        Team team, EntityManager* mgr, sf::Uint8* charId);

    void update(float dt);
};

#endif /* GAME_CONTAINER_HPP */
