#include "TracyOpenGLCtx.hpp"
#include <cassert>

namespace tracy
{
	static GpuCtx g_GpuCtx{ nullptr };

	GpuCtx xTracyD3D11Ctx()
	{
		return g_GpuCtx;
	}
	void xTracyD3D11Context(ID3D11Device* device, ID3D11DeviceContext* devicectx)
	{
		assert(g_D3D11Ctx == nullptr);
		g_D3D11Ctx = TracyD3D11Context(device, devicectx);
	}
	void xTracyD3D11Destroy()
	{
		TracyD3D11Destroy(g_D3D11Ctx);
		g_D3D11Ctx = nullptr;
	}
}
