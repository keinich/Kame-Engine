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

    createImage(
      device, physicalDevice, 
      getWidth(), getHeight(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, 
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
      _Image, _ImageMemory
    );

    changeLayout(device, commandPool, queue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    writeBufferToImage(device, commandPool, queue, stagingBuffer);
    changeLayout(device, commandPool, queue, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    createImageView(device, _Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, _ImageView);

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

    VkResult result = vkCreateSampler(device, &samplerCreateInfo, nullptr, &_Sampler);

    _Uploaded = true;
  }

  void writeBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkBuffer buffer) {
    VkCommandBuffer commandBuffer = startSingleTimeCommandBuffer(device, commandPool);

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

    endSingleTimeCommandBuffer(device, queue, commandPool, commandBuffer);
  }

  void changeLayout(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkImageLayout layout) {
    changeImageLayout(device, commandPool, queue, _Image, VK_FORMAT_R8G8B8A8_UNORM, _ImageLayout, layout);

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