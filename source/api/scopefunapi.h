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
// scopefunapi.h - ScopeFun 硬件通信 API 头文件（C接口）
// 功能：定义与 ScopeFun USB 示波器硬件通信所需的数据结构、
//       类型定义、枚举常量、帧格式和接口函数声明。
//       包括硬件控制寄存器、校准数据、固件升级等。
//==============================================================================
#ifndef SCOPEFUN_API_HEADER
#define SCOPEFUN_API_HEADER


//==============================================================================
// 基础类型定义 - 确保跨平台字节宽度一致性
//==============================================================================
typedef unsigned char      byte;       // 无符号8位
typedef short              ishort;     // 有符号16位
typedef unsigned short     ushort;     // 无符号16位
typedef int                iint;       // 有符号32位
typedef unsigned int       uint;       // 无符号32位
typedef long long          ilarge;     // 有符号64位
typedef unsigned long long ularge;     // 无符号64位

//==============================================================================
// 硬件版本号
//==============================================================================
#define HARDWARE_VERSION 2             // 当前支持的硬件版本

//==============================================================================
// 位掩码生成宏
//==============================================================================
#define BIT(index) (1<<index)          // 将索引转换为位掩码值

//==============================================================================
// 模拟通道功能枚举 - 定义两通道间的运算方式
//   MEDIUM       : 取平均值 (ch0+ch1)/2
//   SUB_CH0_CH1  : ch0 - ch1
//   SUB_CH1_CH0  : ch1 - ch0
//   ADD          : ch0 + ch1
//   MIN          : 取最小值
//   MAX          : 取最大值
//   CUSTOM       : 用户自定义（Lua脚本）
//   UPLOADED     : 已上传的自定义函数
//==============================================================================
enum AnalogFunction
{
    ANALOG_FUNCTION_MEDIUM = 0,
    ANALOG_FUNCTION_SUB_CH0_CH1,
    ANALOG_FUNCTION_SUB_CH1_CH0,
    ANALOG_FUNCTION_ADD,
    ANALOG_FUNCTION_MIN,
    ANALOG_FUNCTION_MAX,
    ANALOG_FUNCTION_CUSTOM,
    ANALOG_FUNCTION_UPLOADED,
};

//==============================================================================
// 模拟通道属性标志位
//   CHANNEL_ATTR_B     : 通道B属性
//   CHANNEL_ATTR_A     : 通道A属性
//   CHANNEL_B_GROUND   : 通道B接地
//   CHANNEL_A_GROUND   : 通道A接地
//   CHANNEL_B_ACDC     : 通道B AC/DC耦合
//   CHANNEL_A_ACDC     : 通道A AC/DC耦合
//   CHANNEL_INTERLEAVE : 通道交织模式
//==============================================================================
enum AnalogFlag
{
    CHANNEL_ATTR_B = BIT(0),
    CHANNEL_ATTR_A = BIT(1),
    CHANNEL_B_GROUND = BIT(2),
    CHANNEL_A_GROUND = BIT(3),
    CHANNEL_B_ACDC = BIT(4),
    CHANNEL_A_ACDC = BIT(5),
    CHANNEL_INTERLEAVE = BIT(6),
};

//==============================================================================
// 控制类型1 - FPGA/CPLD 控制寄存器命令
//   SHUTDOWN : 关闭
//   RESET    : 复位 (0x0A5A)
//   TEST     : 测试模式 (0x0640)
//   WAKEUP   : 唤醒 (0x0003)
//   NORMAL   : 正常工作 (0x0600)
//==============================================================================
enum ControllType1
{
    CONTROLL1_SHUTDOWN = 0x0000,
    CONTROLL1_RESET = 0x0A5A,
    CONTROLL1_TEST = 0x0640,
    CONTROLL1_WAKEUP = 0x0003,
    CONTROLL1_NORMAL = 0x0600,
};

