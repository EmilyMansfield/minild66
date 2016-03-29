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
    sf::UdpSocket mSocket;

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

        Event() {}
    };

private:
        std::queue<Event> mEventQueue;

public:

    NetworkManager(const JsonBox::Value& v)
    {
        mPort = 49518;
        JsonBox::Object o = v.getObject();
        if(o.find("port") != o.end())
        {
            mPort = o["port"].getInteger();
        }

        // Open UDP socket
        if(mSocket.bind(mPort) != sf::Socket::Done)
        {
            throw std::runtime_error("Failed to open socket on port "
                + std::to_string(mPort));
        }
        if(ld::isServer) servout << "Bound to port " << mSocket.getLocalPort() << std::endl;
        else             clntout << "Bound to port " << mSocket.getLocalPort() << std::endl;
    }

    NetworkManager() : mPort(49518) {}

    ~NetworkManager()
    {
        mSocket.unbind();
    }

    unsigned short getPort() const
    {
        return mPort;
    }

    // Construct a packet from an event and send it
    sf::Socket::Status send(const Event& event, const sf::IpAddress& remoteAddress, unsigned remotePort)
    {
        sf::Packet packet;
        switch(event.type)
        {
            case Event::Nop:
                packet << event.nop.sender.toInteger()
                       << event.nop.port;
                break;
            case Event::Connect:
                packet << event.connect.sender.toInteger()
                       << event.connect.port
                       << event.connect.gameId
                       << event.connect.charId;
                break;
            case Event::Disconnect:
                packet << event.disconnect.sender.toInteger()
                       << event.disconnect.port
                       << event.disconnect.gameId
                       << event.disconnect.charId;
                break;
            default: return sf::Socket::Error;
        }
        return mSocket.send(packet, remoteAddress, remotePort);
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
        sf::Socket::Status returnCode;
        if((returnCode = mSocket.receive(packet, sender, port)) != sf::Socket::Done)
        {
            if(ld::isServer)
            {
                servout << "Failed with status " << returnCode << std::endl;
                servout << "\tPacket size was " << packet.getDataSize() << std::endl;
                servout << "\t" << std::strerror(errno) << std::endl;
            }
            else
            {
                clntout << "Failed with status " << returnCode << std::endl;
                clntout << "\tPacket size was " << packet.getDataSize() << std::endl;
                clntout << "\t" << std::strerror(errno) << std::endl;
            }
            sf::sleep(sf::seconds(0.1f));
            return false;
        }

        // Extract event type. Can't send and receive enums directly
        // so force them into something we know the size of
        sf::Uint16 t = 0;
        packet >> t;
        if(t == 0 || t >= static_cast<sf::Uint8>(Event::Count))
        {
            // Invalid packet
            if(ld::isServer) servout << "Packet had invalid type" << std::endl;
            else             clntout << "Packet had invalid type" << std::endl;
            return false;
        }
        auto type = static_cast<Event::EventType>(t);

        // Extract ip
        sf::Uint32 eventIp = 0;
        if(!(packet >> eventIp))
        {
            // Invalid packet
            if(ld::isServer) servout << "Packet had invalid ip" << std::endl;
            else             clntout << "Packet had invalid ip" << std::endl;
            return false;
        }

        // Extract port
        sf::Uint16 eventPort = 0;
        if(!(packet >> eventPort))
        {
            // Invalid packet
            if(ld::isServer) servout << "Packet had invalid port" << std::endl;
            else             clntout << "Packet had invalid port" << std::endl;
            return false;
        }

        // Depending on the type of the packet, we extract different
        // data
        switch(type)
        {
            case Event::Nop:
            {
                Event e;
                e.nop = {
                    .sender = sf::IpAddress(eventIp),
                    .port = eventPort
                };
                e.type = Event::Nop;
                mEventQueue.push(e);
                return true;
            }
            case Event::Connect:
            {
                sf::Uint16 gameId = 0;
                sf::Uint8 charId = 0;
                if(!(packet >> gameId >> charId)) return false;
                Event e;
                e.connect = {
                   .sender = sf::IpAddress(eventIp),
                   .port = eventPort,
                   .gameId = gameId,
                   .charId = charId
                };
                e.type = Event::Connect;
                mEventQueue.push(e);
                return true;
            }
            case Event::Disconnect:
            {
                sf::Uint16 gameId = 0;
                sf::Uint8 charId = 0;
                if(!(packet >> gameId >> charId)) return false;
                Event e;
                e.disconnect = {
                   .sender = sf::IpAddress(eventIp),
                   .port = eventPort,
                   .gameId = gameId,
                   .charId = charId
                };
                e.type = Event::Disconnect;
                mEventQueue.push(e);
                return true;
            }
            default: return false;
        }
    }
};

#endif /* NETWORK_MANAGER_HPP */
