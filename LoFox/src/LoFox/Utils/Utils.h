#pragma once

#include "LoFox/Core/Core.h"

#include <fstream>

namespace LoFox {

	namespace Utils {

        static std::vector<char> ReadFileAsBytes(const std::string& path) {

            std::ifstream file(path, std::ios::ate | std::ios::binary);

            LF_CORE_ASSERT(file.is_open(), "Failed to open file!");

            size_t fileSize = (size_t)file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            file.close();

            return buffer;
        }

        static std::string ReadFileAsString(const std::string& path) {

			std::string result;
			std::ifstream in(path, std::ios::in | std::ios::binary);
			if (in) {

				in.seekg(0, std::ios::end);
				size_t size = in.tellg();
				if (size != -1) {

					result.resize(size);
					in.seekg(0, std::ios::beg);
					in.read(&result[0], size);
				}
				else {

					LF_CORE_ERROR("Could not read from file \"{0}\"", path);
				}
			}
			else {
				LF_CORE_ERROR("Couldn't open file \"{0}\"", path);
			}

			return result;
        }
	}
}