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

#include "game_container.hpp"

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
        };
        struct ConnectEvent
        {
            sf::IpAddress ip;
            sf::Uint16 port;
            sf::Uint16 gameId; // Game connecting to
            sf::Uint8 charId; // Which character slot they are
            GameContainer::Team team; // Which team they were assigned to
        };
        struct DisconnectEvent
        {
            sf::IpAddress ip;
            sf::Uint16 port;
            sf::Uint16 gameId; // Game they were connected to
            sf::Uint8 charId; // Which slot they were in
        };
        struct GameFullEvent
        {
            sf::Uint16 gameId;
        };
        struct MoveEvent
        {
            sf::Uint16 gameId;
            sf::Uint8 charId;
            sf::Vector2f target;
            sf::Vector2f pos;
        };
        struct DamageEvent
        {
            sf::Uint16 gameId;
            sf::Uint8 charId;
            float hp;
        };
        struct AutoAttackEvent
        {
            sf::Uint16 gameId;
            sf::Uint8 charId;
            sf::Uint8 targetId;
            bool cancel; // Attacks can be cancelled mid-animation
        };

        enum EventType {
            Nop,        // No request
            Connect,    // A player has connected
            Disconnect, // A player has disconnected
            GameFull,   // No room to join a game, both teams full
            Move,       // Creature is moving
            Damage,     // Creature has taken damage (or healed)
            AutoAttack, // Creature is attacking (or cancelling)
            Count
        };
        EventType type;

        union {
            NopEvent            nop;
            ConnectEvent        connect;
            DisconnectEvent     disconnect;
            GameFullEvent       gameFull;
            MoveEvent           move;
            DamageEvent         damage;
            AutoAttackEvent     autoAttack;
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
    bool disconnectFromServer(sf::Uint16 gameId, sf::Uint8 charId);

    // Construct a packet from an event and send it
    sf::Socket::Status send(const Event& event,
        const sf::IpAddress& remoteAddress,
        unsigned short remotePort);
    sf::Socket::Status send(const Event& event);
    sf::Socket::Status sendSelf(const Event& event);

    // Take the next event out of the event queue
    bool pollEvent(Event& event);

    // Wait for an incoming connect and parse it as an event. If it
    // was a valid event, add it to the event queue and return true
    bool waitEvent();

};

// Overload packet operators for common structures
template<typename T>
sf::Packet& operator<<(sf::Packet& packet, const sf::Vector2<T>& v)
{
    return packet << v.x << v.y;
}
template<typename T>
sf::Packet& operator>>(sf::Packet& packet, sf::Vector2<T>& v)
{
    return packet >> v.x >> v.y;
}

#endif /* NETWORK_MANAGER_HPP */