//==============================================================================
// 控制类型2 - 辅助控制寄存器命令
//==============================================================================
enum ControllType2
{
    CONTROLL2_NORMAL = 0x0000,          // 正常模式
    CONTROLL2_TEST = 0x0002,            // 测试模式
    CONTROLL2_RESET = 0x0004,           // 复位
};

//==============================================================================
// 校准频率枚举 - 用于自校准的测试信号频率
//==============================================================================
enum CalibrateFreq
{
    CALIBRATE_1K,                       // 1kHz
    CALIBRATE_5K,                       // 5kHz
    CALIBRATE_10K,                      // 10kHz
    CALIBRATE_100K,                     // 100kHz
    CALIBRATE_200K,                     // 200kHz
    CALIBRATE_500K,                     // 500kHz
    CALIBRATE_1M,                       // 1MHz
    CALIBRATE_2M,                       // 2MHz
};

/* Generator and Digital enums removed */


//==============================================================================
// 数据帧常量定义
//   FRAME_HEADER  : 帧头大小 (1024字节)
//   FRAME_DATA    : 帧数据最大容量 (500MB)
//   FRAME_PACKET  : 数据包大小 (1MB)
//   FRAME_MEMORY  : 帧内存分配大小 (512MB)
//==============================================================================
#define SCOPEFUN_FRAME_HEADER                  (1024)
#define SCOPEFUN_FRAME_DATA           (512*1000*1000)
#define SCOPEFUN_FRAME_PACKET             (1024*1024)
#define SCOPEFUN_FRAME_DATA           (512*1000*1000)
#define SCOPEFUN_FRAME_MEMORY         (512*1024*1024)
#define SCOPEFUN_FRAME_PACKET             (1024*1024)

//==============================================================================
// API 常量定义
//   MAX_VOLTAGE           : ADC最大电压值 (+8191)
//   MIN_VOLTAGE           : ADC最小电压值 (-8192)
//   VOLTAGE_RANGE         : ADC电压范围 (16383)
//   DISPLAY               : 显示采样点数 (10000)
//   DISPLAY_FFT           : FFT显示采样点数 (1M)
//   FIRMWARE_FX3          : FX3固件最大大小 (16KB)
//   FIRMWARE_FPGA         : FPGA固件最大大小 (4MB)
//   EEPROM_BYTES          : EEPROM容量 (256KB)
//   EEPROM_FIRMWARE_NAME_BYTES : 固件名称最大长度 (16字节)
//==============================================================================
#define SCOPEFUN_MAX_VOLTAGE                 +8191            // ADC最大量化值
#define SCOPEFUN_MIN_VOLTAGE                 -8192            // ADC最小量化值
#define SCOPEFUN_VOLTAGE_RANGE               16383            // ADC量化范围
#define SCOPEFUN_DISPLAY                    10000             // 时域显示点数
#define SCOPEFUN_DISPLAY_FFT                (1*1024*1024)    // 频域显示点数
#define SCOPEFUN_FIRMWARE_FX3               16384             // FX3固件大小
#define SCOPEFUN_FIRMWARE_FPGA              (4*1024*1024)    // FPGA固件大小
#define SCOPEFUN_EEPROM_BYTES               (256*1024)        // EEPROM容量
#define SCOPEFUN_EEPROM_FIRMWARE_NAME_BYTES (16)              // 固件名长度

/*----------------------------------------
      ScopeFun API - Errors
----------------------------------------*/
#define SCOPEFUN_SUCCESS                0
#define SCOPEFUN_FAILURE               -1


/*----------------------------------------
      ScopeFun API - Array
----------------------------------------*/
#ifdef SWIG
#define SCOPEFUN_ARRAY(name,type,size)           \
    typedef struct {                                 \
        type bytes[size];                            \
        %extend {                                    \
            int __len() const { return size; }        \
            type __getitem(int i)                     \
            {                                           \
                return self->bytes[i];                \
            }                                        \
            void __setitem(int i,type v)           \
            {                                        \
                self->bytes[i] = v;                   \
            }                                        \
        }                                           \
    }name; \
    %typemap(memberin) type bytes[size]                                                                                  \
    {                                                                                                                    \
        memcpy($1, $input, size);                                                                                        \
    }
