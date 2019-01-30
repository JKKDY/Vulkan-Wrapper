#pragma once

#include "vkw_Config.h"

#include "vkw_Core.h"
#include "vkw_Assets.h"
#include "vkw_Resources.h"
#include "vkw_Operations.h"



/* classes that can be instantiated without any arguments (theoretically):
	Semaphore
	Fence
	ComputeCommandPool
	GraphicsCommandPool
	TransferCommandPool

	TODO: review if they should have that ability. If so then the should only create a Vk Object if a device has been created
*/

// TODO: implement a proper Debugger
// TODO: methods of image class should be redone
// TODO: vkw::updateDescriptorSets should support copying
// TODO: support of multiple gpus
// TODO: add pnext to every ::CreateInfo
// TODO: standardize which and how much data gets stored in the vkw::Objects
// TODO: implement a to-be-deleted stack so objects can be declared in a time independent manor

// NOTE: rather use at() for stl containers


