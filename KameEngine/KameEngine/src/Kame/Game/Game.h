#pragma once

namespace Kame {

  class Game {
    friend class Application;
  public:
    Game() {}
    ~Game() {}

  private:
    void Initialize();
  };

}