#else
#define SCOPEFUN_ARRAY(name,type,size)      \
    typedef struct {                            \
        type bytes[size];                       \
    }name;
#endif


/*----------------------------------------

   ScopeFun API - Structures

----------------------------------------*/
typedef enum _EFunctionType
{
    dfMax,
    dfMin,
    dfMedium,
    dfAdd,
    dfCh0SubCh1,
    dfCh1SubCh0,
    dfScript,
} EFunctionType;


//==============================================================================
// SHardware - 硬件控制寄存器结构体
// 映射USB数据传输中的128字节硬件配置块，用于控制示波器前端：
//   controlAddr/data : FPGA/CPLD 控制寄存器地址和数据
//   vgaina/vgainb    : 通道A/B 的可编程增益放大器(PGA)控制
//   offseta/offsetb  : 通道A/B 的直流偏置调整
//   analogswitch     : 模拟开关配置
//   triggerMode/Source/Slope/Level : 触发起始、源、边沿和电平
//   xRange           : 时基范围
//   holdoff          : 触发释抑时间
//   sampleSize       : 采样点数
//   average          : 平均次数
//   preTrigger       : 预触发深度
//   frameDataSetup   : 帧数据传输配置
//==============================================================================
typedef struct
{
    ushort controlAddr;                  // FPGA控制寄存器地址
    ushort controlData;                  // FPGA控制寄存器数据
    ushort vgaina;                       // 通道A 增益控制
    ushort vgainb;                       // 通道B 增益控制
    ushort offseta;                      // 通道A 偏移
    ushort offsetb;                      // 通道B 偏移
    ushort analogswitch;                 // 模拟开关
    ushort triggerMode;                  // 触发模式
    ushort triggerSource;                // 触发源
    ushort triggerSlope;                 // 触发边沿（上升/下降）
    short  triggerLevel;                 // 触发电平
    ushort triggerHis;                   // 触发迟滞
    ushort reserved1;                    // 保留
    ushort xRange;                       // 时基范围
    ushort holdoffH;                     // 释抑时间高16位
    ushort holdoffL;                     // 释抑时间低16位
    ushort sampleSizeH;                  // 采样点数高16位
    ushort sampleSizeL;                  // 采样点数低16位
    // Generator and digital fields removed
    ushort average;                      // 平均采样次数
    ushort preTriggerH;                  // 预触发深度高16位
    ushort preTriggerL;                  // 预触发深度低16位
    ushort frameDataSetup;               // 帧数据配置
    ushort reserved3;                    // 保留
    ushort reserved4;                    // 保留
    ushort reserved5;                    // 保留
    ushort reserved6;                    // 保留
    ushort reserved7;                    // 保留
} SHardware;

