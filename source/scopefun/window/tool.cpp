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

////////////////////////////////////////////////////////////////////////////////
//
// Basic
//
////////////////////////////////////////////////////////////////////////////////
float multiplyerFromEnum(int enumerated)
{
    switch(enumerated)
    {
        case 0:
            return 1.f;
        case 1:
            return MILI;
        case 2:
            return MICRO;
        case 3:
            return NANO;
    };
    return 0.f;
}

double multiplyerDoubleFromValue(double value)
{
    value = fabs(value);
    if(value < NANO)
    {
        return 1.0 / NANO;
    }
    if(value < MICRO)
    {
        return 1.0 / MICRO;
    }
    if(value < MILI)
    {
        return 1.0 / MILI;
    }
    if(value > GIGA)
    {
        return 1.0 / GIGA;
    }
    if(value > MEGA)
    {
        return 1.0 / MEGA;
    }
    if(value > KILO)
    {
        return 1.0 / KILO;
    }
    return 1.0;
}

int multiplyerIndexFromValue(double value)
{
    value = fabs(value);
    if(value < NANO)
    {
        return  -3;
    }
    if(value < MICRO)
    {
        return  -2;
    }
    if(value < MILI)
    {
        return  -1;
    }
    if(value > GIGA)
    {
        return  3;
    }
    if(value > MEGA)
    {
        return  2;
    }
    if(value > KILO)
    {
        return  1;
    }
    return 0;
}

uint  multiplyerFromValue(float value)
{
    if(value < MICRO)
    {
        return 3;
    }
    if(value < MILI)
    {
        return 2;
    }
    if(value < 1.f)
    {
        return 1;
    }
    return 0;
}

const char* captureTimeToStr(int enumerated)
{
    int version = pOsciloscope->thread.getVersion();
    if(version == 1)
    {
        switch(enumerated)
        {
            case tc10ns:
                return "10ns";
            case tc20ns:
                return "20ns";
            case tc50ns:
                return "50ns";
            case tc100ns:
                return "100ns";
            case tc200ns:
                return "200ns";
            case tc500ns:
                return "500ns";
            case tc1us:
                return "1us";
            case tc2us:
                return "2us";
            case tc5us:
                return "5us";
            case tc10us:
                return "10us";
            case tc20us:
                return "20us";
            case tc50us:
                return "50us";
            case tc100us:
                return "100us";
            case tc200us:
                return "200us";
            case tc500us:
                return "500us";
            case tc1ms:
                return "1ms";
            case tc2ms:
                return "2ms";
            case tc5ms:
                return "5ms";
            case tc10ms:
                return "10ms";
            case tc20ms:
                return "20ms";
            case tc50ms:
                return "50ms";
            case tc100ms:
                return "100ms";
            case tc200ms:
                return "200ms";
            case tc500ms:
                return "500ms";
            case tc1s:
                return "1s";
        };
    }
    if(version == 2)
    {
        switch(enumerated)
        {
            case t2c2ns:
                return "2ns";
            case t2c4ns:
                return "4ns";
            case t2c8ns:
                return "8ns";
            case t2c20ns:
                return "20ns";
            case t2c40ns:
                return "40ns";
            case t2c80ns:
                return "80ns";
            case t2c200ns:
                return "200ns";
            case t2c400ns:
                return "400ns";
            case t2c800ns:
                return "800ns";
            case t2c2us:
                return "2us";
            case t2c4us:
                return "4us";
            case t2c8us:
                return "8us";
            case t2c20us:
                return "20us";
            case t2c40us:
                return "40us";
            case t2c80us:
                return "80us";
            case t2c200us:
                return "200us";
            case t2c400us:
                return "400us";
            case t2c800us:
                return "800us";
            case t2c2ms:
                return "2ms";
            case t2c4ms:
                return "4ms";
            case t2c8ms:
                return "8ms";
            case t2c20ms:
                return "20ms";
        };
    }
    return "invalid";
}

