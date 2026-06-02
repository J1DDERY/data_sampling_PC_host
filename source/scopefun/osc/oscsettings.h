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
#ifndef __OSC__SETTINGS__
#define __OSC__SETTINGS__

#define CYPRESS_VID 1204
#define CYPRESS_PID 243

////////////////////////////////////////////////////////////////////////////////
//
// OscHardware
//
////////////////////////////////////////////////////////////////////////////////
class OscHardware
{
private:
    int version;
public:
    OscHardware(int version);
public:
    UsbGuid usbGuid;
    uint    usbVendor;
    uint    usbProduct;
    uint    usbSerial;
    String  usbFirmware;
public:
    String fpgaFirmware;
    uint   fpgaEtsIndex;
    uint   fpgaEtsCount;
public:
    double digitalVoltageCoeficient;
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