/*----------------------------------------
   SFrameHeader
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayMagic,                 byte,     4);
SCOPEFUN_ARRAY(SArrayDeviceTemp,            byte,     4);
SCOPEFUN_ARRAY(SArrayETS,                   byte,     4);
SCOPEFUN_ARRAY(SArrayDigPatternCompleteCnt, byte,     4);
SCOPEFUN_ARRAY(SArrayDebug,                 byte,     4);
SCOPEFUN_ARRAY(SArrayPaddingBefore,         byte,   236);
SCOPEFUN_ARRAY(SArrayHardware,              byte,   128);
SCOPEFUN_ARRAY(SArrayPaddingAfter,          byte,   639);
SCOPEFUN_ARRAY(SArrayCrc,                   byte,     1);

//==============================================================================
// SFrameHeader - USB数据传输帧头结构
// 每帧数据以1024字节的帧头开始，包含：
//   magic       : 魔术字（4字节，用于帧同步验证）
//   deviceTemp  : 设备温度
//   etsDelay    : 等效时间采样(ETS)延迟
//   digitalPatternCompleteCnt : 数字通道模式完成计数
//   debug       : 调试数据
//   paddBefore  : 填充字节（对齐到128字节边界）
//   hardware    : 硬件控制寄存器快照（128字节）
//   paddAfter   : 填充字节
//   crc         : CRC校验字节
//==============================================================================
typedef struct
{
    SArrayMagic                 magic;                       // 帧魔术字（0-3字节）
    SArrayDeviceTemp            deviceTemp;                  // 设备温度（4-7字节）
    SArrayETS                   etsDelay;                    // ETS等效时间采样延迟
    SArrayDigPatternCompleteCnt digitalPatternCompleteCnt;   // 数字模式完成计数
    SArrayDebug                 debug;                       // 调试信息
    SArrayPaddingBefore         paddBefore;                  // 前置填充（对齐）
    SArrayHardware              hardware;                    // 硬件配置快照（128字节）
    SArrayPaddingAfter          paddAfter;                   // 后置填充（对齐）
    SArrayCrc                   crc;                         // CRC校验
} SFrameHeader;


/*----------------------------------------
   SArrayString256
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayString256, byte, 256);

/*----------------------------------------
   SFrameData
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayFrameData, byte, SCOPEFUN_FRAME_MEMORY);
typedef struct
{
    SArrayFrameData data;
} SFrameData;

//==============================================================================
// SEeprom - EEPROM数据存储结构
// 用于存储校准参数、设备证书和固件备份信息
// 总容量 256KB，通过 I2C 接口读写
//==============================================================================
SCOPEFUN_ARRAY(SArrayEEPROM, byte, SCOPEFUN_EEPROM_BYTES);
typedef struct
{
    SArrayEEPROM data;                   // EEPROM原始数据 (256KB)
} SEeprom;

//==============================================================================
// SGUID - 全局唯一标识符
// 用于设备身份识别，与校准证书关联以确保设备合法性
//==============================================================================
typedef struct
{
    uint    data1;                       // GUID第一部分
    ushort  data2;                       // GUID第二部分
    ushort  data3;                       // GUID第三部分
    byte    data4[8];                    // GUID第四部分 (8字节)
} SGUID;

//==============================================================================
// SFx3 - FX3 USB控制器固件
// Cypress FX3 是 USB 3.0 外设控制器，负责上位机与FPGA之间的高速数据传输
// 固件大小限制为 SCOPEFUN_FIRMWARE_FX3 (16KB)
//==============================================================================
SCOPEFUN_ARRAY(SArrayFx3, byte, SCOPEFUN_FIRMWARE_FX3);
typedef struct
{
    uint      size;                      // 固件实际字节数
    SArrayFx3 data;                      // 固件二进制数据
} SFx3;

//==============================================================================
// SFpga - FPGA配置固件
// FPGA负责高速采样控制、触发逻辑和数字信号处理
// 固件大小限制为 SCOPEFUN_FIRMWARE_FPGA (4MB)
//==============================================================================
SCOPEFUN_ARRAY(SArrayFpga, byte, SCOPEFUN_FIRMWARE_FPGA);
typedef struct
{
    uint       size;                     // 固件实际字节数
    SArrayFpga data;                     // 固件二进制数据
} SFpga;

/* Generator config/data types removed */

/*----------------------------------------
   ESimulateType
----------------------------------------*/
typedef enum _ESimulateType
{
    stSin,
    stCos,
    stInc,
    stDec,
    stConstant,
    stRandom,
    stSquare,
    stDelta,
} ESimulateType;

/*----------------------------------------
   SSimulate
----------------------------------------*/
typedef struct
{
    byte                    active0;
    byte                    active1;
    ESimulateType           type0;
    ESimulateType           type1;
    float                   period0;
    float                   period1;
    float                   peakToPeak0;
    float                   peakToPeak1;
    float                   avery0;
    float                   avery1;
    float                   speed0;
    float                   speed1;
    uint                    etsIndex;
    uint                    etsActive;
    uint                    etsMax;
    float                   time;
    float                   voltage0;
    float                   voltage1;
} SSimulate;