double captureTimeFromEnumV1(int enumerated)
{
    switch(enumerated)
    {
        case tc10ns:
            return 10.0 * DOUBLE_NANO;
        case tc20ns:
            return 20.0 * DOUBLE_NANO;
        case tc50ns:
            return 50.0 * DOUBLE_NANO;
        case tc100ns:
            return 100.0 * DOUBLE_NANO;
        case tc200ns:
            return 200.0 * DOUBLE_NANO;
        case tc500ns:
            return 500.0 * DOUBLE_NANO;
        case tc1us:
            return 1.0 * DOUBLE_MICRO;
        case tc2us:
            return 2.0 * DOUBLE_MICRO;
        case tc5us:
            return 5.0 * DOUBLE_MICRO;
        case tc10us:
            return 10.0 * DOUBLE_MICRO;
        case tc20us:
            return 20.0 * DOUBLE_MICRO;
        case tc50us:
            return 50.0 * DOUBLE_MICRO;
        case tc100us:
            return 100.0 * DOUBLE_MICRO;
        case tc200us:
            return 200.0 * DOUBLE_MICRO;
        case tc500us:
            return 500.0 * DOUBLE_MICRO;
        case tc1ms:
            return 1.0 * DOUBLE_MILI;
        case tc2ms:
            return 2.0 * DOUBLE_MILI;
        case tc5ms:
            return 5.0 * DOUBLE_MILI;
        case tc10ms:
            return 10.0 * DOUBLE_MILI;
        case tc20ms:
            return 20.0 * DOUBLE_MILI;
        case tc50ms:
            return 50.0 * DOUBLE_MILI;
        case tc100ms:
            return 100.0 * DOUBLE_MILI;
        case tc200ms:
            return 200.0 * DOUBLE_MILI;
        case tc500ms:
            return 500.0 * DOUBLE_MILI;
        case tc1s:
            return 1.0;
    };
    return 0.0;
}

double captureTimeFromEnumV2(int enumerated)
{
    switch(enumerated)
    {
        case t2c2ns:
            return 2.0 * DOUBLE_NANO;
        case t2c4ns:
            return 4.0 * DOUBLE_NANO;
        case t2c8ns:
            return 8.0 * DOUBLE_NANO;
        case t2c20ns:
            return 20.0 * DOUBLE_NANO;
        case t2c40ns:
            return 40.0 * DOUBLE_NANO;
        case t2c80ns:
            return 80.0 * DOUBLE_NANO;
        case t2c200ns:
            return 200.0 * DOUBLE_NANO;
        case t2c400ns:
            return 400.0 * DOUBLE_NANO;
        case t2c800ns:
            return 800.0 * DOUBLE_NANO;
        case t2c2us:
            return 2.0 * DOUBLE_MICRO;
        case t2c4us:
            return 4.0 * DOUBLE_MICRO;
        case t2c8us:
            return 8.0 * DOUBLE_MICRO;
        case t2c20us:
            return 20.0 * DOUBLE_MICRO;
        case t2c40us:
            return 40.0 * DOUBLE_MICRO;
        case t2c80us:
            return 80.0 * DOUBLE_MICRO;
        case t2c200us:
            return 200.0 * DOUBLE_MICRO;
        case t2c400us:
            return 400.0 * DOUBLE_MICRO;
        case t2c800us:
            return 800.0 * DOUBLE_MICRO;
        case t2c2ms:
            return 2.0 * DOUBLE_MILI;
        case t2c4ms:
            return 4.0 * DOUBLE_MILI;
        case t2c8ms:
            return 8.0 * DOUBLE_MILI;
        case t2c20ms:
            return 20.0 * DOUBLE_MILI;
    };
    return 0.0;
}

double captureTimeFromEnumVersion(int enumerated, int version)
{
    if(version == 1)
    {
        return captureTimeFromEnumV1(enumerated);
    }
    if(version == 2)
    {
        return captureTimeFromEnumV2(enumerated);
    }
    return 0.0;
}


uint captureTimeMaxReceive(int enumerated, int version)
{
    double captureTimeMax  = 1.0;
    double captureSamples  = captureTimeMax / captureTimeFromEnumVersion(enumerated, version);
    double minFrameSamples = 1024.0;
    uint    toReceiveBytes = max<uint>(minFrameSamples, captureSamples);
    if(version == 1) { toReceiveBytes *= 6; }
    if(version == 2) { toReceiveBytes *= 4; }
    return toReceiveBytes;
}

