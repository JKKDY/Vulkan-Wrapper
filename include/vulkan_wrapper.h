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

	IDEA: review if they should have that ability. If so then the should only create a Vk Object if a device has been created
*/

// TODO: implement a proper Debugger
// IDEA: add pnext to every ::CreateInfo
// TODO: standardize which (and how much) data gets stored in the vkw::Objects
// IDEA: implement a to-be-deleted stack so objects can be declared in a time independent manner

// NOTE: rather use at() for stl containers


