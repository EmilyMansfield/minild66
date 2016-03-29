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

With only 3 days left it may look like there's a lot left to do (and frankly,
there is), but at this point the rendering and network engine are mostly
complete. What remains is to add gameplay and ensure that client/server sync
correctly with correct packet-loss handling. Which doesn't quite sound like as
much! Maybe I'll finish on time after all.
