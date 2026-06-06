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
// wnddisplay.cpp - 显示设置窗口模型实现
// 功能：管理示波器显示相关的配置参数默认值，包括：
//   - 时域波形显示（网格、坐标轴、单位）
//   - FFT频谱显示（网格、坐标轴、单位、分贝参考、对数频率）
//   - 信号渲染风格（线宽、类型）
//   - 3D渲染参数（实体、光照、深度测试、细分、透明度）
//==============================================================================
#include<scopefun/ScopeFun.h>

////////////////////////////////////////////////////////////////////////////////
//
// WndDisplay - 显示配置构造函数
// 初始化所有显示相关参数的默认值
//
////////////////////////////////////////////////////////////////////////////////
WndDisplay::WndDisplay()
{
    Default();
}

void WndDisplay::Default()
{
    // ---- 时域(OSC)显示 ----
    oscGrid  = 1;                        // 显示网格
    oscAxis  = 1;                        // 显示坐标轴
    oscUnits = 1;                        // 显示单位标签
    // ---- 频域(FFT)显示 ----
    fftGrid  = 1;                        // 显示FFT网格
    fftAxis  = 0;                        // 不显示FFT坐标轴
    fftUnits = 1;                        // 显示FFT单位
    // ---- 信号渲染 ----
    signalWidth    = MAX_TRIANGLE_SIZE / 2.f;  // 时域信号线宽
    signalType     = SIGNAL_TYPE_LINE;          // 时域信号类型（线条）
    fftWidth       = MAX_TRIANGLE_SIZE / 2.f;  // FFT信号线宽
    fftType        = SIGNAL_TYPE_LINE;          // FFT信号类型（线条）
    fftDecibel     = 10;                        // FFT分贝参考值 (dB)
    fftLogFreq     = 1;                         // FFT频率轴对数显示
    // ---- 3D渲染 ----
    solid3d        = 1;                         // 实体填充
    light3d        = 1;                         // 光照效果
    depthTest3d    = 1;                         // 深度测试
    tessalation3d  = 256;                       // 3D曲面细分等级
    tessalation2d  = 1;                         // 2D细分等级
    // ---- 透明度 ----
    alpha3dCh0     = 255;                       // 通道A 3D透明度 (255=不透明)
    alpha3dCh1     = 255;                       // 通道B 3D透明度
    alpha3dFun     = 255;                       // 函数通道 3D透明度
}

////////////////////////////////////////////////////////////////////////////////
// 文件结束 - wnddisplay.cpp
////////////////////////////////////////////////////////////////////////////////
