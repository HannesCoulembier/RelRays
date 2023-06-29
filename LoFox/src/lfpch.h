#pragma once

#include "LoFox/Core/Core.h"

#ifdef LF_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>


#include "LoFox/Core/Log.h"

#ifdef LF_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
