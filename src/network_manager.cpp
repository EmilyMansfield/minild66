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
#include "network_manager.hpp"
#include "constants.hpp"

NetworkManager::NetworkManager(const JsonBox::Value& v)
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

NetworkManager::NetworkManager() : mPort(49518) {}

NetworkManager::~NetworkManager()
{
    mSocket.unbind();
}

unsigned short NetworkManager::getPort() const
{
    return mPort;
}

// Construct a packet from an event and send it
sf::Socket::Status NetworkManager::send(const Event& event, const sf::IpAddress& remoteAddress, unsigned remotePort)
{
    sf::Packet packet;
    packet << static_cast<sf::Uint16>(event.type);
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
bool NetworkManager::pollEvent(Event& event)
{
    if(mEventQueue.empty()) return false;
    event = mEventQueue.front();
    mEventQueue.pop();
    return true;
}

// Wait for an incoming connect and parse it as an event. If it
// was a valid event, add it to the event queue and return true
bool NetworkManager::waitEvent()
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
