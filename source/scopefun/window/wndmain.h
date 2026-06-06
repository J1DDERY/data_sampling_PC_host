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
// wndmain.h - 主窗口数据模型头文件
// 功能：定义主窗口的数据结构，包括：
//   - 视图选择枚举（2D/3D 时域/频域）
//   - 垂直通道配置 (MWVertical)
//   - 数学函数通道配置 (MWFunction)
//   - 水平时基配置 (MWHorizontal)
//   - 触发配置 (MWTrigger)
//   - 测量配置 (MWMeasure)
//   - 存储配置 (WndStorage)
//   - 进度条状态 (MWProgress)
//   - 主窗口类 (WndMain) - UI控件的完整定义
//==============================================================================
#ifndef __OSCILOSCOPE__MAIN__WINDOW__
#define __OSCILOSCOPE__MAIN__WINDOW__

#define MEASURE_HISTORY_MAX 32           // 测量历史记录最大条目数

//==============================================================================
// ViewSelect - 视图选择枚举
// 使用位标志组合，支持同时显示多种视图
//   OSC_3D : 3D时域波形视图
//   OSC_2D : 2D时域波形视图
//   FFT_3D : 3D频谱视图
//   FFT_2D : 2D频谱视图
//==============================================================================
enum ViewSelect
{
    VIEW_SELECT_OSC_3D  = BIT(0),        // 3D示波器视图
    VIEW_SELECT_OSC_2D  = BIT(1),        // 2D示波器视图
    VIEW_SELECT_FFT_3D  = BIT(2),        // 3D FFT视图
    VIEW_SELECT_FFT_2D  = BIT(3),        // 2D FFT视图
};

////////////////////////////////////////////////////////////////////////////////
//
// MWVertical
//
////////////////////////////////////////////////////////////////////////////////
class MWVertical
{
public:
    int channelId;
public:
    float              Capture;
    float              Scale;
    float              Display;
    float              YPosition;
    int                OscOnOff;
    int                FFTOnOff;
    int                Invert;
    int                Ground;
    int                AcDc;
public:
    MWVertical();
public:
    void Default();
};

////////////////////////////////////////////////////////////////////////////////
//
// MWFunction
//
////////////////////////////////////////////////////////////////////////////////
class MWFunction
{
public:
    int Type;
    int OscOnOff;
    int FFTOnOff;
    int xyGraph;
    OsciloscopeFunction custom;
public:
    MWFunction();
public:
    void Default();
};

////////////////////////////////////////////////////////////////////////////////
//
// MWHorizontal
//
////////////////////////////////////////////////////////////////////////////////
class MWHorizontal
{
public:
    double             Capture;
    float              Display;
    float              Position;
    uint               Mode;
    uint               Control;
    uint               Frame;
    uint               FrameSize;
    uint               FFTSize;
    uint               ETS;
    uint               Full;
    //public:
    //    SDL_atomic_t       uiActive;
    //    SDL_atomic_t       uiRange;
    //    SDL_atomic_t       uiValue;
public:
    MWHorizontal();
public:
    void Default();
};

////////////////////////////////////////////////////////////////////////////////
//
// MWTrigger
//
////////////////////////////////////////////////////////////////////////////////
class MWTrigger
{
public:
    uint               Source;
    uint               Slope;
    uint               Mode;
    int                Level;
    int                His;
    double             Percent;
    uint               Holdoff;
public:
    int                stage;
    int                stageStart;
    int                stageMode;
    int                stageChannel;
    ushort             delay[4];
    Array<int, 16>     mask[4];
    Array<int, 16>     pattern[4];
public:
    MWTrigger();
public:
    void Default();
};



