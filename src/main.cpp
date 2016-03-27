#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

#include "constants.hpp"
#include "game_state.hpp"
#include "game_state_title.hpp"
#include "game_state_game.hpp"
#include "entity_manager.hpp"

class Tileset;
class GameMap;

int main()
{
    EntityManager entityManager;

    // Load entities
    entityManager.load<Tileset>("tilesets.json");
    entityManager.load<GameMap>("game_map.json");

    // Create the window
    sf::RenderWindow window(sf::VideoMode(ld::width, ld::height), ld::title,
            sf::Style::Titlebar | sf::Style::Close);

    // Disable key repeating
    window.setKeyRepeatEnabled(false);
    
    // Clamp framerate
    window.setVerticalSyncEnabled(true);

    // Window viewport
    sf::View view(sf::FloatRect(0, 0, ld::width, ld::height));
    view.setCenter(ld::width / 2.0f, ld::height / 2.0f);
    window.setView(view);

    // Pointer to current game state
    std::shared_ptr<GameState> state;

    // Load initial game state
    //state.reset(new GameStateTitle(state, state, &entityManager));
    state.reset(new GameStateGame(state, state, &entityManager));

    // Frame time
    sf::Clock clock;

    // Game loop
    while(window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        // Handle events
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                window.close();
            }
            if(state != nullptr) state->handleEvent(event, window);
        }

        if(state != nullptr)
        {
            // Update window
            state->handleInput(dt, window);
            state->update(dt);
            // Draw window
            window.clear(sf::Color::Black);
            window.draw(*state);
            window.display();
        }
        else
        {
            window.close();
        }
    }

    return 0;
}
