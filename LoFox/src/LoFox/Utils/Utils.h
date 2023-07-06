#pragma once

#include "LoFox/Core/Core.h"

#include <fstream>

namespace LoFox {

	namespace Utils {

        static std::vector<uint32_t> ReadFileAsBytes(const std::string& path) {

            std::ifstream file(path, std::ios::ate | std::ios::binary);

            LF_CORE_ASSERT(file.is_open(), "Failed to open file!");

            size_t fileSize = (size_t)file.tellg();
            std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

            file.seekg(0);
            file.read((char*)buffer.data(), fileSize);

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
	
		static std::string GetCacheDirectory() { return "Assets/cache"; }

		static std::string GetFileNameFromPath(const std::string& path) {

			int lastSlash = path.find_last_of("/");
			int lastDot = path.find_last_of(".");
			LF_CORE_ASSERT(lastSlash != std::string::npos, "Unable to extract filename from path!");
			if (lastDot == std::string::npos) // folder/file
				return path.substr(lastSlash + 1, path.size() - lastSlash - 1);
			return path.substr(lastSlash + 1, lastDot - lastSlash - 1); // folder/file.suffix
		}

		static bool PathExists(const std::string& path) {

			return std::filesystem::exists(path);
		}

		static void CreatePathIfNeeded(const std::string& path) {

			if (!std::filesystem::exists(path))
				std::filesystem::create_directories(path);
		}

		static void WriteFileInBytes(const std::string& path, const std::vector<uint32_t>& bytes) {

			std::ofstream out(path, std::ios::out | std::ios::binary);
			if (out.is_open()) {

				out.write((char*)bytes.data(), bytes.size() * sizeof(uint32_t));
				out.flush();
				out.close();
			}

		}

		static void WriteFileInString(const std::string& path, const std::string& data) {

			std::ofstream out(path, std::ios::out | std::ios::binary);
			if (out.is_open()) {

				out.write((char*)data.data(), data.size() * sizeof(char));
				out.flush();
				out.close();
			}
		}

	}
}