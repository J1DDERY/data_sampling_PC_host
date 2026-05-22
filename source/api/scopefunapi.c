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
#include <core/purec/pureusb.h>
#include <core/purec/purec.h>
#include <core/purec/puresocket.h>
#include <api/scopefunapi.h>
#include <SDL.h>
#include <version/sfversion.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <locale.h>

/*--------------------------------------------------------------------------------
   helper
--------------------------------------------------------------------------------*/

#define PI                 3.14159265358979323846f
#define NUM_SAMPLES        10000
#define NUM_SAMPLESF       10000.f
#define MAXOSCVALUE        8191.f

float fClamp(float a, float min, float max)
{
    if(a < min)
    {
        return min;
    }
    if(a > max)
    {
        return max;
    }
    return a;
}


double dClamp(double a, double min, double max)
{
    if(a < min)
    {
        return min;
    }
    if(a > max)
    {
        return max;
    }
    return a;
}

ularge lClamp(ularge a, ularge min, ularge max)
{
    if(a < min)
    {
        return min;
    }
    if(a > max)
    {
        return max;
    }
    return a;
}

ilarge ilClamp(ilarge a, ilarge min, ilarge max)
{
    if(a < min)
    {
        return min;
    }
    if(a > max)
    {
        return max;
    }
    return a;
}

int iClamp(int a, int min, int max)
{
    if(a < min)
    {
        return min;
    }
    if(a > max)
    {
        return max;
    }
    return a;
}

uint uMin(uint a, uint b)
{
    if(a < b) { return a; }
    else       { return b; }
}

uint uMax(uint a, uint b)
{
    if(a > b) { return a; }
    else       { return b; }
}


int iMin(int a, int b)
{
    if(a < b) { return a; }
    else       { return b; }
}

int iMax(int a, int b)
{
    if(a > b) { return a; }
    else       { return b; }
}


ularge lMin(ularge a, ularge b)
{
    if(a < b) { return a; }
    else       { return b; }
}

ularge lMax(ularge a, ularge b)
{
    if(a > b) { return a; }
    else       { return b; }
}

float rand_FloatRange(float a, float b)
{
    return (float)((b - a) * ((float)rand() / RAND_MAX)) + a;
}

ishort leadBitShift(ushort value)
{
    ishort isLeadBit = value & 0x200;
    if(isLeadBit)
    {
        return (value | 0xFE00);
        // return (~(value&(~0x200))) + 1;
    }
    return value;
}


ishort leadBitCompl(ushort value)
{
    ishort isLeadBit = value & 0x200;
    if(isLeadBit)
    {
        return (~(value & (~0x200))) + 1;
    }
    return value;
}

ushort raiseFlag16(ushort attr, ushort bits)
{
    return attr | bits;
}
ushort lowerFlag16(ushort attr, ushort bits)
{
    return attr & ~bits;
}

void togleFlag16(ushort attr, ushort bits)
{
    attr ^= bits;
}

ushort isFlag16(ushort attr, ushort bits)
{
    return attr & bits;
}
ushort bitFlag16(ushort attr, ushort bits, ushort value)
{
    attr ^= ((~value + 1) ^ attr) & bits;
    return attr;
}

