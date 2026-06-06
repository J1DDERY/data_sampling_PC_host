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
// oscsettings.h - 设备配置管理头文件
// 功能：定义硬件设备配置和校准参数的数据结构：
//   OscHardware : 硬件描述类（USB标识、固件路径、校准参数）
//   - USB供应商ID/产品ID (Cypress: VID=1204, PID=243)
//   - FX3固件和FPGA固件路径
//   - 电压校准系数
//   - 配置文件JSON序列化接口
//==============================================================================
#ifndef __OSC__SETTINGS__
#define __OSC__SETTINGS__

#define CYPRESS_VID 1204                  // Cypress USB 供应商ID
#define CYPRESS_PID 243                   // Cypress USB 产品ID

//==============================================================================
// OscHardware - 硬件描述类
// 存储与当前连接的示波器硬件相关的所有标识和配置信息：
//   usbGuid      : USB设备GUID
//   usbVendor    : USB供应商ID
//   usbProduct   : USB产品ID
//   usbSerial    : USB序列号
//   usbFirmware  : FX3固件文件路径
//   fpgaFirmware : FPGA固件文件路径
//   fpgaEtsIndex/Count : ETS等效时间采样参数
//   digitalVoltageCoeficient : 数字通道电压换算系数
//==============================================================================
class OscHardware
{
private:
    int version;                          // 硬件版本号
public:
    OscHardware(int version);
public:
    UsbGuid usbGuid;                      // USB设备GUID
    uint    usbVendor;                    // USB供应商ID
    uint    usbProduct;                   // USB产品ID
    uint    usbSerial;                    // USB序列号
    String  usbFirmware;                  // FX3固件路径
public:
    String fpgaFirmware;                  // FPGA固件路径
    uint   fpgaEtsIndex;                  // ETS延迟表索引
    uint   fpgaEtsCount;                  // ETS延迟表条目数
public:
    double digitalVoltageCoeficient;      // 数字通道电压系数
public:
    double generatorFs;
public:
    int    referenceInvert[2];
    int    referenceFramesPerCapture;
    int    referenceMaxIterations;
    double referenceExitPercentage;
    double referenceOffsetMaxValue;
    double referenceOffsetMinValue;
    int    referenceGenerator;
    double referenceGeneratorMaxValue;
    double referenceGeneratorMinValue;
    double referenceGeneratorVoltagePerStep;
    ushort referenceGainValue[vcLast];
    double referenceGainVoltage[vcLast];
    ushort referenceGainMin[vcLast];
    ushort referenceGainMax[vcLast];
    ushort referenceGainAttr[vcLast];
    double referenceStepMax;
    double referenceStepMin;
public:
    int    calibratedOffsets[ctLast][2][vcLast];
    double calibratedVoltageStep[ctLast][2][vcLast];
    ushort calibratedGainValue[ctLast][2][vcLast];
    int    calibratedOffsetsGenerator[ctLast][2];
public:
    SUsb   getUSB(bool factory);
public:
    int    getAnalogOffset(float time, int ch, float volt);
    double getAnalogOffsetDouble(float time, int ch, float volt);
    double getAnalogStep(float time, int ch, float volt);
    ushort getAnalogGain(float time, int ch, float volt);
    int    getGeneratorOffset(float time, int ch);
public:
    cJSON* json;
public:
    void loadCalibrated(cJSON* parent, ECalibrationType type);
    void saveCalibrated(cJSON* parent, ECalibrationType type);
public:
    void loadCalibrationFromFile();
    void loadHardwareFromFile();
    void saveCalibrationToFile();
};

////////////////////////////////////////////////////////////////////////////////
//
// OscSettings
//
////////////////////////////////////////////////////////////////////////////////
class OscSettings
{
public:
    OscSettings();
public:
    int   renderDepthBuffer;
    int   renderShaders21;
    int   renderVertexBufferSizeMegaByte;
    float renderFps;
    float renderEventTimer;
    uint  renderThreadCount;
    uint  priorityCapture;
    uint  priorityUpdate;
    uint  priorityRender;
    uint  priorityMain;
    uint  delayCapture;
    uint  delayUpdate;
    uint  delayRender;
    uint  delayMain;
    uint  speedLow;
    uint  speedMedium;
    uint  speedHigh;
    uint  memoryRld;
    uint  memoryHistory;
    uint  memoryFrame;
    uint  historyFrameCount;
    uint  historyFrameDisplay;
    uint  historyFrameLoadSave;
    uint  historyFrameClipboard;
    int   windowDebug;
    int   windowDisplayWidth;
    int   windowDisplayHeight;
    int   windowControlWidth;
    int   windowControlHeight;
public:
    cJSON* json;
public:
    void load();
    void save();
};

////////////////////////////////////////////////////////////////////////////////
//
// OscColors
//
////////////////////////////////////////////////////////////////////////////////
class OscColors
{
public:
    OscColors();
public:
    uint windowDefault;
    uint windowFront;
    uint windowBack;
    uint renderBackground;
    uint renderTime;
    uint renderChannel0;
    uint renderChannel1;
    uint renderFunction;
    uint renderXyGraph;
    uint renderGrid;
    uint renderBorder;
    uint renderTrigger;
    uint renderDigital;
public:
    cJSON* json;
public:
    uint   JsonToColorABGR(cJSON* jArray);
    uint   JsonToColorARGB(cJSON* jArray);
    cJSON* ColorABGRToJson(uint color);
    cJSON* ColorARGBToJson(uint color);
public:
    void load();
    void save();
};


////////////////////////////////////////////////////////////////////////////////
//
// OscSettings
//
////////////////////////////////////////////////////////////////////////////////
class OscSettingsInterface
{
protected:
    OscSettings   settings;
    OscColors     colors;
protected:
    OscHardware*  pHardware;
    OscHardware   hardware2;
public:
    void load();
    void save();
public:
    OscHardware* getHardware();
    OscSettings* getSettings();
    OscColors*   getColors();
public:
    OscSettingsInterface();
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
