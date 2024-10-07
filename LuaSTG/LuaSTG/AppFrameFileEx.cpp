#include "stb_image.h"

#include "AppFrame.h"
#include "Core/FileManager.hpp"

using namespace LuaSTGPlus;

int AppFrame::LoadCompressedTextFile(lua_State* L_, const char* path, const char *packname)noexcept
{
    if (ResourceMgr::GetResourceLoadingLog()) {
        if (packname)
            spdlog::info("[luastg] Reading compressed text file '{}' in package '{}'", packname, path);
        else
            spdlog::info("[luastg] Reading compressed text file '{}'", path);
    }
    bool loaded = false;
    std::vector<uint8_t> src, decompressed;
    if (packname)
    {
        auto& arc = GFileManager().getFileArchive(packname);
        if (!arc.empty())
        {
            loaded = arc.load(path, src);
        }
    }
    else
    {
        loaded = GFileManager().loadEx(path, src);
    }
    if (!loaded) {
        spdlog::error("[luastg] Unable to load file '{}'", path);
        return 0;
    }
    int decompressed_len;
    char* decompressed_buf = stbi_zlib_decode_malloc((char*)src.data(), src.size(), &decompressed_len);
    if (!decompressed_buf) {
        spdlog::error("[luastg] Unable to allocate buffer for compressed text file '{}'", path);
        return 0;
    }
    if (stbi_zlib_decode_buffer(decompressed_buf, decompressed_len, (char*)src.data(), src.size()) == -1) {
        spdlog::error("[luastg] Unable to decompress text file '{}'", path);
        return 0;
    }
    lua_pushlstring(L_, decompressed_buf, decompressed_len);
    return 1;
}