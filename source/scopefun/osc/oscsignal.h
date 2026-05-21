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
#ifndef __OSC__SIGNAL__
#define __OSC__SIGNAL__

#define MAXOSCVALUE    511.f
#define NUM_SAMPLES    10000
#define NUM_FFT        1048576
#define CAPTURE_BUFFER                62464
#define CAPTURE_BUFFER_HEADER          2048
#define CAPTURE_BUFFER_PADDING          416
#define CAPTURE_BUFFER_DATA           60000

class OsciloscopeFrame;

////////////////////////////////////////////////////////////////////////////////
//
// SignalMode
//
////////////////////////////////////////////////////////////////////////////////
enum SignalMode
{
    SIGNAL_MODE_PLAY,
    SIGNAL_MODE_PAUSE,
    SIGNAL_MODE_CAPTURE,
    SIGNAL_MODE_SIMULATE,
    SIGNAL_MODE_CLEAR,
};

////////////////////////////////////////////////////////////////////////////////
//
// FrameAttribute
//
////////////////////////////////////////////////////////////////////////////////
enum FrameAttribute
{
    FRAME_ATTRIBUTE_HIDE_SIGNAL      = 1,
    FRAME_ATTRIBUTE_TRIGGERED_LED    = 2,
    FRAME_ATTRIBUTE_ROLL_DISPLAY     = 4,
};

////////////////////////////////////////////////////////////////////////////////
//
// EtsAttribute
//
////////////////////////////////////////////////////////////////////////////////
enum Etsttribute
{
    ETS_CLEAR = 1,
    ETS_PAUSE = 2,
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeFrame
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeFrame
{
public:
    time_t                     utc;
    ularge                     firstFrame;
    ularge                     thisFrame;
    ularge                     triggerTime;
    double                     captureTime;
    double                     generateTime[2];
    double                     deltaTime[2];
public:
    uint                       edgeSample;
    double                     edgeOffset;
    Array<byte, NUM_SAMPLES>    attr;
    Array<ishort, NUM_SAMPLES>  analog[2];
    Array<ushort, NUM_SAMPLES>  digital;
    Array<byte, 2464>          debug;
public:
    uint                       ets;
    uint                       etsAttr;
public:
    OsciloscopeFrame();
public:
    ishort getAnalogShort(uint channel, uint sample);
    float  getAnalog(uint channel, uint sample);
    double getAnalogDouble(uint channel, uint sample);
    ishort getDigital(uint channel, uint sample);
    ushort getDigitalChannels(uint sample);
public:
    int  isFull();
    void clear();
public:
    void getTime(char* buffer, int size);
    void generate(double dt, uint count, double captureStart, double captureFreq);
    bool captureHeader(byte* src, uint size, ularge captureStart, ularge captureFreq);
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeFFT
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeFFT
{
public:
    float* aRe;
    float* aIm;
    float* aAmpl;
public:
    void init()
    {
        aRe   = (float*)pMemory->allocate(NUM_FFT * sizeof(float));
        aIm   = (float*)pMemory->allocate(NUM_FFT * sizeof(float));
        aAmpl = (float*)pMemory->allocate(NUM_FFT * sizeof(float));
    }

    void clear()
    {
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeFunction
//
////////////////////////////////////////////////////////////////////////////////

enum Token
{
    tAdd,
    tSub,
    tMul,
    tDiv,
    tMod,
    tMin,
    tMax,
    tSin,
    tCos,
    tCh0,
    tCh1,
    tNumber,
    tDouble,
    tLeft,
    tRight,
    tSeperator,
    tLast,
};

class OscToken
{
public:
    Token     type;
    double    value;
public:
    double evaluate(double par1, double par2);
    double evaluate(double par);
};

class OsciloscopeFunction
{
public:
    Array<OscToken, 128>  tokens;
    Array<OscToken, 128>  postfix;
    Array<double, 128>    stack;
public:
    void           tokenize(String input);
    void           parse();
    double         evaluate(double ch0, double ch1);
};

/* TODO: erase Lua
#define SCOPEFUN_MAX_SCRIPT 8
#define SCOPEFUN_LUA_BUFFER 16*1024
#define SCOPEFUN_LUA_ERROR  2048

typedef int (*LuaOnMsg)(const char* msg,void* window);

class OsciloscopeScript
{
private:
    SDL_SpinLock m_spinLock;
    lua_State*   m_luaState;
    LuaOnMsg     m_msg;
    SDL_atomic_t m_active;
    void*        m_userData;
    int          m_arrayIdx;
public:
    String       m_fileName;
    byte*        m_luaScript;
public:
    OsciloscopeScript(int index);
public:
    int OnFrame(SFrameData* data, int len, float* pos, float* zoom, void* user);
    int OnSample(int sample, ishort* ch0, ishort* ch1, ishort* fun, ushort* dig, float* pos, float* zoom, void* user);
    int OnDisplay(SDisplay* data, float* pos, float* zoom, void* user);
    int OnConfigure(SHardware* hw);
    int OnInit(SFContext* ctx);
    int OnFunction(ishort ch0, ishort ch1, ishort* fun);
    int OnUpload(SGenerator* gen, uint* sampleCount);
public:
    int LuaError(const char* str);
    int LuaPrint(const char* str);
public:
    int Load(String fileName, LuaOnMsg msg);
    int Reload();
    int Run();
    int Stop();
    int Active(int onOff);
public:
    void    SetUserData(void* user);
    void*   GetUserData();
    int     GetArrayIdx();
public:
    const char* GetPrint();
    void        ClrPrint();
};

class OsciloscopeCallback
{
private:
    byte*                                          m_help;
    SCallback                                      m_callback;
    LuaOnMsg                                       m_msg;
    Array<OsciloscopeScript*, SCOPEFUN_MAX_SCRIPT> m_script;
public:
    OsciloscopeCallback();
public:
    SCallback*          Ptr();
    OsciloscopeScript*  Get(int i);
    int                 Add(String fileName, LuaOnMsg msg);
    int                 SetHelp(String helpFile);
    const char*         GetHelp();
    int                 Clear();
    int                 Count();
};
*/
#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
