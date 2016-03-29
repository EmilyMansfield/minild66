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
#include <sstream>
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

NetworkManager::NetworkManager() : mPort(49518), mRemotePort(0) {}

NetworkManager::~NetworkManager()
{
    mSocket.unbind();
}

bool NetworkManager::connectToServer(const sf::IpAddress &remoteAddress,
    unsigned short remotePort,
    sf::Uint16 gameId)
{
    // Bail if server is connecting to server
    if(ld::isServer) return false;

    // Work out if this is the remote address is WAN or LAN
    // This is not great and probably doesn't actually work,
    // it just assumes an IP is local iff it's in reserved in RFC1918.
    // Needed because the address is used in events to identify the host.
    // Note that this does not support a situation where people are connected
    // to the same server with some on LAN and some on WAN, but this solution
    // is simple and that's all I care about for now
    // TODO: Support simultaneous LAN & WAN connections
    std::string ipStr = remoteAddress.toString();
    for(int i = 0; i < ipStr.size(); ++i)
    {
        if(ipStr[i] == '.') ipStr[i] = ' ';
    }
    std::istringstream ipStrIs(ipStr);
    int blocks[4] = {0,0,0,0};
    ipStrIs >> blocks[0] >> blocks[1] >> blocks[2] >> blocks[3];
    if(remoteAddress == sf::IpAddress::LocalHost ||
        blocks[0] == 10 ||
        (blocks[0] == 172 && 16 <= blocks[1] && blocks[1] <= 31) ||
        (blocks[0] == 192 && blocks[1] == 168))
    {
        mIp = sf::IpAddress::getLocalAddress();
    }
    else
    {
        mIp = sf::IpAddress::getPublicAddress();
    }

    Event e;
    e.connect = {
        .sender = mIp,
        .port = mPort,
        .gameId = gameId,
        .charId = 0 // Allocated by server
    };
    e.type = NetworkManager::Event::Connect;
    if(send(e, remoteAddress, remotePort) != sf::Socket::Done)
    {
        clntout << "Failed to send connect message to "
            << remoteAddress.toString() << " on port "
            << remotePort << std::endl;
        clntout << "\t" << std::strerror(errno) << std::endl;
        return false;
    }

    mRemotePort = remotePort;
    mRemoteIp = remoteAddress;

    return true;
}

bool NetworkManager::disconnectFromServer()
{
    if(ld::isServer) return false;

    Event e;
    e.disconnect = {
        .sender = mIp,
        .port = mPort,
        .gameId = 0,
        .charId = 0
    };
    e.type = NetworkManager::Event::Disconnect;
    if(send(e, mRemoteIp, mRemotePort) != sf::Socket::Done)
    {
        clntout << "Failed to send disconnect message to "
            << mRemoteIp.toString() << " on port "
            << mRemotePort << std::endl;
        clntout << "\t" << std::strerror(errno) << std::endl;
        return false;
    }

    return true;
}

unsigned short NetworkManager::getPort() const
{
    return mPort;
}

const sf::IpAddress& NetworkManager::getIp() const
{
    return mIp;
}

// Construct a packet from an event and send it
sf::Socket::Status NetworkManager::send(const Event& event,
    const sf::IpAddress& remoteAddress,
    unsigned short remotePort)
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
        case Event::GameFull:
            packet << event.gameFull.sender.toInteger()
                   << event.gameFull.port
                   << event.gameFull.gameId;
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
        case Event::GameFull:
        {
            sf::Uint16 gameId = 0;
            if(!(packet >> gameId)) return false;
            Event e;
            e.gameFull = {
                .sender = sf::IpAddress(eventIp),
                .port = eventPort,
                .gameId = gameId,
            };
            e.type = Event::GameFull;
            mEventQueue.push(e);
            return true;
        }
        default: return false;
    }
}
