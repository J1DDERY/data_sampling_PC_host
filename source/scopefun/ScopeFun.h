////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 - 2021 David Košenina
//    Copyright (C) 2021 - 2022 Dejan Priveršek
//
//    This file is part of ScopeFun Oscilloscope.
//
//    ScopeFun Oscilloscope is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    ScopeFun Oscilloscope is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this ScopeFun Oscilloscope.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
// ScopeFun.h - 示波器应用程序主头文件
// 功能：聚合所有核心模块的头文件，作为整个应用程序的编译包含入口。
//       包含引擎核心(core)、硬件API(scopefunapi)以及示波器各功能模块。
//==============================================================================
#ifndef __OSCILOSCOPE__
#define __OSCILOSCOPE__

//==============================================================================
// 引擎核心 - 包含基础类型、数学库、内存管理、管理器框架等
//==============================================================================
#include <core/core.h>

//==============================================================================
// 硬件API - C接口，提供与ScopeFun硬件设备通信的低层函数
//==============================================================================
extern "C" {
#include <api/scopefunapi.h>
}

//==============================================================================
// 示波器各功能模块
//==============================================================================
#include <scopefun/window/tool.h>       // 工具函数（单位转换、时间格式等）
#include <scopefun/osc/oscsignal.h>     // 信号处理（数据捕获、缓冲、历史记录）
#include <scopefun/osc/oscfile.h>       // 文件操作（保存/加载波形数据）
#include <scopefun/osc/oscsettings.h>   // 设置管理（JSON格式读写配置）
#include <scopefun/osc/oscfft.h>        // FFT频谱分析
#include <scopefun/osc/oscrender.h>     // 波形渲染（2D/3D OpenGL渲染）
#include <scopefun/window/wnddisplay.h> // 显示设置窗口
#include <scopefun/window/wndshadow.h>  // 阴影窗口
#include <scopefun/window/wndmain.h>    // 主窗口（UI布局与交互逻辑）
#include <scopefun/osc/osccontrol.h>    // 控制逻辑（触发、时基、通道控制）
#include <scopefun/osc/oscmng.h>        // 示波器管理器（线程管理、主循环）

#endif
////////////////////////////////////////////////////////////////////////////////
// 文件结束 - ScopeFun.h
////////////////////////////////////////////////////////////////////////////////