////////////////////////////////////////////////////////////////////////////////
//
// MeasureBits
//
////////////////////////////////////////////////////////////////////////////////
enum MeasureBits
{
    MEASURE_CH0_PICK         = BIT(0),
    MEASURE_CH0_START        = BIT(1),
    MEASURE_CH0_START_TEXT   = BIT(2),
    MEASURE_CH0_END          = BIT(3),
    MEASURE_CH0_END_TEXT     = BIT(4),
    MEASURE_CH1_PICK         = BIT(5),
    MEASURE_CH1_START        = BIT(6),
    MEASURE_CH1_START_TEXT   = BIT(7),
    MEASURE_CH1_END          = BIT(8),
    MEASURE_CH1_END_TEXT     = BIT(9),
    MEASURE_CHF_PICK         = BIT(10),
    MEASURE_CHF_START        = BIT(11),
    MEASURE_CHF_START_TEXT   = BIT(12),
    MEASURE_CHF_END          = BIT(13),
    MEASURE_CHF_END_TEXT     = BIT(14),
    MEASURE_CH0_PICK_TEXT    = BIT(15),
    MEASURE_CH1_PICK_TEXT    = BIT(16),
    MEASURE_CHF_PICK_TEXT    = BIT(17),
    MEASURE_CH0_START_Z_TEXT = BIT(18),
    MEASURE_CH0_END_Z_TEXT   = BIT(19),
    MEASURE_CH1_START_Z_TEXT = BIT(20),
    MEASURE_CH1_END_Z_TEXT   = BIT(21),
    MEASURE_CHF_START_Z_TEXT = BIT(22),
    MEASURE_CHF_END_Z_TEXT   = BIT(23),
    MEASURE_CH0_PICK_Z_TEXT  = BIT(24),
    MEASURE_CH1_PICK_Z_TEXT  = BIT(25),
    MEASURE_CHF_PICK_Z_TEXT  = BIT(26),
};

enum LockFrameBits
{
    MEASURE_LOCK_FRAME_CH0_START = BIT(0),
    MEASURE_LOCK_FRAME_CH1_START = BIT(1),
    MEASURE_LOCK_FRAME_CHF_START = BIT(2),
    MEASURE_LOCK_FRAME_CH0_END   = BIT(3),
    MEASURE_LOCK_FRAME_CH1_END   = BIT(4),
    MEASURE_LOCK_FRAME_CHF_END   = BIT(5),
    MEASURE_LOCK_FRAME_CH0_PICK  = BIT(6),
    MEASURE_LOCK_FRAME_CH1_PICK  = BIT(7),
    MEASURE_LOCK_FRAME_CHF_PICK  = BIT(8),
};

class MeasurePos
{
public:
    double x;
    double y;
    double z;
    uint   frame;
    uint   index;
public:
    double xTime;
    double xFreq;
    double yVolt;
public:
    MeasurePos()
    {
        clear();
    }
public:
    void clear()
    {
        x = 0;
        y = 0;
        z = 0;
        frame = 0;
        index = 0;
        xTime = 0;
        xFreq = 0;
        yVolt = 0;
    }

    float FrameToZ(int iframe);
    int   ZToFrame(float fz);
public:
    uint  getXIndex(double sx,double sz,uint frameSize);
public:
    void setXTime(double sx,double sz,double time);
    void setXFreq(double freq);
    void setYVolt(double volt, int channel);
public:
    double getXTime(double sx,double sz);
    double getXFreq();
    double getYVolt(int channel);
public:
    void  setZ(int frame);
    int   getZ();
};

enum MeasureHistory
{
    MEASURE_CURRENT,
    MEASURE_AVERAGE,
    MEASURE_MINIMUM,
    MEASURE_MAXIMUM,
    MEASURE_LAST,
};

class MeasurePick
{
public:
    bool         active;
    bool         updateUI;
    bool         lineMove;
    MeasurePos   position;
public:
    MeasurePick()
    {
        clear();
    }
public:
    void onActivate()
    {
        active = true;
        lineMove = true;
    }

    void clear()
    {
        updateUI = false;
        active = false;
        lineMove = false;
        position.clear();
    }

