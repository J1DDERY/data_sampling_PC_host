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
#ifndef SCOPEFUN_API_HEADER
#define SCOPEFUN_API_HEADER


/*----------------------------------------
      ScopeFun API - Types
----------------------------------------*/
typedef unsigned char      byte;
typedef short              ishort;
typedef unsigned short     ushort;
typedef int                iint;
typedef unsigned int       uint;
typedef long long          ilarge;
typedef unsigned long long ularge;

/*----------------------------------------
   version
----------------------------------------*/
#define HARDWARE_VERSION 2

/*----------------------------------------
   bit
----------------------------------------*/
#define BIT(index) (1<<index)

/*----------------------------------------
   Control
-----------------------------------------*/
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

enum ControllType1
{
    CONTROLL1_SHUTDOWN = 0x0000,
    CONTROLL1_RESET = 0x0A5A,
    CONTROLL1_TEST = 0x0640,
    CONTROLL1_WAKEUP = 0x0003,
    CONTROLL1_NORMAL = 0x0600,
};

enum ControllType2
{
    CONTROLL2_NORMAL = 0x0000,
    CONTROLL2_TEST = 0x0002,
    CONTROLL2_RESET = 0x0004,
};

enum CalibrateFreq
{
    CALIBRATE_1K,
    CALIBRATE_5K,
    CALIBRATE_10K,
    CALIBRATE_100K,
    CALIBRATE_200K,
    CALIBRATE_500K,
    CALIBRATE_1M,
    CALIBRATE_2M,
};

/* Generator and Digital enums removed */


/*----------------------------------------

      ScopeFun API - Frame Constants

----------------------------------------*/
#define SCOPEFUN_FRAME_HEADER                  (1024)
#define SCOPEFUN_FRAME_DATA           (512*1000*1000)
#define SCOPEFUN_FRAME_PACKET             (1024*1024)
#define SCOPEFUN_FRAME_DATA           (512*1000*1000)
#define SCOPEFUN_FRAME_MEMORY         (512*1024*1024)
#define SCOPEFUN_FRAME_PACKET             (1024*1024)

/*----------------------------------------

      ScopeFun API - Constants

----------------------------------------*/
#define SCOPEFUN_MAX_VOLTAGE                 +8191
#define SCOPEFUN_MIN_VOLTAGE                 -8192
#define SCOPEFUN_VOLTAGE_RANGE               16383
#define SCOPEFUN_DISPLAY                    10000
#define SCOPEFUN_DISPLAY_FFT                (1*1024*1024)
#define SCOPEFUN_FIRMWARE_FX3               16384
#define SCOPEFUN_FIRMWARE_FPGA              (4*1024*1024)
#define SCOPEFUN_EEPROM_BYTES               (256*1024)
#define SCOPEFUN_EEPROM_FIRMWARE_NAME_BYTES (16)
#define SCOPEFUN_GENERATOR                  0 /* removed */

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


/*----------------------------------------
SHardware
----------------------------------------*/
typedef struct
{
    ushort controlAddr;
    ushort controlData;
    ushort vgaina;
    ushort vgainb;
    ushort offseta;
    ushort offsetb;
    ushort analogswitch;
    ushort triggerMode;
    ushort triggerSource;
    ushort triggerSlope;
    short  triggerLevel;
    ushort triggerHis;
    ushort reserved1;
    ushort xRange;
    ushort holdoffH;
    ushort holdoffL;
    ushort sampleSizeH;
    ushort sampleSizeL;
    // Generator and digital fields removed
    ushort average;
    ushort preTriggerH;
    ushort preTriggerL;
    ushort frameDataSetup;
    ushort reserved3;
    ushort reserved4;
    ushort reserved5;
    ushort reserved6;
    ushort reserved7;
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

typedef struct
{
    SArrayMagic                 magic;
    SArrayDeviceTemp            deviceTemp;
    SArrayETS                   etsDelay;                    /* restored ETS delay bytes */
    SArrayDigPatternCompleteCnt digitalPatternCompleteCnt;   /* restored digital pattern complete count */
    SArrayDebug                 debug;
    SArrayPaddingBefore         paddBefore;
    SArrayHardware              hardware;
    SArrayPaddingAfter          paddAfter;
    SArrayCrc                   crc;
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

/*----------------------------------------
   SEEPROM
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayEEPROM, byte, SCOPEFUN_EEPROM_BYTES);
typedef struct
{
    SArrayEEPROM data;
} SEeprom;

/*----------------------------------------
   SGUID
----------------------------------------*/
typedef struct
{
    uint    data1;
    ushort  data2;
    ushort  data3;
    byte    data4[8];
} SGUID;

/*----------------------------------------
   SFx3
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayFx3, byte, SCOPEFUN_FIRMWARE_FX3);
typedef struct
{
    uint      size;
    SArrayFx3 data;
} SFx3;

/*----------------------------------------
   SFpga
----------------------------------------*/
SCOPEFUN_ARRAY(SArrayFpga, byte, SCOPEFUN_FIRMWARE_FPGA);
typedef struct
{
    uint       size;
    SArrayFpga data;
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