/*----------------------------------------
   SUsb
----------------------------------------*/
typedef struct
{
    uint         idVendor;
    uint         idProduct;
    uint         idSerial;
    uint         timeoutEp2;
    uint         timeoutEp4;
    uint         timeoutEp6;
    uint         xferSize;
    SGUID        guid;
} SUsb;


/*----------------------------------------
   SDisplay
----------------------------------------*/
enum EDisplayAttribute
{
    daCHANNEL_ATTR_B   = 0,
    daCHANNEL_ATTR_A   = 1,
    daCHANNEL_B_GROUND = 2,
    daCHANNEL_A_GROUND = 3,
    daCHANNEL_B_ACDC   = 4,
    daCHANNEL_A_ACDC   = 5,
    daCHANNEL_INTERLEAVE = 6,
};
SCOPEFUN_ARRAY(SArrayDisplayAnalog0, float,  SCOPEFUN_DISPLAY);
SCOPEFUN_ARRAY(SArrayDisplayAnalog1, float,  SCOPEFUN_DISPLAY);
SCOPEFUN_ARRAY(SArrayDisplayDigital, ushort, SCOPEFUN_DISPLAY);
SCOPEFUN_ARRAY(SArrayDisplayFFT0,    float,  SCOPEFUN_DISPLAY_FFT);
SCOPEFUN_ARRAY(SArrayDisplayFFT1,    float,  SCOPEFUN_DISPLAY_FFT);
typedef struct
{
    SArrayDisplayAnalog0 analog0;
    SArrayDisplayAnalog1 analog1;
    SArrayDisplayDigital digital;
    SArrayDisplayFFT0    fft0;
    SArrayDisplayFFT1    fft1;
    uint                 samples;
    uint                 captured;
    uint                 ppi;
    // ets removed
    uint                 attr; //channel attributes
    uint                 digitalPatternCompleteCnt;
    /* ETS-related fields removed */
} SDisplay;

/*----------------------------------------

ScopeFun API - Context

----------------------------------------*/
typedef struct
{
    int value;
} SAtomic;

typedef int SSpinLock;

typedef struct
{
    SAtomic           on;
    SSimulate         data;
    uint              active;
} SCtxSimulate;

typedef struct
{
    uint              maxMemory;
    SFrameData        data;
    uint              received;
    uint              frameSize;
} SCtxFrame;

typedef struct
{
    SDisplay          data;
} SCtxDisplay;

typedef struct
{
    uint              version;
    uint              major;
    uint              minor;
    uint              active;
    SAtomic           thread;
    SSpinLock         lock;
    uint              timeout;
} SCtxApi;

typedef struct
{
    SCtxApi           api;
    SAtomic           simulateOn;
    SSimulate         simulateData;
    SCtxFrame         frame;
    EFunctionType     functionType;
    //SGeneratorConfig* genConfig;
    void*             pCallback;
    void*             pUserData;
    byte*             usb;
} SFContext;

typedef struct _SCallback
{
    int (*onFrame)(SFrameData* data, int len, double* pos, double* zoom, void* user);
    int (*onSample)(int sample, ishort* ch0, ishort* ch1, ishort* fun, ushort* dig, double* pos, double* zoom, void* user);
    int (*onDisplay)(SDisplay* data, double* pos, double* zoom, void* user);
    int (*onConfigure)(SHardware* hw);
    int (*onInit)(void* ptr);
    int (*onFunction)(ishort ch0, ishort ch1, ishort* fun);
} SCallback;

typedef struct _SInt
{
    int value;
} SInt;

typedef struct _SFloat
{
    float value;
} SFloat;

/*----------------------------------------

   ScopeFun API - Functions

----------------------------------------*/
#define SCOPEFUN_API extern

