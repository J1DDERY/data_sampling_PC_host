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
#include <scopefun/ScopeFun.h>
#include <array>
#include <algorithm>
////////////////////////////////////////////////////////////////////////////////
//
// SoftwareGeneratorChannel
//
////////////////////////////////////////////////////////////////////////////////
SoftwareGeneratorChannel::SoftwareGeneratorChannel()
{
    Default();
}

void SoftwareGeneratorChannel::Default()
{
    type = GENERATE_SINUS;
    period = 50.f * MICRO;
    peakToPeak = 17.f;
    speed = 0.01f;
    every = 0.0f;
    onOff = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// WndSoftwareGenerator
//
////////////////////////////////////////////////////////////////////////////////
WndSoftwareGenerator::WndSoftwareGenerator()
{
}

void WndSoftwareGenerator::Default()
{
    channel[0].Default();
    channel[1].Default();
    channel[1].peakToPeak = 12.f;
    channel[1].speed      = 0.05f;
    channel[1].peakToPeak = 12.f;
    channel[1].period     = 42.f * MICRO;
}


////////////////////////////////////////////////////////////////////////////////
//
// WndHardwareGenerator
//
////////////////////////////////////////////////////////////////////////////////

WndHardwareGenerator::WndHardwareGenerator()
{
    Default();
}

void WndHardwareGenerator::loadCustomAwg(int idx, const char* path)
{
    char*  memory = 0;
    ilarge memorySize = 0;
    int ret = fileLoadString(path, &memory, &memorySize);
    if(ret)
    {
        return;
    }
    cJSON* root = cJSON_Parse(memory);
    if(!root)
    {
        const char* error = cJSON_GetErrorPtr();
        return;
    }
    cJSON* arrayValues = cJSON_GetObjectItem(root, "signal");
    cJSON* val = cJSON_GetArrayItem(arrayValues, 0);
    for(uint i = 0; i < GENERATOR_ANALOG_SAMPLES; i++)
    {
        if(!arrayValues)
        {
            continue;
        }
        if(!val)
        {
            continue;
        }
        if(idx == 0)
        {
            wndCustomAwg1.type = GENERATOR_TYPE_ANALOG;
            wndCustomAwg1.length = GENERATOR_ANALOG_SAMPLES;
            wndCustomAwg1.data.bytes[i] = (ushort&)val->valueint;
        }
        if(idx == 1)
        {
            wndCustomAwg2.type = GENERATOR_TYPE_ANALOG;
            wndCustomAwg2.length = GENERATOR_ANALOG_SAMPLES;
            wndCustomAwg2.data.bytes[i] = (ushort&)val->valueint;
        }
        if(val) { val = val->next; }
    }
    cJSON_Delete(root);
    fileFree(memory);
}

void WndHardwareGenerator::loadCustomDigital(const char* path)
{
    char*  memory = 0;
    ilarge memorySize = 0;
    int ret = fileLoadString(path, &memory, &memorySize);
    if(ret)
    {
        return;
    }
    cJSON* root = cJSON_Parse(memory);
    if(!root)
    {
        const char* error = cJSON_GetErrorPtr();
        return;
    }
    FORMAT_BUFFER();
    FORMAT("digitalChannel%d", 0);
    cJSON* arrayValues0 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 1);
    cJSON* arrayValues1 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 2);
    cJSON* arrayValues2 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 3);
    cJSON* arrayValues3 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 4);
    cJSON* arrayValues4 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 5);
    cJSON* arrayValues5 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 6);
    cJSON* arrayValues6 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 7);
    cJSON* arrayValues7 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 8);
    cJSON* arrayValues8 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 9);
    cJSON* arrayValues9 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 10);
    cJSON* arrayValues10 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 11);
    cJSON* arrayValues11 = cJSON_GetObjectItem(root, formatBuffer);
    cJSON* val0 = 0;
    cJSON* val1 = 0;
    cJSON* val2 = 0;
    cJSON* val3 = 0;
    cJSON* val4 = 0;
    cJSON* val5 = 0;
    cJSON* val6 = 0;
    cJSON* val7 = 0;
    cJSON* val8 = 0;
    cJSON* val9 = 0;
    cJSON* val10 = 0;
    cJSON* val11 = 0;
    std::array<int, 12> len = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if(arrayValues0)
    {
        val0 = cJSON_GetArrayItem(arrayValues0, 0);
        len[0] = cJSON_GetArraySize(arrayValues0);
    }
    if(arrayValues1)
    {
        val1 = cJSON_GetArrayItem(arrayValues1, 0);
        len[1] = cJSON_GetArraySize(arrayValues1);
    }
    if(arrayValues2)
    {
        val2 = cJSON_GetArrayItem(arrayValues2, 0);
        len[2] = cJSON_GetArraySize(arrayValues2);
    }
    if(arrayValues3)
    {
        val3 = cJSON_GetArrayItem(arrayValues3, 0);
        len[3] = cJSON_GetArraySize(arrayValues3);
    }
    if(arrayValues4)
    {
        val4 = cJSON_GetArrayItem(arrayValues4, 0);
        len[4] = cJSON_GetArraySize(arrayValues4);
    }
    if(arrayValues5)
    {
        val5 = cJSON_GetArrayItem(arrayValues5, 0);
        len[5] = cJSON_GetArraySize(arrayValues5);
    }
    if(arrayValues6)
    {
        val6 = cJSON_GetArrayItem(arrayValues6, 0);
        len[6] = cJSON_GetArraySize(arrayValues6);
    }
    if(arrayValues7)
    {
        val7 = cJSON_GetArrayItem(arrayValues7, 0);
        len[7] = cJSON_GetArraySize(arrayValues7);
    }
    if(arrayValues8)
    {
        val8 = cJSON_GetArrayItem(arrayValues8, 0);
        len[8] = cJSON_GetArraySize(arrayValues8);
    }
    if(arrayValues9)
    {
        val9 = cJSON_GetArrayItem(arrayValues9, 0);
        len[9] = cJSON_GetArraySize(arrayValues9);
    }
    if(arrayValues10)
    {
        val10 = cJSON_GetArrayItem(arrayValues10, 0);
        len[10] = cJSON_GetArraySize(arrayValues10);
    }
    if(arrayValues11)
    {
        val11 = cJSON_GetArrayItem(arrayValues11, 0);
        len[11] = cJSON_GetArraySize(arrayValues11);
    }
    int maxLen = *std::max_element(len.begin(), len.end());
    for(uint i = 0; i < GENERATOR_DIGITAL_SAMPLES; i++)
    {
        ushort bits = 0;
        if(val0)
        {
            bits = bits | (val0->valueint << 0);
        }
        if(val1)
        {
            bits = bits | (val1->valueint << 1);
        }
        if(val2)
        {
            bits = bits | (val2->valueint << 2);
        }
        if(val3)
        {
            bits = bits | (val3->valueint << 3);
        }
        if(val4)
        {
            bits = bits | (val4->valueint << 4);
        }
        if(val5)
        {
            bits = bits | (val5->valueint << 5);
        }
        if(val6)
        {
            bits = bits | (val6->valueint << 6);
        }
        if(val7)
        {
            bits = bits | (val7->valueint << 7);
        }
        if(val8)
        {
            bits = bits | (val8->valueint << 8);
        }
        if(val9)
        {
            bits = bits | (val9->valueint << 9);
        }
        if(val10)
        {
            bits = bits | (val10->valueint << 10);
        }
        if(val11)
        {
            bits = bits | (val11->valueint << 11);
        }

        wndCustomDigital.type = GENERATOR_TYPE_DIGITAL;
        wndCustomDigital.length = GENERATOR_DIGITAL_SAMPLES;
        wndCustomDigital.data.bytes[i] = bits;
        if(val0) { val0  = val0->next; }
        if(val1) { val1  = val1->next; }
        if(val2) { val2  = val2->next; }
        if(val3) { val3  = val3->next; }
        if(val4) { val4  = val4->next; }
        if(val5) { val5  = val5->next; }
        if(val6) { val6  = val6->next; }
        if(val7) { val7  = val7->next; }
        if(val8) { val8  = val8->next; }
        if(val9) { val9  = val9->next; }
        if(val10) { val10 = val10->next; }
        if(val11) { val11 = val11->next; }
    }
    cJSON_Delete(root);
    fileFree(memory);
}

