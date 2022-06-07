﻿////////////////////////////////////////////////////////////////////////////////
/// @file  f2dEngineImpl.h
/// @brief fancy2D引擎接口实现
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fcyRefObj.h>
#include "f2dEngine.h"
#include "Sound/f2dSoundSysImpl.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief fancy2D引擎接口实现
////////////////////////////////////////////////////////////////////////////////
class f2dEngineImpl :
	public fcyRefObjImpl<f2dEngine>
{
private:
	f2dSoundSysImpl* m_pSoundSys;
public: // 接口实现
	f2dSoundSys* GetSoundSys() { return m_pSoundSys; }
public:
	f2dEngineImpl();
	~f2dEngineImpl();
};
