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

CORE_INLINE float complexMultiplyReal(float aRe, float aIm, float bRe, float bIm)
{
    return aRe * bRe - aIm * bIm;
}

CORE_INLINE float complexMultiplyImag(float aRe, float aIm, float bRe, float bIm)
{
    return aIm * bRe + aRe * bIm;
}

void OsciloscopeThreadRenderer::dftCalculate(iint n, double* inRe, double* inIm, double* outRe, double* outIm)
{
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            double angle = 2.f * PI * double(i) * double(j) / double(n);
            double cosRe =  cos(angle);
            double sinIm = -sin(angle);
            outRe[i] += complexMultiplyReal(cosRe, sinIm, inRe[j], inIm[j]);
            outIm[i] += complexMultiplyImag(cosRe, sinIm, inRe[j], inIm[j]);
        }
    }
}

void OsciloscopeThreadRenderer::fftCalculate(uint samples, float* real, float* imag)
{
    if(samples <= NUM_FFT)
    {
        size_t          size = (size_t)cfgSize;
        kiss_fft_cfg     cfg = kiss_fft_alloc(samples, 0, cfgMem, &size);
        kiss_fft_cpx* fftIn  = in;
        kiss_fft_cpx* fftOut = out;
        for(uint i = 0; i < samples; i++)
        {
            fftIn[i].r = real[i];
            fftIn[i].i = imag[i];
        }
        kiss_fft(cfg, fftIn, fftOut);
        for(uint i = 1; i < samples; i++)
        {
            real[i] = fftOut[i].r / float(samples);
            imag[i] = fftOut[i].i / float(samples);
        }
        real[0] = fftOut[0].r / float(samples);
    }
}

void OsciloscopeThreadRenderer::fftRCalculateAmp(uint samples, float* real, float* imag, float* outAmpl)
{
    if(samples <= NUM_FFT)
    {
        size_t          size = (size_t)cfgSize;
        kiss_fft_cfg     cfg = kiss_fft_alloc(samples, 0, cfgMem, &size);
        kiss_fft_cpx* fftIn  = in;
        kiss_fft_cpx* fftOut = out;
        for(uint i = 0; i < samples; i++)
        {
            fftIn[i].r = real[i];
            fftIn[i].i = imag[i];
        }
        // calculate FFT
        kiss_fft(cfg, fftIn, fftOut);
        // output Amplitude (single sided FFT spectrum)
        for(uint i = 1; i < (samples / 2); i++)
        {
            float fftR = fftOut[i].r;
            float fftI = fftOut[i].i;
            float absFFT = sqrtf((fftR * fftR) + (fftI * fftI));
            outAmpl[i] = 2.0 * absFFT / float(samples);
        }
        // DC component
        float fftR = fftOut[0].r;
        float fftI = fftOut[0].i;
        float absFFTdc = sqrtf((fftR * fftR) + (fftI * fftI));
        outAmpl[0] = absFFTdc / float(samples);
    }
}


void OsciloscopeThreadRenderer::fftAmplitude(iint samples, float* gAmp, float* gRe, float* gIm)
{
    for(iint x = 0; x < samples; x++)
    {
        gAmp[x] = gRe[x] * gRe[x] + gIm[x] * gIm[x];
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