// TODO: Upload custom signal
void WndHardwareGenerator::uploadAwg(int idx)
{
    if (idx == 0) {
        pOsciloscope->thread.generatorConfig.GeneratorCustomDataMap = 1; // AWG1
        pOsciloscope->thread.setGeneratorData(&wndCustomAwg1);
    }
    else {
        pOsciloscope->thread.generatorConfig.GeneratorCustomDataMap = 2; // AWG2
        pOsciloscope->thread.setGeneratorData(&wndCustomAwg2);
    }
    pOsciloscope->thread.function(afUploadGenerator);
}

void WndHardwareGenerator::uploadDigital()
{
    pOsciloscope->thread.generatorConfig.GeneratorCustomDataMap = 3; // Digital
    pOsciloscope->thread.setGeneratorData(&wndCustomDigital);
    pOsciloscope->thread.function(afUploadGenerator);
}

void WndHardwareGenerator::Default()
{
    // generator 0
    type0       = GENERATOR_SIN;
    frequency0  = 1000.f;
    voltage0    = 1024;
    offset0     = 0;
    phase0      = 0.f;
    squareDuty0 = 50;
    onOff0 = 0;
    // generator 1
    type1       = GENERATOR_SIN;
    frequency1  = 1000.f;
    voltage1    = 1024;
    offset1     = 0;
    phase1      = 0.f;
    squareDuty1 = 50;
    onOff1 = 0;
    // customData
    memset(&wndCustomAwg1,    0, sizeof(wndCustomAwg1));
    memset(&wndCustomAwg2,    0, sizeof(wndCustomAwg2));
    memset(&wndCustomDigital, 0, sizeof(wndCustomDigital));
}


////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
