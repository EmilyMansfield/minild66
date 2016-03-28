#include "constants.hpp"

unsigned int ld::width = 640;
unsigned int ld::height = 480;
const unsigned int ld::heightTiles = 15;
const unsigned int ld::widthTiles = ld::width / ((float)ld::height / ld::heightTiles);
const char* ld::title = "minild66";
const float ld::cameraPanSpeed = 6.0f;
