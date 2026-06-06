////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 - 2021 David Košenina
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
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this ScopeFun Oscilloscope.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
// core.cpp - 引擎核心函数实现
// 功能：提供底层基础功能的SDL封装实现，包括：
//   - 错误消息弹窗
//   - 调试断点触发
//   - 调试日志输出
//   - 引擎终止清理
//==============================================================================
#include<core/core.h>

//==============================================================================
// coreMessage - 显示SDL错误消息框
// 使用 SDL_ShowSimpleMessageBox 在桌面上弹出错误提示
//==============================================================================
void coreMessage(const char* msg)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "error", msg, 0);
}

//==============================================================================
// coreDebugBreak - 触发调试器断点
// 使用 SDL_TriggerBreakpoint 插入软件断点，便于调试
//==============================================================================
void coreDebugBreak()
{
    SDL_TriggerBreakpoint();
}

//==============================================================================
// coreDebugOutput - 输出调试日志
// 使用 SDL_LogDebug 将调试信息写入 SDL 日志系统
//==============================================================================
void coreDebugOutput(const char* msg)
{
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, msg);
}

//==============================================================================
// coreTerminate - 终止引擎
// 调用 SDL_Quit() 清理所有SDL子系统并退出
//==============================================================================
void coreTerminate()
{
    SDL_Quit();
}

////////////////////////////////////////////////////////////////////////////////
// 文件结束 - core.cpp
////////////////////////////////////////////////////////////////////////////////
