#pragma once

#include "LoFox/Core/Core.h"

#include <fstream>

namespace LoFox {

	namespace Utils {

        static std::vector<char> ReadFile(const std::string& filename) {

            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            LF_CORE_ASSERT(file.is_open(), "Failed to open file!");

            size_t fileSize = (size_t)file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            file.close();

            return buffer;
        }
	}
}