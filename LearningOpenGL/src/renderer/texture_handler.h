#pragma once
#include <vector>
#include <string>

#include "core/types.h"

// Forward declarations
struct Assets;
enum TextureType;

// Function for adding the texture ID into the assets struct
bool add_2D_texture(Assets* assets, TextureType texture, const char* path);
bool add_cubemap_texture(Assets* assets, TextureType slot, const std::vector<std::string> &faces);

// Loading textures - private?
u32 load_2D_texture(const char* path);
u32 load_cubemap_texture(const std::vector<std::string>& faces);

void destroy_texture(u32* texture);
