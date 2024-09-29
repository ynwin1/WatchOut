#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

enum class TEXTURE_ASSET_ID {
	JEFF = 0,
    TEXTURE_COUNT = JEFF + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;