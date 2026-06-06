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
// wnddisplay.h - 显示设置头文件
// 功能：定义示波器显示参数的配置结构：
//   SignalType : 信号渲染类型（线条/三角形）
//   WndDisplay : 显示配置类，包含：
//     - 时域(OSC)波形显示设置（网格/坐标轴/单位）
//     - FFT频谱显示设置（网格/坐标轴/单位/分贝/对数频率）
//     - 信号渲染风格（线宽/类型）
//     - 3D渲染参数（实体/光照/深度/细分/透明度）
//==============================================================================
#ifndef __OSCILOSCOPE__DISPLAY__WINDOW__
#define __OSCILOSCOPE__DISPLAY__WINDOW__

#define MAX_TRIANGLE_SIZE 0.01f            // 三角形渲染最大尺寸

//==============================================================================
// SignalType - 信号渲染类型
//   LINE     : 线条连接（标准矢量波形）
//   TRIANGLE : 三角形填充（3D渲染模式）
//==============================================================================
enum SignalType
{
    SIGNAL_TYPE_LINE,                      // 线条模式
    SIGNAL_TYPE_TRIANGLE,                  // 三角形模式（3D）
};

//==============================================================================
// WndDisplay - 显示配置类
// 管理所有与波形显示相关的视觉参数
//==============================================================================
class WndDisplay
{
public:
    // ---- 时域(OSC)显示 ----
    int           oscGrid;                 // 显示网格线
    int           oscAxis;                 // 显示坐标轴
    int           oscUnits;                // 显示单位标签
public:
    // ---- FFT显示 ----
    int           fftGrid;                 // 显示FFT网格
    int           fftAxis;                 // 显示FFT坐标轴
    int           fftUnits;                // 显示FFT单位
public:
    // ---- 信号样式 ----
    SignalType    signalType;              // 时域信号渲染类型
    float         signalWidth;             // 时域信号线宽
public:
    SignalType    fftType;                 // FFT信号渲染类型
    uint          fftDecibel;              // FFT分贝参考值 (dB)
    uint          fftLogFreq;              // FFT频率轴对数/线性
    float         fftWidth;                // FFT信号线宽
public:
    // ---- 3D渲染 ----
    int           solid3d;                 // 实体填充
    int           light3d;                 // 光照效果
    int           depthTest3d;             // 深度测试
    uint          tessalation3d;           // 3D曲面细分等级
    uint          tessalation2d;           // 2D细分等级
    uint          alpha3dCh0;              // 通道A 3D透明度
    uint          alpha3dCh1;              // 通道B 3D透明度
    uint          alpha3dFun;              // 函数通道 3D透明度
public:
    WndDisplay();
public:
    void Default();                        // 恢复默认值
};

#endif
////////////////////////////////////////////////////////////////////////////////
// 文件结束 - wnddisplay.h
////////////////////////////////////////////////////////////////////////////////
