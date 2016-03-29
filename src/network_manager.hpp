#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <stdexcept>
#include <string>
#include <map>
#include <queue>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <JsonBox.h>
#include <iostream>
#include <cerrno>
#include <cstring>

// Should probably just define a new stream?
#define servout (std::cout << "[SERVER] ")
#define clntout (std::cout << "[CLIENT] ")

class NetworkManager
{
private:

    unsigned short mPort;
    sf::IpAddress mIp;
    sf::UdpSocket mSocket;

    // The server the client is connected to. Ignored on a server
    unsigned short mRemotePort;
    sf::IpAddress mRemoteIp;

public:

    // Use similar structure to sf::Event
    class Event
    {
    public:
        struct NopEvent {
            sf::IpAddress sender;
            sf::Uint16 port;
        };
        struct ConnectEvent
        {
            sf::IpAddress sender;
            sf::Uint16 port;
            sf::Uint16 gameId; // Game connecting to
            sf::Uint8 charId; // Which character slot they are
        };
        struct DisconnectEvent
        {
            sf::IpAddress sender;
            sf::Uint16 port;
            sf::Uint16 gameId; // Game they were connected to
            sf::Uint8 charId; // Which slot they were in
        };
        struct GameFullEvent
        {
            sf::IpAddress sender;
            sf::Uint16 port;
            sf::Uint16 gameId;
        };

        enum EventType {
            Nop,        // No request
            Connect,    // A player has connected
            Disconnect, // A player has disconnected
            GameFull,   // No room to join a game, both teams full
            Move,       // Creature is moving
            Count
        };
        EventType type;

        union {
            NopEvent            nop;
            ConnectEvent        connect;
            DisconnectEvent     disconnect;
            GameFullEvent       gameFull;
        };

        Event() {}
    };

private:

    std::queue<Event> mEventQueue;

public:

    NetworkManager(const JsonBox::Value& v);
    NetworkManager();
    ~NetworkManager();

    unsigned short getPort() const;
    const sf::IpAddress& getIp() const;

    // Connect to a server. Does nothing on a client
    bool connectToServer(const sf::IpAddress &remoteAddress,
        unsigned short remotePort,
        sf::Uint16 gameId);

    // Disconnect from server. Does nothing on a client or
    // if not connected to a server
    bool disconnectFromServer();

    // Construct a packet from an event and send it
    sf::Socket::Status send(const Event& event,
        const sf::IpAddress& remoteAddress,
        unsigned short remotePort);

    // Take the next event out of the event queue
    bool pollEvent(Event& event);

    // Wait for an incoming connect and parse it as an event. If it
    // was a valid event, add it to the event queue and return true
    bool waitEvent();
};

#endif /* NETWORK_MANAGER_HPP */
