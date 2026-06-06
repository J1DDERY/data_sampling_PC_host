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
// osccontrol.cpp - 硬件控制命令
// 功能：实现与示波器硬件的控制命令交互：
//   - FPGA/CPLD 控制寄存器读写
//   - 触发控制（模式、源、边沿、电平）
//   - 通道设置（增益、耦合、接地）
//   - 时基和采样率控制
//   - 硬件初始化与复位
//==============================================================================
#include<scopefun/ScopeFun.h>


////////////////////////////////////////////////////////////////////////////////
// 文件结束 - osccontrol.cpp
////////////////////////////////////////////////////////////////////////////////