#ifndef SWIG
    #define INPUT
    #define OUTPUT
    #define INOUT
#endif

/*----------------------------------------
   defines
----------------------------------------*/
#define SCOPEFUN_CREATE(name) SCOPEFUN_API name* sfCreate##name();
#define SCOPEFUN_DELETE(name) SCOPEFUN_API void  sfDelete##name(name* INPUT);

/*----------------------------------------
   create
----------------------------------------*/
SCOPEFUN_CREATE(SFloat)
SCOPEFUN_CREATE(SInt)
SCOPEFUN_CREATE(SFContext)
SCOPEFUN_CREATE(SDisplay)
SCOPEFUN_CREATE(SSimulate)
SCOPEFUN_CREATE(SUsb)
SCOPEFUN_CREATE(SHardware)
SCOPEFUN_CREATE(SFx3)
SCOPEFUN_CREATE(SFpga)
/* Generator config/data types removed */
SCOPEFUN_CREATE(SEeprom)
SCOPEFUN_CREATE(SFrameData)
SCOPEFUN_CREATE(SFrameHeader)
SCOPEFUN_CREATE(SHardware)

/*----------------------------------------
   delete
----------------------------------------*/
SCOPEFUN_DELETE(SFloat)
SCOPEFUN_DELETE(SInt)
SCOPEFUN_DELETE(SFContext)
SCOPEFUN_DELETE(SDisplay)
SCOPEFUN_DELETE(SSimulate)
SCOPEFUN_DELETE(SUsb)
SCOPEFUN_DELETE(SHardware)
SCOPEFUN_DELETE(SFrameData)
SCOPEFUN_DELETE(SFx3)
SCOPEFUN_DELETE(SFpga)
/* Generator types removed */
SCOPEFUN_DELETE(SEeprom)
SCOPEFUN_DELETE(SFrameData)
SCOPEFUN_DELETE(SFrameHeader)
SCOPEFUN_DELETE(SHardware)

/*----------------------------------------
  Initialization
----------------------------------------*/
SCOPEFUN_API int sfApiInit();
SCOPEFUN_API int sfApiCreateContext(SFContext* INOUT, int INPUT);
SCOPEFUN_API int sfApiDeleteContext(SFContext* INOUT);
SCOPEFUN_API int sfApiVersion(SFContext* INOUT, SInt* INOUT, SInt* INOUT, SInt* INOUT);
SCOPEFUN_API int sfSetThreadSafe(SFContext* INOUT, int INPUT);
SCOPEFUN_API int sfIsThreadSafe(SFContext* INOUT);
SCOPEFUN_API int sfSetActive(SFContext* INOUT,  int  INPUT);
SCOPEFUN_API int sfIsActive(SFContext* INOUT);
SCOPEFUN_API int sfSetTimeOut(SFContext* INOUT, int  INPUT);
SCOPEFUN_API int sfGetTimeOut(SFContext* INOUT, SInt* INOUT);
SCOPEFUN_API int sfApiExit();

/*----------------------------------------
  Hardware
----------------------------------------*/
SCOPEFUN_API int sfHardwareOpen(SFContext* INOUT, SUsb* INOUT, int INPUT);
SCOPEFUN_API int sfHardwareReset(SFContext* INOUT);
SCOPEFUN_API int sfHardwareIsOpened(SFContext* INOUT, SInt* INOUT);
SCOPEFUN_API int sfHardwareConfig(SFContext* INOUT, SHardware* INOUT);
SCOPEFUN_API int sfHardwareCapture(SFContext* INOUT, SFrameData* INOUT, int INPUT, int INPUT, SInt* INOUT);
SCOPEFUN_API int sfHardwareUploadFx3(SFContext* INOUT, SFx3* INOUT);
SCOPEFUN_API int sfHardwareUploadFpga(SFContext* INOUT, SFpga* INOUT);
/* Generator upload removed */
SCOPEFUN_API int sfHardwareEepromRead(SFContext* INOUT, SEeprom* INOUT, int INPUT, int INPUT);
SCOPEFUN_API int sfHardwareEepromReadFirmwareID(SFContext* INOUT, SEeprom* INOUT, int INPUT);
SCOPEFUN_API int sfHardwareReadFpgaStatus(SFContext* ctx, SInt* INOUT);
SCOPEFUN_API int sfHardwareEepromWrite(SFContext* INOUT, SEeprom* INOUT, int INPUT, int INPUT);
SCOPEFUN_API int sfHardwareEepromErase(SFContext* INOUT);
SCOPEFUN_API int sfHardwareClose(SFContext* INOUT);

