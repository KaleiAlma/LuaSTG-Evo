#include "AdapterPolicy.hpp"

#ifdef WIN32
using uint32 = unsigned int;

static_assert(sizeof(uint32) == 4);

extern "C" __declspec(dllexport) uint32 NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) uint32 AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif

// namespace Platform
// {
// 	void AdapterPolicy::setAll(bool active)
// 	{
// 		setNVIDIA(active);
// 		setAMD(active);
// 	}
// 	void AdapterPolicy::setNVIDIA(bool active)
// 	{
// 		NvOptimusEnablement = active ? 0x00000001 : 0x00000000;
// 	}
// 	void AdapterPolicy::setAMD(bool active)
// 	{
// 		AmdPowerXpressRequestHighPerformance = active ? 0x00000001 : 0x00000000;
// 	}
// }
