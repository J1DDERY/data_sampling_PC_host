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
#include <scopefun/ScopeFun.h>


////////////////////////////////////////////////////////////////////////////////
//
// WndStorage
//
////////////////////////////////////////////////////////////////////////////////
WndStorage::WndStorage()
{
    Default();
}

void WndStorage::Default()
{
    type   = mtRAM;
    size   = 1024;
    packet = ptAutomatic;
}

ularge WndStorage::getPacketSize(int version)
{
    switch(packet)
    {
        case ptAutomatic:
            if(version == 1)
            {
                return  131072;
            }
            if(version == 2)
            {
                return 1048576;
            }
            break;
        case pt512:
            return 512;
        case pt16384:
            return 16384;
        case pt131072:
            return 131072;
        case pt1048576:
            return 1048576;
    };
    return 16384;
}

////////////////////////////////////////////////////////////////////////////////
//
// MWProgress
//
////////////////////////////////////////////////////////////////////////////////
MWProgress::MWProgress()
{
    uiActive  = 0;
    uiValue   = 0;
    uiRange   = 0;
    uiPulse   = 0;
    uiText    = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// MWVertical
//
////////////////////////////////////////////////////////////////////////////////
MWVertical::MWVertical()
{
    Default();
}

void MWVertical::Default()
{
    Capture   = 2.f;
    Scale     = 0.f;
    Display   = 2.f;
    YPosition = 0;
    OscOnOff  = 1;
    FFTOnOff  = 0;
    Invert = 0;
    Ground = 0;
    AcDc   = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// MWFunction
//
////////////////////////////////////////////////////////////////////////////////
MWFunction::MWFunction()
{
    Default();
}

void MWFunction::Default()
{
    Type = 0;
    OscOnOff = 0;
    FFTOnOff = 0;
    xyGraph = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// MWHorizontal
//
////////////////////////////////////////////////////////////////////////////////
MWHorizontal::MWHorizontal()
{
    Default();
}

void MWHorizontal::Default()
{
    Capture  = 0.00000001f;
    Display  = 0.00000001f;
    Position = 0.f;
    Mode     = SIGNAL_MODE_PAUSE;
    Control  = 0;
    Frame = 0;
    FrameSize = NUM_SAMPLES;
    FFTSize   = NUM_SAMPLES;
    ETS = 0;
    Full = 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// MWTrigger
//
////////////////////////////////////////////////////////////////////////////////
MWTrigger::MWTrigger()
{
    Default();
}
void MWTrigger::Default()
{
    Source  = 0;
    Slope   = 0;
    Mode    = 0;
    Level   = 0;
    His     = 0;
    Percent = 0;
    Holdoff = 0;
    //
    stage = 0;
    stageStart = 0;
    stageMode = 0;
    stageChannel = 0;
    for(int i = 0; i < 4; i++)
    {
        delay[i] = 0;
        mask[i].setCount(16);
        pattern[i].setCount(16);
        for(int j = 0; j < 16; j++)
        {
            mask[i][j] = 0;
            pattern[i][j] = 0;
        }
    }
}




////////////////////////////////////////////////////////////////////////////////
//
// MWMeasure
//
////////////////////////////////////////////////////////////////////////////////
MeasureData::MeasureData()
{
    Clear();
}

void MeasureData::Clear()
{
    memset(&pick,    0, sizeof(pick));
    memset(&column,  0, sizeof(column));
    memset(&history, 0, sizeof(history));
    historyCnt = 0;
    historyIndex = 0;
}

void MeasureData::Average(MeasureChannelData& output)
{
    if (historyCnt < MEASURE_HISTORY_MAX)
    {
        historyCnt++;
    }
    for(int i = 0; i < Last1; i++)
    {
        double dRow = 0;
        for(int historyIdx = 0; historyIdx < historyCnt; historyIdx++)
        {
            dRow += history[historyIdx].row[i];
        }
        output.row[i] = dRow / historyCnt;
    }
    if (historyIndex == (MEASURE_HISTORY_MAX - 1))
    {
        historyIndex = 0;
    }
    else
    {
        historyIndex++;
    }
}

double MeasurePickData::RowD(EValueGrid2 r)
{
    if(r < Last2)
    {
        return row[r];
    }
    return 0;
}

ushort MeasurePickData::RowU(EValueGrid2 r)
{
    if(r == DigitalX0)
    {
        return DigitalX0;
    }
    if(r == DigitalX1)
    {
        return DigitalX1;
    }
    return 0;
}

double MeasureChannelData::Row(EValueGrid1 r)
{
    if(r < Last1)
    {
        return row[r];
    }
    return 0;
}

void MeasureChannelData::SetValue(double value)
{
    for(int i = 0; i < Last1; i++)
    {
        row[i] = value;
    }
}

void MeasureChannelData::Minimum(MeasureChannelData& other)
{
    for(int i = 0; i < Last1; i++)
    {
        row[i] = min(row[i], other.row[i]);
    }
}

void MeasureChannelData::Maximum(MeasureChannelData& other)
{
    for(int i = 0; i < Last1; i++)
    {
        row[i] = max(row[i], other.row[i]);
    }
}

void MeasureChannelData::ClearData()
{
    averageN   = 0;
    for(int i = 0; i < Last1; i++)
    {
        row[i] = 0;
    }
}

uint MeasurePos::getXIndex(double sx, double sz, uint frameSize)
{
    double xMin = (sx + 0.5 - 0.5 * sz);
    double xMax = (sx + 0.5 + 0.5 * sz);
    return uint((xMin + (x * (xMax - xMin))) * double((frameSize - 1)));
}

void MeasurePos::setXTime(double sx,double sz,double time)
{
    xTime = time;
    double maxTime = double(pOsciloscope->window.horizontal.Capture) * double(pOsciloscope->window.horizontal.FrameSize);
       //double xMin = (sx - 0.5 - 0.5*sz);
       //double xMax = (sx - 0.5 + 0.5*sz);
                 x  = time / maxTime;
              //    x  =  (x - xMin)/(xMax-xMin);
                 x += double(pOsciloscope->window.trigger.Percent / 100.0);
}

void MeasurePos::setXFreq(double freq)
{
    xFreq = freq;
    double      time = pOsciloscope->window.horizontal.Capture;
    double  maxHertz = (1.f / time) * 0.5f;
    iint       szFFT = pOsciloscope->window.horizontal.FFTSize;
    int       szBins = szFFT / 2;
    if(pOsciloscope->window.display.fftLogFreq > 0)
    {
        double norm  = freq / maxHertz;
        double hertz = norm * szBins;
        x = log2(hertz) / log2(szBins);
    }
    else
    {
        x = double(freq) / double(maxHertz);
    }
}

void MeasurePos::setYVolt(double volt, int channel)
{
    switch(channel)
    {
        case 0:
            yVolt = volt;
            y = volt / (double(pOsciloscope->window.channel01.Capture) * double(pOsciloscope->grid.yCount));
            break;
        case 1:
            yVolt = volt;
            y = volt / (double(pOsciloscope->window.channel02.Capture) * double(pOsciloscope->grid.yCount));
            break;
    };
}

double MeasurePos::getXTime(double sx,double sz)
{
    double maxTime = double(pOsciloscope->window.horizontal.Capture) * double(pOsciloscope->window.horizontal.FrameSize);
    double tMin        = (sx - 0.5*sz) * maxTime;
    double tMax        = (sx + 0.5*sz) * maxTime;
                xTime  = tMin + x*(tMax-tMin) - maxTime * (double(pOsciloscope->window.trigger.Percent) / 100.0);
    return xTime;
}

double freqFromPosition(int index, int maxIndex, double maxHertz);

double MeasurePos::getXFreq()
{
    double      time = pOsciloscope->window.horizontal.Capture;
    double  maxHertz = (1.f / time) * 0.5f;
    iint       szFFT = pOsciloscope->window.horizontal.FFTSize;
    int       szBins = szFFT / 2;
    if(pOsciloscope->window.display.fftLogFreq > 0)
    {
        double pos = (x);
        double hertz = float(pos) * maxHertz;
        hertz = pow(szBins, pos);
        double norm = hertz / szBins;
        hertz = norm * maxHertz;
        xFreq = hertz;
    }
    else
    {
        xFreq = x * double(maxHertz);
    }
    return xFreq;
}

double MeasurePos::getYVolt(int channel)
{
    switch(channel)
    {
        case 0:
            yVolt = y * double(pOsciloscope->window.channel01.Capture) * double(pOsciloscope->grid.yCount);
            break;
        case 1:
            yVolt = y * double(pOsciloscope->window.channel02.Capture) * double(pOsciloscope->grid.yCount);
            break;
    };
    return yVolt;
}

float MeasurePos::FrameToZ(int iframe)
{
    int maximum = pOsciloscope->settings.getSettings()->historyFrameCount; // todo: historyFrameDisplay
    return float(iframe % maximum) / float(maximum - 1);
}

int MeasurePos::ZToFrame(float fz)
{
    int maximum = pOsciloscope->settings.getSettings()->historyFrameCount; // todo: historyFrameDisplay
    return int(fz * float(maximum - 1));
}

void MeasurePos::setZ(int frame)
{
    int maximum = pOsciloscope->settings.getSettings()->historyFrameCount; // todo: historyFrameDisplay
    z = float(frame % maximum) / float(maximum - 1);
}

int MeasurePos::getZ()
{
    int maximum = pOsciloscope->settings.getSettings()->historyFrameCount; // todo: historyFrameDisplay
    return int(z * float(maximum - 1));
}

MWMeasure::MWMeasure()
{
    Default();
}

void MWMeasure::Default()
{
    uiOpen = 0;
    data.historyCnt = 0;
    data.historyIndex = 0;
    automaticDataClear = true;
    automaticDataClearTrigger = false;
    ClearCapture();
}

////////////////////////////////////////////////////////////////////////////////
//
// WndMain
//
////////////////////////////////////////////////////////////////////////////////
WndMain::WndMain()
{
    Default();
}

void WndMain::Default()
{
    speed = USB_SPEED_AUTOMATIC;
    display.Default();
    thermal.Default();
    storage.Default();
    measure.Default();
    channel01.Default();
    channel02.Default();
    function.Default();
    horizontal.Default();
    trigger.Default();
    fftDigital.raise(VIEW_SELECT_OSC_2D);
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