/*----------------------------------------
  Frame
----------------------------------------*/
SCOPEFUN_API int sfFrameCapture(SFContext* INOUT, SInt* INOUT, SInt* INOUT);
SCOPEFUN_API int sfFrameOutput(SFContext*  ctx, SFrameData* INOUT, int INPUT);
SCOPEFUN_API int sfFrameDisplay(SFContext* INOUT, SFrameData* INOUT, int INPUT, SDisplay* INOUT, double INPUT, double INPUT);
SCOPEFUN_API int sfFrameDisplayFunction(SFContext* INOUT, EFunctionType INPUT);
SCOPEFUN_API int sfFrameDisplayCallback(SFContext* INOUT, SCallback* INOUT, void* INPUT);


/*----------------------------------------
  Header
----------------------------------------*/
SCOPEFUN_API int sfGetHeader(SFContext* INOUT, SFrameData* INOUT, SFrameHeader* INOUT);
SCOPEFUN_API int sfGetHeaderHardware(SFrameHeader* INOUT, SHardware* INOUT);
SCOPEFUN_API int sfGetHeaderEts(SFrameHeader* INOUT, uint* OUTPUT);
SCOPEFUN_API int sfGetHeaderTemperature(SFrameHeader* INOUT, SFloat* INOUT);
SCOPEFUN_API int sfGetHeaderDigitalPatternCompleteCnt(SFrameHeader* INOUT, uint* OUTPUT);

/*----------------------------------------
  hardware
----------------------------------------*/
SCOPEFUN_API int sfHardwareWordCnt(int* INOUT);
SCOPEFUN_API int sfHardwareWordId(int INPUT, SArrayString256* INOUT);

/*----------------------------------------
  Data
----------------------------------------*/
SCOPEFUN_API int sfSetData(byte* INPUT, ishort  INPUT, ishort  INPUT, ushort  INPUT);
SCOPEFUN_API int sfGetData(uint  INPUT, ishort* INOUT, ishort* INOUT, ushort* INOUT);

/*----------------------------------------
   Simulate
----------------------------------------*/
SCOPEFUN_API int sfIsSimulate(SFContext* INOUT);
SCOPEFUN_API int sfSetSimulateData(SFContext* INOUT, SSimulate* INOUT);
SCOPEFUN_API int sfGetSimulateData(SFContext* INOUT, SSimulate* INOUT);
SCOPEFUN_API int sfSetSimulateOnOff(SFContext* INOUT, int INOUT);
SCOPEFUN_API int sfSimulate(SFContext* INOUT, SHardware* INOUT, SInt* INOUT, SInt* INOUT, float INOUT);

/*----------------------------------------
   Set
----------------------------------------*/

