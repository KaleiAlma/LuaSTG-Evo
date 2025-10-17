#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"
#include "stb_vorbis.c"
#undef L // fuck stb_vorbis
#define MA_IMPLEMENTATION
#define MA_USE_STDINT
#include "miniaudio.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
