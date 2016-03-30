#include <string>
#include <SFML/System.hpp>

#include "game_container.hpp"
#include "entity_manager.hpp"
#include "target_attack.hpp"

bool GameContainer::add(const std::string& characterId,
	Team team, EntityManager* mgr, sf::Uint8* charId)
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
    sf::Vector2f startPos = (assignedTeam == Team::One ? 
    	map->team1Spawns[*charId] : map->team2Spawns[*charId]);
   
    character.c.pfHelper = PathfindingHelper(startPos, startPos, &map->graph);
    character.c.setPos(startPos);
    characters[*charId] = character;

    return true;
}

void GameContainer::update(float dt)
{
    for(auto& ch : characters)
    {
        ch.second.c.update(dt);
    }
    for(auto attack : targetAttacks)
    {
        attack->update(dt);
    }
    // Shitty way of deleting finished events
    // No time or inclination right now for a proper solution
    for(int i = 0; i < targetAttacks.size(); ++i)
    {
        if(targetAttacks[i]->isDone())
        {
            targetAttacks.erase(targetAttacks.begin()+i);
            --i;
        }
    }
}
