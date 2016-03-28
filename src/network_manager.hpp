#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <stdexcept>
#include <string>
#include <map>
#include <functional>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>

class NetworkManager
{
private:
    unsigned short mPort;
    sf::UdpSocket mSocket;

public:

    // Use similar structure to sf::Event
    class Event
    {
    public:
        struct NopEvent {
            sf::IpAddress sender;
        };
        struct ConnectEvent
        {
            sf::IpAddress sender;
            sf::Uint8 gameId; // Game connecting to
            sf::Uint8 charId; // Which character slot they are
        };
        struct DisconnectEvent
        {
            sf::IpAddress sender;
            sf::Uint8 gameId; // Game they were connected to
            sf::Uint8 charId; // Which slot they were in
        };

        enum EventType {
            Nop,        // No request
            Connect,    // A player has connected
            Disconnect, // A player has disconnected
            Move,       // Creature is moving
        };
        EventType type;

        union {
            NopEvent            nop;
            ConnectEvent        connect;
            DisconnectEvent     disconnect;
        };
    };

    NetworkManager() : mPort(49518) {}
    NetworkManager(unsigned short port) :
        mPort(port)
    {
        // Open UDP socket
        sf::UdpSocket mSocket;
        if(mSocket.bind(mPort) != sf::Socket::Done)
        {
            throw std::runtime_error("Failed to open socket on port "
                + std::to_string(mPort));
        }
    }
};

#endif /* NETWORK_MANAGER_HPP */
