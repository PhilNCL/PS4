#pragma once

#include <gnm.h>
#include <gnmx\context.h>
#include <.\graphics\api_gnm\toolkit\allocators.h>
#include <.\graphics\api_gnm\toolkit\stack_allocator.h>

enum MemoryLocation {
	GARLIC,
	ONION,
	MEMORYMAX
};


class PS4MemoryAware	{
protected:
	PS4MemoryAware() {}
	~PS4MemoryAware() {}

	static sce::Gnmx::Toolkit::IAllocator	onionAllocator;
	static sce::Gnmx::Toolkit::IAllocator	garlicAllocator;
	static sce::Gnm::OwnerHandle			ownerHandle;
};