// analog
SCOPEFUN_API int    sfSetDefault(SHardware* INOUT);
SCOPEFUN_API int    sfSetFrameSize(SHardware*  INOUT, uint  INPUT);
SCOPEFUN_API int    sfSetNumSamples(SHardware* INOUT, uint  INPUT);
SCOPEFUN_API int    sfSetAnalogSwitchBit(SHardware* INOUT, int INPUT, int INPUT);
/* sfSetEts removed */
SCOPEFUN_API int    sfSetYRangeScaleA(SHardware* INOUT, ushort INPUT, ushort INPUT);
SCOPEFUN_API int    sfSetYPositionA(SHardware* INOUT, int INPUT);
SCOPEFUN_API int    sfSetYRangeScaleB(SHardware* INOUT, ushort INPUT, ushort INPUT);
SCOPEFUN_API int    sfSetYPositionB(SHardware* INOUT, int INPUT);
SCOPEFUN_API int    sfSetXRange(SHardware* INOUT, ishort INPUT);
SCOPEFUN_API int    sfSetControl(SHardware* INOUT, uint INPUT);
SCOPEFUN_API int    sfSetSampleSize(SHardware* INOUT, uint INPUT);
SCOPEFUN_API int    sfSetTriggerSource(SHardware* INOUT, int INPUT);
SCOPEFUN_API int    sfSetTriggerReArm(SHardware* INOUT, int INPUT);
SCOPEFUN_API int    sfSetTriggerMode(SHardware* INOUT, int INPUT);
SCOPEFUN_API int    sfSetTriggerSlope(SHardware* INOUT, int INPUT);
SCOPEFUN_API int    sfSetTriggerPre(SHardware* INOUT, float INPUT);
SCOPEFUN_API int    sfSetTriggerHis(SHardware* INOUT, int INPUT);
SCOPEFUN_API int    sfSetTriggerLevel(SHardware* INOUT, int INPUT);
SCOPEFUN_API int    sfSetHoldoff(SHardware* INOUT, uint INPUT);
SCOPEFUN_API int    sfSetAverage(SHardware* INOUT, int INPUT);

/* Generator setters removed */

/* Digital trigger setters removed */

/* Digital GPIO setters removed */

// data
SCOPEFUN_API int    sfSetDataEncodingFormat(SHardware* INOUT, int INPUT);

/*----------------------------------------
   Get
----------------------------------------*/

// analog
SCOPEFUN_API uint   sfGetFrameSize(SHardware*  INPUT);
SCOPEFUN_API uint   sfGetNumSamples(SHardware* INPUT);
SCOPEFUN_API ushort sfGetAnalogSwitch(SHardware* INPUT);
/* sfGetEts removed */
SCOPEFUN_API uint   sfGetControl(SHardware* INPUT);
SCOPEFUN_API uint   sfGetYGainA(SHardware* INPUT);
SCOPEFUN_API float  sfGetYScaleA(SHardware* INPUT);
SCOPEFUN_API int    sfGetYPositionA(SHardware* INPUT);
SCOPEFUN_API uint   sfGetYGainB(SHardware* INPUT);
SCOPEFUN_API float  sfGetYScaleB(SHardware* INPUT);
SCOPEFUN_API int    sfGetYPositionB(SHardware* INPUT);
SCOPEFUN_API ushort sfGetTriggerSource(SHardware* INPUT);
SCOPEFUN_API ushort sfGetTriggerMode(SHardware* INPUT);
SCOPEFUN_API ushort sfGetTriggerSlope(SHardware* INPUT);
SCOPEFUN_API float  sfGetTriggerPre(SHardware* INPUT);
SCOPEFUN_API int    sfGetTriggerHis(SHardware* INPUT);
SCOPEFUN_API int    sfGetTriggerLevel(SHardware* INPUT);
SCOPEFUN_API ishort sfGetXRange(SHardware* INPUT);
SCOPEFUN_API uint   sfGetSampleSize(SHardware* INPUT);
SCOPEFUN_API uint   sfGetHoldoff(SHardware* INPUT);
SCOPEFUN_API int    sfGetAverage(SHardware* INPUT);

/* Generator getters removed */

/* Digital trigger getters removed */

/* Digital GPIO getters removed */

// data
SCOPEFUN_API int    sfGetDataEncodingFormat(SHardware* INOUT);

#ifndef SWIG
    #undef INPUT
    #undef OUTPUT
    #undef INOUT
#endif

/*----------------------------------------
  http://www.ScopeFun.com
----------------------------------------*/
#endif
