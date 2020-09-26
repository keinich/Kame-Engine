//#include "KameEngine.h"

//#include <Kame/Application/Application.h>
#include <Kame/Game/Game.h>
#include <Kame/Application/Entrypoint.h>

class Sandbox : public Kame::Game {
public:
  Sandbox() {}
  ~Sandbox() {}
};

Kame::Game* CreateGame() {
  //return new Kame::Tutorial4(L"Learning DirectX 12 - Lesson 4", 1280, 720, true);
  return new Sandbox();
}


//int main() {
//
//  startGlfw();
//  startVulkan();
//  gameLoop();
//  shutdownVulkan();
//  shutdownGlfw();
//
//}