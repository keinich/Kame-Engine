#pragma once

namespace Kame {

  class Game {
    friend class Application;
  public:
    Game(const char* name);
    ~Game() {}

    inline const char* GetName() { return _Name; }

  private: // Fields
    const char* _Name;

  private: // Methods
    void Initialize();
  };

}