    void onUpdate(double x, double y, double z, int frame)
    {
        if(active && lineMove)
        {
            position.x = x;
            position.y = y;
            position.z = z;
            position.frame = frame;
            updateUI = true;
        }
    }

    void onClick()
    {
        if(lineMove)
        {
            lineMove = false;
            updateUI = true;
        }
    }
};

enum EValueGrid1
{
    // channel 0
    Channel0,
    Ch0Vavg,
    Ch0VrmsDc,
    Ch0VrmsAc,
    Ch0Vmin,
    Ch0Vmax,
    Ch0Vpp,
    Ch0Tperiod,
    Ch0Tfreq,
    Ch0Surface,
    Ch0XCursor,
    Ch0XV0,
    Ch0XV1,
    Ch0XVD,
    Ch0XT0,
    Ch0XT1,
    Ch0XTD,
    Ch0XInvTD,
    Ch0YCursor,
    Ch0YV0,
    Ch0YV1,
    Ch0YVD,
    // channel 1
    Channel1,
    Ch1Vavg,
    Ch1VrmsDc,
    Ch1VrmsAc,
    Ch1Vmin,
    Ch1Vmax,
    Ch1Vpp,
    Ch1Tperiod,
    Ch1Tfreq,
    Ch1Surface,
    Ch1XCursor,
    Ch1XV0,
    Ch1XV1,
    Ch1XVD,
    Ch1XT0,
    Ch1XT1,
    Ch1XTD,
    Ch1XInvTD,
    Ch1YCursor,
    Ch1YV0,
    Ch1YV1,
    Ch1YVD,
    // function
    Function,
    FunVavg,
    FunVrmsDc,
    FunVrmsAc,
    FunVmin,
    FunVmax,
    FunVpp,
    FunTperiod,
    FunTfreq,
    FunSurface,
    FunXCursor,
    FunXV0,
    FunXV1,
    FunXVD,
    FunXT0,
    FunXT1,
    FunXTD,
    FunXInvTD,
    FunYCursor,
    FunYV0,
    FunYV1,
    FunYVD,
    // fft
    FFTCh0,
    FFTCh0V0db,
    FFTCh0V1db,
    FFTCh0VD,
    FFTCh0F0,
    FFTCh0F1,
    FFTCh0FD,
    FFTCh1,
    FFTCh1V0db,
    FFTCh1V1db,
    FFTCh1VD,
    FFTCh1F0,
    FFTCh1F1,
    FFTCh1FD,
    FFTFun,
    FFTFunV0db,
    FFTFunV1db,
    FFTFunVD,
    FFTFunF0,
    FFTFunF1,
    FFTFunFD,
    // digital
    DigitalX0,
    X0Bit0,
    X0Bit1,
    X0Bit2,
    X0Bit3,
    X0Bit4,
    X0Bit5,
    X0Bit6,
    X0Bit7,
    X0Bit8,
    X0Bit9,
    X0Bit10,
    X0Bit11,
    DigitalX1,
    X1Bit0,
    X1Bit1,
    X1Bit2,
    X1Bit3,
    X1Bit4,
    X1Bit5,
    X1Bit6,
    X1Bit7,
    X1Bit8,
    X1Bit9,
    X1Bit10,
    X1Bit11,
    Last1,
};

class MeasureChannelData
{
public:
    int averageN;
public:
    double row[Last1];
public:
    double Row(EValueGrid1 r);
public:
    void Minimum(MeasureChannelData& other);
    void Maximum(MeasureChannelData& other);
    void SetValue(double value);
    void ClearData();
public:
    MeasureChannelData()
    {
        ClearData();
    }
};