ECalibrationType getCalibrationType(int enumerated)
{
    int version = pOsciloscope->thread.getVersion();
    if(version == 2 && enumerated == t2c2ns)
    {
        return ct500Mhz;
    }
    return ctNormal;
}

ECalibrationType getCalibrationType(float value)
{
    int version = pOsciloscope->thread.getVersion();
    if(version == 2 && captureTimeFromValue(value) == t2c2ns)
    {
        return ct500Mhz;
    }
    return ctNormal;
}

double captureTimeFromEnum(int enumerated)
{
    int version = pOsciloscope->thread.getVersion();
    return captureTimeFromEnumVersion(enumerated, version);
}

uint captureTimeFromValue(float value)
{
    int version = pOsciloscope->thread.getVersion();
    if(version == 1)
    {
        if(value < 20.f * NANO)
        {
            return tc10ns;
        }
        if(value < 50.f * NANO)
        {
            return tc20ns;
        }
        if(value < 100.f * NANO)
        {
            return tc50ns;
        }
        if(value < 200.f * NANO)
        {
            return tc100ns;
        }
        if(value < 500.f * NANO)
        {
            return tc200ns;
        }
        if(value < 1.f * MICRO)
        {
            return tc500ns;
        }
        if(value < 2.f * MICRO)
        {
            return tc1us;
        }
        if(value < 5.f * MICRO)
        {
            return tc2us;
        }
        if(value < 10.f * MICRO)
        {
            return tc5us;
        }
        if(value < 20.f * MICRO)
        {
            return tc10us;
        }
        if(value < 50.f * MICRO)
        {
            return tc20us;
        }
        if(value < 100.f * MICRO)
        {
            return tc50us;
        }
        if(value < 200.f * MICRO)
        {
            return tc100us;
        }
        if(value < 500.f * MICRO)
        {
            return tc200us;
        }
        if(value < 1.f * MILI)
        {
            return tc500us;
        }
        if(value < 2.f * MILI)
        {
            return tc1ms;
        }
        if(value < 5.f * MILI)
        {
            return tc2ms;
        }
        if(value < 10.f * MILI)
        {
            return tc5ms;
        }
        if(value < 20.f * MILI)
        {
            return tc10ms;
        }
        if(value < 50.f * MILI)
        {
            return tc20ms;
        }
        if(value < 100.f * MILI)
        {
            return tc50ms;
        }
        if(value < 200.f * MILI)
        {
            return tc100ms;
        }
        if(value < 500.f * MILI)
        {
            return tc200ms;
        }
        if(value < 1.f)
        {
            return tc500ms;
        }
        return tc1s;
    }
    if(version == 2)
    {
        if(value < 4.f * NANO)
        {
            return t2c2ns;
        }
        if(value < 8.f * NANO)
        {
            return t2c4ns;
        }
        if(value < 20.f * NANO)
        {
            return t2c8ns;
        }
        if(value < 40.f * NANO)
        {
            return t2c20ns;
        }
        if(value < 80.f * NANO)
        {
            return t2c40ns;
        }
        if(value < 200.f * NANO)
        {
            return t2c80ns;
        }
        if(value < 400.f * NANO)
        {
            return t2c200ns;
        }
        if(value < 800.f * NANO)
        {
            return t2c400ns;
        }
        if(value < 2.f * MICRO)
        {
            return t2c800ns;
        }
        if(value < 4.f * MICRO)
        {
            return t2c2us;
        }
        if(value < 8.f * MICRO)
        {
            return t2c4us;
        }
        if(value < 20.f * MICRO)
        {
            return t2c8us;
        }
        if(value < 40.f * MICRO)
        {
            return t2c20us;
        }
        if(value < 80.f * MICRO)
        {
            return t2c40us;
        }
        if(value < 200.f * MICRO)
        {
            return t2c80us;
        }
        if(value < 400.f * MICRO)
        {
            return t2c200us;
        }
        if(value < 800.f * MICRO)
        {
            return t2c400us;
        }
        if(value < 2.f * MILI)
        {
            return t2c800us;
        }
        if(value < 4.f * MILI)
        {
            return t2c2ms;
        }
        if(value < 8.f * MILI)
        {
            return t2c4ms;
        }
        if(value < 20.f * MILI)
        {
            return t2c8ms;
        }
        return t2c20ms;
    }
    return 0;
}

