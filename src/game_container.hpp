#ifndef GAME_CONTAINER_HPP
#define GAME_CONTAINER_HPP

#include <vector>
#include <memory>
#include <SFML/System.hpp>
#include <iostream>

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
    };

    GameMap* gamemap;
    std::vector<CharWrapper> characters;

    GameContainer() {}

    // Add a character to the given team, if possible
    // Automatically allocate to the team with the fewest players if
    // team is set to Any
    bool add(const std::string& characterId, Team team, EntityManager* mgr)
    {
        Team assignedTeam = Team::None;

        // Count team members
        sf::Uint8 team1Count = 0;
        sf::Uint8 team2Count = 0;
        for(int i = 0; i < characters.size(); ++i)
        {
            CharWrapper* c = &characters[i];
            if(c->team == Team::One) ++team1Count;
            else if(c->team == Team::Two) ++team2Count;
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
        // Add the character to the team
        characters.push_back(CharWrapper(characterId, team, mgr));
        return true;
    }
};

#endif /* GAME_CONTAINER_HPP */