enum EValueGrid2
{
    vg2First2    = 1,
    vg2XT0       = 2,
    vg2XT1       = 3,
    vg2XTD       = 4,
    vg2XInvTD    = 5,
    vg2YCh0V0    = 8,
    vg2YCh0V1    = 9,
    vg2YCh0VD    = 10,
    vg2YCh1V0    = 12,
    vg2YCh1V1    = 13,
    vg2YCh1VD    = 14,
    vg2YFunV0    = 16,
    vg2YFunV1    = 17,
    vg2YFunVD    = 18,
    vg2FFTV0db   = 20,
    vg2FFTF0     = 21,
    vg2FFTV1db   = 22,
    vg2FFTF1     = 23,
    vg2FFTVD     = 24,
    vg2FFTFD     = 25,
    vg2DigitalX0 = 28,
    vg2DigitalX1 = 30,
    Last2     = 31,
};

class MeasurePickData
{
public:
    double row[Last2];
    bool   display[Last2];
public:
    ushort DigitalX0;
    ushort DigitalX1;
public:
    double RowD(EValueGrid2 r);
    ushort RowU(EValueGrid2 r);
public:
    MeasurePickData()
    {
        memset(row,     0, sizeof(row));
        memset(display, 0, sizeof(display));
        DigitalX0 = DigitalX1 = 0;
    }
};

class MeasureData
{
public:
    MeasurePick pickX0;
    MeasurePick pickX1;
    MeasurePick pickY0;
    MeasurePick pickY1;
    MeasurePick pickFFT0;
    MeasurePick pickFFT1;
public:
    MeasurePickData    pick;
    MeasureChannelData column[MEASURE_LAST];
    MeasureChannelData history[MEASURE_HISTORY_MAX];
public:
    int historyCnt;
    int historyIndex;
public:
    MeasureData();
public:
    void Average(MeasureChannelData& column);
    void Clear();
};

class MWMeasure
{
public:
    MeasureData  data;
public:
    bool    automaticDataClear;
    bool    automaticDataClearTrigger;
    int     uiOpen;
public:
    void ClearCapture()
    {
        if(automaticDataClear)
        {
            data.Clear();
        }
    };
    void ClearTrigger()
    {
        if(automaticDataClearTrigger)
        {
            data.Clear();
        }
    };
public:
    MWMeasure();
public:
    void Default();
};

////////////////////////////////////////////////////////////////////////////////
//
// MWCalibrate
//
////////////////////////////////////////////////////////////////////////////////
class MWCalibrate
{
public:
    int onoff;
    int freq;
public:
    MWCalibrate()
    {
        onoff = 0;
        freq = 0;
    };
};

////////////////////////////////////////////////////////////////////////////////
//
// MWProgress
//
////////////////////////////////////////////////////////////////////////////////
class MWProgress
{
public:
    int    uiActive;
    int    uiValue;
    int    uiRange;
    int    uiPulse;
    int    uiText;
public:
    MWProgress();
};

enum UsbSpeed
{
    USB_SPEED_AUTOMATIC,
    USB_SPEED_LOW,
    USB_SPEED_MEDIUM,
    USB_SPEED_HIGH,
};

////////////////////////////////////////////////////////////////////////////////
//
// WndStorage
//
////////////////////////////////////////////////////////////////////////////////

enum MemoryType
{
    mtRAM,
    mtSSD,
};

enum PacketType
{
    ptAutomatic,
    pt512,
    pt16384,
    pt131072,
    pt1048576,
};

class WndStorage
{
public:
    MemoryType type;
    ularge     size;
    PacketType packet;
public:
    WndStorage();
public:
    void Default();
public:
    ularge getPacketSize(int version = 2);
};

////////////////////////////////////////////////////////////////////////////////
//
// WndMain
//
////////////////////////////////////////////////////////////////////////////////
class WndMain
{
public:
    UsbSpeed speed;
public:
    WndDisplay          display;
    WndThermal          thermal;
    WndStorage           storage;
public:
    MWProgress     progress;
    MWMeasure      measure;
    MWVertical     channel01;
    MWVertical     channel02;
    MWFunction     function;
    MWHorizontal   horizontal;
    MWTrigger      trigger;
    MWCalibrate    calibrate;
public:
    Flag32  fftDigital;
public:
    WndMain();
public:
    void Default();
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
