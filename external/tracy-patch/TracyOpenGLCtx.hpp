#pragma once
// #ifndef D3D11_SDK_VERSION 
// #define NOMINMAX
// #include <d3d11.h>
// #undef NOMINMAX
// #endif // !D3D11_SDK_VERSION
#include <gl.h>
#include "TracyOpenGL.hpp"

namespace tracy
{
	GpuCtx xTracyGpuCtx();
	void xTracyGpuContext(IGpuDevice* device, IGpuDeviceContext* devicectx);
	void xTracyGpuDestroy();
}