float captureVoltFromEnum(int enumerated)
{
    switch(enumerated)
    {
        case vc2Volt:
            return 2.f;
        case vc1Volt:
            return 1.f;
        case vc500Mili :
            return 500.f * MILI;
        case vc200Mili :
            return 200.f * MILI;
        case vc100Mili :
            return 100.f * MILI;
        case vc50Mili  :
            return 50.f * MILI;
        case vc20Mili  :
            return 20.f * MILI;
        case vc10Mili  :
            return 10.f * MILI;
    };
    return 0;
}

uint captureVoltFromValue(float value)
{
    if(value < 0.02f)
    {
        return vc10Mili;
    }
    if(value < 0.05f)
    {
        return vc20Mili;
    }
    if(value < 0.1f)
    {
        return vc50Mili;
    }
    if(value < 0.2f)
    {
        return vc100Mili;
    }
    if(value < 0.5f)
    {
        return vc200Mili;
    }
    if(value < 1.f)
    {
        return vc500Mili;
    }
    if(value < 2.f)
    {
        return vc1Volt;
    }
    return vc2Volt;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ToolText::Time(char* buffer, int size, double value, int precision)
{
    // unit
    double secs = value;
    double mili = value / DOUBLE_MILI;
    double micr = value / DOUBLE_MICRO;
    double nano = value / DOUBLE_NANO;
    double piko = value / DOUBLE_PIKO;
    // format
    if (precision == 0) {
        if(SDL_fabs(secs) >= 1.0)      { pFormat->formatString(buffer, size, "%.0f s",  secs); }
        else if(SDL_fabs(mili) >= 1.0) { pFormat->formatString(buffer, size, "%.0f ms", mili); }
        else if(SDL_fabs(micr) >= 1.0) { pFormat->formatString(buffer, size, "%.0f us", micr); }
        else if(SDL_fabs(nano) >= 1.0) { pFormat->formatString(buffer, size, "%.0f ns", nano); }
        else if(SDL_fabs(piko) >= 1.0) { pFormat->formatString(buffer, size, "%.0f ps", piko); }
        else { pFormat->formatString(buffer, size, "0.0");}
    }
    else if (precision == 1) {
        if(SDL_fabs(secs) >= 1.0)      { pFormat->formatString(buffer, size, "%.1f s",  secs); }
        else if(SDL_fabs(mili) >= 1.0) { pFormat->formatString(buffer, size, "%.1f ms", mili); }
        else if(SDL_fabs(micr) >= 1.0) { pFormat->formatString(buffer, size, "%.1f us", micr); }
        else if(SDL_fabs(nano) >= 1.0) { pFormat->formatString(buffer, size, "%.1f ns", nano); }
        else if(SDL_fabs(piko) >= 1.0) { pFormat->formatString(buffer, size, "%.1f ps", piko); }
        else { pFormat->formatString(buffer, size, "0.0");}
    }
    else if (precision == 2) {
        if(SDL_fabs(secs) >= 1.0)      { pFormat->formatString(buffer, size, "%.2f s",  secs); }
        else if(SDL_fabs(mili) >= 1.0) { pFormat->formatString(buffer, size, "%.2f ms", mili); }
        else if(SDL_fabs(micr) >= 1.0) { pFormat->formatString(buffer, size, "%.2f us", micr); }
        else if(SDL_fabs(nano) >= 1.0) { pFormat->formatString(buffer, size, "%.2f ns", nano); }
        else if(SDL_fabs(piko) >= 1.0) { pFormat->formatString(buffer, size, "%.2f ps", piko); }
        else { pFormat->formatString(buffer, size, "0.0");}
    }
    else {
        if(SDL_fabs(secs) >= 1.0)      { pFormat->formatString(buffer, size, "%.3f s",  secs); }
        else if(SDL_fabs(mili) >= 1.0) { pFormat->formatString(buffer, size, "%.3f ms", mili); }
        else if(SDL_fabs(micr) >= 1.0) { pFormat->formatString(buffer, size, "%.3f us", micr); }
        else if(SDL_fabs(nano) >= 1.0) { pFormat->formatString(buffer, size, "%.3f ns", nano); }
        else if(SDL_fabs(piko) >= 1.0) { pFormat->formatString(buffer, size, "%.3f ps", piko); }
        else { pFormat->formatString(buffer, size, "0.0");}
    }
}

void ToolText::Unit(char* buffer, int size, double value, double unit)
{
    // value
    double secsV = value;
    double miliV = value / DOUBLE_MILI;
    double micrV = value / DOUBLE_MICRO;
    double nanoV = value / DOUBLE_NANO;
    double pikoV = value / DOUBLE_PIKO;
    // unit
    double secs = unit;
    double mili = unit / DOUBLE_MILI;
    double micr = unit / DOUBLE_MICRO;
    double nano = unit / DOUBLE_NANO;
    double piko = unit / DOUBLE_PIKO;
    // format
    if(SDL_fabs(secs) >= 1.0)      { pFormat->formatString(buffer, size, "%.3f", secsV); }
    else if(SDL_fabs(mili) >= 1.0) { pFormat->formatString(buffer, size, "%.3f", miliV); }
    else if(SDL_fabs(micr) >= 1.0) { pFormat->formatString(buffer, size, "%.3f", micrV); }
    else if(SDL_fabs(nano) >= 1.0) { pFormat->formatString(buffer, size, "%.3f", nanoV); }
    else if(SDL_fabs(piko) >= 1.0) { pFormat->formatString(buffer, size, "%.3f", pikoV); }
    else { pFormat->formatString(buffer, size, "0.0"); }
}

void ToolText::xUnit(char* buffer, int size, double unit)
{
    // unit
    double secs = unit;
    double mili = unit / DOUBLE_MILI;
    double micr = unit / DOUBLE_MICRO;
    double nano = unit / DOUBLE_NANO;
    double piko = unit / DOUBLE_PIKO;
    // format
    if(SDL_fabs(secs) >= 1.0)      { pFormat->formatString(buffer, size, "[s]"); }
    else if(SDL_fabs(mili) >= 1.0) { pFormat->formatString(buffer, size, "[ms]"); }
    else if(SDL_fabs(micr) >= 1.0) { pFormat->formatString(buffer, size, "[us]"); }
    else if(SDL_fabs(nano) >= 1.0) { pFormat->formatString(buffer, size, "[ns]"); }
    else if(SDL_fabs(piko) >= 1.0) { pFormat->formatString(buffer, size, "[ps]"); }
    else { pFormat->formatString(buffer, size, "0.0"); }
}

void ToolText::Hertz(char* buffer, int size, float value)
{
    if(value < PIKO)
    {
        pFormat->formatString(buffer, size, "0 Hz");
    }
    if(value >= PIKO && value < NANO)
    {
        pFormat->formatString(buffer, size, "%.3f nHz", value * GIGA);
    }
    if(value >= NANO && value < MICRO)
    {
        pFormat->formatString(buffer, size, "%.3f uHz", value * MEGA);
    }
    else if(value >= MICRO && value < MILI)
    {
        pFormat->formatString(buffer, size, "%.3f mHz", value * KILO);
    }
    else if(value >= MILI && value < KILO)
    {
        pFormat->formatString(buffer, size, "%.3f Hz", value);
    }
    else if(value >= KILO && value < MEGA)
    {
        pFormat->formatString(buffer, size, "%.3f KHz", value / KILO);
    }
    else if(value >= MEGA && value < GIGA)
    {
        pFormat->formatString(buffer, size, "%.3f MHz", value / MEGA);
    }
    else if(value >= GIGA)
    {
        pFormat->formatString(buffer, size, "%.3f GHz", value / GIGA);
    }
}

void ToolText::Decibels(char* buffer, int size, float value)
{
    pFormat->formatString(buffer, size, "%.2f dB", value);
}

void ToolText::Volt(char* buffer, int size, float value)
{
    pFormat->formatString(buffer, size, "%.2f V", value);
}


////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////

