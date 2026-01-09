#pragma once

#include "core/types.h"

using FileTimestamp = u64;

std::string get_executable_directory();

FileTimestamp get_file_timestamp(const std::string& path);
