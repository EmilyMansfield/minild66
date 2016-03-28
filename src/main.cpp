#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <memory>

#include "constants.hpp"
#include "game_state.hpp"
#include "game_state_title.hpp"
#include "game_state_game.hpp"
#include "entity_manager.hpp"
#include "network_manager.hpp"

class Tileset;
class GameMap;
class Character;

int main(int argc, char* argv[])
{
    // Check for a server or a client
    // Should really make two different executables, but
    // there's not really enough time for that
    // TODO: Implement arguments instead of this
    if(argc > 1)
    {
        ld::isServer = true;
    }

    EntityManager entityManager;
    // Load entities
    entityManager.load<Tileset>("tilesets.json");
    entityManager.load<GameMap>("game_map.json");
    entityManager.load<Character>("characters.json");


    NetworkManager networkManager;

    //////////////////////////////////////////////////////////////////
    // SERVER
    //////////////////////////////////////////////////////////////////
    if(ld::isServer)
    {
// TODO TEST CODE REMOVE
        sf::UdpSocket socket;
        socket.bind(49518);

        bool running = true;
        while(running)
        {
            sf::Packet packet;
            sf::IpAddress sender;
            unsigned short port;
            
            // Wait for an incoming connection
            socket.receive(packet, sender, port);
            std::string msg;
            packet >> msg;
            std::cout << sender.toString() << " said: " << msg << std::endl;
        }
//////////////////////////////
    }
    //////////////////////////////////////////////////////////////////
    // CLIENT
    //////////////////////////////////////////////////////////////////
    else
    {
// TODO TEST CODE REMOVE
        // Networking
        sf::UdpSocket socket;
        socket.bind(49519);

        // Send test message
        for(;;)
        {
            sf::Packet packet;
            packet << "hello, world";
            socket.send(packet, sf::IpAddress("192.168.0.43"), 49518);
            sf::sleep(sf::seconds(0.1f));            
        }
////////////////////////////////

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
    }

    return 0;
}
