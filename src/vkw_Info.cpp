#include "vkw_Info.h"

namespace vkw {
	namespace Info {
		utils::ReadOnlyReference<AvailableQueueFamilies, vkw::PhysicalDevice> availableQueueFamilies;

		utils::ReadOnlyReference<QueueFamiliesInUse, vkw::Device> queueFamiliesInUse;

		utils::ReadOnly<std::vector<Info::Surface>, vkw::Surface> surfaces;

		utils::ReadOnly<std::vector<GPU>, vkw::PhysicalDevice> physicalDevices;
	}
}