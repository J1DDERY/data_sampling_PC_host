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
#include<scopefun/ScopeFun.h>

extern "C" ishort leadBitShift(ushort value);
extern "C" ishort leadBitCompl(ushort value);

uint ScopeFunCaptureBuffer::save(const char* path)
{
    SDL_AtomicSet(&m_active, 1);

    lock();
    // frame index
    int   frameIndex = SDL_AtomicGet(&m_frameIndex);
    FORMAT_BUFFER();
    // .sf
    FORMAT("%s", path);
    SDL_RWops* sfFile = SDL_RWFromFile(formatBuffer, "w+b");
    int frameCount  = SDL_AtomicGet(&m_frameCount);
    int frameOffset = SDL_AtomicGet(&m_frameOffset);
    FORMAT("----scopefun----\n", frameCount);
    SDL_RWwrite(sfFile, &formatBuffer, SDL_strlen(formatBuffer), 1);
    // structures
    SFContext*             ctx       = sfCreateSFContext();
    SFrameData*     frameData        = 0;
    SFrameHeader*   frameHeader      = sfCreateSFrameHeader();
    SHardware*      frameHardware    = sfCreateSHardware();
    uint            frameEts         = 0;
    SFloat          frameTemperature = { 0.0 };
    // context
    sfApiCreateContext(ctx, SCOPEFUN_FRAME_MEMORY);
    // info
    int   frameSize  = SDL_AtomicGet(&m_frameSize);
    ularge  framePos = ((frameIndex-1)%frameCount) * frameSize;
    // header
    frameData = (SFrameData*)&m_dataPtr[framePos];
    // get
    sfGetHeader(ctx, frameData, frameHeader);
    sfGetHeaderHardware(frameHeader, frameHardware);
    sfGetHeaderEts(frameHeader, &frameEts);
    sfGetHeaderTemperature(frameHeader, &frameTemperature);
    // frame
    FORMAT("frame.index,%d\n", (frameIndex%frameCount) );
    SDL_RWwrite(sfFile, &formatBuffer, SDL_strlen(formatBuffer), 1);
    FORMAT("frame.count,%d\n", frameCount);
    SDL_RWwrite(sfFile, &formatBuffer, SDL_strlen(formatBuffer), 1);
    FORMAT("frame.size,%d\n", frameSize);
    SDL_RWwrite(sfFile, &formatBuffer, SDL_strlen(formatBuffer), 1);
    FORMAT("frame.offset,%d\n", frameOffset);
    SDL_RWwrite(sfFile, &formatBuffer, SDL_strlen(formatBuffer), 1);
    FORMAT("frame.samples,%d\n", sfGetNumSamples(frameHardware));
    SDL_RWwrite(sfFile, &formatBuffer, SDL_strlen(formatBuffer), 1);
    // header
    FORMAT("header.magic,%08x\n", *(uint*)&frameHeader->magic.bytes[0]);
    SDL_RWwrite(sfFile, &formatBuffer, SDL_strlen(formatBuffer), 1);
    FORMAT("header.etsDelay,%d\n", *(char*)&frameHeader->etsDelay.bytes[0]);
    SDL_RWwrite(sfFile, &formatBuffer, SDL_strlen(formatBuffer), 1);
    FORMAT("header.crc,%02x\n", *(unsigned char*)&frameHeader->crc.bytes[0]);
    SDL_RWwrite(sfFile, &formatBuffer, SDL_strlen(formatBuffer), 1);
    // hardware
    int cnt = 0;
    sfHardwareWordCnt(&cnt);
    for(int j = 0; j < cnt; j++)
    {
        SArrayString256 stringId = { 0 };
        sfHardwareWordId(j, &stringId);
        ushort* ptr = (ushort*)&frameHardware->controlAddr;
        FORMAT("hardware.%s,%04x\n", (char*)stringId.bytes, ptr[j]);
        SDL_RWwrite(sfFile, &formatBuffer, SDL_strlen(formatBuffer), 1);
    }

    SDL_RWwrite(sfFile, &formatBuffer, SDL_strlen(formatBuffer), 1);
    // samples
    int sampleCount = sfGetNumSamples(frameHardware);
    if(sampleCount)
    {
        int textSampleBytes = 16;
        int textSampleCount = 64 * 1024;
        char* stringArray = (char*)pMemory->allocate(textSampleBytes * textSampleCount);
        int loop   = sampleCount / textSampleCount;
        int modulo = sampleCount % textSampleCount;
        for(int j = 0; j < loop + 1; j++)
        {
            SDL_AtomicSet(&m_progress, 100 * j / (loop + 1));
            if(SDL_AtomicGet(&m_active) == 0)
            { break; }
            int kMax = textSampleCount;
            if(j == loop) { kMax = modulo; }
            char* stringStart = (char*)stringArray;
            SDL_memset(stringStart, 0, textSampleBytes * textSampleCount);
            for(int k = 0; k < kMax; k++)
            {
                if(SDL_AtomicGet(&m_active) == 0)
                { break; }
                ishort ch0 = 0;
                ishort ch1 = 0;
                ushort dig = 0;
                ularge offset = framePos + SCOPEFUN_FRAME_HEADER + (j * textSampleCount + k) * 4;
                uint     data = *(uint*)&m_dataPtr[offset];
                sfGetData(data, &ch0, &ch1, &dig);
                    macroString(stringStart, textSampleBytes, "%d,%d,%04x\n", ch0, ch1, 0);
                stringStart += SDL_strlen(stringStart);
            }
            SDL_RWwrite(sfFile, stringArray, SDL_strlen(stringArray), 1);
        }
        pMemory->free(stringArray);
    }
    SDL_RWclose(sfFile);
    SDL_AtomicSet(&m_active, 0);
    unlock();
    return 0;
}

