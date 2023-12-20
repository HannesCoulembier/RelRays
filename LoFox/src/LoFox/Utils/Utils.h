#pragma once

namespace LoFox {

	namespace Utils {

		std::vector<uint32_t> ReadFileAsBytes(const std::string& path);

		std::string ReadFileAsString(const std::string& path);
	
		std::string GetCacheDirectory();

		std::string GetFileNameFromPath(const std::string& path);

		bool PathExists(const std::string& path);

		void CreatePathIfNeeded(const std::string& path);

		void WriteFileInBytes(const std::string& path, const std::vector<uint32_t>& bytes);

		void WriteFileInString(const std::string& path, const std::string& data);

		std::vector<std::string> split(const std::string& str, const std::string& token);
	}
}