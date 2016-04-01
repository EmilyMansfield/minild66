#minild66

A small MOBA written in C++ using SFML for MiniLD 66.

A MOBA is an RTS, right?

##Features

  - Still waiting on those

##Progress
###Day 1
  - Simple tiled game map
  - Map can be panned using arrow keys

###Day 2
  - Map can be panned using the mouse, and stays in bounds
  - Characters can be drawn along with animations
  - Started to create navmesh system
    - Convex polygon handling with interior points, intersection, and
      subtraction

###Day 3
  - *Mostly* finished navmesh algorithms
    - Emphasis on mostly, since I then moved on
  - Added navgraph system using traditional grid approach
    - Added BFS and A* pathfinding with static object avoidance and off-grid
      final positions
  - Improved screen resolution support
  - Began to add client/server subdivision (one executable for now)
  - Started to add a network protocol using UDP. Aiming to finish this before
    moving on, since multiple on screen characters are required to do much of
    anything else

###Day 4
  - Started to add a GameContainer to encapsulate each game
  - Added game creation to the server using GameContainer
  - Clients can now join games if they have room
    - Details are given in `config.json`
  - Game state wraps a Game Container, with the client controlling one character
    - TODO Still needs to be synced with other clients

###Day 5
  - Server now identifies clients based on charId and gameID, so supports
    multiple clients with the same IP address
  - `config.json` accepts a default port of 0, which automatically allocates
    a free port
  - Synced movement of characters between clients (WOOO!)
  - Made graphics a little bit nicer
  - Added a percentage `Bar` class and used it to display a health overlay
  - Started to add the ability to attack, it's a lot more complicated than
    it seemed initially; attack anims must be synced between clients, but the
    attack can be cancelled before it activates, so each attack requires up
    to two different packets

###Day 6
  - Clicking on an enemy causes you to follow them
  - Client now processes its own events as if they came from someone else, making
    code neater and reducing complexity

With one day left it's pretty clear I'm not going to finish this, but I'll keep
going to make it as complete as possible, and development isn't going to stop
just because a week's up!
