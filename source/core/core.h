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
// core.h - 引擎核心主头文件
// 功能：聚合所有底层模块的头文件，包括纯C接口、SDL、基础类型定义、
//       数据结构（数组/集合/映射/环缓冲）、数学库（向量/矩阵）、
//       管理器框架、计时器、输入、内存管理和格式化工具。
//       这是整个引擎的基础包含层，所有上层模块都依赖于此。
//==============================================================================
#ifndef __CORE__
#define __CORE__

//==============================================================================
// 纯C接口 - USB通信、套接字、基础C工具函数
//==============================================================================
extern "C" {
#include <core/purec/purec.h>           // 纯C基础工具（错误处理、日志等）
#include <core/purec/pureusb.h>         // USB通信层（与硬件设备交互）
#include <core/purec/puresocket.h>      // 网络套接字通信
};

//==============================================================================
// SDL - 跨平台多媒体库（窗口管理、线程、原子操作等）
//==============================================================================
#include <SDL.h>

//==============================================================================
// 宽字符支持
//==============================================================================
#include <wchar.h>

//==============================================================================
// 核心接口函数声明
//   coreMessage     - 显示错误消息框
//   coreDebugBreak  - 触发调试断点
//   coreDebugOutput - 输出调试日志
//   coreTerminate   - 终止引擎（清理SDL资源）
//==============================================================================
void coreMessage(const char* msg);
void coreDebugBreak();
void coreDebugOutput(const char* msg);
void coreTerminate();

//==============================================================================
// 全局定义 - 平台宏、通用宏、基础类型、字节序
//==============================================================================
#include <core/global/platform.h>       // 平台检测宏（WIN/LINUX/MINGW）
#include <core/global/macro.h>          // 通用宏定义（CORE_INLINE、BIT等）
#include <core/global/type.h>           // 基础类型定义（int/float/string等）
#include <core/global/endian.h>         // 字节序转换工具

//==============================================================================
// C++标准库
//==============================================================================
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <ctime>
#include <locale.h>
#include <iostream>

//==============================================================================
// 核心数据结构与数学库
//==============================================================================
#include <core/flag/flag.h>             // 标志位管理
#include <core/data/array.h>            // 动态数组
#include <core/data/set.h>              // 集合
#include <core/data/map.h>              // 映射表
#include <core/math/interval.h>         // 区间数学
#include <core/data/ring.h>             // 环形缓冲区
#include <core/math/const.h>            // 数学常量（PI、KILO、MEGA等）
#include <core/math/vector4.h>          // 四维向量
#include <core/math/matrix4x4.h>        // 4x4矩阵
#include <core/string/corestring.h>     // 字符串工具类

//==============================================================================
// 管理器框架、计时器、输入
//==============================================================================
#include <core/manager/manager.h>       // 管理器框架（生命周期管理）
#include <core/timer/timer.h>           // 高性能计时器
#include <core/input/input.h>           // 输入设备（键盘/鼠标）

//==============================================================================
// 内存管理与格式化
//==============================================================================
#include <core/memory/memory.h>         // 内存池分配器
#include <core/format/format.h>         // 字符串格式化与路径工具

////////////////////////////////////////////////////////////////////////////////
// opengl
////////////////////////////////////////////////////////////////////////////////
#include <core/opengl/opengl.h>

////////////////////////////////////////////////////////////////////////////////
// render
////////////////////////////////////////////////////////////////////////////////
#include <core/render/camera/camera.h>
#include <core/render/canvas/canvas2d.h>
#include <core/render/canvas/canvas3d.h>
#include <core/render/font/font.h>
#include <core/render/render/render.h>

////////////////////////////////////////////////////////////////////////////////
// file
////////////////////////////////////////////////////////////////////////////////
#include <core/file/file.h>

////////////////////////////////////////////////////////////////////////////////
// cjson
////////////////////////////////////////////////////////////////////////////////
#include <cJSON.h>

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
