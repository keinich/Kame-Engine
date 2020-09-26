#include "kmpch.h"
#include "Application.h"

#include "KameEngine.h"

namespace Kame {

  void Application::Create() {
    startGlfw();
    startVulkan();
  }

  void Application::Destroy() {
    shutdownVulkan();
    shutdownGlfw();
  }

  void Application::ReportLiveObjects() {
  }

  int Application::Run(Reference<Game> game) {
    gameLoop();

    return 0;
  }

}
