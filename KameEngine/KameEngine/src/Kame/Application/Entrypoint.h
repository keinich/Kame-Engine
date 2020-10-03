#pragma once

#include <Kame/Application/Application.h>

extern Kame::Game* CreateGame();

int main(int argc, char** argv) {

  int retCode = 0;

  //Kame::Log::Init();
  //KM_CORE_WARN("Initialized Log!");

  Kame::Reference<Kame::Game> game(CreateGame());
  Kame::Application::Create(game);

  //std::make_shared<Kame::Tutorial4>(L"Learning DirectX 12 - Lesson 4", 1280, 720, true);
  retCode = Kame::Application::Run();

  Kame::Application::Destroy();

  atexit(&Kame::Application::ReportLiveObjects);

  return retCode;
}
