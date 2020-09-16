#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

class EasyImage {
public:
  EasyImage() {
    _Width = 0;
    _Height = 0;
    _Channels = 0;
    _Pixels = nullptr;
    _Loaded = false;
  }

  ~EasyImage() {
    destroy();
  }

  EasyImage(const EasyImage&) = delete;
  EasyImage(EasyImage&&) = delete;
  EasyImage& operator=(const EasyImage&) = delete;
  EasyImage& operator=(EasyImage&&) = delete;

  EasyImage(const char* path) {
    load(path);
  }

  void load(const char* path) {
    if (_Loaded) {
      throw std::logic_error("EasyImage was already loaded");
    }

    _Pixels = stbi_load(path, &_Width, &_Height, &_Channels, STBI_rgb_alpha);

    if (_Pixels == nullptr) {
      throw std::invalid_argument("Could not load image or image is corrupt");
    }

    _Loaded = true;
  }

  void upload(const VkDevice& device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue) {
    if (!_Loaded) {
      throw std::logic_error("EasyImage was not loaded!");
    }

    if (_Uploaded) {
      throw std::logic_error("EasyImage was already uploaded!");
    }

    _Device = device;

    VkDeviceSize imageSize = getSizeInBytes();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(
      device, physicalDevice, imageSize,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      stagingBufferMemory
    );

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, getRaw(), imageSize);
    vkUnmapMemory(device, stagingBufferMemory);

    VkImageCreateInfo imageCreateInfo;
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.flags = 0;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageCreateInfo.extent.width = getWidth();
    imageCreateInfo.extent.height = getHeight();
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    imageCreateInfo.initialLayout = _ImageLayout;

    VkResult result = vkCreateImage(device, &imageCreateInfo, nullptr, &_Image);
    ASSERT_VULKAN(result);

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device, _Image, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo;
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = nullptr;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(
      physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &_ImageMemory);
    ASSERT_VULKAN(result);

    vkBindImageMemory(device, _Image, _ImageMemory, 0);

    changeLayout(device, commandPool, queue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    writeBufferToImage(device, commandPool, queue, stagingBuffer);
    changeLayout(device, commandPool, queue, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    VkImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.pNext = nullptr;
    imageViewCreateInfo.flags = 0;
    imageViewCreateInfo.image = _Image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &_ImageView);
    ASSERT_VULKAN(result);

    VkSamplerCreateInfo samplerCreateInfo;
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.pNext = nullptr;
    samplerCreateInfo.flags = 0;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.anisotropyEnable = VK_TRUE;
    samplerCreateInfo.maxAnisotropy = 16;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = 0.0f;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

    result = vkCreateSampler(device, &samplerCreateInfo, nullptr, &_Sampler);

    _Uploaded = true;
  }

  void writeBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkBuffer buffer) {
    VkCommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
    ASSERT_VULKAN(result);

    VkCommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    ASSERT_VULKAN(result);

    VkBufferImageCopy bufferImageCopy;
    bufferImageCopy.bufferOffset = 0;
    bufferImageCopy.bufferRowLength = 0;
    bufferImageCopy.bufferImageHeight = 0;
    bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferImageCopy.imageSubresource.mipLevel = 0;
    bufferImageCopy.imageSubresource.baseArrayLayer = 0;
    bufferImageCopy.imageSubresource.layerCount = 1;
    bufferImageCopy.imageOffset = { 0,0,0 };
    bufferImageCopy.imageExtent = { (uint32_t)getWidth(), (uint32_t)getHeight(), (uint32_t)1 };

    vkCmdCopyBufferToImage(commandBuffer, buffer, _Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

    vkEndCommandBuffer(commandBuffer);
    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
  }

  void changeLayout(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkImageLayout layout) {
    VkCommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
    ASSERT_VULKAN(result);

    VkCommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    ASSERT_VULKAN(result);

    VkImageMemoryBarrier imageMemoryBarrier;
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext = nullptr;
    if (_ImageLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    else if (_ImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }
    else {
      throw std::logic_error("Layout transition not yet supported!");
    }

    imageMemoryBarrier.oldLayout = _ImageLayout;
    imageMemoryBarrier.newLayout = layout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = _Image;
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(
      commandBuffer,
      VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
      0,
      0, nullptr,
      0, nullptr,
      1, &imageMemoryBarrier
    );

    vkEndCommandBuffer(commandBuffer);
    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

    _ImageLayout = layout;

  }

  void destroy() {
    if (_Loaded) {
      stbi_image_free(_Pixels);
      _Loaded = false;
    }

    if (_Uploaded) {
      vkDestroySampler(_Device, _Sampler, nullptr);
      vkDestroyImageView(_Device, _ImageView, nullptr);

      vkDestroyImage(_Device, _Image, nullptr);
      vkFreeMemory(_Device, _ImageMemory, nullptr);

      _Uploaded = false;
    }
  }

  int getHeight() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }

    return _Height;
  }

  int getWidth() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }

    return _Width;
  }

  int getChannels() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }
    return 4;
  }

  int getSizeInBytes() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }
    return getWidth() * getHeight() * getChannels();
  }

  stbi_uc* getRaw() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }
    return _Pixels;
  }

  VkSampler getSampler() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }

    return _Sampler;
  }

  VkImageView getImageView() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }

    return _ImageView;
  }

private:
  int _Width;
  int _Height;
  int _Channels;
  stbi_uc* _Pixels;
  bool _Loaded;
  bool _Uploaded = false;
  VkImage _Image;
  VkDeviceMemory _ImageMemory;
  VkImageView _ImageView;
  VkImageLayout _ImageLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
  VkDevice _Device;
  VkSampler _Sampler;

};