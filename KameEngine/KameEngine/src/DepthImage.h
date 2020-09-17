#pragma once

#include "VulkanUtils.h"

class DepthImage {

public:
  DepthImage() {}

  ~DepthImage() {
    destroy();
  }

  DepthImage(const DepthImage&) = delete;
  DepthImage(DepthImage&&) = delete;
  DepthImage& operator=(const DepthImage&) = delete;
  DepthImage& operator=(DepthImage&&) = delete;

  void create(
    VkDevice device, VkPhysicalDevice physicalDevice,
    VkCommandPool commandPool, VkQueue queue,
    uint32_t width, uint32_t height
  ) {
    if (_Created) {
      throw std::logic_error("DepthImage was already created");
    }

    _Device = device;

    VkFormat depthFormat = findDepthFormat(physicalDevice);

    createImage(
      device, physicalDevice, width, height, depthFormat,
      VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      _Image, _ImageMemory
    );
    createImageView(device, _Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, _ImageView);
    changeImageLayout(
      device, commandPool, queue, _Image, depthFormat,
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );

    _Created = true;
  }

  void destroy() {
    if (_Created) {
      vkDestroyImageView(_Device, _ImageView, nullptr);
      vkDestroyImage(_Device, _Image, nullptr);
      vkFreeMemory(_Device, _ImageMemory, nullptr);
      _Created = false;

      _Image = VK_NULL_HANDLE;
      _ImageMemory = VK_NULL_HANDLE,
        _ImageView = VK_NULL_HANDLE;
      _Device = VK_NULL_HANDLE;
    }
  }

  VkImageView getImageView() {
    return _ImageView;
  }

  static VkFormat findDepthFormat(VkPhysicalDevice physicalDevice) {
    std::vector<VkFormat> possibleFormats = {
      VK_FORMAT_D32_SFLOAT_S8_UINT,
      VK_FORMAT_D24_UNORM_S8_UINT,
      VK_FORMAT_D32_SFLOAT
    };

    return findSupportedFormat(
      physicalDevice, possibleFormats,
      VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
  }

  static VkAttachmentDescription getDepthAttachment(VkPhysicalDevice physicalDevice) {
    VkAttachmentDescription depthAttachment;
    depthAttachment.flags = 0;
    depthAttachment.format = findDepthFormat(physicalDevice);
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    return depthAttachment;
  }

  static VkPipelineDepthStencilStateCreateInfo getDepthStencilStateCreateInfoOpaque() {
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.pNext = nullptr;
    depthStencilStateCreateInfo.flags = 0;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.front = {};
    depthStencilStateCreateInfo.back = {};
    depthStencilStateCreateInfo.minDepthBounds = 0.0f;
    depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

    return depthStencilStateCreateInfo;
  }

private:
  VkImage _Image = VK_NULL_HANDLE;
  VkDeviceMemory _ImageMemory = VK_NULL_HANDLE;
  VkImageView _ImageView = VK_NULL_HANDLE;
  VkDevice _Device = VK_NULL_HANDLE;

  bool _Created = false;
};