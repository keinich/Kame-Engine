#include "KameEngine.h"

int main() {

  startGlfw();
  startVulkan();
  gameLoop();
  shutdownVulkan();
  shutdownGlfw();

}