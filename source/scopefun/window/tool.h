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
// tool.h - 工具函数与枚举定义头文件
// 功能：定义UI和数据处理中使用的通用枚举类型和工具函数声明：
//   ECalibrationType : 校准类型（正常/500MHz）
//   Time             : 时间单位枚举（秒/毫秒/微秒/纳秒）
//   Volt             : 电压单位枚举（伏/毫伏/微伏/纳伏）
//   Frequency        : 频率单位枚举（Hz/kHz/MHz/GHz）
//   CaptureTime      : 采集时间参数枚举
//   VoltageControl   : 电压量程控制枚举
//   以及各种单位转换和时间/电压格式化的工具函数
//==============================================================================
#ifndef __OSCILOSCOPE__TOOL__WINDOW__
#define __OSCILOSCOPE__TOOL__WINDOW__


//==============================================================================
// ECalibrationType - 校准类型枚举
//==============================================================================
enum ECalibrationType
{
    ctNormal = 0,                          // 正常校准
    ct500Mhz = 1,                          // 500MHz带宽校准模式
    ctLast = 2,                            // 保留
};

//==============================================================================
// Time - 时间单位枚举
//==============================================================================
enum Time
{
    tSecond,                               // 秒 (s)
    tMili,                                 // 毫秒 (ms)
    tMicro,                                // 微秒 (us)
    tNano,                                 // 纳秒 (ns)
};

//==============================================================================
// Volt - 电压单位枚举
//==============================================================================
enum Volt
{
    vVolt,                                 // 伏特 (V)
    vMili,                                 // 毫伏 (mV)
    vMicro,                                // 微伏 (uV)
    vNano,                                 // 纳伏 (nV)
};

//==============================================================================
// Frequency - 频率单位枚举
//==============================================================================
enum Frequency
{
    fHertz,                                // 赫兹 (Hz)
    fKilo,                                 // 千赫兹 (kHz)
    fMega,                                 // 兆赫兹 (MHz)
    fGiga,                                 // 吉赫兹 (GHz)
};

////////////////////////////////////////////////////////////////////////////////
//
// ToolCaptureTime
//
////////////////////////////////////////////////////////////////////////////////
enum TimeCapture
{
    tc10ns,
    tc20ns,
    tc50ns,
    tc100ns,
    tc200ns,
    tc500ns,
    tc1us,
    tc2us,
    tc5us,
    tc10us,
    tc20us,
    tc50us,
    tc100us,
    tc200us,
    tc500us,
    tc1ms,
    tc2ms,
    tc5ms,
    tc10ms,
    tc20ms,
    tc50ms,
    tc100ms,
    tc200ms,
    tc500ms,
    tc1s,
    tcLast,
};

enum TimeCapture2
{
    t2c4ns,
    t2c8ns,
    t2c20ns,
    t2c40ns,
    t2c80ns,
    t2c200ns,
    t2c400ns,
    t2c800ns,
    t2c2us,
    t2c4us,
    t2c8us,
    t2c20us,
    t2c40us,
    t2c80us,
    t2c200us,
    t2c400us,
    t2c800us,
    t2c2ms,
    t2c4ms,
    t2c8ms,
    t2c20ms,
    t2cLast,
};

////////////////////////////////////////////////////////////////////////////////
//
// ToolCaptureVolt
//
////////////////////////////////////////////////////////////////////////////////
enum VoltageCapture
{
    vc2Volt,
    vc1Volt,
    vc500Mili,
    vc200Mili,
    vc100Mili,
    vc50Mili,
    vc20Mili,
    vc10Mili,
    vcLast,
};

////////////////////////////////////////////////////////////////////////////////
//
// Functions
//
////////////////////////////////////////////////////////////////////////////////
class ToolText
{
public:
    static void Time(char* buffer, int size, double value, int precision);
    static void Unit(char* buffer, int size, double value, double unit);
    static void xUnit(char* buffer, int size, double unit);
    static void Hertz(char* buffer, int size, float value);
    static void Decibels(char* buffer, int size, float value);
    static void Volt(char* buffer, int size, float value);
};

////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////
double multiplyerDoubleFromValue(double value);
int    multiplyerIndexFromValue(double value);

float multiplyerFromEnum(int enumerated);
uint  multiplyerFromValue(float value);

const char* captureTimeToStr(int enumerated);
double captureTimeFromEnum(int enumerated);
double captureTimeFromEnumV1(int enumerated);
double captureTimeFromEnumV2(int enumerated);
double captureTimeFromEnumVersion(int enumerated, int version);
uint   captureTimeMaxReceive(int enumerated, int version);
uint  captureTimeFromValue(float value);
ECalibrationType getCalibrationType(int enumerated);
ECalibrationType getCalibrationType(float value);

float captureVoltFromEnum(int enumerated);
uint  captureVoltFromValue(float value);

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
