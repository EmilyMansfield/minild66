#ifndef GAME_CONTAINER_HPP
#define GAME_CONTAINER_HPP

#include <map>
#include <memory>
#include <SFML/System.hpp>
#include <iostream>
#include <cstdlib>

#include "game_map.hpp"
#include "character.hpp"
#include "entity_manager.hpp"

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
    bool add(const std::string& characterId, Team team, EntityManager* mgr, sf::Uint8* charId)
    {
        Team assignedTeam = Team::None;

        // Count team members
        sf::Uint8 team1Count = 0;
        sf::Uint8 team2Count = 0;
        for(auto& ch : characters)
        {
            if(ch.second.team == Team::One) ++team1Count;
            else if(ch.second.team == Team::Two) ++team2Count;
        }

        // If both are available and desired, pick the one with the
        // fewest players
        if((team1Count < playersPerTeam || team2Count < playersPerTeam) && team == Team::Any)
        {
            assignedTeam = team1Count <= team2Count ? Team::One : Team::Two;
        }
        else if(team1Count < playersPerTeam && team == Team::One)
        {
            assignedTeam = Team::One;
        }
        else if(team2Count < playersPerTeam && team == Team::Two)
        {
            assignedTeam = Team::Two;
        }
        if(assignedTeam == Team::None) return false;

        // If on the server, add to the next available slot
        if(*charId == 255 || characters.count(*charId) > 0)
            *charId = characters.size();

        CharWrapper character(characterId, assignedTeam, mgr);
        // Calculate their starting position
        // TODO: Do this with proper spawns
        sf::Vector2f startPos(
            map->tilemap.w * (0.1f + 0.8f * (float)(std::rand()) / RAND_MAX),
            map->tilemap.h * (0.1f + 0.8f * (float)(std::rand()) / RAND_MAX));
        character.c.pfHelper = PathfindingHelper(startPos, startPos, &map->graph);
        character.c.setPos(startPos);
        characters[*charId] = character;

        return true;
    }
};

#endif /* GAME_CONTAINER_HPP */
