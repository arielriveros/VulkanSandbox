#pragma once
#include <vulkan/vulkan.hpp>
#include "Device.h"

class Texture
{
public:
	Texture(Device &device);
	~Texture();

	Texture(const Texture &) = delete;
	void operator=(const Texture &) = delete;
	Texture(Texture &&) = delete;
	Texture &operator=(Texture &&) = delete;

	void LoadFromFile(const std::string &filename);
	void LoadFromBuffer(const std::vector<void*> &buffer, uint32_t width, uint32_t height);
	void Destroy();
	vk::DescriptorImageInfo DescriptorInfo();

	vk::ImageView GetImageView() const { return m_ImageView; }

private:
	void TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void CreateSampler();

	Device& m_Device;
	vk::Image m_Image;
	vk::DeviceMemory m_ImageMemory;
	vk::ImageView m_ImageView;
	vk::Sampler m_Sampler;
};