uint ScopeFunCaptureBuffer::load(const char* path)
{
    SDL_AtomicSet(&m_active, 1);
    lock();
    FORMAT_BUFFER();
    // .sf
    FORMAT("%s", path);
    SDL_RWops* sfFile = SDL_RWFromFile(formatBuffer, "r+b");
    Sint64 fileSize = SDL_RWsize(sfFile);
    // header
    int    textheaderBytes = FORMAT_BUFFER_SIZE;
    const int textheaderCount = 71;
    char*     lineMem = (char*)pMemory->allocate(textheaderBytes * textheaderCount);
    SDL_memset(lineMem, 0, textheaderBytes * textheaderCount);
    SDL_RWread(sfFile, lineMem, textheaderBytes * textheaderCount, 1);
    // load header line by line
    char*                             lineArray = lineMem;
    char    headerArray[textheaderCount][FORMAT_BUFFER_SIZE] = { 0 };
    ularge                           headerSize = 0;
    for(int l = 0; l < textheaderCount; l++)
    {
        for(int ch = 0; ch < FORMAT_BUFFER_SIZE; ch++)
        {
            headerSize++;
            headerArray[l][ch] = *lineArray; lineArray++;
            if(headerArray[l][ch] == '\n')
            { break; }
        }
    }
    // seek to end of header where samples start
    SDL_RWseek(sfFile, headerSize, RW_SEEK_SET);
    pMemory->free(lineMem);

    // "frame.index,%d\n"
    int frameIndex = 0;
    //SDL_sscanf(&headerArray[1][0], "frame.index,%d\n", &frameIndex);
    // get current frameIndex
    frameIndex == SDL_AtomicGet(&m_frameIndex);
    //append loaded frame to last frame
    //frameIndex++;
    SDL_AtomicSet(&m_frameIndex, frameIndex);
    // "frame.count,%d\n"
    int frameCount = 0;
    SDL_sscanf(&headerArray[2][0], "frame.count,%d\n", &frameCount);
    // "frame.size,%d\n"
    int frameSize = 0;
    SDL_sscanf(&headerArray[3][0], "frame.size,%d\n", &frameSize);
    // "frame.frameOffset,%d\n"
    int frameOffset = 0;
    SDL_sscanf(&headerArray[4][0], "frame.offset,%d\n", &frameOffset);
    // "frame.samples,%d\n"
    int frameSamples = 0;
    SDL_sscanf(&headerArray[5][0], "frame.samples,%d\n", &frameSamples);

    //SDL_AtomicSet(&m_frameIndex,   frameIndex);
    SDL_AtomicSet(&m_frameCount,   frameCount);
    SDL_AtomicSet(&m_frameSize,    frameSize);
    SDL_AtomicSet(&m_frameOffset,  frameOffset);
    SDL_AtomicSet(&m_frameSamples, frameSamples);

    // framePos, spaceLeft
    ularge  framePos = (frameIndex % frameCount) * frameSize;
    ularge spaceLeft = m_dataMax - framePos;

    // parse header
    SFrameHeader* frameHeader   = (SFrameHeader*)&m_dataPtr[framePos];
    SHardware*    frameHardware = (SHardware*)&frameHeader->hardware.bytes[0];

    // "header.magic,%c%c%c%c\n"
    uint magic = 0;
    SDL_sscanf(&headerArray[6][0], "header.magic,%08x\n", &magic);
    *(uint*)&frameHeader->magic.bytes[0] = magic;
    // "header.etsDelay,%c\n"
    int etsDelay = 0;
    SDL_sscanf(&headerArray[7][0], "header.etsDelay,%02x\n", &etsDelay);
    frameHeader->etsDelay.bytes[0] = etsDelay;
    // "header.crc,%c\n"
    int crc = 0;
    SDL_sscanf(&headerArray[8][0], "header.crc,%d\n", &crc);
    frameHeader->crc.bytes[0] = crc;
    // hardware
    int cnt = 0;
    sfHardwareWordCnt(&cnt);
    for(int i = 0; i < cnt; i++)
    {
        SArrayString256 stringId = {0};
        sfHardwareWordId(i, &stringId);
        FORMAT("hardware.%s,%%04x\n", stringId.bytes);
        uint hex = 0;
        SDL_sscanf(&headerArray[i + 9][0], formatBuffer, &hex);
        ushort* ptr = (ushort*)&frameHardware->controlAddr;
        ptr[i] = cSwap16((ushort*)&hex);
    }
    // samples
    ularge fileTextSize    = min(fileSize - headerSize, spaceLeft);
    byte*  memorySamples   = &m_dataPtr[framePos];
    memorySamples  += SCOPEFUN_FRAME_HEADER;
    char* samplesLineMem = (char*)pMemory->allocate(frameSamples*16);
    char* samplesLine = samplesLineMem;
    SDL_RWread(sfFile, samplesLineMem, frameSamples*16, 1);
    for(int i = 0; i < frameSamples; i++)
    {
        SDL_AtomicSet(&m_progress, 100*i/frameSamples);
        if(SDL_AtomicGet(&m_active) == 0)
        { break; }
        char sampleBuffer[16] = { 0 };
        for(int k = 0; k < 16; k++)
        {
            sampleBuffer[k] = samplesLine[0]; samplesLine++;
            if(sampleBuffer[k] == '\n')
            { break; }
        }
        int    ch0 = 0;
        int    ch1 = 0;
        uint   dig = 0;
        SDL_sscanf(sampleBuffer, "%d,%d,%04x\n", &ch0, &ch1, &dig);
            sfSetData(memorySamples, ch0, ch1, 0);
        memorySamples += 4;
    }
    SDL_RWclose(sfFile);
    pMemory->free(samplesLineMem);
    SDL_AtomicSet(&m_frameSize, frameSize);
    unlock();
    SDL_AtomicSet(&m_active, 0);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeFrame
//
////////////////////////////////////////////////////////////////////////////////
OsciloscopeFrame::OsciloscopeFrame()
{
    SDL_zero(*this);
}

ishort OsciloscopeFrame::getAnalogShort(uint channel, uint sample)
{
    uint count = analog[channel].getCount();
    if(!count)
    {
        return 0;
    }
    uint      idx = clamp<uint>(sample, 0, count - 1);
    ishort ivalue = analog[channel][idx];
    return ivalue;
}

float OsciloscopeFrame::getAnalog(uint channel, uint sample)
{
    uint count = analog[channel].getCount();
    if(!count)
    {
        return 0.f;
    }
    int       idx = clamp<uint>(sample, 0, count - 1);
    ishort ivalue = analog[channel][idx];
    float  fvalue = float(ivalue) / MAXOSCVALUE;
    return clamp(fvalue, -1.f, 1.f);
}

double OsciloscopeFrame::getAnalogDouble(uint channel, uint sample)
{
    uint count = analog[channel].getCount();
    if(!count)
    {
        return 0.0;
    }
    int       idx = clamp<uint>(sample, 0, count - 1);
    ishort ivalue = analog[channel][idx];
    double  fvalue = double(ivalue) / double(MAXOSCVALUE);
    return clamp(fvalue, -1.0, 1.0);
}

float rand_FloatRange(float a, float b)
{
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

void OsciloscopeFrame::generate(double dt, uint amount, double captureStart, double captureFreq)
{
    if(SDL_AtomicGet(&pOsciloscope->signalMode) != SIGNAL_MODE_SIMULATE)
    { return; }
    // attrobutes
    for(uint i = 0; i < amount; i++)
    {
        attr.pushBack(0);
    }
    // debug
    debug.clear();
    for(uint i = 0; i < 416; i++)
    {
        debug.pushBack(rand() % 256);
    }
    triggerTime = 0;
    time(&utc);
    thisFrame   = firstFrame = SDL_GetPerformanceCounter();
    captureTime = double(SDL_GetPerformanceCounter() - captureStart) / double(captureFreq);
}

////////////////////////////////////////////////////////////////
//
// time
//
////////////////////////////////////////////////////////////////
void OsciloscopeFrame::getTime(char* buffer, int size)
{
    FORMAT_BUFFER();
    struct tm* local  = localtime((time_t*)&utc);
    if(local)
    {
        ularge     offset = (this->thisFrame - this->firstFrame) + this->triggerTime;
        FORMAT("%d|%d|%d - %d|%d|%d - %llu", (local->tm_year + 1900), (local->tm_mon + 1), (local->tm_mday), (local->tm_hour + 1), (local->tm_min + 1), (local->tm_sec + 1), offset);
    }
    else
    {
        FORMAT("%s", "triggerTime zero missing!");
    }
    memcpy(buffer, formatBuffer, min(size, FORMAT_BUFFER_SIZE));
}

////////////////////////////////////////////////////////////////
//
// header  - 2464 bytes
// data    - 60000 bytes
//
////////////////////////////////////////////////////////////////
bool OsciloscopeFrame::captureHeader(byte* src, uint size, ularge captureStart, ularge captureFreq)
{
    // src
    if(!src)
    {
        return false;
    }
    // size
    if(size < CAPTURE_BUFFER_HEADER)
    {
        return false;
    }
    // debug
    debug.clear();
    for(uint i = 0; i < CAPTURE_BUFFER_HEADER; i++)
    {
        debug.pushBack(src[i]);
    }
    // magic DDDD
    ushort header = *(ushort*)src;
    if(header != 56797)
    {
        return false;
    }
    // ets
    int index = clamp<int>(pOsciloscope->settings.getHardware()->fpgaEtsIndex, 0, debug.getCount() - 1);
    ets = debug[index];
    // trigger
    triggerTime = *(ularge*)(src + 2);
    if(triggerTime == 0)
    {
        firstFrame = SDL_GetPerformanceCounter();
        utc        = ::time(0);
    }
    thisFrame = SDL_GetPerformanceCounter();
    // captureTime
    captureTime = double(thisFrame - captureStart) / double(captureFreq);
    return true;
}

int OsciloscopeFrame::isFull()
{
    int numSamples = int(NUM_SAMPLES); //pOsciloscope->control.frameSize;
    if(analog[0].getCount() >= numSamples)
    {
        return 1;
    }
    if(analog[1].getCount() >= numSamples)
    {
        return 1;
    }
    return 0;
}

void OsciloscopeFrame::clear()
{
    analog[0].clear();
    analog[1].clear();
    attr.clear();
}

///////////////////////////////////////////////////////////////////////////
//
// parser
//
///////////////////////////////////////////////////////////////////////////

double OscToken::evaluate(double par1, double par2)
{
    switch(type)
    {
        case tAdd:
            return par1 + par2;
        case tSub:
            return par1 - par2;
        case tMul:
            return par1 * par2;
        case tDiv:
            return par1 / par2;
        case tMod:
            return (double)((int)par1 % max<int>(1, (int)par2));
        case tMin:
            return min<double>(par1, par2);
        case tMax:
            return max<double>(par1, par2);
    };
    return 0;
}

double OscToken::evaluate(double par)
{
    switch(type)
    {
        case tSin:
            return sin(par);
        case tCos:
            return cos(par);
    };
    return 0;
}

void OsciloscopeFunction::parse()
{
    int precedence[tLast] =
    {
        2, // tAdd
        2, // tSub
        3, // tMul
        3, // tDiv
        3, // tMod
        3, // tMin
        3, // tMax
        3, // tSin
        3, // tCos
        1, // tCh0
        1, // tCh1
        1, // tNumber
        1, // tDouble
        1, // tLeft
        1, // tRight
        0, // tSeparator
    };
    postfix.setCount(0);
    Array<OscToken, 128> stack;
    for(int i = 0; i < tokens.getCount(); i++)
    {
        OscToken token = tokens[i];
        switch(token.type)
        {
            case tSeperator:
                break;
            case tCh0:
            case tCh1:
            case tNumber:
            case tDouble:
                {
                    postfix.pushBack(token);
                }
                break;
            case tLeft:
                {
                    stack.pushBack(token);
                }
                break;
            case tRight:
                {
                    OscToken top = stack.last();
                    stack.popBack();
                    while(top.type != tLeft)
                    {
                        postfix.pushBack(top);
                        top = stack.last();
                        stack.popBack();
                    }
                }
                break;
            default:
                {
                    while(stack.getCount() && precedence[stack.last().type] >= precedence[token.type])
                    {
                        postfix.pushBack(stack.last());
                        stack.popBack();
                    }
                    stack.pushBack(token);
                }
                break;
        };
    };
    while(stack.getCount())
    {
        postfix.pushBack(stack.last());
        stack.popBack();
    }
}

void OsciloscopeFunction::tokenize(String in)
{
    String input;
    int len = 0;
    for(int i = 0; i < in.getLength(); i++)
    {
        if(in[i] == '0' ||
           in[i] == '1' ||
           in[i] == '2' ||
           in[i] == '3' ||
           in[i] == '4' ||
           in[i] == '5' ||
           in[i] == '6' ||
           in[i] == '7' ||
           in[i] == '8' ||
           in[i] == '9' ||
           in[i] == '+' ||
           in[i] == '-' ||
           in[i] == '*' ||
           in[i] == '/' ||
           in[i] == '%' ||
           in[i] == ' ' ||
           in[i] == ',' ||
           in[i] == '.' ||
           in[i] == 'm' ||
           in[i] == 'i' ||
           in[i] == 'n' ||
           in[i] == 'm' ||
           in[i] == 'a' ||
           in[i] == 'x' ||
           in[i] == 's' ||
           in[i] == 'i' ||
           in[i] == 'n' ||
           in[i] == 'c' ||
           in[i] == 'o' ||
           in[i] == 's' ||
           in[i] == '(' ||
           in[i] == ')' ||
           in[i] == 'c' ||
           in[i] == 'h' ||
           in[i] == '0' ||
           in[i] == 'c' ||
           in[i] == 'h' ||
           in[i] == '1')
        {
            input.setLength(i + 1);
            input[i] = in[i];
        }
    }
    tokens.setCount(0);
    // tokenize
    OscToken token;
    String   number;
    int     i = 0;
    while(i < input.getLength())
    {
        // number
        {
            number.setLength(0);
            token.type = tLast;
            while(i < input.getLength() && (input[i] == '0' ||
                                            input[i] == '1' ||
                                            input[i] == '2' ||
                                            input[i] == '3' ||
                                            input[i] == '4' ||
                                            input[i] == '5' ||
                                            input[i] == '6' ||
                                            input[i] == '7' ||
                                            input[i] == '8' ||
                                            input[i] == '9' ||
                                            input[i] == '.'))
            {
                if(input[i] == '.')
                {
                    token.type = tDouble;
                }
                else if(token.type == tLast)
                {
                    token.type = tNumber;
                }
                number.insert(number.getLength(), input[i]);
                i++;
            }
            if(token.type == tNumber)
            {
                token.value = atoi(number.asChar());
                tokens.pushBack(token);
                continue;
            }
            if(token.type == tDouble)
            {
                token.value = atof(number.asChar());
                tokens.pushBack(token);
                continue;
            }
        }
        // operators
        if(input[i] == '+')
        {
            token.type = tAdd;
            tokens.pushBack(token);
            i++;
            continue;
        }
        if(input[i] == '-')
        {
            token.type = tSub;
            tokens.pushBack(token);
            i++;
            continue;
        }
        if(input[i] == '*')
        {
            token.type = tMul;
            tokens.pushBack(token);
            i++;
            continue;
        }
        if(input[i] == '/')
        {
            token.type = tDiv;
            tokens.pushBack(token);
            i++;
            continue;
        }
        if(input[i] == '%')
        {
            token.type = tMod;
            tokens.pushBack(token);
            i++;
            continue;
        }
        // brace
        if(input[i] == '(')
        {
            token.type = tLeft;
            tokens.pushBack(token);
            i++;
            continue;
        }
        if(input[i] == ')')
        {
            token.type = tRight;
            tokens.pushBack(token);
            i++;
            continue;
        }
        // separator
        if(input[i] == ',' || input[i] == ' ')
        {
            token.type = tSeperator;
            tokens.pushBack(token);
            i++;
            continue;
        }
        // functions
        if(i + 2 < input.getLength())
        {
            int pos = 0;
            // functions
            pos = input.pos("sin", i);
            if(pos == i)
            {
                token.type = tSin;
                tokens.pushBack(token);
                i += 3;
                continue;
            }
            pos = input.pos("cos", i);
            if(pos == i)
            {
                token.type = tCos;
                tokens.pushBack(token);
                i += 3;
                continue;
            }
            pos = input.pos("min", i);
            if(pos == i)
            {
                token.type = tMin;
                tokens.pushBack(token);
                i += 3;
                continue;
            }
            pos = input.pos("max", i);
            if(pos == i)
            {
                token.type = tMax;
                tokens.pushBack(token);
                i += 3;
                continue;
            }
            // channel
            pos = input.pos("ch0", i);
            if(pos == i)
            {
                token.type = tCh0;
                tokens.pushBack(token);
                i += 3;
                continue;
            }
            pos = input.pos("ch1", i);
            if(pos == i)
            {
                token.type = tCh1;
                tokens.pushBack(token);
                i += 3;
                continue;
            }
        }
        // safety, just in case
        i++;
    }
}

double OsciloscopeFunction::evaluate(double ch0, double ch1)
{
    for(int i = 0; i < postfix.getCount(); i++)
    {
        if(postfix[i].type == tCh0)
        {
            postfix[i].value = ch0;
        }
        if(postfix[i].type == tCh1)
        {
            postfix[i].value = ch1;
        }
    }
    stack.setCount(0);
    for(int i = 0; i < postfix.getCount(); i++)
    {
        OscToken token = postfix[i];
        switch(token.type)
        {
            case tCh0:
            case tCh1:
            case tNumber:
            case tDouble:
                stack.pushBack(token.value);
                break;
            case tSin:
            case tCos:
                {
                    double result = 0;
                    if(stack.getCount() >= 1)
                    {
                        double    par = stack.last();
                        stack.popBack();
                        result = token.evaluate(par);
                    }
                    stack.pushBack(result);
                }
                break;
            default:
                {
                    double result = 0;
                    if(stack.getCount() >= 2)
                    {
                        double   par1 = stack.last();
                        stack.popBack();
                        double   par2 = stack.last();
                        stack.popBack();
                        result = token.evaluate(par1, par2);
                    }
                    stack.pushBack(result);
                }
        };
    }
    return stack.last();
}

/* TODO: erase
extern "C"
{
//#include "api/scopefunapi_wrap_lua.c"
}

int LuaOnPrint(lua_State* L)
{
    if(!L) { return 0; }
    const char* str = lua_tostring(L, -1);
    lua_getglobal(L, "ScriptPointer");
    OsciloscopeScript* pScript = (OsciloscopeScript*)lua_touserdata(L, -1);
    pScript->LuaPrint(str);
    lua_pop(L, 2);
    return 0;
}


int LuaOnPrint(lua_State* L,const char* msg)
{
   if (!L) { return 0; }
   const char* str = lua_tostring(L, -1);
   lua_getglobal(L, "ScriptPointer");
   OsciloscopeScript* pScript = (OsciloscopeScript*)lua_touserdata(L, -1);
   pScript->LuaPrint(msg);
   lua_pop(L, 1);
   return 0;
}


int LuaOnError(lua_State* L)
{
    if(!L) { return 0; }
    const char* str = lua_tostring(L, -1);
    lua_getglobal(L, "ScriptPointer");
    OsciloscopeScript* pScript = (OsciloscopeScript*)lua_touserdata(L, -1);
    pScript->LuaError(str);
    return 0;
}


int LuaOnFrame(lua_State* L, SFrameData* data, int len, float* pos, float* zoom, void* user)
{
    if(!L) { return 0; }

    try
    {
       // push function
       if (lua_getglobal(L, "onFrame") == LUA_TFUNCTION)
       {
          // push parameters
          SWIG_Lua_NewPointerObj(L, data, SWIGTYPE_p_SFrameData, 0);
          lua_pushinteger(L, len);
          // execute
          if (lua_pcall(L, 2, 2, 0) != LUA_OK)
          {
             LuaOnError(L);
          }
          else
          {
             // pop parameters
             SWIG_Lua_ConvertPtr(L, -2, (void**)&data, SWIGTYPE_p_SFrameData, 0);
             lua_pop(L, 1);
             // cleanup
             lua_pop(L, 1);
          }
       }
       else
       {
          lua_pop(L, 1);
          LuaOnPrint(L, "function is missing: onFrame(data,len)");
       }
    }
    catch (std::exception msg)
    {
       LuaOnPrint(L, msg.what());
    }
    lua_gc(L, LUA_GCCOLLECT, 0);
    return 0;
}

int LuaOnSample(lua_State* L, int sample, ishort* ch0, ishort* ch1, ishort* fun, ushort* dig, float* pos, float* zoom, void* user)
{
    if(!L) { return 0; }

    try
    {
       // push function
       if (lua_getglobal(L, "onSample") == LUA_TFUNCTION)
       {
          // push parameters
          lua_pushinteger(L, sample);
          lua_pushinteger(L, *ch0);
          lua_pushinteger(L, *ch1);
          lua_pushinteger(L, *fun);
          lua_pushinteger(L, *dig);
          // execute
          if (lua_pcall(L, 5, 5, 0) != LUA_OK)
          {
             LuaOnError(L);
          }
          else
          {
             // pop parameters
             sample = lua_tointeger(L, -5);
             *ch0 = lua_tointeger(L, -4);
             *ch1 = lua_tointeger(L, -3);
             *fun = lua_tointeger(L, -2);
             *dig = lua_tointeger(L, -1);
             lua_pop(L, 5);
          }
       }
       else
       {
          lua_pop(L, 1);
          LuaOnPrint(L, "function is missing: onSample(ch0,ch1,fun,dig)");
       }
    }
    catch (std::exception msg)
    {
       LuaOnPrint(L, msg.what());
    }
    return 0;
}

int LuaOnDisplay(lua_State* L, SDisplay* data, float* pos, float* zoom, void* user)
{
    if(!L) { return 0; }

    try
    {
       // push function
       if (lua_getglobal(L, "onDisplay") == LUA_TFUNCTION)
       {
          // push parameters
          SWIG_Lua_NewPointerObj(L, data, SWIGTYPE_p_SDisplay, 0);
          // execute
          if (lua_pcall(L, 1, 1, 0) != LUA_OK)
          {
             LuaOnError(L);
          }
          else
          {
             // pop parameters
             SWIG_Lua_ConvertPtr(L, -1, (void**)&data, SWIGTYPE_p_SDisplay, 0);
             lua_pop(L, 1);
          }
       }
       else
       {
          lua_pop(L, 1);
          LuaOnPrint(L, "function is missing: onDisplay(data)");
       }
    }
    catch (std::exception msg)
    {
       LuaOnPrint(L, msg.what());
    }
    lua_gc(L, LUA_GCCOLLECT, 0);
    return 0;
}

int LuaOnConfigure(lua_State* L, SHardware* hw)
{
    if(!L) { return 0; }

    try
    {
       // push function
       if (lua_getglobal(L, "onConfigure") == LUA_TFUNCTION)
       {
          // push parameters
          SWIG_Lua_NewPointerObj(L, hw, SWIGTYPE_p_SHardware, 0);
          // execute
          if (lua_pcall(L, 1, 1, 0) != LUA_OK)
          {
             LuaOnError(L);
          }
          else
          {
             // pop parameters
             SWIG_Lua_ConvertPtr(L, -1, (void**)&hw, SWIGTYPE_p_SHardware, 0);
             lua_pop(L, 1);
          }
       }
       else
       {
          lua_pop(L, 1);
          LuaOnPrint(L, "function is missing: onConfigure(hw)");
       }
    }
    catch (std::exception msg)
    {
       LuaOnPrint(L, msg.what());
    }
    lua_gc(L, LUA_GCCOLLECT, 0);
    return 0;
}

int LuaOnInit(lua_State* L, SFContext* ctx)
{
    if(!L) { return 0; }

    try
    {
       // push function
       if (lua_getglobal(L, "onInit") == LUA_TFUNCTION)
       {
          // push parameters
          SWIG_Lua_NewPointerObj(L, ctx, SWIGTYPE_p_SFContext, 0);
          // execute
          if (lua_pcall(L, 1, 1, 0) != LUA_OK)
          {
             LuaOnError(L);
          }
          else
          {
             // pop parameters
             SWIG_Lua_ConvertPtr(L, -1, (void**)&ctx, SWIGTYPE_p_SFContext, 0);
             lua_pop(L, 1);
          }
       }
       else
       {
          lua_pop(L, 1);
          LuaOnPrint(L, "function is missing: onInit(ctx)");
       }
    }
    catch (std::exception msg)
    {
       LuaOnPrint(L, msg.what() );
    }
    lua_gc(L, LUA_GCCOLLECT, 0);
    return 0;
}

int LuaOnFunction(lua_State* L, ishort ch0, ishort ch1, ishort* fun)
{
   if (!L) { return 0; }

   try
   {
      // push function
      if (lua_getglobal(L, "onFunction") == LUA_TFUNCTION)
      {
         // push parameters
         lua_pushinteger(L, ch0);
         lua_pushinteger(L, ch1);

         // execute
         if (lua_pcall(L, 2, 1, 0) != LUA_OK)
         {
            LuaOnError(L);
         }
         else
         {
            // pop parameters
            int ret = lua_tointeger(L, -1);
            lua_pop(L, 1);

            *fun = ret;
         }
      }
      else
      {
         lua_pop(L, 1);
         LuaOnPrint(L, "function is missing: onFunction(ch0,ch1)");
      }
   }
   catch (std::exception msg)
   {
      LuaOnPrint(L, msg.what());
   }
   return 0;
}

int LuaOnUpload(lua_State* L, SGenerator* gen, uint* sampleCount)
{
   if (!L) { return 0; }

   try
   {
      // push function
      if (lua_getglobal(L, "onUpload") == LUA_TFUNCTION)
      {
         // push parameters
         SWIG_Lua_NewPointerObj(L, gen, SWIGTYPE_p_SGenerator, 0);

         // execute
         if (lua_pcall(L, 1, 2, 0) != LUA_OK)
         {
            LuaOnError(L);
         }
         else
         {
            // pop parameters
            SWIG_Lua_ConvertPtr(L, -2, (void**)&gen, SWIGTYPE_p_SGenerator, 0);

            for (int i = 0; i < SCOPEFUN_GENERATOR; i++)
            {
               gen->digital.bytes[i] = endianSwap16(gen->digital.bytes[i]);
            }

            *sampleCount = lua_tointeger(L, -1);
            lua_pop(L, 2);
         }
      }
      else
      {
         lua_pop(L, 1);
         LuaOnPrint(L, "function is missing: onUpload(gen)");
      }
   }
   catch (std::exception msg)
   {
      LuaOnPrint(L, msg.what());
   }
   lua_gc(L, LUA_GCCOLLECT, 0);
   return 0;
}

OsciloscopeScript::OsciloscopeScript(int index)
{
    m_arrayIdx = index;
    m_spinLock = 0;
    m_userData = 0;
    m_luaState = 0;
    SDL_AtomicSet(&m_active, 0);
    m_luaScript = 0;
}
int OsciloscopeScript::OnFrame(SFrameData* data, int len, float* pos, float* zoom, void* user)
{
    if (!SDL_AtomicGet(&m_active))
      return 0;
    int ret = 0;
    SDL_AtomicLock(&m_spinLock);
    ret = LuaOnFrame(m_luaState, data, len, pos, zoom, user);
    SDL_AtomicUnlock(&m_spinLock);
    return ret;
}
int OsciloscopeScript::OnSample(int sample, ishort* ch0, ishort* ch1, ishort* fun, ushort* dig, float* pos, float* zoom, void* user)
{
   if (!SDL_AtomicGet(&m_active))
      return 0;
    int ret = 0;
    SDL_AtomicLock(&m_spinLock);
    ret = LuaOnSample(m_luaState, sample, ch0, ch1, fun, dig, pos, zoom, user);
    SDL_AtomicUnlock(&m_spinLock);
    return ret;
}
int OsciloscopeScript::OnDisplay(SDisplay* data, float* pos, float* zoom, void* user)
{
   if (!SDL_AtomicGet(&m_active))
      return 0;
    int ret = 0;
    SDL_AtomicLock(&m_spinLock);
    ret = LuaOnDisplay(m_luaState, data, pos, zoom, user);
    SDL_AtomicUnlock(&m_spinLock);
    return ret;
}

int OsciloscopeScript::OnFunction(ishort ch0, ishort ch1, ishort* fun)
{
   if (!SDL_AtomicGet(&m_active))
      return 0;
   int ret = 0;
   SDL_AtomicLock(&m_spinLock);
   ret = LuaOnFunction(m_luaState, ch0, ch1, fun);
   SDL_AtomicUnlock(&m_spinLock);
   return ret;
}

int OsciloscopeScript::OnConfigure(SHardware* hw)
{
   if (!SDL_AtomicGet(&m_active))
      return 0;
   int ret = 0;
   if (SDL_AtomicTryLock(&m_spinLock) == SDL_TRUE)
   {
      ret = LuaOnConfigure(m_luaState, hw);
      SDL_AtomicUnlock(&m_spinLock);
   }
   return ret;
}

int OsciloscopeScript::OnInit(SFContext* ctx)
{
   if (!SDL_AtomicGet(&m_active))
      return 0;
    int ret = 0;
    SDL_AtomicLock(&m_spinLock);
    ret = LuaOnInit(m_luaState, ctx);
    SDL_AtomicUnlock(&m_spinLock);
    return ret;
}

int OsciloscopeScript::OnUpload(SGenerator* gen, uint* sampleCount)
{
   if (!SDL_AtomicGet(&m_active))
      return 0;
   int ret = 0;
   SDL_AtomicLock(&m_spinLock);
   ret = LuaOnUpload(m_luaState, gen, sampleCount);
   SDL_AtomicUnlock(&m_spinLock);
   return ret;
}

int OsciloscopeScript::LuaPrint(const char* str)
{
    if (!str)
      return 0;

    m_msg(str, m_userData);
    return 0;
}


int OsciloscopeScript::LuaError(const char* str)
{
    if (!str)
      return 0;

    m_msg(str, m_userData);
    lua_close(m_luaState);
    m_luaState = 0;
    return 0;
}

int OsciloscopeScript::Load(String fileName, LuaOnMsg msg)
{
    m_fileName = fileName;
    m_msg = msg;
    return 0;
}

int callInit(void* script);

int OsciloscopeScript::Run()
{
    if(m_luaState)
    { return 1; }

    int ret = 0;
    SDL_AtomicLock(&m_spinLock);
    try
    {
       const char* redirect = "\r\n"
          "print_stdout = print\r\n"
          "\r\n"
          "print = function(...)\r\n"
          "  local arg = { ... }\r\n"
          "  for i, v in ipairs(arg) do\r\n"
          "    LuaPrint(v)\r\n"
          "  end\r\n"
          "end\r\n";
       m_luaState = luaL_newstate();
       luaopen_base(m_luaState);
       luaL_openlibs(m_luaState);
       luaopen_ScopeFun(m_luaState);
       lua_pushlightuserdata(m_luaState, this);
       lua_setglobal(m_luaState, "ScriptPointer");
       lua_register(m_luaState, "LuaPrint", LuaOnPrint);
       luaL_dostring(m_luaState, redirect);
       ret = luaL_dofile(m_luaState, m_fileName.asChar());
       if (ret == LUA_OK)
       {
          LuaPrint("script loaded: ");
          LuaPrint(m_fileName.asChar());
          LuaPrint("\n");
       }
       else
       {
          LuaOnError(m_luaState);
       }
    }
    catch (std::exception msg)
    {
       LuaOnPrint(m_luaState, msg.what());
    }
    SDL_AtomicUnlock(&m_spinLock);

    // active
    Active(1);

    // Init
    callInit(this);
    return ret;
}

int OsciloscopeScript::Reload()
{
    ilarge size = 0;
    if(m_luaScript)
       pMemory->free(m_luaScript);
    fileLoadString(m_fileName.asChar(), (char**)&m_luaScript, &size);
    return 0;
}

int OsciloscopeScript::Stop()
{
    if(!m_luaState) { return 1; }
    Active(0);
    SDL_AtomicLock(&m_spinLock);
    lua_close(m_luaState);
    m_luaState = 0;
    SDL_AtomicUnlock(&m_spinLock);
    return 0;
}

int OsciloscopeScript::Active(int onoff)
{
   SDL_AtomicSet(&m_active, onoff);
   return 0;
}

void OsciloscopeScript::SetUserData(void* user)
{
    m_userData = user;
}

void* OsciloscopeScript::GetUserData()
{
    return m_userData;
}

int OsciloscopeScript::GetArrayIdx()
{
    return m_arrayIdx;
}

int callFrame(SFrameData* data, int len, float* pos, float* zoom, void* user)
{
    for(int i = 0; i < pOsciloscope->m_callback.Count(); i++)
    {
       pOsciloscope->m_callback.Get(i)->OnFrame(data, len, pos, zoom, user);
    }
    return 0;
}

int callDisplay(SDisplay* data, float* pos, float* zoom, void* user)
{
   for (int i = 0; i < pOsciloscope->m_callback.Count(); i++)
   {
      pOsciloscope->m_callback.Get(i)->OnDisplay(data, pos, zoom, user);
   }
   return 0;
}

int callSample(int sample, ishort* ch0, ishort* ch1, ishort* fun, ushort* dig, float* pos, float* zoom, void* user)
{
    for(int i = 0; i < pOsciloscope->m_callback.Count(); i++)
    {
       pOsciloscope->m_callback.Get(i)->OnSample(sample, ch0, ch1, fun, dig, pos, zoom, user);
    }
    return 0;
}

int callFunction(ishort ch0, ishort ch1, ishort* fun)
{
   for (int i = 0; i < pOsciloscope->m_callback.Count(); i++)
   {
      pOsciloscope->m_callback.Get(i)->OnFunction(ch0, ch1, fun);
   }
   return 0;
}

int callUpload(SGenerator* gen, uint* sampelCount)
{
   for (int i = 0; i < pOsciloscope->m_callback.Count(); i++)
   {
      pOsciloscope->m_callback.Get(i)->OnUpload(gen, sampelCount);
   }
   return 0;
}

int callConfigure(SHardware* hw)
{
   for (int i = 0; i < pOsciloscope->m_callback.Count(); i++)
   {
      pOsciloscope->m_callback.Get(i)->OnConfigure((SHardware*)hw);
   }
   return 0;
}

int callInit(void* data)
{
   OsciloscopeScript* script = (OsciloscopeScript*)data;
   script->OnInit(getCtx());
   return 0;
}

OsciloscopeCallback::OsciloscopeCallback()
{
    //m_callback.onInit      = callInit;
    //m_callback.onFrame     = callFrame;
    //m_callback.onSample    = callSample;
    //m_callback.onDisplay   = callDisplay;
    m_callback.onConfigure = callConfigure;
    m_callback.onFunction  = callFunction;
    m_callback.onUpload    = callUpload;
}

int OsciloscopeCallback::SetHelp(String fileName)
{
   ilarge size = 0;
   fileLoadString(fileName.asChar(),(char**)&m_help, &size);
   return 0;
}
const char* OsciloscopeCallback::GetHelp()
{
   return (const char*)m_help;
}
int OsciloscopeCallback::Add(String fileName, LuaOnMsg msg)
{
    if(m_script.getCount() < SCOPEFUN_MAX_SCRIPT)
    {
        OsciloscopeScript* script = new OsciloscopeScript(m_script.getCount());
        script->Load(fileName,msg);
        script->Reload();
        m_script.pushBack(script);
        return 0;
    }
    return 1;
}

int OsciloscopeCallback::Clear()
{
    for(int i = 0; i < m_script.getCount(); i++)
    { delete m_script[i]; }
    m_script.clear();
    return 0;
}

int OsciloscopeCallback::Count()
{
    return m_script.getCount();
}

OsciloscopeScript* OsciloscopeCallback::Get(int i)
{
    if(m_script.getCount() > i)
    { return m_script[i]; }
    return 0;
}

SCallback* OsciloscopeCallback::Ptr()
{
    return &m_callback;
}
*/

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
