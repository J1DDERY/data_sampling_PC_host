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
//    along with this ScopeFun Oscilloscope.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
// managers.cpp - 引擎管理器创建与初始化配置
// 功能：定义 create() 和 setup() 函数，负责：
//   - 创建所有核心管理器单例（MANAGER_CREATE）
//   - 配置各管理器的启动(start)、更新(update)、停止(stop)顺序
//==============================================================================
#include<scopefun/ScopeFun.h>

//==============================================================================
// create - 创建所有核心管理器实例
// 各管理器通过 MANAGER_CREATE 宏注册为全局单例，按依赖关系排列：
//   Manager    - 管理器框架本身
//   Format     - 字符串格式化与路径管理
//   Memory     - 内存池分配器
//   Timer      - 高性能计时器
//   Render     - OpenGL渲染器
//   Canvas2d   - 2D画布
//   Canvas3d   - 3D画布
//   Font       - 字体管理
//   Camera     - 3D摄像机
//   Osciloscope- 示波器核心（依赖以上所有模块）
//   Input      - 输入设备管理
//==============================================================================
void create()
{
    // managers
    MANAGER_CREATE(Manager);          // 管理器框架
    MANAGER_CREATE(Format);           // 字符串格式化
    MANAGER_CREATE(Memory);           // 内存管理
    MANAGER_CREATE(Timer);            // 计时器
    MANAGER_CREATE(Render);           // 渲染器
    MANAGER_CREATE(Canvas2d);         // 2D画布
    MANAGER_CREATE(Canvas3d);         // 3D画布
    MANAGER_CREATE(Font);             // 字体
    MANAGER_CREATE(Camera);           // 摄像机
    MANAGER_CREATE(Osciloscope);      // 示波器核心
    MANAGER_CREATE(Input);            // 输入
}

//==============================================================================
// setup - 配置各管理器的生命周期执行顺序
// start : 按顺序调用的初始化函数
// update: 每帧按顺序调用的更新函数
// stop  : 退出时按顺序调用的清理函数
//==============================================================================
void setup()
{
    // start - 初始化阶段（按依赖顺序）
    pManager->addStart("Osciloscope");   // 先初始化示波器核心
    pManager->addStart("Timer");         // 再启动计时器
    pManager->addStart("Input");         // 最后初始化输入
    // update - 每帧更新阶段
    pManager->addUpdate("Input");        // 先处理输入
    pManager->addUpdate("Render");       // 更新渲染器
    pManager->addUpdate("Canvas2d");     // 更新2D画布
    pManager->addUpdate("Canvas3d");     // 更新3D画布
    pManager->addUpdate("Osciloscope");  // 更新示波器状态
    pManager->addUpdate("Camera");       // 最后更新摄像机
    // stop - 清理阶段
    pManager->addStop("Osciloscope");    // 停止示波器
}

////////////////////////////////////////////////////////////////////////////////
// 文件结束 - managers.cpp
////////////////////////////////////////////////////////////////////////////////
