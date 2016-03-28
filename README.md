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
    - Convex polygon handling with interior points, intersection, and subtraction

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