uint apiMin(uint a, uint b)
{
    if(a < b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

void apiLock(SFContext* ctx)
{
    if(SDL_AtomicGet((SDL_atomic_t*)&ctx->api.thread) > 0)
    {
        SDL_AtomicLock(&ctx->api.lock);
    }
    else
    {
        int debug = 1;
    }
}

void apiUnlock(SFContext* ctx)
{
    if(SDL_AtomicGet((SDL_atomic_t*)&ctx->api.thread) > 0)
    {
        SDL_AtomicUnlock(&ctx->api.lock);
    }
    else
    {
        int debug = 1;
    }
}

unsigned short endianByteSwap16(unsigned short value)
{
    unsigned short low  = (value & 0x00FF);
    unsigned short high = (value & 0xFF00);
    return (low << 8) | (high >> 8);
}

uint apiFrameSize(int version, char* headerPtr)
{
    SFrameHeader* header = (SFrameHeader*)headerPtr;
    uint sampleSize = ((uint)header->hardware.bytes[32 + 0] << 24) | ((uint)header->hardware.bytes[32 + 1] << 16) | ((uint)header->hardware.bytes[32 + 2] << 8) | ((uint)header->hardware.bytes[32 + 3] << 0);
    uint frameSize  = sampleSize * 4;
    if(frameSize % 1024 != 0)
    {
        frameSize = ((frameSize / 1024) + 1) * 1024;
    }
    frameSize += SCOPEFUN_FRAME_HEADER;
    return frameSize;
}

int apiResult(int ret)
{
    if(ret == 0)
    {
        return SCOPEFUN_SUCCESS;
    }
    return SCOPEFUN_FAILURE;
}

/*--------------------------------------------------------------------
   structures
---------------------------------------------------------------------*/

#define SCOPEFUN_CREATE_DELETE(name) \
    name* sfCreate##name()           { name* ptr = malloc(sizeof(name)); SDL_zerop(ptr); return ptr; } \
    void  sfDelete##name(name* ptr)  { free(ptr); }

SCOPEFUN_CREATE_DELETE(SFloat)
SCOPEFUN_CREATE_DELETE(SInt)
SCOPEFUN_CREATE_DELETE(SFContext)
SCOPEFUN_CREATE_DELETE(SDisplay)
SCOPEFUN_CREATE_DELETE(SSimulate)
SCOPEFUN_CREATE_DELETE(SUsb)
SCOPEFUN_CREATE_DELETE(SHardware)
SCOPEFUN_CREATE_DELETE(SFx3)
SCOPEFUN_CREATE_DELETE(SFpga)
SCOPEFUN_CREATE_DELETE(SGeneratorConfig)
SCOPEFUN_CREATE_DELETE(SGeneratorData)
SCOPEFUN_CREATE_DELETE(SEeprom)
SCOPEFUN_CREATE_DELETE(SFrameData)
SCOPEFUN_CREATE_DELETE(SFrameHeader)


/*--------------------------------------------------------------------
   Initialization
---------------------------------------------------------------------*/
int sfApiInit()
{
    usbFxxInit(0);
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfApiCreateContext(SFContext* ctx, int memory)
{
    cMemSet((char*)ctx, 0, sizeof(SFContext));
    apiLock(ctx);
    // frame
    ctx->frame.maxMemory = memory;
    cMemSet((char*)ctx->frame.data.data.bytes, 0, ctx->frame.maxMemory);
    // usb
    ctx->usb = cMalloc(sizeof(struct UsbContext));
    struct UsbContext* pCtx = (UsbContext*)ctx->usb;
    cMemSet((char*)pCtx, 0, sizeof(struct UsbContext));
    // version
    ctx->api.version = atoi(EXE_VERSION_MAJOR);
    ctx->api.major   = atoi(EXE_VERSION_MINOR);
    ctx->api.minor   = atoi(EXE_VERSION_MICRO);
    // callback
    ctx->pCallback = 0;
    // function
    ctx->functionType = dfMedium;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfApiDeleteContext(SFContext* ctx)
{
    // usb
    cFree((char*)ctx->usb);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfApiVersion(SFContext* ctx, SInt* version, SInt* major, SInt* minor)
{
    apiLock(ctx);
    version->value = ctx->api.version;
    major->value   = ctx->api.major;
    minor->value   = ctx->api.minor;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfApiExit()
{
    socketExit();
    usbFxxExit(0);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfIsActive(SFContext* ctx)
{
    int active = 0;
    apiLock(ctx);
    active = ctx->api.active;
    apiUnlock(ctx);
    return active;
}

SCOPEFUN_API int sfIsThreadSafe(SFContext* ctx)
{
    int ts = 0;
    apiLock(ctx);
    ts = SDL_AtomicGet((SDL_atomic_t*)&ctx->api.thread);
    apiUnlock(ctx);
    return ts;
}

SCOPEFUN_API int sfSetThreadSafe(SFContext* ctx, int threadSafe)
{
    while(SDL_AtomicCAS((SDL_atomic_t*)&ctx->api.thread, 0, threadSafe) == SDL_FALSE)
    {
        cSleep(1);
    }
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetActive(SFContext* ctx, int active)
{
    apiLock(ctx);
    ctx->api.active = active;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetTimeOut(SFContext* ctx, int timeout)
{
    apiLock(ctx);
    ctx->api.timeout = timeout;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfGetTimeOut(SFContext* ctx, SInt* timeout)
{
    apiLock(ctx);
    timeout->value = ctx->api.timeout;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------
    hardware
---------------------------------------------------------------------*/
SCOPEFUN_API int sfHardwareOpen(SFContext* ctx, SUsb* cfg, int version)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        UsbGuid id;
        cMemCpy((char*)&id, (char*)&cfg->guid, sizeof(struct UsbGuid));
        usbDevice* deviceList[4] = { 0 };
        usbFxxGuidVidPid(pUsbCtx, id, cfg->idVendor, cfg->idProduct, cfg->idSerial);
        int ret = PUREUSB_SUCCESS;
        ret += usbFxxFreeList(pUsbCtx);
        ret += usbFxxFindList(pUsbCtx, deviceList, 4);
        ret += usbFxxOpenNormal(pUsbCtx, deviceList, 4);
        result = apiResult(ret);
        pUsbCtx->version = version;
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfHardwareReset(SFContext* ctx)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int ret = PUREUSB_FAILURE;
        ret = usbFx3Reset(pUsbCtx);
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfHardwareIsOpened(SFContext* ctx, SInt* open)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        open->value = usbFxxIsConnected(pUsbCtx);
        result = SCOPEFUN_SUCCESS;
    }
    if(SDL_AtomicGet((SDL_atomic_t*)&ctx->simulateOn) > 0)
    {
        open->value = 1;
        result = SCOPEFUN_SUCCESS;
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int sfHardwareConfig(SFContext* ctx, SHardware* hw)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int swap = 1;
        int transfered = 0;
        int ret = usbFxxTransferDataOut(pUsbCtx, 2, (char*)hw, sizeof(SHardware), swap, ctx->api.timeout, &transfered);
        result = apiResult(ret);
        if(ctx->pCallback)
        { ((SCallback*)ctx->pCallback)->onConfigure(hw); }
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfHardwareCapture(SFContext* ctx, SFrameData* buffer, int len, int offset, SInt* received)
{
    int debug = 0;
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    uint size = apiMin(len, SCOPEFUN_FRAME_MEMORY);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int swap = 0;
        if (debug) printf("C++ inside sfHardwareCapture, usbFxxTransferDataIn, requested %i Bytes\n", size);
        int ret = usbFxxTransferDataIn(pUsbCtx, 6, (byte*)&buffer->data.bytes[offset], size, swap, ctx->api.timeout, (int*)received);
        if (debug) {
            printf("                   usbFxxTransferDataIn ret= %i, received  %i Bytes\n", ret, received->value);
            //const char* errcode = libusb_error_name(ret);
            //logerror(errcode);
        }
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfHardwareUploadFx3(SFContext* ctx, SFx3* fx3)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int ret = usbFx3UploadFirmwareToFx3(pUsbCtx, fx3->data.bytes);
        result = apiResult(ret);
        //cSleep(3000);
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfHardwareUploadFpga(SFContext* ctx, SFpga* fpga)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int ret = 0;
        char id[EEPROM_BYTE_COUNT] = { 0 };
        ret += usbFx3CheckFirmwareID(pUsbCtx, id);
        ret += usbFx3UploadFirmwareToFpga(pUsbCtx, (byte*)fpga->data.bytes, fpga->size, 0);
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfHardwareUploadGenerator(SFContext* ctx, SGeneratorConfig* genConfig, SGeneratorData* genData)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int transfered = 0;
        int ret = 0;
        ret += usbFxxTransferDataOut(pUsbCtx, 4, (byte*)genConfig,           sizeof(SGeneratorConfig), 1, ctx->api.timeout, &transfered);
        ret += usbFxxTransferDataOut(pUsbCtx, 4, (byte*)genData->data.bytes, genData->length * 2,      1, ctx->api.timeout, &transfered);
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfHardwareEepromRead(SFContext* ctx, SEeprom* eeprom, int size, int adress)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int ret = usbFx3ReadEEPROM(pUsbCtx, (byte*)&eeprom->data.bytes[0], size, adress);
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfHardwareEepromReadFirmwareID(SFContext* ctx, SEeprom* eeprom, int adress)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int ret = usbFx3ReadEEPROMFirmwareID(pUsbCtx, (byte*)&eeprom->data.bytes[0], SCOPEFUN_EEPROM_FIRMWARE_NAME_BYTES, adress);
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}

SCOPEFUN_API int sfHardwareReadFpgaStatus(SFContext* ctx, SInt* fpga)
{
   int result = SCOPEFUN_FAILURE;
   apiLock(ctx);
   if (ctx->api.active > 0)
   {
      struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
      int ret = usbFx3ReadFpgaStatus(pUsbCtx, &fpga->value );
      result = apiResult(ret);
   }
   apiUnlock(ctx);
   return result;
}

SCOPEFUN_API int sfHardwareEepromWrite(SFContext* ctx, SEeprom* eeprom, int size, int adress)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        int ret = usbFx3WriteEEPROM(pUsbCtx, (byte*)&eeprom->data.bytes[0], size, adress);
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int sfHardwareEepromErase(SFContext* ctx)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        UsbEEPROM eeprom;
        cMemSet((char*)&eeprom, 0, sizeof(struct UsbEEPROM));
        int ret = usbFx3WriteEEPROM(pUsbCtx, (byte*)&eeprom, sizeof(UsbEEPROM), 0);
        result = apiResult(ret);
    }
    apiUnlock(ctx);
    return result;
}
SCOPEFUN_API int sfHardwareClose(SFContext* ctx)
{
    int result = SCOPEFUN_FAILURE;
    apiLock(ctx);
    if(ctx->api.active > 0)
    {
        struct UsbContext* pUsbCtx = (struct UsbContext*)ctx->usb;
        usbFxxClose(pUsbCtx);
        result = SCOPEFUN_SUCCESS;
    }
    apiUnlock(ctx);
    return result;
}

/*--------------------------------------------------------------------
   simulate
---------------------------------------------------------------------*/

SCOPEFUN_API int sfIsSimulate(SFContext* ctx)
{
    int simulate = 0;
    apiLock(ctx);
    simulate = SDL_AtomicGet((SDL_atomic_t*)&ctx->simulateOn);
    apiUnlock(ctx);
    return simulate;
}

SCOPEFUN_API int sfSetSimulateData(SFContext* ctx, SSimulate* sim)
{
    apiLock(ctx);
    ctx->simulateData = *sim;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetSimulateOnOff(SFContext* ctx, int on)
{
    apiLock(ctx);
    SDL_AtomicSet((SDL_atomic_t*)&ctx->simulateOn, on);
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfGetSimulateData(SFContext* ctx, SSimulate* data)
{
    apiLock(ctx);
    *data = ctx->simulateData;
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

int softwareGenerator(int frameVersion, int frameHeader, int frameData, int framePacket, SFContext* ctx, SHardware* hw, SSimulate* sim, double timer)
{
    int frameSize = sfGetFrameSize(hw);
    SDL_memset(&ctx->frame.data.data.bytes[0], 0, apiMin(sizeof(SFrameData), frameSize));
    ctx->frame.received = 0;
    // header, channel0, channel1 and digital bits
    byte* packet = &ctx->frame.data.data.bytes[0];
    SFrameHeader* header = (SFrameHeader*)packet;
    uint  numSamples  = (uint)frameData / 4;
    float numSamplesF = (float)numSamples;
    // header
    SDL_memset(header, 0, frameHeader);
    // config
    SDL_memcpy(&header->hardware.bytes[0], hw, sizeof(SHardware));
    // magic
    header->magic.bytes[0] = 0xDD;
    header->magic.bytes[1] = 0xDD;
    header->magic.bytes[2] = 0xDD;
    header->magic.bytes[3] = 0xDD;
    // frameSize
    header->hardware.bytes[32 + 0] = (numSamples >> 24) & 0xff;
    header->hardware.bytes[32 + 1] = (numSamples >> 16) & 0xff;
    header->hardware.bytes[32 + 2] = (numSamples >> 8) & 0xff;
    header->hardware.bytes[32 + 3] = (numSamples >> 0) & 0xff;
    // ets
    header->etsDelay.bytes[0] = (byte)(rand() % 32);
    if(sim->etsActive > 0)
    {
        timer = 0;
    }
    // crc
    header->crc.bytes[0] = 0;
    // analog — generate both channels per sample, use sfSetData for 14-bit packing
    srand((uint)SDL_GetPerformanceCounter());
    // capture params (ch0 / ch1)
    float captureTime0 = sim->time * numSamplesF;
    float captureTime1 = sim->time * numSamplesF;
    float generateTime0 = (float)timer * sim->speed0;
    float generateTime1 = (float)timer * sim->speed1;
    float peakToPeak0   = sim->peakToPeak0;
    float peakToPeak1   = sim->peakToPeak1;
    float period0       = sim->period0 * numSamplesF / NUM_SAMPLESF;
    float period1       = sim->period1 * numSamplesF / NUM_SAMPLESF;
    float captureVolt0  = sim->voltage0;
    float captureVolt1  = sim->voltage1;
    float minpeak0 = -peakToPeak0 / 2.f;
    float minpeak1 = -peakToPeak1 / 2.f;
    float maxpeak0 =  peakToPeak0 / 2.f;
    float maxpeak1 =  peakToPeak1 / 2.f;

    for(uint j = 0; j < numSamples; j++)
    {
        float sample  = (float)j;
        float value0  = 0.f;
        float value1  = 0.f;

        // --- channel 0 ---
        float normalizedTime0 = sample / numSamplesF + generateTime0;
        float signaltime0 = normalizedTime0 * captureTime0;
        float t0 = signaltime0 / period0;
        float angle2Pi0 = 2.f * PI * t0;
        float normalizedPeriod0 = period0 / captureTime0;
        float mod0 = fmodf(normalizedTime0, normalizedPeriod0);
        float div0 = mod0 / normalizedPeriod0;
        float timeFrac0 = fmodf(normalizedTime0, 1.0f);

        switch(sim->type0)
        {
            case stSin:  value0 = maxpeak0 * sinf(angle2Pi0); break;
            case stCos:  value0 = maxpeak0 * cosf(angle2Pi0); break;
            case stInc:  value0 = div0 * peakToPeak0 + minpeak0; break;
            case stDec:  value0 = (1.f - div0) * peakToPeak0 + minpeak0; break;
            case stConstant: value0 = peakToPeak0; break;
            case stRandom:   value0 = rand_FloatRange(-1.f, 1.f) * (peakToPeak0 / 2.f); break;
            case stSquare:
                value0 = (timeFrac0 < normalizedPeriod0) ? peakToPeak0 : 0.f;
                break;
            case stDelta:
            {
                float tt0 = timeFrac0 / normalizedPeriod0 / 2.f;
                if(timeFrac0 < normalizedPeriod0 / 2.f)
                    value0 = 4.0f * tt0 * peakToPeak0;
                else if(timeFrac0 < normalizedPeriod0)
                    value0 = 4.0f * (0.5f - tt0) * peakToPeak0;
                else
                    value0 = 0.f;
                break;
            }
        }
        float norm0 = fClamp(value0 / (5.f * captureVolt0), -1.0f, 1.0f);
        ishort ival0 = (ishort)(norm0 * MAXOSCVALUE);

        // --- channel 1 ---
        float normalizedTime1 = sample / numSamplesF + generateTime1;
        float signaltime1 = normalizedTime1 * captureTime1;
        float t1 = signaltime1 / period1;
        float angle2Pi1 = 2.f * PI * t1;
        float normalizedPeriod1 = period1 / captureTime1;
        float mod1 = fmodf(normalizedTime1, normalizedPeriod1);
        float div1 = mod1 / normalizedPeriod1;
        float timeFrac1 = fmodf(normalizedTime1, 1.0f);

        switch(sim->type1)
        {
            case stSin:  value1 = maxpeak1 * sinf(angle2Pi1); break;
            case stCos:  value1 = maxpeak1 * cosf(angle2Pi1); break;
            case stInc:  value1 = div1 * peakToPeak1 + minpeak1; break;
            case stDec:  value1 = (1.f - div1) * peakToPeak1 + minpeak1; break;
            case stConstant: value1 = peakToPeak1; break;
            case stRandom:   value1 = rand_FloatRange(-1.f, 1.f) * (peakToPeak1 / 2.f); break;
            case stSquare:
                value1 = (timeFrac1 < normalizedPeriod1) ? peakToPeak1 : 0.f;
                break;
            case stDelta:
            {
                float tt1 = timeFrac1 / normalizedPeriod1 / 2.f;
                if(timeFrac1 < normalizedPeriod1 / 2.f)
                    value1 = 4.0f * tt1 * peakToPeak1;
                else if(timeFrac1 < normalizedPeriod1)
                    value1 = 4.0f * (0.5f - tt1) * peakToPeak1;
                else
                    value1 = 0.f;
                break;
            }
        }
        float norm1 = fClamp(value1 / (5.f * captureVolt1), -1.0f, 1.0f);
        ishort ival1 = (ishort)(norm1 * MAXOSCVALUE);

        // pack both 14-bit channels into 4 bytes — digital = 0
        sfSetData(packet + frameHeader + j * 4, ival0, ival1, 0);
    }
    // digital removed — buffer was already zeroed by SDL_memset
    return 0;
}

SCOPEFUN_API int sfSimulate(SFContext* ctx, SHardware* hw, SInt* received, SInt* frameSize, float time)
{
    apiLock(ctx);
    int simulate = SDL_AtomicGet((SDL_atomic_t*)&ctx->simulateOn);
    if(simulate > 0)
    {
        int numSamples       = sfGetNumSamples(hw);
        received->value      = sfGetFrameSize(hw);
        frameSize->value     = sfGetFrameSize(hw);;
        ctx->frame.received  = received->value;
        ctx->frame.frameSize = frameSize->value;
        softwareGenerator(HARDWARE_VERSION, SCOPEFUN_FRAME_HEADER, numSamples * 4, SCOPEFUN_FRAME_PACKET, ctx, hw, &ctx->simulateData, time);
    }
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------
   Frame
---------------------------------------------------------------------*/
SCOPEFUN_API int sfFrameCapture(SFContext* ctx, SInt* received, SInt* frameSize)
{
    apiLock(ctx);
    int debug = 0;
    int ret= 0;
    // start capture (frame header)
    if (ctx->frame.received == 0)
    {
        // beginning of frame (nothing is received yet)
        int transfered = 0;
        SFloat FpgaTemp = { 0.0 };
        SHardware recvdHw = { 0 };
        if (debug) printf("C++ sfHardwareCapture, START\n");
        ret = sfHardwareCapture(ctx, &ctx->frame.data, SCOPEFUN_FRAME_HEADER, ctx->frame.received, (SInt*)&transfered);
        SFrameHeader* header = (SFrameHeader*)&ctx->frame.data.data.bytes[0];
        int magic = 0;
        if (header->magic.bytes[0] == 0xdd &&
            header->magic.bytes[1] == 0xdd &&
            header->magic.bytes[2] == 0xdd &&
            header->magic.bytes[3] == 0xdd )
        {
            magic = 1;
        }
        if (magic)
        {
            //SHardware hw = { 0 };
            sfGetHeaderHardware((SFrameHeader*)&ctx->frame.data.data.bytes[0], &recvdHw);
            ctx->frame.frameSize = sfGetFrameSize(&recvdHw);
            if (!(transfered == SCOPEFUN_FRAME_HEADER || transfered == 0))
            	SDL_ShowSimpleMessageBox(0, "Frame De-Sync", "USB capture must be multiple of 1024 Bytes.", 0);
            ctx->frame.received += transfered;
            if (debug)
            {
                printf("C++ ctx->frame.received= %i \n", ctx->frame.received);
                sfGetHeaderTemperature(header, &FpgaTemp);
                printf("C++ FPGA Temperature = %3f \n", FpgaTemp.value);
            }
        }
        else
        {
            if (debug) printf("NO MAGIC!\n");
            return -1;
        }
    }
    // capture samples
    if(ctx->frame.received >= SCOPEFUN_FRAME_HEADER)
    {
        int transfered = 0;
        int maxTransferSize = 12*1024*1024;
        /*
        int xRange = (int)sfGetXRange(&recvdHw);
        if (xRange < 10)
            maxTransfer = 10*1024*1024;
        else
            maxTransfer = 1*1024;
        */
        int frameLeftToReceive = iMin(maxTransferSize, (int)ctx->frame.frameSize - (int)ctx->frame.received );
        if (debug) printf("C++ frameLeftToReceive: %i\n", frameLeftToReceive);
        if(frameLeftToReceive > 0)
        {
            if (debug) printf("C++ sfHardwareCapture, DATA\n");
            ret = sfHardwareCapture(ctx, &ctx->frame.data, frameLeftToReceive, ctx->frame.received, (SInt*)&transfered);
            ctx->frame.received += transfered;
            if (debug) printf("C++ ctx->frame.received= %i \n", ctx->frame.received);
        }
    }
    frameSize->value = ctx->frame.frameSize;
    received->value  = ctx->frame.received;
    // new frame ?
    if(ctx->frame.received == ctx->frame.frameSize)
    { ctx->frame.received = 0; } //frame end (reset frame.received)
    apiUnlock(ctx);
    return ret;
}

SCOPEFUN_API int sfFrameOutput(SFContext* ctx, SFrameData* buffer, int len)
{
    apiLock(ctx);
    SDL_memcpy(&buffer->data.bytes[0], &ctx->frame.data.data.bytes[0], iMin(ctx->frame.frameSize, len));
    apiUnlock(ctx);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfFrameDisplayFunction(SFContext* ctx, EFunctionType fun)
{
    ctx->functionType = fun;
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfFrameDisplayCallback(SFContext* ctx, SCallback* callback, void* userData)
{
    ctx->pCallback = callback;
    ctx->pUserData = userData;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int cDisplayFunction(SFContext* ctx, EFunctionType function, ushort channel0, ushort channel1, ushort digital)
{
    switch(function)
    {
        case dfMax:
            return uMax(channel0, channel1);
        case dfMin:
            return uMin(channel0, channel1);
        case dfMedium:
            return (channel0 + channel1) / 2;
        case dfAdd:
            return iClamp((int)(channel0 + channel1), SCOPEFUN_MIN_VOLTAGE, SCOPEFUN_MAX_VOLTAGE);
        case dfCh0SubCh1:
            return iClamp((int)(channel0 - channel1), SCOPEFUN_MIN_VOLTAGE, SCOPEFUN_MAX_VOLTAGE);
        case dfCh1SubCh0:
            return iClamp((int)(channel1 - channel0), SCOPEFUN_MIN_VOLTAGE, SCOPEFUN_MAX_VOLTAGE);
        case dfScript:
            if (ctx->pCallback)
            {
               ishort func = 0;
               ((SCallback*)ctx->pCallback)->onFunction(channel0, channel1, &func);
               return func;
            }
    }
    return 0;
}

SCOPEFUN_API int sfFrameDisplay(SFContext* ctx, SFrameData* buffer, int len, SDisplay* display, double displayPos, double displayZoom)
{
    // get frame data for rendering

    // header
    SHardware hw = { 0 };
    sfGetHeaderHardware((SFrameHeader*)&buffer->data.bytes[0], &hw);
    uint numSamples = sfGetNumSamples(&hw);
    if(numSamples == 0) { return 1; }
    if(numSamples > SCOPEFUN_FRAME_DATA / 4) { return 1; }

    uint lenSamples = 0;
    if (len >= SCOPEFUN_FRAME_HEADER)
       lenSamples = (len - SCOPEFUN_FRAME_HEADER) / 4;

    // init display
    SDL_memset(display, 0, sizeof(SDisplay));
    //display->samples = SCOPEFUN_DISPLAY;
    display->captured = numSamples;
    // frame size
    uint frameSize = sfGetFrameSize(&hw);
    if(frameSize > SCOPEFUN_FRAME_MEMORY) { return 1; }
    // ets delay
    sfGetHeaderEts((SFrameHeader*)&buffer->data.bytes[0], &display->ets);
    // channel attributes
    display->attr = sfGetAnalogSwitch(&hw);
    // digital loop counter
    sfGetHeaderDigitalPatternCompleteCnt((SFrameHeader*)&buffer->data.bytes[0], &display->digitalPatternCompleteCnt);
    // resample (sfFrameDisplay)
    double resamplePos = displayPos;
    double resampleMin = resamplePos - 0.5 * (double)displayZoom;
    double resampleMax = resamplePos + 0.5 * (double)displayZoom;
    // sample: min / max
    double dNumSamples   = numSamples;
    ilarge zoomSamplePos = resamplePos * dNumSamples;
    ilarge zoomSampleMin = zoomSamplePos - (dNumSamples / 2) * displayZoom;     // [0..n]
    ilarge zoomSampleMax = zoomSamplePos + (dNumSamples / 2) * displayZoom - 1; // [0..n]
    ilarge zoomSampleCnt = ilClamp(zoomSampleMax - zoomSampleMin + 1, 0, numSamples); // [0..n]
    // samples
    //display->samples = 0;
    EFunctionType function = dfMedium;
    if(frameSize > 0 && numSamples > 0)
    {
        if(ctx->pCallback)
        { ((SCallback*)ctx->pCallback)->onFrame(buffer, len, &displayPos, &displayZoom, ctx->pUserData); }
        byte*           dataStart = &buffer->data.bytes[SCOPEFUN_FRAME_HEADER];
        // intervals
        uint          intervalCnt = (uint)SDL_ceil((double)zoomSampleCnt / (double)(SCOPEFUN_DISPLAY));
        uint   lPointsPerInterval = intervalCnt;
        ilarge  rendererSampleCnt = zoomSampleCnt / intervalCnt;
        //  number of samples to renderer
        display->samples = rendererSampleCnt;
        display->ppi     = lPointsPerInterval;
        // resample
        if(lPointsPerInterval > 1)
        {
            int n = 0;
            ishort ch0PeakPos = SCOPEFUN_MIN_VOLTAGE;
            ishort ch0PeakNeg = SCOPEFUN_MAX_VOLTAGE;
            ishort ch1PeakPos = SCOPEFUN_MIN_VOLTAGE;
            ishort ch1PeakNeg = SCOPEFUN_MAX_VOLTAGE;
            ishort funPeakPos = SCOPEFUN_MIN_VOLTAGE;
            ishort funPeakNeg = SCOPEFUN_MAX_VOLTAGE;
            ushort digPeakNeg = 0;
            ushort digPeakPos = 0;
            // loop through all samples in frame
            for(ilarge i = 0; i < (rendererSampleCnt * lPointsPerInterval); i++) //n [0:]
            {
                ushort  ch0 = 0;
                ushort  ch1 = 0;
                ushort  dig = 0;

                ishort ich0 = 0;
                ishort ich1 = 0;

                if (i > lenSamples)
                    break;

                // get current sample
                byte byte0 = *(dataStart + (i * 4) + 0);
                byte byte1 = *(dataStart + (i * 4) + 1);
                byte byte2 = *(dataStart + (i * 4) + 2);
                byte byte3 = *(dataStart + (i * 4) + 3);
                sfGetData((uint)byte0 | ((uint)byte1 << 8)| ((uint)byte2 << 16) | ((uint)byte3 << 24), &ich0, &ich1, &dig);
                /*
                if(ctx->pCallback)
                { ((SCallback*)ctx->pCallback)->onSample(sample, &ich0, &ich1, &fun, &dig, &displayPos, &displayZoom, ctx->pUserData); }
                */
                int saveTrig = i % (2 * lPointsPerInterval);
                // interval start (load initial values)
                if (saveTrig == 0) {
                    ch0PeakPos = SCOPEFUN_MIN_VOLTAGE;
                    ch0PeakNeg = SCOPEFUN_MAX_VOLTAGE;
                    ch1PeakPos = SCOPEFUN_MIN_VOLTAGE;
                    ch1PeakNeg = SCOPEFUN_MAX_VOLTAGE;
                    digPeakNeg = dig;
                    digPeakPos = dig;
                }
                // get peak values
                ch0PeakNeg = iMin(ich0, ch0PeakNeg);
                ch1PeakNeg = iMin(ich1, ch1PeakNeg);
                ch0PeakPos = iMax(ich0, ch0PeakPos);
                ch1PeakPos = iMax(ich1, ch1PeakPos);
                digPeakNeg = dig & digPeakNeg;  // keep zeroes
                digPeakPos = dig | digPeakPos;  // keep ones
                // save peak values for current render interval
                if (saveTrig == (2 * lPointsPerInterval) - 1)
                {
                    display->analog0.bytes[n + 0] = 2.0 * (((float)ch0PeakNeg - SCOPEFUN_MIN_VOLTAGE) / (float)SCOPEFUN_VOLTAGE_RANGE) - 1;
                    display->analog1.bytes[n + 0] = 2.0 * (((float)ch1PeakNeg - SCOPEFUN_MIN_VOLTAGE) / (float)SCOPEFUN_VOLTAGE_RANGE) - 1;
                    display->digital.bytes[n + 0] = digPeakNeg;
                    display->analog0.bytes[n + 1] = 2.0 * (((float)ch0PeakPos - SCOPEFUN_MIN_VOLTAGE) / (float)SCOPEFUN_VOLTAGE_RANGE) - 1;
                    display->analog1.bytes[n + 1] = 2.0 * (((float)ch1PeakPos - SCOPEFUN_MIN_VOLTAGE) / (float)SCOPEFUN_VOLTAGE_RANGE) - 1;
                    display->digital.bytes[n + 1] = digPeakPos;
                    n = n + 2;
                }
            }
        }
        else
        {
            // loop through samples in display range
            for (ilarge i = 0; i <= rendererSampleCnt; i++)
            {
                if (i > SCOPEFUN_DISPLAY - 1)  // limit display to the range of 0 to SCOPEFUN_DISPLAY
                    break;

                uint offset = 0;
                ushort  ch0 = 0;
                ushort  ch1 = 0;
                ushort  dig = 0;
                ilarge sample = iClamp(zoomSampleMin + i, 0, numSamples - 1);
                offset = sample * 4;
                byte byte0 = *(dataStart + offset + 0);
                byte byte1 = *(dataStart + offset + 1);
                byte byte2 = *(dataStart + offset + 2);
                byte byte3 = *(dataStart + offset + 3);

                ishort ich0 = 0;
                ishort ich1 = 0;

                sfGetData((uint)byte0 | ((uint)byte1 << 8) | ((uint)byte2 << 16) | ((uint)byte3 << 24), &ich0, &ich1, &dig);
                ushort digital = dig;
                /*
                if (ctx->pCallback)
                {
                    ((SCallback*)ctx->pCallback)->onSample(sample, &ich0, &ich1, &function, &digital, &displayPos, &displayZoom, ctx->pUserData);
                }
                */
                // Output:
                display->analog0.bytes[i] = 2.0 * (((float)ich0 - SCOPEFUN_MIN_VOLTAGE) / (float)SCOPEFUN_VOLTAGE_RANGE) - 1;
                display->analog1.bytes[i] = 2.0 * (((float)ich1 - SCOPEFUN_MIN_VOLTAGE) / (float)SCOPEFUN_VOLTAGE_RANGE) - 1;
                display->digital.bytes[i] = digital;
            }
        }
        // get all samples for FFT display
        for (ilarge i = 0; i < numSamples; i++)
        {
            if (numSamples > SCOPEFUN_DISPLAY_FFT)
                break;

            ishort ich0 = 0;
            ishort ich1 = 0;
            ushort  dig = 0;

            // get current sample
            byte byte0 = *(dataStart + (i * 4) + 0);
            byte byte1 = *(dataStart + (i * 4) + 1);
            byte byte2 = *(dataStart + (i * 4) + 2);
            byte byte3 = *(dataStart + (i * 4) + 3);
            sfGetData((uint)byte0 | ((uint)byte1 << 8)| ((uint)byte2 << 16) | ((uint)byte3 << 24), &ich0, &ich1, &dig);
            if (i < SCOPEFUN_DISPLAY_FFT)
            {
                display->fft0.bytes[i] = 2.0 * (((float)ich0 - SCOPEFUN_MIN_VOLTAGE) / (float)SCOPEFUN_VOLTAGE_RANGE) - 1;
                display->fft1.bytes[i] = 2.0 * (((float)ich1 - SCOPEFUN_MIN_VOLTAGE) / (float)SCOPEFUN_VOLTAGE_RANGE) - 1;
            }
        }
        if (ctx->pCallback)
        {
            ((SCallback*)ctx->pCallback)->onDisplay(display, &displayPos, &displayZoom, ctx->pUserData);
        }
    }
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------------------
   Header
--------------------------------------------------------------------------------*/
SCOPEFUN_API int sfGetHeader(SFContext* ctx, SFrameData* frame, SFrameHeader* header)
{
    SDL_memcpy(header, &frame->data.bytes[0], sizeof(SFrameHeader));
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfGetHeaderHardware(SFrameHeader* header, SHardware* hw)
{
    SDL_memcpy(hw, &header->hardware.bytes[0], sizeof(SHardware));
    uint         count = sizeof(SHardware) / 2;
    ushort* swapBuffer = (ushort*)hw;
    for(int i = 0; i < count; i++)
    {
        swapBuffer[i] = cSwap16(&swapBuffer[i]);
    }
    return SCOPEFUN_SUCCESS;
}
SCOPEFUN_API int sfGetHeaderEts(SFrameHeader* header, uint* ets)
{
    *ets = header->etsDelay.bytes[0];
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfGetHeaderTemperature(SFrameHeader* header, SFloat* temperature)
{
    byte* ptr = (byte*)&header->magic.bytes[0];
    unsigned short adc0 = (ptr)[6] << 8;
    unsigned short adc1 = (ptr)[7];
    unsigned short adc = adc1 | adc0;
    temperature->value = (((float)adc * 503.975) / 4096.0) - 273.15;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfGetHeaderDigitalPatternCompleteCnt(SFrameHeader* header, uint* digitalPatternCompleteCnt)
{
    //digitalPatternCompleteCnt->value = (uint*)(&header->digitalPatternCompleteCnt.bytes[0]);
    //byte* ptr = (byte*)&header->digitalPatternCompleteCnt.bytes[0];
    byte byte0 = header->digitalPatternCompleteCnt.bytes[0];
    byte byte1 = header->digitalPatternCompleteCnt.bytes[1];
    byte byte2 = header->digitalPatternCompleteCnt.bytes[2];
    byte byte3 = header->digitalPatternCompleteCnt.bytes[3];
    uint digitalPatternCompleteCntValue = ((uint)byte0 << 24) | ((uint)byte1 << 16) | ((uint)byte2 << 8) | ((uint)byte3 << 0);
    *digitalPatternCompleteCnt = digitalPatternCompleteCntValue;
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------------------
  Hardware
--------------------------------------------------------------------------------*/
char* hardwareIds[] =
{
    "controlAddr",
    "controlData",
    "vgaina",
    "vgainb",
    "offseta",
    "offsetb",
    "analogswitch",
    "triggerMode",
    "triggerSource",
    "triggerSlope",
    "triggerLevel",
    "triggerHis",
    "triggerPercent",
    "xRange",
    "holdoffH",
    "holdoffL",
    "sampleSizeH",
    "sampleSizeL",
    "generatorType0",
    "generatorVoltage0",
    "generatorOffset0",
    "generatorDeltaH0",
    "generatorDeltaL0",
    "generatorSquareDuty0",
    "generatorType1",
    "generatorVoltage1",
    "generatorOffset1",
    "generatorDeltaH1",
    "generatorDeltaL1",
    "generatorSquareDuty1",
    "digitalPattern1a",
    "digitalPattern1b",
    "digitalMask1",
    "digitalPattern2a",
    "digitalPattern2b",
    "digitalMask2",
    "digitalPattern3a",
    "digitalPattern3b",
    "digitalMask3",
    "digitalPattern4a",
    "digitalPattern4b",
    "digitalMask4",
    "dt_delayMaxcnt1",
    "dt_delayMaxcnt2",
    "dt_delayMaxcnt3",
    "dt_delayMaxcnt4",
    "dt_control",
    "digitalVoltage",
    "digitalInputOutput",
    "digitalOutputBit",
    "digitalOutputMask",
    "digitalClkDivideH",
    "digitalClkDivideL",
    "average",
    "preTriggerH",
    "preTriggerL",
    "generatorPhase0",
    "generatorPhase1",
    "reserved3",
    "reserved4",
    "reserved5",
};

SCOPEFUN_API int sfHardwareWordCnt(int* cnt)
{
    *cnt = sizeof(hardwareIds) / sizeof(char*);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfHardwareWordId(int index, SArrayString256* string)
{
    index = iMax(index, 0);
    index = iMin(index, sizeof(hardwareIds));
    SDL_memcpy(string->bytes, (void*)hardwareIds[index], SDL_strlen(hardwareIds[index]));
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------------------
  Data
--------------------------------------------------------------------------------*/
ushort twos_complement(ushort input_value, ushort num_bits)
{
    ushort mask = 1 << (num_bits - 1);
    return -(input_value & mask) + (input_value & ~mask);
}

uint endianSwap32(uint value)
{
    uint v0 = (value & 0x000000FF);
    uint v1 = (value & 0x0000FF00);
    uint v2 = (value & 0x00FF0000);
    uint v3 = (value & 0xFF000000);
    return (v0 << 24) | (v1 << 8) | (v2 >> 8) | (v3 >> 24);
}

uint reverseBits(uint v)
{
    // Bit Twiddling Hacks
    uint r = v;
    int s = sizeof(v) * 8 - 1;
    for(v >>= 1; v; v >>= 1)
    {
        r <<= 1;
        r |= v & 1;
        s--;
    }
    r <<= s;
    return r;
}

uint rotateBits(uint x, uint n)
{
    uint r = (x << n) | (x >> (-n & 31));
    return r;
}

byte reverseByte(byte v)
{
    byte r = v & 1; // r will be reversed bits of v; first get LSB of v
    int s = sizeof(v) * 8 - 1; // extra shift needed at end
    for(v >>= 1; v; v >>= 1)
    {
        r <<= 1;
        r |= v & 1;
        s--;
    }
    r <<= s; // shift when v's highest bits are zero
    return r;
}

SCOPEFUN_API int sfSetData(byte* ptr, ishort channel0, ishort channel1, ushort digital)
{
    // Pack two signed 14-bit channels into 4 bytes (28 bits used). digital removed.
    uint ch0 = ((uint)channel0) & 0x3FFF; // 14 bits
    uint ch1 = ((uint)channel1) & 0x3FFF; // 14 bits
    // layout (LSB first): [ byte0 | byte1 | byte2 | byte3 ]
    // pack: lower 8 bits of ch0 -> byte0
    //       next 6 bits of ch0 + lower 2 bits of ch1 -> byte1
    //       next 8 bits of ch1 -> byte2
    //       remaining upper 4 bits of ch1 -> byte3 (in low nibble), rest zero
    byte b0 = (byte)(ch0 & 0xFF);
    byte b1 = (byte)(((ch0 >> 8) & 0x3F) | ((ch1 & 0x3) << 6));
    byte b2 = (byte)((ch1 >> 2) & 0xFF);
    byte b3 = (byte)((ch1 >> 10) & 0x0F);
    ptr[0] = b0;
    ptr[1] = b1;
    ptr[2] = b2;
    ptr[3] = b3;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfGetData(uint data, ishort* analogCh1, ishort* analogCh2, ushort* digital)
{
    // Unpack two signed 14-bit channels from 4 bytes packed by sfSetData (digital removed).
    byte* dataStart = (byte*)&data;
    byte byte0 = dataStart[0];
    byte byte1 = dataStart[1];
    byte byte2 = dataStart[2];
    byte byte3 = dataStart[3];
    uint ch0 = (uint)byte0 | (((uint)byte1 & 0x3F) << 8);
    uint ch1 = (((uint)byte1 >> 6) & 0x03) | ((uint)byte2 << 2) | (((uint)byte3 & 0x0F) << 10);
    // sign extend 14-bit values to 16-bit signed
    if (ch0 & (1 << 13)) ch0 |= 0xFFFFC000;
    if (ch1 & (1 << 13)) ch1 |= 0xFFFFC000;
    *analogCh1 = (ishort)(ch0 & 0xFFFF);
    *analogCh2 = (ishort)(ch1 & 0xFFFF);
    if (digital) *digital = 0;
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------------------
  Set
--------------------------------------------------------------------------------*/

SCOPEFUN_API int sfSetDefault(SHardware* hw)
{
    SDL_memset(hw, 0, sizeof(SHardware));
    hw->vgaina = 1416;
    hw->vgainb = 1416;
    hw->sampleSizeH = 0;
    hw->sampleSizeL = SCOPEFUN_DISPLAY;
    sfSetXRange(hw, 1);
    sfSetGeneratorType0(hw, GENERATOR_SIN);
    sfSetGeneratorType1(hw, GENERATOR_SIN);
    /*
    sfSetGeneratorVoltage0(hw, 1);
    sfSetGeneratorFrequency0(hw, 10000.f, 200000000.f);
    sfSetGeneratorVoltage1(hw, 1);
    sfSetGeneratorFrequency1(hw, 10000.f, 200000000.f);
    */
    sfSetDigitalInputOutput(hw, 1, 1);   //digital channels are configured as inputs
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetFrameSize(SHardware*  hw, uint  frameSize)
{
    uint   dataSize = frameSize - SCOPEFUN_FRAME_HEADER;
    uint numSamples = dataSize / 4;
    hw->sampleSizeL =         numSamples & 0x0000ffff;
    hw->sampleSizeH = (numSamples >> 16) & 0x0000ffff;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetNumSamples(SHardware* hw, uint  numSamples)
{
    hw->sampleSizeL = numSamples & 0x0000ffff;
    hw->sampleSizeH = (numSamples >> 16) & 0x0000ffff;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetAnalogSwitchBit(SHardware* hw, int bit, int value)
{
    hw->analogswitch ^= ((~value + 1) ^ hw->analogswitch) & bit;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetEts(SHardware* hw, int enable)
{
    if(enable) { hw->analogswitch = raiseFlag16(hw->analogswitch, CHANNEL_ETS); }
    else        { hw->analogswitch = lowerFlag16(hw->analogswitch, CHANNEL_ETS); }
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetYRangeScaleA(SHardware* hw, ushort attr, ushort gain)
{
    if(attr) { hw->analogswitch = raiseFlag16(hw->analogswitch, CHANNEL_ATTR_A); }
    else      { hw->analogswitch = lowerFlag16(hw->analogswitch, CHANNEL_ATTR_A); }
    hw->vgaina = gain;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetYPositionA(SHardware* hw, int pos)
{
    pos = iMin(pos,  1500);
    pos = iMax(pos, -1500);
    hw->offseta = pos;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetYRangeScaleB(SHardware* hw, ushort attr, ushort gain)
{
    if(attr) { hw->analogswitch = raiseFlag16(hw->analogswitch, CHANNEL_ATTR_B); }
    else      { hw->analogswitch = lowerFlag16(hw->analogswitch, CHANNEL_ATTR_B); }
    hw->vgainb = gain;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetYPositionB(SHardware* hw, int pos)
{
    pos = iMin(pos,  1500);
    pos = iMax(pos, -1500);
    hw->offsetb = pos;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetXRange(SHardware* hw, ishort range)
{
    // 2ns is interleaved
    if(range == 0) { hw->analogswitch = raiseFlag16(hw->analogswitch, CHANNEL_INTERLEAVE); }
    else           { hw->analogswitch = lowerFlag16(hw->analogswitch, CHANNEL_INTERLEAVE); }
    hw->xRange = range;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetControl(SHardware* hw, uint selected)
{
    switch(selected)
    {
        case 0:
            hw->controlData = CONTROLL2_NORMAL;
            break;
        case 1:
            hw->controlData = CONTROLL2_TEST;
            break;
        case 3:
            hw->controlData = CONTROLL2_RESET;
            break;
    };
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetSampleSize(SHardware* hw, uint value)
{
    value = uMin(value, (SCOPEFUN_FRAME_DATA / 4));
    uint size = 4 * (value / 4);
    value = uMax(256U, size);
    hw->sampleSizeH = (value >> 16) & 0xFFFF;
    hw->sampleSizeL = (value & 0xFFFF);
    return SCOPEFUN_SUCCESS;
}


SCOPEFUN_API int  sfSetTriggerSource(SHardware* hw, int value)
{
    hw->triggerSource = value;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetTriggerReArm(SHardware* hw, int on)
{
    ushort mask = (1 << 2);
    if(on)
    {
        hw->triggerMode = hw->triggerMode | mask;
    }
    else
    {
        hw->triggerMode = hw->triggerMode & ~mask;
    }
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetTriggerMode(SHardware* hw, int value)
{
    hw->triggerMode = value;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetTriggerSlope(SHardware* hw, int value)
{
    hw->triggerSlope = value;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetTriggerPre(SHardware* hw, float perc)
{
    uint numSamples = (((uint)hw->sampleSizeH) << 16) | (uint)hw->sampleSizeL;

        uint preTrigger = (perc / 100.f) * (float)(numSamples - 1);
            preTrigger = 4 * (preTrigger / 4);
    hw->preTriggerH = (preTrigger >> 16) & 0xFFFF;
    hw->preTriggerL = (preTrigger & 0xFFFF);
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetTriggerHis(SHardware* hw, int perc)
{
    hw->triggerHis = perc;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetTriggerLevel(SHardware* hw, int perc)
{
    hw->triggerLevel = perc;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetHoldoff(SHardware* hw, uint holdoff)
{
    hw->holdoffH = (holdoff & 0xFFFF0000) >> 16;
    hw->holdoffL = holdoff & 0x0000FFFF;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetAverage(SHardware* hw, int enable)
{
    hw->average = enable;
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------------------
   generator 0
--------------------------------------------------------------------------------*/

SCOPEFUN_API int sfSetGeneratorType0(SHardware* hw, ushort type)
{
    uint mask = 1 << 8;
    if(hw->generatorType0 & mask)
    {
        hw->generatorType0 = type | mask;
    }
    else
    {
        hw->generatorType0 = type;
    }
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetGeneratorOn0(SHardware* hw, int onoff)
{
    uint mask = 1 << 8;
    if(onoff)
    {
        hw->generatorType0 = hw->generatorType0 | mask;
    }
    else
    {
        hw->generatorType0 = hw->generatorType0 & ~mask;
    }
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetGeneratorVoltage0(SHardware* hw, int volt)
{
    uint voltage = volt;
    uint mask = 0x7FF;
    hw->generatorVoltage0 = voltage & mask;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetGeneratorOffset0(SHardware* hw, int offset)
{
    int offsetSet = iClamp(offset, -2048, 2047);
    hw->generatorOffset0 = offsetSet;
    return SCOPEFUN_SUCCESS;
}

uint DoubleToBinary(double number)
{
   // parts
   uint    integer  = number;
   double  fraction = number - integer;
   // whole part
   int resInt = 0;
   for (int k = 0; k < 15; k++)
   {
      uint modulo = integer % 2;
      if (modulo == 1)
      {
         resInt = resInt | BIT(k);
      }
      integer /= 2;
   }
   // fractional part
   uint resDec = 0;
   for(int k=17;k>0;k--)
   {
      fraction *= 2;
      uint fract = fraction;
      if (fract == 1)
      {
         fraction -= fract;
         resDec = resDec | BIT(k-1);
      }
   }
   return (resInt << 17) | resDec;
}


double BinaryToDouble(uint number)
{
   // parts
   uint Int = number >> 17;
   uint Dec = (number << 15) >> 15;

   // result
   double result = 0;

   // whole part
   for (int k = 0; k < 15; k++)
   {
      uint modulo = Int % 2;
      if (modulo == 1)
      {
         result += SDL_pow(2, k);
      }
      Int /= 2;
   }
   // fractional part
   for (int k = 0; k < 17; k++)
   {
      uint modulo = Dec % 2;
      if (modulo == 1)
      {
         result += 1.0 / SDL_pow(2, 17 - k);
      }
      Dec /= 2;
   }
   return result;
}

SCOPEFUN_API int sfSetGeneratorFrequency0(SHardware* hw, double freq, float fs)
{
    freq = dClamp(freq, 0, 50000000);

    double delta = 0;
    uint mask = (1 << 8);
    mask = ~mask;
    uint type = hw->generatorType0 & mask;
    switch(type)
    {
        case GENERATOR_CUSTOM:
        case GENERATOR_SIN:
        case GENERATOR_COS:
        case GENERATOR_TRIANGLE:
        case GENERATOR_RAMP_UP:
        case GENERATOR_RAMP_DOWN:
        case GENERATOR_SQUARE:
        case GENERATOR_DELTA:
        case GENERATOR_DC:
        case GENERATOR_NOISE:
            delta = 2*16384.0 * freq / (double)fs;
            break;
    };

    uint genDelta = DoubleToBinary(delta);
    hw->generatorDeltaH0 = (genDelta & 0xFFFF0000) >> 16;
    hw->generatorDeltaL0 = genDelta & 0x0000FFFF;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetGeneratorSquareDuty0(SHardware* hw, int duty)
{
    hw->generatorSquareDuty0 = duty & 0x7FF;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetGeneratorPhase0(SHardware* hw, int phase)
{
    hw->generatorPhase0 = phase & 0x7FFF;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetGeneratorDelta0(SHardware* hw, uint delta)
{
    hw->generatorDeltaH0 = (delta & 0xFFFF0000) >> 16;
    hw->generatorDeltaL0 =  delta & 0x0000FFFF;
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------------------
   generator 1
--------------------------------------------------------------------------------*/

SCOPEFUN_API int  sfSetGeneratorType1(SHardware* hw, ushort type)
{
    uint mask = 1 << 8;
    if(hw->generatorType1 & mask)
    {
        hw->generatorType1 = type | mask;
    }
    else
    {
        hw->generatorType1 = type;
    }
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetGeneratorOn1(SHardware* hw, int onoff)
{
    uint mask = 1 << 8;
    if(onoff)
    {
        hw->generatorType1 = hw->generatorType1 | mask;
    }
    else
    {
        hw->generatorType1 = hw->generatorType1 & ~mask;
    }
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetGeneratorVoltage1(SHardware* hw, int volt)
{
    uint voltage = volt;
    uint mask = 0x7FF;
    hw->generatorVoltage1 = voltage & mask;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetGeneratorOffset1(SHardware* hw, int offset)
{
    int offsetSet = iClamp(offset, -2048, 2047);
    hw->generatorOffset1 = offsetSet;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetGeneratorFrequency1(SHardware* hw, double freq, float fs)
{
    freq = dClamp(freq, 0, 50000000);

    double delta = 0;
    uint mask = (1 << 8);
    mask = ~mask;
    uint type = hw->generatorType1 & mask;
    switch(type)
    {
        case GENERATOR_CUSTOM:
        case GENERATOR_SIN:
        case GENERATOR_COS:
        case GENERATOR_TRIANGLE:
        case GENERATOR_RAMP_UP:
        case GENERATOR_RAMP_DOWN:
        case GENERATOR_SQUARE:
        case GENERATOR_DELTA:
        case GENERATOR_DC:
        case GENERATOR_NOISE:
            delta = 2*16384.0 * freq / (double)fs;
            break;
    };
    //
    uint genDelta = DoubleToBinary(delta);
    hw->generatorDeltaH1 = (genDelta & 0xFFFF0000) >> 16;
    hw->generatorDeltaL1 =  genDelta & 0x0000FFFF;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetGeneratorSquareDuty1(SHardware* hw, int duty)
{
    hw->generatorSquareDuty1 = duty & 0x7FF;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetGeneratorPhase1(SHardware* hw, int phase)
{
    hw->generatorPhase1 = phase & 0x7FFF;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetGeneratorDelta1(SHardware* hw, uint delta)
{
    hw->generatorDeltaH1 = (delta & 0xFFFF0000) >> 16;
    hw->generatorDeltaL1 =  delta & 0x0000FFFF;
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------------------
   digital GPIO
--------------------------------------------------------------------------------*/

SCOPEFUN_API int  sfSetDigitalVoltage(SHardware* hw, double volt, double kDigital)
{
    double rab = 50000;
    double rw = 75;
    double rwb1 = 29763;
    hw->digitalVoltage = 256 * (rwb1*((volt / 1.235) - 1.0) - rw) / rab;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetDigitalInputOutput(SHardware* hw, uint inout15, uint inout7)
{
    // 00 = OUT, OUT
    // 01 = OUT, IN
    // 10 = IN,  OUT
    // 11 = IN,  IN
    ushort inOutWordTmp = 0;
    inOutWordTmp = hw->digitalInputOutput & 0xFFFFFFFC;  // clear last two bits
    inOutWordTmp = inOutWordTmp | inout15 << 1 | inout7; // set last two bits
    hw->digitalInputOutput = inOutWordTmp;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetDigitalOutputBit(SHardware* hw, int bit, int onoff)
{
    ushort output = 0;
    ishort mask   = 0;
    output = hw->digitalOutputBit;
    mask   = hw->digitalOutputMask;
    if(onoff == 0 || onoff == 1) // bit value '0' or '1'
    {
        output = bitFlag16(output, BIT(bit), onoff);
        mask   = bitFlag16(mask,   BIT(bit), 1);
    }
    else if(onoff == 2)         // bit value from file
    {
        output = bitFlag16(output, BIT(bit), onoff);
        mask   = bitFlag16(mask,   BIT(bit), 0);
    }
    hw->digitalOutputBit  = output;
    hw->digitalOutputMask = mask;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetDigitalClockDivide(SHardware* hw, uint divider)
{
    hw->digitalClkDivideH = (divider & 0xFFFF0000) >> 16;
    hw->digitalClkDivideL =  divider & 0x0000FFFF;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetDigitalPatternOutputEn(SHardware* hw, int outputEnable)
{
    ushort inOutWord = hw->digitalInputOutput;
    if (outputEnable == 1) {
        hw->digitalInputOutput = inOutWord | (1 << 8);
    }
    else {
        hw->digitalInputOutput = inOutWord & ~(1 << 8);
    }
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetDigitalPatternOutputMode(SHardware* hw, int mode)
{
    ushort inOutWord = hw->digitalInputOutput;
    if (mode == 1) {
        hw->digitalInputOutput = inOutWord | (1 << 9);
    }
    else {
        hw->digitalInputOutput = inOutWord & ~(1 << 9);
    }
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetDigitalPatternOutputRestart(SHardware* hw, int restart)
{
    ushort inOutWord = hw->digitalInputOutput;
    if (restart == 1) {
        hw->digitalInputOutput = inOutWord | (1 << 10);
    }
    else {
        hw->digitalInputOutput = inOutWord & ~(1 << 10);
    }
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetDigitalPatternOutputRestartOnUpload(SHardware* hw, int restart)
{
    ushort inOutWord = hw->digitalInputOutput;
    if (restart == 1) {
        hw->digitalInputOutput = inOutWord | (1 << 11);
    }
    else {
        hw->digitalInputOutput = inOutWord & ~(1 << 11);
    }
    return SCOPEFUN_SUCCESS;
}

/*--------------------------------------------------------------------------------
   digital trigger
--------------------------------------------------------------------------------*/

SCOPEFUN_API int sfSetDigitalStart(SHardware* hw, int start)
{
    ushort   startBits = start;
    ushort controlBits = hw->dt_control;
    controlBits = bitFlag16(controlBits, 1 << 8, isFlag16(startBits, 1));
    controlBits = bitFlag16(controlBits, 1 << 9, isFlag16(startBits, 2));
    hw->dt_control = controlBits;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetDigitalMode(SHardware* hw, int mode)
{
    ushort    modeBits = mode;
    ushort controlBits = hw->dt_control;
    controlBits = bitFlag16(controlBits, 1 << 4, isFlag16(modeBits, 1));
    hw->dt_control = controlBits;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetDigitalChannel(SHardware* hw, int channel)
{
    ushort channelBits = channel;
    ushort controlBits = hw->dt_control;
    controlBits = bitFlag16(controlBits, 1 << 0, isFlag16(channelBits, 1));
    controlBits = bitFlag16(controlBits, 1 << 1, isFlag16(channelBits, 2));
    controlBits = bitFlag16(controlBits, 1 << 2, isFlag16(channelBits, 4));
    controlBits = bitFlag16(controlBits, 1 << 3, isFlag16(channelBits, 8));
    hw->dt_control = controlBits;
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetDigitalDelay(SHardware* hw, uint stage, ushort delay)
{
    switch(stage)
    {
        case DIGITAL_STAGE_0:
            hw->dt_delayMaxcnt1 = delay;
            break;
        case DIGITAL_STAGE_1:
            hw->dt_delayMaxcnt2 = delay;
            break;
        case DIGITAL_STAGE_2:
            hw->dt_delayMaxcnt3 = delay;
            break;
        case DIGITAL_STAGE_3:
            hw->dt_delayMaxcnt4 = delay;
            break;
        default:
            return SCOPEFUN_FAILURE;
    };
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int  sfSetDigitalMask(SHardware* hw, uint stage, uint bit, int value)
{
    ushort mask = 0;
    switch(stage)
    {
        case DIGITAL_STAGE_0:
            mask = hw->digitalMask1;
            break;
        case DIGITAL_STAGE_1:
            mask = hw->digitalMask2;
            break;
        case DIGITAL_STAGE_2:
            mask = hw->digitalMask3;
            break;
        case DIGITAL_STAGE_3:
            mask = hw->digitalMask4;
            break;
        default:
            return SCOPEFUN_FAILURE;
    };
    if(value)  { mask = raiseFlag16(mask, BIT(bit)); }
    else       { mask = lowerFlag16(mask, BIT(bit)); }
    switch(stage)
    {
        case DIGITAL_STAGE_0:
            hw->digitalMask1 = mask;
            break;
        case DIGITAL_STAGE_1:
            hw->digitalMask2 = mask;
            break;
        case DIGITAL_STAGE_2:
            hw->digitalMask3 = mask;
            break;
        case DIGITAL_STAGE_3:
            hw->digitalMask4 = mask;
            break;
        default:
            return SCOPEFUN_FAILURE;
    };
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetDigitalPattern(SHardware* hw, ushort stage, ushort bit, ushort pattern)
{
    ushort patternA = 0;
    ushort patternB = 0;
    switch(stage)
    {
        case 0:
            patternA = hw->digitalPattern1a;
            patternB = hw->digitalPattern1b;
            break;
        case 1:
            patternA = hw->digitalPattern2a;
            patternB = hw->digitalPattern2b;
            break;
        case 2:
            patternA = hw->digitalPattern3a;
            patternB = hw->digitalPattern3b;
            break;
        case 3:
            patternA = hw->digitalPattern4a;
            patternB = hw->digitalPattern4b;
            break;
        default:
            return SCOPEFUN_FAILURE;
    };
    switch(pattern)
    {
        case DIGITAL_PATTERN_0:       // 00
            patternA = lowerFlag16(patternA, BIT(bit));
            patternB = lowerFlag16(patternB, BIT(bit));
            break;
        case DIGITAL_PATTERN_1:       // 11
            patternA = raiseFlag16(patternA, BIT(bit));
            patternB = raiseFlag16(patternB, BIT(bit));
            break;
        case DIGITAL_PATTERN_RISING:  // 01
            patternA = lowerFlag16(patternA, BIT(bit));
            patternB = raiseFlag16(patternB, BIT(bit));
            break;
        case DIGITAL_PATTERN_FALLING: // 10
            patternA = raiseFlag16(patternA, BIT(bit));
            patternB = lowerFlag16(patternB, BIT(bit));
            break;
        default:
            return SCOPEFUN_FAILURE;
    };
    switch(stage)
    {
        case 0:
            hw->digitalPattern1a = patternA;
            hw->digitalPattern1b = patternB;
            break;
        case 1:
            hw->digitalPattern2a = patternA;
            hw->digitalPattern2b = patternB;
            break;
        case 2:
            hw->digitalPattern3a = patternA;
            hw->digitalPattern3b = patternB;
            break;
        case 3:
            hw->digitalPattern4a = patternA;
            hw->digitalPattern4b = patternB;
            break;
        default:
            return SCOPEFUN_FAILURE;
    };
    return SCOPEFUN_SUCCESS;
}

SCOPEFUN_API int sfSetDataEncodingFormat(SHardware* hw, int encoding)
{
    ushort encodingBits = encoding;
    ushort frameDataSetupWord = hw->frameDataSetup;
    frameDataSetupWord = bitFlag16(frameDataSetupWord, 1 << 0, isFlag16(encodingBits, 1));
    frameDataSetupWord = bitFlag16(frameDataSetupWord, 1 << 1, isFlag16(encodingBits, 2));
    frameDataSetupWord = bitFlag16(frameDataSetupWord, 1 << 2, isFlag16(encodingBits, 4));
    frameDataSetupWord = bitFlag16(frameDataSetupWord, 1 << 3, isFlag16(encodingBits, 8));
    hw->frameDataSetup = frameDataSetupWord;
    return SCOPEFUN_SUCCESS;
}


/*--------------------------------------------------------------------------------
   Get
--------------------------------------------------------------------------------*/

SCOPEFUN_API uint sfGetFrameSize(SHardware*  hw)
{
    int    encoding = hw->frameDataSetup & 0xF;
    uint numSamples = (((uint)hw->sampleSizeH) << 16) | (uint)hw->sampleSizeL;
    uint   dataSize = 0;
    switch(encoding)
    {
        case 15:
            dataSize = numSamples * 4;
            break;
        case 10:
        case 9:
        case 6:
        case 5:
            dataSize = numSamples * 2;
            break;
        default:
            dataSize = numSamples * 4;
            break;
    };
    if(dataSize % 1024 != 0)
    { dataSize = ((dataSize / 1024) + 1) * 1024; }
    uint  frameSize = dataSize + SCOPEFUN_FRAME_HEADER;
    return frameSize;
}

SCOPEFUN_API uint sfGetNumSamples(SHardware* hw)
{
    uint numSamples = (((uint)hw->sampleSizeH) << 16) | (uint)hw->sampleSizeL;
    return numSamples;
}

SCOPEFUN_API ushort sfGetAnalogSwitch(SHardware* hw)
{
    return hw->analogswitch;
}

SCOPEFUN_API int sfGetEts(SHardware* hw)
{
    ushort tmp = hw->analogswitch;
    return isFlag16(tmp, CHANNEL_ETS);
}

SCOPEFUN_API uint sfGetControl(SHardware* hw)
{
    switch(hw->controlData)
    {
        case CONTROLL2_NORMAL:
            return 0;
        case CONTROLL2_TEST:
            return 1;
        case CONTROLL2_RESET:
            return 3;
    };
    return 0;
}

SCOPEFUN_API uint sfGetYGainA(SHardware* hw)
{
    return hw->vgaina;
}

SCOPEFUN_API float sfGetYScaleA(SHardware* hw)
{
    return 0.f;
}
SCOPEFUN_API int sfGetYPositionA(SHardware* hw)
{
    if(hw->offseta > 1500) { return -(int)(65536 - hw->offseta); }
    return (int)hw->offseta;
}

SCOPEFUN_API uint sfGetYGainB(SHardware* hw)
{
    return hw->vgainb;
}
SCOPEFUN_API float sfGetYScaleB(SHardware* hw)
{
    return 0.f;
}
SCOPEFUN_API int sfGetYPositionB(SHardware* hw)
{
    if(hw->offsetb > 1500) { return - (int)(65536 - hw->offsetb); }
    return (int)hw->offsetb;
}

SCOPEFUN_API ushort sfGetTriggerSource(SHardware* hw)
{
    return hw->triggerSource;
}

SCOPEFUN_API ushort sfGetTriggerMode(SHardware* hw)
{
    return hw->triggerMode;
}

SCOPEFUN_API ushort sfGetTriggerSlope(SHardware* hw)
{
    return hw->triggerSlope;
}

SCOPEFUN_API float sfGetTriggerPre(SHardware* hw)
{
    uint lo = hw->sampleSizeL;
    uint hi = hw->sampleSizeH;
    uint sampleSize = lo | (hi << 16);
    uint perTrigger = hw->preTriggerL | (hw->preTriggerH << 16);
    return 100.f * ((float)(perTrigger) / (float)(sampleSize - 1));
}

SCOPEFUN_API int sfGetTriggerHis(SHardware* hw)
{
    return hw->triggerHis;
}

SCOPEFUN_API int sfGetTriggerLevel(SHardware* hw)
{
    return hw->triggerLevel;
}

SCOPEFUN_API ishort sfGetXRange(SHardware* hw)
{
    return hw->xRange;
}

SCOPEFUN_API uint sfGetSampleSize(SHardware* hw)
{
    uint lo = hw->sampleSizeL;
    uint hi = hw->sampleSizeH;
    uint sampleSize = lo | (hi << 16);
    return sampleSize;
}

SCOPEFUN_API uint sfGetHoldoff(SHardware* hw)
{
    uint holdOffInt = (uint)hw->holdoffL | ((uint)hw->holdoffH << 16);
    return holdOffInt;
}

SCOPEFUN_API int sfGetDigitalStart(SHardware* hw)
{
    return (hw->dt_control & 0x300) >> 8;
}

SCOPEFUN_API int sfGetDigitalMode(SHardware* hw)
{
    return (hw->dt_control & 0x10) >> 4;
}

SCOPEFUN_API int sfGetDigitalChannel(SHardware* hw)
{
    return hw->dt_control & 0xF;
}

SCOPEFUN_API ushort sfGetDigitalDelay(SHardware* hw, ushort stage)
{
    switch(stage)
    {
        case DIGITAL_STAGE_0:
            return hw->dt_delayMaxcnt1;
        case DIGITAL_STAGE_1:
            return hw->dt_delayMaxcnt2;
        case DIGITAL_STAGE_2:
            return hw->dt_delayMaxcnt3;
        case DIGITAL_STAGE_3:
            return hw->dt_delayMaxcnt4;
        default:
            return SCOPEFUN_FAILURE;
    };
}

SCOPEFUN_API int sfGetDigitalMask(SHardware* hw, ushort stage, ushort bit)
{
    ushort mask = 0;
    switch(stage)
    {
        case DIGITAL_STAGE_0:
            mask = hw->digitalMask1;
            break;
        case DIGITAL_STAGE_1:
            mask = hw->digitalMask2;
            break;
        case DIGITAL_STAGE_2:
            mask = hw->digitalMask3;
            break;
        case DIGITAL_STAGE_3:
            mask = hw->digitalMask4;
            break;
        default:
            return SCOPEFUN_FAILURE;
    };
    return isFlag16(mask, BIT(bit));
}

SCOPEFUN_API ushort sfGetDigitalPattern(SHardware* hw, ushort stage, ushort bit)
{
    ushort patternA = 0;
    ushort patternB = 0;
    switch(stage)
    {
        case 0:
            patternA = hw->digitalPattern1a;
            patternB = hw->digitalPattern1b;
            break;
        case 1:
            patternA = hw->digitalPattern2a;
            patternB = hw->digitalPattern2b;
            break;
        case 2:
            patternA = hw->digitalPattern3a;
            patternB = hw->digitalPattern3b;
            break;
        case 3:
            patternA = hw->digitalPattern4a;
            patternB = hw->digitalPattern4b;
            break;
        default:
            return SCOPEFUN_FAILURE;
    };
    int bitA = isFlag16(patternA, BIT(bit));
    int bitB = isFlag16(patternB, BIT(bit));
    if(bitA &&  bitB)
    {
        return DIGITAL_PATTERN_1;
    }
    if(!bitA && !bitB)
    {
        return DIGITAL_PATTERN_0;
    }
    if(!bitA &&  bitB)
    {
        return DIGITAL_PATTERN_RISING;
    }
    if(bitA && !bitB)
    {
        return DIGITAL_PATTERN_FALLING;
    }
    return DIGITAL_PATTERN_1;
}

/*--------------------------------------------------------------------------------
   generator 0
--------------------------------------------------------------------------------*/

SCOPEFUN_API ushort sfGetGeneratorType0(SHardware* hw)
{
    uint mask = 0xF;
    return (ushort)((hw->generatorType0) & mask);
}

SCOPEFUN_API int sfGetGeneratorOn0(SHardware* hw)
{
    uint mask = 1;
    return (hw->generatorType0 >> 8) & mask;
}

SCOPEFUN_API int sfGetGeneratorVoltage0(SHardware* hw)
{
    return hw->generatorVoltage0;
}

SCOPEFUN_API int sfGetGeneratorOffset0(SHardware* hw)
{
    return hw->generatorOffset0;
}

SCOPEFUN_API double sfGetGeneratorFrequency0(SHardware* hw, float fs)
{
    uint gDelta = (uint)(hw->generatorDeltaL0) | (uint)(hw->generatorDeltaH0 << 16);
    double delta = BinaryToDouble(gDelta);
    return delta * (double)(fs) / (2 * 16384.0);
}

SCOPEFUN_API int sfGetGeneratorSquareDuty0(SHardware* hw)
{
    return hw->generatorSquareDuty0;
}

SCOPEFUN_API int sfGetGeneratorPhase0(SHardware* hw)
{
    return hw->generatorPhase0;
}

SCOPEFUN_API uint sfGetGeneratorDelta0(SHardware* hw)
{
    uint gDelta = (uint)(hw->generatorDeltaL0) | (uint)(hw->generatorDeltaH0 << 16);
    return gDelta;
}

/*--------------------------------------------------------------------------------
   generator 1
--------------------------------------------------------------------------------*/

SCOPEFUN_API ushort sfGetGeneratorType1(SHardware* hw)
{
    uint mask = 0xF;
    return (ushort)((hw->generatorType1) & mask);
}

SCOPEFUN_API int sfGetGeneratorOn1(SHardware* hw)
{
    uint mask = 1;
    return (hw->generatorType1 >> 8) & mask;
}

SCOPEFUN_API int sfGetGeneratorVoltage1(SHardware* hw)
{
    uint mask = 1 << 12;
    return hw->generatorVoltage1;
}

SCOPEFUN_API int sfGetGeneratorOffset1(SHardware* hw)
{
    return hw->generatorOffset1;
}

SCOPEFUN_API double sfGetGeneratorFrequency1(SHardware* hw, float fs)
{
   uint gDelta = (uint)(hw->generatorDeltaL1) | (uint)(hw->generatorDeltaH1 << 16);
   double delta = BinaryToDouble(gDelta);
   return delta * (double)(fs) / (2 * 16384.0);
}

SCOPEFUN_API int sfGetGeneratorSquareDuty1(SHardware* hw)
{
    return hw->generatorSquareDuty1;
}

SCOPEFUN_API int sfGetGeneratorPhase1(SHardware* hw)
{
    return hw->generatorPhase1;
}

SCOPEFUN_API uint sfGetGeneratorDelta1(SHardware* hw)
{
    uint gDelta = (uint)(hw->generatorDeltaL1) | (uint)(hw->generatorDeltaH1 << 16);
    return gDelta;
}

/*--------------------------------------------------------------------------------
   digital
--------------------------------------------------------------------------------*/

SCOPEFUN_API double sfGetDigitalVoltage(SHardware* hw, double kDigital)
{
    double rab  = 50000;
    double rw   = 75;
    double rwb1 = 29763;
    double    a = hw->digitalVoltage;
    double rwb0    = (rab*a / 256.0) + rw;
    double voltage = 1.235*((rwb0/rwb1) + 1.0);
    return voltage;
}

SCOPEFUN_API int sfGetDigitalInputOutput11to6(SHardware* hw)
{
    int ret = (hw->digitalInputOutput >> 1) & 0x1;
    return ret;
}

SCOPEFUN_API int sfGetDigitalInputOutput5to0(SHardware* hw)
{
    return hw->digitalInputOutput & 0x1;
}

SCOPEFUN_API int sfGetDigitalOutputBit(SHardware* hw, int bit)
{
    ushort bits = 0;
    bits = hw->digitalOutputBit;
    return isFlag16(bits, BIT(bit));
}

SCOPEFUN_API uint sfGetDigitalClockDivide(SHardware* hw)
{
    return (uint)(hw->digitalClkDivideL) | (uint)(hw->digitalClkDivideH << 16);
}

SCOPEFUN_API int sfGetDigitalPatternOutputEn(SHardware* hw)
{
    int ret = (hw->digitalInputOutput >> 8) & 0x1;
    return ret;
}

SCOPEFUN_API int sfGetDigitalPatternOutputMode(SHardware* hw)
{
    int ret = (hw->digitalInputOutput >> 9) & 0x1;
    return ret;
}

SCOPEFUN_API int sfGetDigitalPatternOutputRestart(SHardware* hw)
{
    int ret = (hw->digitalInputOutput >> 10) & 0x1;
    return ret;
}

SCOPEFUN_API int sfGetDigitalPatternOutputRestartOnUpload(SHardware* hw)
{
    int ret = (hw->digitalInputOutput >> 11) & 0x1;
    return ret;
}

SCOPEFUN_API int sfGetAverage(SHardware* hw)
{
    return hw->average;
}

/*--------------------------------------------------------------------------------
   data format
--------------------------------------------------------------------------------*/
SCOPEFUN_API int sfGetDataEncodingFormat(SHardware* hw)
{
    int encoding = hw->frameDataSetup & 0xF;
    return encoding;
}
