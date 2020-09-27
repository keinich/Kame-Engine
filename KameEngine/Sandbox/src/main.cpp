// Kame 
#include <Kame/Application/Entrypoint.h>

// Game
#include "Sandbox.h"

Kame::Game* CreateGame() {
  return new Sandbox();
}