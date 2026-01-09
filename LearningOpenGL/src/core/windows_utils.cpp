#include "windows_utils.h"

#include <windows.h>

std::string get_executable_directory() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	// Find the last backslash to get directory path
	std::string path_to_exe(buffer);
	size_t last_slash = path_to_exe.find_last_of("\\/");

	if (last_slash != std::string::npos) {
		return path_to_exe.substr(0, last_slash);
	}

	return ""; // Fallback if something went wrong
}

FileTimestamp get_file_timestamp(const std::string& path) {
	WIN32_FIND_DATAA find_data;
	HANDLE find_handle = FindFirstFileA(path.c_str(), &find_data);

	if (find_handle == INVALID_HANDLE_VALUE) {
		return 0;
	}

	FindClose(find_handle);

	ULARGE_INTEGER result;
	result.LowPart  = find_data.ftLastWriteTime.dwLowDateTime;
	result.HighPart = find_data.ftLastWriteTime.dwHighDateTime;
	
	return result.QuadPart;
}
