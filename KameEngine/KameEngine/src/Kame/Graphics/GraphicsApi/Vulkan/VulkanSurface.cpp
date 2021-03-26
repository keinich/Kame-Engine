#include "kmpch.h"
#include "VulkanSurface.h"

#include "VulkanAPi.h"

Kame::VulkanSurface::VulkanSurface(){
  _Handle = VK_NULL_HANDLE;
}

Kame::VulkanSurface::~VulkanSurface(){
}

void Kame::VulkanSurface::Destroy(){
  vkDestroySurfaceKHR(VulkanApi::Get()->GetInstance().GetVkInstance(), _Handle, nullptr);
}

void Kame::VulkanSurface::CreateFromGlfwWindow(GLFWwindow* glfwWindow){
  VkResult result = glfwCreateWindowSurface(
    VulkanApi::Get()->GetInstance().GetVkInstance(), glfwWindow, nullptr, &_Handle
  );
  ASSERT_VULKAN(result);
}
