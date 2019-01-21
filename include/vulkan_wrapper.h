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

// TODO: implement Debugger for every object
// TODO: methods of image class should be redone
// TODO: updateDescriptorSets supports copying
// TODO: support of multiple gpus
// TODO: add pnext to every ::CreateInfo
// TODO: standardize wich and how much data gets stored in the vkw::Objects 

// NOTE: rather use at() for stl containers


