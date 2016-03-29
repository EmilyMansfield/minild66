#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <stdexcept>
#include <string>
#include <map>
#include <queue>
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
            Count
        };
        EventType type;

        union {
            NopEvent            nop;
            ConnectEvent        connect;
            DisconnectEvent     disconnect;
        };
    };

private:
        std::queue<Event> mEventQueue;

public:

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

    // Take the next event out of the event queue
    bool pollEvent(Event& event)
    {
        if(mEventQueue.empty()) return false;
        event = mEventQueue.front();
        mEventQueue.pop();
        return true;
    }

    // Wait for an incoming connect and parse it as an event. If it
    // was a valid event, add it to the event queue and return true
    bool waitEvent()
    {
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        
        // Wait for an incoming connection
        mSocket.receive(packet, sender, port);

        // Extract event type. Can't send and receive enums directly
        // so force them into something we know the size of
        sf::Uint16 t = 0;
        packet >> t;
        if(t == 0 || t >= static_cast<sf::Uint8>(Event::Count))
        {
            // Invalid packet
            return false;
        }
        auto type = static_cast<Event::EventType>(t);

        // Depending on the type of the packet, we extract different
        // data
        switch(type)
        {
            case Event::Nop:
            {
                Event e =
                {
                    .nop = {
                        .sender = sender
                    },
                    .type = Event::Nop
                };
                mEventQueue.push(e);
                return true;
            }
            case Event::Connect:
            {
                sf::Uint8 gameId = 0;
                sf::Uint8 charId = 0;
                if(!(packet >> gameId >> charId)) return false;
                Event e =
                {
                    .connect = {
                        .sender = sender,
                        .gameId = gameId,
                        .charId = charId
                    },
                    .type = Event::Connect
                };
                mEventQueue.push(e);
                return true;
            }
            case Event::Disconnect:
            {
                sf::Uint8 gameId = 0;
                sf::Uint8 charId = 0;
                if(!(packet >> gameId >> charId)) return false;
                Event e =
                {
                    .disconnect = {
                        .sender = sender,
                        .gameId = gameId,
                        .charId = charId
                    },
                    .type = Event::Disconnect
                };
                mEventQueue.push(e);
                return true;
            }
            default: return false;
        }
    }
};

#endif /* NETWORK_MANAGER_HPP */
