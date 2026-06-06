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
// oscrender.cpp - 波形渲染引擎
// 功能：负责将采集数据渲染为可视化波形，包括：
//   - 通道信号合成计算 (channelFunction)
//   - 时域(OSC)波形渲染预处理
//   - FFT频谱渲染预处理
//   - 2D/3D OpenGL 渲染管线调用
//==============================================================================
#include <scopefun/ScopeFun.h>

float dBPower[] = { 0.1f,
                    0.01f,
                    0.001f,
                    0.0001f,
                    0.00001f,
                    0.000001f,
                    0.0000001f,
                    0.00000001f,
                    0.000000001f,
                    0.0000000001f,
                    0.00000000001f,
                    0.000000000001f
                  };


////////////////////////////////////////////////////////////////////////////////
//
// renderAnalog
//
////////////////////////////////////////////////////////////////////////////////
float channelFunction(float ch0value, float ch1value, int function, WndMain& window)
{
    switch(function)
    {
        case ANALOG_FUNCTION_MEDIUM:
            return (ch0value + ch1value) / 2;
            break;
        case ANALOG_FUNCTION_SUB_CH0_CH1:
            return ch0value - ch1value;
            break;
        case ANALOG_FUNCTION_SUB_CH1_CH0:
            return ch1value - ch0value;
            break;
        case ANALOG_FUNCTION_ADD:
            return ch0value + ch1value;
            break;
        case ANALOG_FUNCTION_MIN:
            return min(ch0value, ch1value);
            break;
        case ANALOG_FUNCTION_MAX:
            return max(ch0value, ch1value);
            break;
        case ANALOG_FUNCTION_CUSTOM:
           {
                /* TODO: erase Lua script
              ishort func = 0;
              pOsciloscope->m_callback.Ptr()->onFunction(ch0value*MAXOSCVALUE, ch1value*MAXOSCVALUE, &func);
              return double(func)/MAXOSCVALUE;
                */
           }
    };
    return 0.f;
}

/////////////////////////////////////////////////////////////////////////////
//
// osc:           x[ -0.5, 0.5 ], y[  -0.5, 0.5 ]
// fft/digital:   x[ -0.5, 0.5 ], y[  -0.5, 0.5 ]
//
/////////////////////////////////////////////////////////////////////////////


void OsciloscopeThreadRenderer::preOscRender(uint threadId, OsciloscopeThreadData& threadData)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    float zoom = 1.0;
    if(wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
    {
        xMin =  0.f  / zoom;
        xMax =  1.0f / zoom;
        yMin = -0.5f;
        yMax =  0.5f;
    }
    else
    {
        xMin =  0.f  / zoom;
        xMax =  1.0f / zoom;
        yMin = -0.5f;
        yMax =  0.5f;
    }
    render.zoomOriginalOsc = render.zoomOsc;
}

void OsciloscopeThreadRenderer::preFftRender(uint threadId, OsciloscopeThreadData& threadData)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    float zoom = render.zoomFFT;
    if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        xMin =  0.f;
        xMax =  1.f;
        yMin = -0.5f;
        yMax =  0.5f;
    }
    else
    {
        xMin =   0.f;
        xMax =   1.f;
        yMin =  -0.5f;
        yMax =   0.5f;
    }
    render.zoomOriginalFFT = render.zoomFFT;
}

void OsciloscopeThreadRenderer::renderFps(uint threadId, OsciloscopeThreadData& threadData)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    if(pOsciloscope->settings.getSettings()->renderFps)
    {
        FORMAT_BUFFER();
        int y = 225;
        FORMAT("generate: %d", pTimer->getFps(TIMER_GENERATE));
        pFont->setSize(threadId, 0.75f);
        pFont->writeText(threadId,  200, y, formatBuffer);
        y +=  25;
        FORMAT("capture: %d", pTimer->getFps(TIMER_CAPTURE));
        pFont->setSize(threadId, 0.75f);
        pFont->writeText(threadId, 200, y, formatBuffer);
        y +=  25;
        FORMAT("bandwidth: %d MB", SDL_AtomicGet(&pOsciloscope->bandWidth));
        pFont->setSize(threadId, 0.75f);
        pFont->writeText(threadId, 200, y, formatBuffer);
        y += 25;
        for(uint i = 0; i < pRender->getThreadCount(); i++)
        {
            FORMAT("update_%d: %d", i, pTimer->getFps(i + TIMER_UPDATE0));
            pFont->setSize(threadId, 0.75f);
            pFont->writeText(threadId, 200, y, formatBuffer);
            y +=  25;
        }
        FORMAT("main: %d", pTimer->getFps(TIMER_MAIN));
        pFont->setSize(threadId, 0.75f);
        pFont->writeText(threadId,  200, y, formatBuffer);
        y +=  25;
        FORMAT("render: %d", pTimer->getFps(TIMER_RENDER));
        pFont->setSize(threadId, 0.75f);
        pFont->writeText(threadId,  200, y, formatBuffer);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderAnalogGrid
//
////////////////////////////////////////////////////////////////////////////////
void renderCircleXY(uint threadid, int n, float r, float z, Matrix4x4 final)
{
    Vector4 center = Vector4(0, 0, 0.5, 1);
    Vector4 a, b;
    pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE, n);
    for(int i = 0; i < n; i++)
    {
        float angle0 = 360.f * (float(i) / float(n)) * TO_RAD;
        float angle1 = 360.f * (float(i + 1) / float(n)) * TO_RAD;
        float x0 = sinf(angle0);
        float y0 = cosf(angle0);
        float x1 = sinf(angle1);
        float y1 = cosf(angle1);
        a = center;
        b = center;
        a.x += r * x0;
        a.y += r * y0;
        a.z = z;
        a.w = 1;
        b.x += r * x1;
        b.y += r * y1;
        b.z = z;
        b.w = 1;
        pCanvas3d->bLine(threadid, a, b);
    }
    pCanvas3d->endBatch(threadid, final, COLOR_ARGB(255, 255, 255, 255));
}

void OsciloscopeThreadRenderer::renderAnalogGrid(uint threadId, OsciloscopeThreadData& threadData)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    double sigPos = render.signalPosition;
    double sigZoom = render.signalZoom;
    uint xCount = 10;
    uint yCount = 10;
    double yMin = -0.5;
    double yMax =  0.5;
    ////////////////////////////////////////////////////////////////////////////////
    // GRID
    ////////////////////////////////////////////////////////////////////////////////
    if(!wndMain.display.oscGrid)
    {
        return;
    }
    float z = 0.f;
    {
        float cx = render.cameraOsc.View.Pos().x;
        if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
        {
            cx = 0.f;
        }
        if(wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
        {
            cx = 0.f;
        }
        // grid
        int   renderCount = double(xCount);
        double gridDelta  = 1.0 / double(xCount);
        ilarge iNumSamples = sfGetNumSamples(&threadData.m_hw);
        // resample (renderAnalogGrid)
        double resamplePos = sigPos;
        double displayZoom = sigZoom;
        double resampleMin = resamplePos - 0.5 * (double)displayZoom;
        double resampleMax = resamplePos + 0.5 * (double)displayZoom;
        // sample: min / max
        double dNumSamples = iNumSamples;
        ilarge zoomSamplePos = resamplePos * dNumSamples;
        ilarge zoomSampleMin = zoomSamplePos - (dNumSamples / 2) * displayZoom;     // [0..n]
        ilarge zoomSampleMax = zoomSamplePos + (dNumSamples / 2) * displayZoom - 1; // [0..n]
        ilarge zoomSampleCnt = clamp<ilarge>(zoomSampleMax - zoomSampleMin + 1, 0, iNumSamples); // [0..n]
        // sample time
        double sampleTime = double(wndMain.horizontal.Capture);
        // frame time
        double frameTime = sampleTime * double(wndMain.horizontal.FrameSize);
        // grid time
        double gridTime = frameTime / 10.0;
        // grid zoom time
        double gridZoomTime = gridTime * displayZoom;
        // grid zoom samples
        double gridZoomSamples = max<double>(1.0, gridZoomTime / sampleTime);
        // units
        double frameStart   = resampleMin;
        double frameEnd     = resampleMax;
        double frameGrid    = 0.1 * displayZoom;
        double gridDivision = resampleMin / frameGrid;
        //  double gridModulo   = SDL_fmodf(resampleMin*1000,frameGrid*1000);
        // double gridValue    = ((double)gridUnit / (double)iNumSamples)*frameTime;
        double gridModulo = SDL_fmod(-resamplePos / displayZoom, 0.1);
        //////////////////////////////////////////////////////////////////////////////////
        // x fine grid lines
        //////////////////////////////////////////////////////////////////////////////////
        pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, int(renderCount));
        for(int i = 1; i <= renderCount; i++)
        {
            // screen units
            double        x   = gridModulo + i * 0.1;
            pCanvas3d->bLine(threadId,  Vector4(x, yMin, z, 1), Vector4(x, yMax, z, 1));
        }
        pCanvas3d->endBatch(threadId, render.cameraOsc.Final, render.colorGrid);
        ////////////////////////////////////////////////////////////////////////////////
        // y grid lines
        ////////////////////////////////////////////////////////////////////////////////
        float yDelta = (yMax - yMin) / float(yCount);
        float y = yMin;
        pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, int(yCount + 1));
        for(uint i = 0; i < yCount; i++)
        {
            pCanvas3d->bLine(threadId, Vector4(xMin, y, z, 1), Vector4(xMax, y, z, 1));
            y += yDelta;
        }
        pCanvas3d->endBatch(threadId, render.cameraOsc.Final, render.colorGrid);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // subgrid with visibility calculation
    ////////////////////////////////////////////////////////////////////////////////
    double         cx = render.cameraOsc.View.Pos().x;
    if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        cx = 0.f;
    }
    if(wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
    {
        cx = 0.f;
    }
    double          wx = sigPos;
    double subCount = double(xCount) / sigZoom;
    double subDelta  = 1.0 / double(subCount);
    double gridDelta = 1.0 / double(xCount);
    // view
    double cxMin = -0.5;
    double cxMax =  0.5;
    double zoomOffset = (1.0 - sigZoom) / 2.0;
    double viewStart  = (cx + cxMin);
    double viewEnd    = (cx + cxMax) + zoomOffset;
    // border
    double borderStart = (wx + xMin);
    double borderEnd   = (wx + xMax);
    // visibility
    double offset     = (viewStart - borderStart);
    int   renderCount = double(xCount + 2);
    double x = viewStart - fmodf(offset, gridDelta);
    x = max(x, borderStart);
    x = min(x, borderEnd);
    double outside = x + double(renderCount) * gridDelta - borderEnd;
    renderCount  = renderCount - uint(max(outside, 0.0) / gridDelta);
    Matrix4x4 model = matIdentity();
    ////////////////////////////////////////////////////////////////////////////////
    // white border
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, 4);
    {
        Matrix4x4 model = render.cameraOsc.World;
        pCanvas3d->bLine(threadId,  matTransform4x4(Vector4(xMin, yMin, z, 1), model), matTransform4x4(Vector4(xMin, yMax, z, 1), model));
        pCanvas3d->bLine(threadId,  matTransform4x4(Vector4(xMax, yMin, z, 1), model), matTransform4x4(Vector4(xMax, yMax, z, 1), model));
        pCanvas3d->bLine(threadId,  matTransform4x4(Vector4(xMin, yMin, z, 1), model), matTransform4x4(Vector4(xMax, yMin, z, 1), model));
        pCanvas3d->bLine(threadId,  matTransform4x4(Vector4(xMin, yMax, z, 1), model), matTransform4x4(Vector4(xMax, yMax, z, 1), model));
        pCanvas3d->endBatch(threadId, render.cameraOsc.Final, render.colorBorder);
    }
    if(wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
    {
        float size = 0.5f;
        pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, 8);
        pCanvas3d->bLine(threadId,  Vector4( size + 0.5,   size, 0, 1), Vector4( size + 0.5,  size, 1, 1));
        pCanvas3d->bLine(threadId,  Vector4( size + 0.5,  -size, 0, 1), Vector4( size + 0.5, -size, 1, 1));
        pCanvas3d->bLine(threadId,  Vector4(-size + 0.5,   size, 0, 1), Vector4(-size + 0.5,  size, 1, 1));
        pCanvas3d->bLine(threadId,  Vector4(-size + 0.5,  -size, 0, 1), Vector4(-size + 0.5, -size, 1, 1));
        pCanvas3d->bLine(threadId,  Vector4( size + 0.5,  -size, 1, 1), Vector4( size + 0.5,  size, 1, 1));
        pCanvas3d->bLine(threadId,  Vector4( size + 0.5,   size, 1, 1), Vector4(-size + 0.5,  size, 1, 1));
        pCanvas3d->bLine(threadId,  Vector4( size + 0.5,  -size, 1, 1), Vector4(-size + 0.5, -size, 1, 1));
        pCanvas3d->bLine(threadId,  Vector4(-size + 0.5,  -size, 1, 1), Vector4(-size + 0.5,  size, 1, 1));
        pCanvas3d->endBatch(threadId,  render.cameraOsc.Final, COLOR_ARGB(255, 0, 0, 0));
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderAnalogAxis
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderAnalogAxis(uint threadId, OsciloscopeThreadData& threadData)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    ////////////////////////////////////////////////////////////////////////////////
    // Axis
    ////////////////////////////////////////////////////////////////////////////////
    if(!wndMain.display.oscAxis)
    {
        return;
    }
    float    sigPos = render.signalPosition;
    ////////////////////////////////////////////////////////////////////////////////
    // x axis
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, 1);
    pCanvas3d->bLine(threadId, Vector4(sigPos + xMin, 0, 0, 1), Vector4(sigPos + xMax, 0, 0, 1));
    pCanvas3d->endBatch(threadId, render.cameraOsc.Final, COLOR_ARGB(255, 255, 0, 0));
    ////////////////////////////////////////////////////////////////////////////////
    // y axis
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, 1);
    pCanvas3d->bLine(threadId, Vector4(sigPos + xMin, yMin, 0, 1), Vector4(sigPos + xMin, yMax, 0, 1));
    pCanvas3d->endBatch(threadId, render.cameraOsc.Final, COLOR_ARGB(255, 0, 255, 0));
}

////////////////////////////////////////////////////////////////////////////////
//
// renderAnalogUnits
//
////////////////////////////////////////////////////////////////////////////////
double getTriggerVoltagePerStep(WndMain& window, float yCount)
{
    float  captureVoltage = 0.f;
    if(window.trigger.Source == 0)
    {
        captureVoltage = window.channel01.Capture;
    }
    else
    {
        captureVoltage = window.channel02.Capture;
    }
    return captureVoltage * yCount / 1024.0;
}

double getTriggerVoltage(WndMain& window)
{
    float  captureVoltage = 0.f;
    if(window.trigger.Source == 0)
    {
        captureVoltage = window.channel01.Capture;
    }
    else
    {
        captureVoltage = window.channel02.Capture;
    }
    return captureVoltage;
}

void OsciloscopeThreadRenderer::renderAnalogUnits(uint threadid, OsciloscopeThreadData& threadData)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    SDisplay&              display = threadData.m_frame;
    ////////////////////////////////////////////////////////////////////////////////
    // Units
    ////////////////////////////////////////////////////////////////////////////////
    double timeDisplay = 0;
    if(!wndMain.display.oscUnits)
    {
        return;
    }
    timeDisplay = double(wndMain.horizontal.Capture) * double(wndMain.horizontal.FrameSize); //10000.0;
    ////////////////////////////////////////////////////////////////////////////////
    // voltage / time
    ////////////////////////////////////////////////////////////////////////////////
    char bufferTime[1024]    = { 0 };
    char bufferVolt0[1024]   = { 0 };
    char bufferVolt1[1024]   = { 0 };
    char bufferTrigger[1024] = { 0 };
    // sigPos, sigZoom
    double   cx = render.cameraOsc.View.Pos().x;
    if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        cx = 0.f;
    }
    if(wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
    {
        cx = 0.f;
    }
    double    sigPos = render.signalPosition;
    double   sigZoom = render.signalZoom;
    double gridStart = cx + xMin;
    double gridEnd   = cx + xMax;
    double gridTime = /*double(render.zoomFFT)**/sigZoom * double(timeDisplay) / 10.0;
    ToolText::Time(bufferTime, 1024, gridTime, 0);
    ToolText::Volt(bufferVolt0, 1024, wndMain.channel01.Capture);
    ToolText::Volt(bufferVolt1, 1024, wndMain.channel02.Capture);
    pFont->setSize(threadid, 0.5f);
    FORMAT_BUFFER();
    FORMAT("Time [ %s / div ]", bufferTime);
    pFont->writeText (threadid, 15, 15, formatBuffer, render.colorTime);
    //pFont->writeText3d(threadid, render.cameraOsc.Final, xMin, yMax, 0.f, Vector4(1, 0, 0, 0), Vector4(0, 1, 0, 0), formatBuffer, render.colorTime, render.oscScaleX, render.oscScaleY);
    FORMAT("Ch0 [ %s / div ]", bufferVolt0);
    pFont->writeText(threadid, 160, 15, formatBuffer, render.colorChannel0);
    FORMAT("Ch1 [ %s / div ]", bufferVolt1);
    pFont->writeText(threadid, 310, 15, formatBuffer, render.colorChannel1);
    ////////////////////////////////////////////////////////////////////////////////
    // size
    ////////////////////////////////////////////////////////////////////////////////
    pFont->setSize(threadid, 0.42f);
    double charHeight = pFont->getLineHeight(threadid) / 2.0;
    double sizeX = 0.035f * render.oscScaleX;
    double sizeY = 0.035f * render.oscScaleY;
    double yCount = 10.f;
    double xCount = 10.f;
    ////////////////////////////////////////////////////////////////////////////////
    // trigger
    ////////////////////////////////////////////////////////////////////////////////
    double triggerVoltage = float(double(wndMain.trigger.Level + wndMain.trigger.His) * getTriggerVoltagePerStep(wndMain, yCount));
    double triggerY = (triggerVoltage / (getTriggerVoltage(wndMain) * (yCount / 2.f))) * 0.50f;
    FORMAT(" Trig", bufferTrigger);
    pFont->writeText3d(threadid, render.cameraOsc.Final, xMax, triggerY + charHeight / 2, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, render.colorTrigger, render.oscScaleX, render.oscScaleY);
    Vector4        tip = Vector4(xMin, triggerY, 0, 1);
    Vector4    arrowUp = Vector4(xMin - sizeX / 2, triggerY - sizeY / 4, 0, 1);
    Vector4  arrowDown = Vector4(xMin - sizeX / 2, triggerY + sizeY / 4, 0, 1);
    Vector4      start = Vector4(xMin - sizeX, triggerY, 0, 1);
    Vector4          a = Vector4(start.x, arrowUp.y, 0, 1);
    Vector4          b = Vector4(start.x, arrowDown.y, 0, 1);
    pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_TRIANGLE, 3);
    pCanvas3d->bTriangle(threadid, tip, arrowUp, arrowDown);
    pCanvas3d->bTriangle(threadid, arrowDown, a, b);
    pCanvas3d->bTriangle(threadid, arrowUp, a, arrowDown);
    pCanvas3d->endBatch(threadid, render.cameraOsc.Final, render.colorTrigger);
    ////////////////////////////////////////////////////////////////////////////////
    // 0 volt for both channels
    ////////////////////////////////////////////////////////////////////////////////
    FORMAT("%s", " 0V");
    uint yellow = render.colorChannel0;
    uint blue   = render.colorChannel1;
    double zeroBlue   = 0.f;
    double zeroYellow = 0.f;
	if(wndMain.channel01.OscOnOff == 1)
	{
		double step = 0.f;
		step = render.analogChannelYVoltageStep0;
		double zeroOffset = 0.f;
		zeroOffset = render.analogChannelPositin0 * step;
		double zeroVoltage = zeroOffset;
		zeroBlue   = zeroVoltage;
		double zeroY = 0.f;
		zeroY = (wndMain.channel01.YPosition / (wndMain.channel01.Capture * yCount));
		pFont->writeText3d(threadid, render.cameraOsc.Final, xMax, zeroY + charHeight / 2, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, yellow, render.oscScaleX, render.oscScaleY);
		Vector4        tip = Vector4(xMin, zeroY, 0, 1);
		Vector4  arrowUp   = Vector4(xMin - sizeX / 2, zeroY - sizeY / 4, 0, 1);
		Vector4  arrowDown = Vector4(xMin - sizeX / 2, zeroY + sizeY / 4, 0, 1);
		Vector4  start     = Vector4(xMin - sizeX, zeroY, 0, 1);
		Vector4         a  = Vector4(start.x, arrowUp.y, 0, 1);
		Vector4         b  = Vector4(start.x, arrowDown.y, 0, 1);
		pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_TRIANGLE, 3);
		pCanvas3d->bTriangle(threadid, tip, arrowUp, arrowDown);
		pCanvas3d->bTriangle(threadid, arrowDown, a, b);
		pCanvas3d->bTriangle(threadid, arrowUp, a, arrowDown);
		pCanvas3d->endBatch(threadid, render.cameraOsc.Final, yellow);
	}

	if(wndMain.channel02.OscOnOff == 1)
	{
		double step = 0.f;
		step = render.analogChannelYVoltageStep1;
		double zeroOffset = 0.f;
		zeroOffset = render.analogChannelPositin1 * step;
		double zeroVoltage = zeroOffset;
		zeroYellow = zeroVoltage;
		double zeroY = 0.f;
		zeroY = (wndMain.channel02.YPosition / (wndMain.channel02.Capture * yCount));
		pFont->writeText3d(threadid, render.cameraOsc.Final, xMax, zeroY + charHeight / 2, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, blue, render.oscScaleX, render.oscScaleY);
		Vector4        tip = Vector4(xMin, zeroY, 0, 1);
		Vector4  arrowUp   = Vector4(xMin - sizeX / 2, zeroY - sizeY / 4, 0, 1);
		Vector4  arrowDown = Vector4(xMin - sizeX / 2, zeroY + sizeY / 4, 0, 1);
		Vector4  start     = Vector4(xMin - sizeX, zeroY, 0, 1);
		Vector4         a  = Vector4(start.x, arrowUp.y, 0, 1);
		Vector4         b  = Vector4(start.x, arrowDown.y, 0, 1);
		pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_TRIANGLE, 3);
		pCanvas3d->bTriangle(threadid, tip, arrowUp, arrowDown);
		pCanvas3d->bTriangle(threadid, arrowDown, a, b);
		pCanvas3d->bTriangle(threadid, arrowUp, a, arrowDown);
		pCanvas3d->endBatch(threadid, render.cameraOsc.Final, blue);
	}
    ////////////////////////////////////////////////////////////////////////////////
    // x units
    ////////////////////////////////////////////////////////////////////////////////
    char buffer[1024]    = { 0 };
    double charYmin       = yMin - charHeight;
    double charYmax       = yMax + charHeight;
    ////////////////////////////////////////////////////////////////////////////////
    // subgrid with visibility calculation
    ////////////////////////////////////////////////////////////////////////////////
    pFont->setSize(threadid, 0.35f);
    int    renderCount = xCount;
    ilarge numSamples = sfGetNumSamples(&threadData.m_hw);
    ilarge preTriggerSample = (wndMain.trigger.Percent / 100.0) * numSamples;
    numSamples = numSamples > 0 ? numSamples : 10000;
    // resample (renderAnalogUnits)
    double resamplePos = sigPos;
    double displayZoom = sigZoom;
    double resampleMin = resamplePos - 0.5 * displayZoom;
    double resampleMid = resamplePos;
    double resampleMax = resamplePos + 0.5 * displayZoom;
    // sample: min / max
    double dNumSamples = numSamples;
    ilarge zoomSamplePos = resamplePos * dNumSamples;
    ilarge zoomSampleMin = zoomSamplePos - (dNumSamples / 2) * displayZoom;     // [0..n]
    ilarge zoomSampleMax = zoomSamplePos + (dNumSamples / 2) * displayZoom - 1; // [0..n]
    ilarge zoomSampleCnt = clamp<ilarge>(zoomSampleMax - zoomSampleMin + 1, 0, numSamples); // [0..n]
    // time between samples
    double sampleTime = double(wndMain.horizontal.Capture);
    // grid time
    double gridStartTime  = (double)zoomSampleMin * sampleTime;
    double gridTimeX      = (double)zoomSampleCnt * sampleTime;
    double gridEndTime    = (double)zoomSampleMax * sampleTime;
    double preTriggerTime = (double)preTriggerSample * sampleTime;
    // time of arrow locations relative to grid
    double arrowXstart = 0;
    if (zoomSampleMin > 0)
        arrowXstart = SDL_fmod(-1.0 * (double)resamplePos / (double)displayZoom, 0.1) + 0.1;
    for(int i = 0; i < renderCount; i++)
    {
        // results
        double arrowX = arrowXstart + (i * 0.1);
        double valueX = gridStartTime + (arrowX * gridTimeX) - preTriggerTime;
        //pFormat->formatString(buffer, size, "%.3f", secsV)
        ToolText::Unit(buffer, 1024, valueX, gridEndTime);
        // draw xValues
        pFont->writeText3d(threadid, render.cameraOsc.Final, arrowX - sizeX, charYmin, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), buffer, render.colorTime, render.oscScaleX, render.oscScaleY);
        // draw arrow
        pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE, 3);
        pCanvas3d->bLine(threadid, Vector4(arrowX, charYmin, 0, 1), Vector4(arrowX, yMin, 0, 1));
        pCanvas3d->bLine(threadid, Vector4(arrowX + sizeX / 4, yMin - sizeY / 4, 0, 1), Vector4(arrowX, yMin, 0, 1));
        pCanvas3d->bLine(threadid, Vector4(arrowX - sizeX / 4, yMin - sizeY / 4, 0, 1), Vector4(arrowX, yMin, 0, 1));
        pCanvas3d->endBatch(threadid, render.cameraOsc.Final, render.colorTime);
    }
    // draw xUnit
    ToolText::xUnit(buffer, 1024, gridEndTime);
    pFont->writeText3d(threadid, render.cameraOsc.Final, xMax, yMin, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), buffer, render.colorTime, render.oscScaleX, render.oscScaleY);
    ////////////////////////////////////////////////////////////////////////////////
    // pre-trigger
    ////////////////////////////////////////////////////////////////////////////////
    FORMAT("%s", "Trigger");
    float preTrigPos = (double(wndMain.trigger.Percent) / 100.0) / sigZoom;
    pFont->setSize(threadid, 0.42f);
    float xPos = -sigPos/sigZoom + 0.5;
    pFont->writeText3d(threadid, render.cameraOsc.Final, xPos + preTrigPos, yMax + charHeight, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, render.colorTrigger, render.oscScaleX, render.oscScaleY);
    {
        Vector4        tip = Vector4(xPos + preTrigPos, yMin, 0, 1);
        Vector4    arrowUp = Vector4(xPos + preTrigPos + sizeX / 4, yMin - sizeY / 2, 0, 1);
        Vector4  arrowDown = Vector4(xPos + preTrigPos - sizeX / 4, yMin - sizeY / 2, 0, 1);
        Vector4      start = Vector4(xPos + preTrigPos, yMin - sizeY, 0, 1);
        Vector4          a = Vector4(arrowUp.x,  start.y, 0, 1);
        Vector4          b = Vector4(arrowDown.x, start.y, 0, 1);
        pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_TRIANGLE, 3);
        pCanvas3d->bTriangle(threadid, tip, arrowUp, arrowDown);
        pCanvas3d->bTriangle(threadid, arrowDown, a, b);
        pCanvas3d->bTriangle(threadid, arrowUp, a, arrowDown);
        pCanvas3d->endBatch(threadid, render.cameraOsc.Final, render.colorTrigger);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderAnalog
//
////////////////////////////////////////////////////////////////////////////////
uint colorFromYPos(Vector4 color, float y, bool abgr = false)
{
    float sr = 1.f;
    float sg = 0.f;
    float sb = 0.f;
    float sa = 1.f;
    float fr = color.x;
    float fg = color.y;
    float fb = color.z;
    float fa = color.w;
    float norm = ((y / 0.5f) + 1.f) * 0.5f; // 0.5f; // [ 0, 1 ]
    float r = lerp(sr, fr, norm);
    float g = lerp(sg, fg, norm);
    float b = lerp(sb, fb, norm);
    if(abgr)
    {
        return COLOR_ARGB(255, int(b * 255.f), int(g * 255.f), int(r * 255.f));
    }
    else
    {
        return COLOR_ARGB(255, int(r * 255.f), int(g * 255.f), int(b * 255.f));
    }
}

double freqFromPosition(int index, int maxIndex, double maxHertz);

void OsciloscopeThreadRenderer::measureSignal(uint threadId, OsciloscopeThreadData& threadData, MeasureData& measure, OsciloscopeFFT& fft)
{
    WndMain&              wndMain = threadData.m_window;
    OsciloscopeRenderData& render = threadData.m_render;
    SDisplay&             display = threadData.m_frame;
    MeasureChannelData&   current = measure.column[MEASURE_CURRENT];
    if(!wndMain.measure.uiOpen)
    {
        current.averageN = 0;
        return;
    }
    if (current.averageN < MEASURE_HISTORY_MAX)
    {
        current.averageN++;
    }
    bool is500MSps = false;
    double sigPos = render.signalPosition;
    double sigZoom = render.signalZoom;
    int iSamples = display.samples;
    int ppi = display.ppi; // display re-sample factor
    double dSamples = (double)iSamples;
    // ETS removed: no time-interleaving offset
    double  etsOffset = 0.0;
    double  yCount   = 10.0;
    double  preTriggerZero = (double(wndMain.trigger.Percent) / 100.0);
    double  yposition0 = wndMain.channel01.YPosition /*- render.analogChannelOffsets0*/;
    double  yposition1 = wndMain.channel02.YPosition /*- render.analogChannelOffsets1*/;
    double  ch0ZeroVolt = double(yposition0) * render.analogChannelYVoltageStep0;
    double  ch1ZeroVolt = double(yposition1) * render.analogChannelYVoltageStep1;
    double  xposition  = (wndMain.horizontal.Position / 100.0) + etsOffset - preTriggerZero;
    double  yGridMax = yCount / 2.0;
    double  yfactor0 = wndMain.channel01.Capture * yGridMax;
    double  yfactor1 = wndMain.channel02.Capture * yGridMax;
    double xMin = (sigPos - 0.5 * sigZoom);
    double xMax = (sigPos + 0.5 * sigZoom);
    double minX = min(wndMain.measure.data.pickX0.position.x, wndMain.measure.data.pickX1.position.x);
    double maxX = max(wndMain.measure.data.pickX0.position.x, wndMain.measure.data.pickX1.position.x);
    double minY = min(wndMain.measure.data.pickY0.position.y, wndMain.measure.data.pickY1.position.y);
    double maxY = max(wndMain.measure.data.pickY0.position.y, wndMain.measure.data.pickY1.position.y);
    double  capture   = wndMain.horizontal.Capture;
    double  frameSize = wndMain.horizontal.FrameSize;
    double  maxTime   = iSamples;
    wndMain.measure.data.pick.row[Ch0YV0] = maxTime;
    wndMain.measure.data.pick.row[Ch1YV0] = maxTime;
    wndMain.measure.data.pick.row[FunYV0] = maxTime;
    wndMain.measure.data.pick.row[Ch0YV1] = maxTime;
    wndMain.measure.data.pick.row[Ch1YV1] = maxTime;
    wndMain.measure.data.pick.row[FunYV1] = maxTime;
    current.row[Ch0Vmin]   = +yfactor0;
    current.row[Ch0Vmax]   = -yfactor0;
    current.row[Ch1Vmin]   = +yfactor1;
    current.row[Ch1Vmax]   = -yfactor1;
    current.row[FunVmin]   = max(-yfactor0, +yfactor1);
    current.row[FunVmax]   = min(-yfactor0, +yfactor1);
    current.row[Ch0VrmsDc] = 0;
    current.row[Ch0VrmsAc] = 0;
    current.row[Ch1VrmsDc] = 0;
    current.row[Ch1VrmsAc] = 0;
    current.row[FunVrmsDc] = 0;
    current.row[FunVrmsAc] = 0;
    current.row[Ch0Surface] = 0;
    current.row[Ch1Surface] = 0;
    current.row[FunSurface] = 0;
    double y0Avg = 0;
    double y1Avg = 0;
    double yFAvg = 0;
    int    y0MinFirst = 0;
    int    y0MaxFirst = 0;
    double y0MinTime = 0;
    double y0MaxTime = 0;
    double y0Min = +yfactor0;
    double y0Max = -yfactor0;
    int    y1MinFirst = 0;
    int    y1MaxFirst = 0;
    double y1MinTime = 0;
    double y1MaxTime = 0;
    double y1Min = +yfactor0;
    double y1Max = -yfactor0;
    int    yFMinFirst = 0;
    int    yFMaxFirst = 0;
    double yFMinTime = 0;
    double yFMaxTime = 0;
    double yFMin = +yfactor0;
    double yFMax = -yfactor0;
    double y0Middle = 0;
    double y1Middle = 0;
    double yFMiddle = 0;
    for(uint pt = 0; pt < iSamples; pt++)
    {
        uint idx = clamp<int>(pt, 0, iSamples - 1);
        double   yPosCh0 = display.analog0.bytes[idx];
        double   yPosCh1 = display.analog1.bytes[idx];
        double   yPosChF = channelFunction(yPosCh0, yPosCh1, wndMain.function.Type, wndMain);
        double   y0 = yPosCh0 * yfactor0 - yposition0 /*ch0ZeroVolt*/;
        double   y1 = yPosCh1 * yfactor1 - yposition1 /*ch1ZeroVolt*/;
        double   yF = channelFunction(y0, y1, wndMain.function.Type, wndMain);
        double xPos = (double(pt) / double(iSamples));
        double    x = xPos + xposition;
        // avg
        y0Avg += y0;
        y1Avg += y1;
        yFAvg += yF;
        // recalc
        y0 = yPosCh0 * yfactor0;
        y1 = yPosCh1 * yfactor1;
        yF = channelFunction(y0, y1, wndMain.function.Type, wndMain);
        // min
        if(y0 < y0Min)
        {
            y0Min = y0;
        }
        if(y1 < y1Min)
        {
            y1Min = y1;
        }
        if(yF < yFMin)
        {
            yFMin = yF;
        }
        // max
        if(y0 > y0Max)
        {
            y0Max = y0;
        }
        if(y1 > y1Max)
        {
            y1Max = y1;
        }
        if(yF > yFMax)
        {
            yFMax = yF;
        }
    }
    y0Avg /= dSamples;
    y1Avg /= dSamples;
    yFAvg /= dSamples;
    current.row[Ch0Vavg] = y0Avg;
    current.row[Ch1Vavg] = y1Avg;
    current.row[FunVavg] = yFAvg;
    y0Middle = y0Min + (y0Max - y0Min) / 2;
    y1Middle = y1Min + (y1Max - y1Min) / 2;
    yFMiddle = yFMin + (yFMax - yFMin) / 2;
    double currTime0 = 0;
    double currTime1 = 0;
    double currTimeF = 0;
    double sumTime0Last = 0;
    double sumTime1Last = 0;
    double sumTimeFLast = 0;
    int previous0 = 0;
    int previous1 = 0;
    int previousF = 0;
    double previousTime = 0;
    double positivePulseWidth = 0;
    double negativePulseWidth = 0;
    Array<double, NUM_SAMPLES> time0;
    Array<double, NUM_SAMPLES> time1;
    Array<double, NUM_SAMPLES> timeF;
    double hyst0 = 0.02 * yfactor0;
    double hyst1 = 0.02 * yfactor1;
    double hystF = 0.02 * channelFunction(hyst0, hyst1, wndMain.function.Type, wndMain);
    for(uint pt = 0; pt < iSamples; pt++)
    {
        uint idx = clamp<int>(pt, 0, iSamples - 1);
        double   yPosCh0 = display.analog0.bytes[idx];
        double   yPosCh1 = display.analog1.bytes[idx];
        double   y0 = yPosCh0 * yfactor0 - yposition0 /*- ch0ZeroVolt*/;
        double   y1 = yPosCh1 * yfactor1 - yposition1 /*- ch1ZeroVolt*/;
        double   yF = channelFunction(y0, y1, wndMain.function.Type, wndMain);
        double deltaT = capture * ppi;
        double idxTime = pt * deltaT;
        int current0 = 0;
        int current1 = 0;
        int currentF = 0;
        if(y0 > hyst0)
        {
            current0 =  1;
        }
        if(y0 < -hyst0)
        {
            current0 = -1;
        }
        if(y1 > hyst1)
        {
            current1 =  1;
        }
        if(y1 < -hyst1)
        {
            current1 = -1;
        }
        if(yF > hystF)
        {
            currentF =  1;
        }
        if(yF < -hystF)
        {
            currentF = -1;
        }
        // init Ch1
        if(previous0 == 0)
        {
            previous0 = current0;
        }
        // + transition to  -
        if(previous0 == 1 && current0 == -1)
        {
            time0.pushBack(idxTime);
            previous0 = current0;
        }
        // - transition to  +
        if(previous0 == -1 && current0 == 1)
        {
            time0.pushBack(idxTime);
            previous0 = current0;
        }
        // init Ch2
        if(previous1 == 0)
        {
            previous1 = current1;
        }
        // + transition to  -
        if(previous1 == 1 && current1 == -1)
        {
            time1.pushBack(idxTime);
            previous1 = current1;
        }
        // - transition to  +
        if(previous1 == -1 && current1 == 1)
        {
            time1.pushBack(idxTime);
            previous1 = current1;
        }
        // init Fun
        if(previousF == 0)
        {
            previousF = currentF;
        }
        // + transition to  -
        if(previousF == 1 && currentF == -1)
        {
            timeF.pushBack(idxTime);
            previousF = currentF;
        }
        // - transition to  +
        if(previousF == -1 && currentF == 1)
        {
            timeF.pushBack(idxTime);
            previousF = currentF;
        }
    }
    // must have odd number of zero transitions
    if(time0.getCount() % 2 == 0 and time0.getCount() > 1)
    {
        time0.popBack();
    }
    if(time1.getCount() % 2 == 0 and time1.getCount() > 1)
    {
        time1.popBack();
    }
    if(timeF.getCount() % 2 == 0 and timeF.getCount() > 1)
    {
        timeF.popBack();
    }
    // get number of zero transitions
    int t0Cnt = time0.getCount();
    int t1Cnt = time1.getCount();
    int tFCnt = timeF.getCount();
    double period0 = 0;
    double period1 = 0;
    double periodF = 0;
    if (time0.getCount() > 1)
    {
        period0 = time0[t0Cnt - 1] - time0[0];
        period0 /= (t0Cnt - 1) / 2;
    }
    if (time1.getCount() > 1)
    {
        period1 = time1[t1Cnt - 1] - time1[0];
        period1 /= (t1Cnt - 1) / 2;
    }
    if (timeF.getCount() > 1)
    {
        periodF = timeF[tFCnt - 1] - timeF[0];
        periodF /= (tFCnt - 1) / 2;
    }
    current.row[Ch0Tperiod] = period0;
    current.row[Ch1Tperiod] = period1;
    current.row[FunTperiod] = periodF;
    // division by zero?
    current.row[Ch0Tfreq] = (period0 > 0) ? (1.0 / current.row[Ch0Tperiod]) : 0;
    current.row[Ch1Tfreq] = (period1 > 0) ? (1.0 / current.row[Ch1Tperiod]) : 0;
    current.row[FunTfreq] = (periodF > 0) ? (1.0 / current.row[FunTperiod]) : 0;
    // y - pick
    double pickCh0Y0    = wndMain.measure.data.pickY0.position.getYVolt(0);
    double pickCh0Y1    = wndMain.measure.data.pickY1.position.getYVolt(0);
    double pickCh1Y0    = wndMain.measure.data.pickY0.position.getYVolt(1);
    double pickCh1Y1    = wndMain.measure.data.pickY1.position.getYVolt(1);
    double pickFunY0    = channelFunction(pickCh0Y0, pickCh1Y0, wndMain.function.Type, wndMain);
    double pickFunY1    = channelFunction(pickCh0Y1, pickCh1Y1, wndMain.function.Type, wndMain);
    double pickCh0YMin  = min(pickCh0Y0, pickCh0Y1);
    double pickCh0YMax  = max(pickCh0Y0, pickCh0Y1);
    double pickCh1YMin  = min(pickCh1Y0, pickCh1Y1);
    double pickCh1YMax  = max(pickCh1Y0, pickCh1Y1);
    double pickFunYMin  = min(pickFunY0, pickFunY1);
    double pickFunYMax  = min(pickFunY0, pickFunY1);
    current.row[Ch0YV0] = pickCh0Y0;
    current.row[Ch0YV1] = pickCh0Y1;
    if (is500MSps == false)
    {
        current.row[Ch1YV0] = pickCh1Y0;
        current.row[Ch1YV1] = pickCh1Y1;
        current.row[FunYV0] = pickFunY0;
        current.row[FunYV1] = pickFunY1;
    }
    // x - pick
    double xTime0 = pOsciloscope->window.measure.data.pickX0.position.getXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom);
    current.row[Ch0XT0] = xTime0;
    if (is500MSps == false)
    {
        current.row[Ch1XT0] = xTime0;
        current.row[FunXT0] = xTime0;
    }
    double xTime1 = pOsciloscope->window.measure.data.pickX1.position.getXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom);
    current.row[Ch0XT1] = xTime1;
    if (is500MSps == false)
    {
        current.row[Ch1XT1] = xTime1;
        current.row[FunXT1] = xTime1;
    }
    ///////////////////////////////////////////////////////////////////////
    // at pick 0 ( x axis )
    ///////////////////////////////////////////////////////////////////////
    double relX0 = pOsciloscope->window.measure.data.pickX0.position.x;
    int   iX0Pos = relX0 * iSamples;
    double y0Ch0 = display.analog0.bytes[iX0Pos] * yfactor0 - yposition0;
    double y0Ch1 = display.analog1.bytes[iX0Pos] * yfactor1 - yposition1;
    double   y0F = channelFunction(y0Ch0, y0Ch1, wndMain.function.Type, wndMain);
    if (is500MSps == false)
    {
        current.row[Ch0XV0] = y0Ch0;
        current.row[Ch1XV0] = y0Ch1;
        current.row[FunXV0] = y0F;
    }
    ///////////////////////////////////////////////////////////////////////
    // at pick 1 ( x axis )
    ///////////////////////////////////////////////////////////////////////
    double relX1 = pOsciloscope->window.measure.data.pickX1.position.x;
    int   iX1Pos = relX1 * iSamples;
    double y1Ch0 = display.analog0.bytes[iX1Pos] * yfactor0 - yposition0;
    double y1Ch1 = display.analog1.bytes[iX1Pos] * yfactor1 - yposition1;
    double   y1F = channelFunction(y1Ch0, y1Ch1, wndMain.function.Type, wndMain);
    if (is500MSps == false)
    {
        current.row[Ch0XV1] = y1Ch0;
        current.row[Ch1XV1] = y1Ch1;
        current.row[FunXV1] = y1F;
    }
    ///////////////////////////////////////////////////////////////////////
    // rms
    ///////////////////////////////////////////////////////////////////////
    double valCh0VrmsDc = 0;
    double valCh1VrmsDc = 0;
    double valFunVrmsDc = 0;
    double valCh0VrmsAc = 0;
    double valCh1VrmsAc = 0;
    double valFunVrmsAc = 0;
    for(int pt = 0; pt < iSamples; pt++)
    {
        uint idx = clamp<uint>(pt, 0, iSamples - 1);
        double   yCh0 = display.analog0.bytes[idx] * yfactor0 - yposition0;
        double   yCh1 = display.analog1.bytes[idx] * yfactor1 - yposition1;
        double   yFun = channelFunction(y0Ch1, y1Ch1, wndMain.function.Type, wndMain);
        double    x = (double(pt) / iSamples);
        x = x + xposition;
        double time = x * capture;
        valCh0VrmsDc += yCh0 * yCh0;
        valCh1VrmsDc += yCh1 * yCh1;
        valFunVrmsDc += yFun * yFun;
        valCh0VrmsAc += (yCh0 - y0Avg) * (yCh0 - y0Avg);
        valCh1VrmsAc += (yCh1 - y1Avg) * (yCh1 - y1Avg);
        valFunVrmsAc += (yFun - yFAvg) * (yFun - yFAvg);
        ///////////////////////////////////////////////////////////////////////
        // x range [ X0, X1 ]
        ///////////////////////////////////////////////////////////////////////
        if(x >= xMin && x <= xMax)
        {
            current.row[Ch0Vmin] = min(yCh0, current.row[Ch0Vmin]);
            current.row[Ch0Vmax] = max(yCh0, current.row[Ch0Vmax]);
            current.row[Ch1Vmin] = min(yCh1, current.row[Ch1Vmin]);
            current.row[Ch1Vmax] = max(yCh1, current.row[Ch1Vmax]);
            current.row[FunVmin] = min(yFun, current.row[FunVmin]);
            current.row[FunVmax] = max(yFun, current.row[FunVmax]);
            current.row[Ch0Surface] += fabs(yCh0) * capture;
            current.row[Ch1Surface] += fabs(yCh1) * capture;
            current.row[FunSurface] += fabs(yFun) * capture;
        }
    }
    if (is500MSps == false)
    {
        current.row[Ch0VrmsDc] = valCh0VrmsDc;
        current.row[Ch1VrmsDc] = valCh1VrmsDc;
        current.row[FunVrmsDc] = valFunVrmsDc;
        current.row[Ch0VrmsAc] = valCh0VrmsAc;
        current.row[Ch1VrmsAc] = valCh1VrmsAc;
        current.row[FunVrmsAc] = valFunVrmsAc;
    }
    current.row[Ch0XTD]    = max(current.row[Ch0XT0], current.row[Ch0XT1]) - min(current.row[Ch0XT0], current.row[Ch0XT1]);
    current.row[Ch0XInvTD] = 1.0 / current.row[Ch0XTD];
    current.row[Ch1XTD]    = max(current.row[Ch1XT0], current.row[Ch1XT1]) - min(current.row[Ch1XT0], current.row[Ch1XT1]);
    current.row[Ch1XInvTD] = 1.0 / current.row[Ch1XTD];
    current.row[FunXTD]    = max(current.row[FunXT0], current.row[FunXT1]) - min(current.row[FunXT0], current.row[FunXT1]);
    current.row[FunXInvTD] = 1.0 / current.row[FunXTD];
    current.row[Ch0YVD]    = max(current.row[Ch0YV0], current.row[Ch0YV1]) - min(current.row[Ch0YV0], current.row[Ch0YV1]);
    current.row[Ch1YVD]    = max(current.row[Ch1YV0], current.row[Ch1YV1]) - min(current.row[Ch1YV0], current.row[Ch1YV1]);
    current.row[FunYVD]    = max(current.row[FunYV0], current.row[FunYV1]) - min(current.row[FunYV0], current.row[FunYV1]);
    current.row[Ch0XVD]    = max(current.row[Ch0XV0], current.row[Ch0XV1]) - min(current.row[Ch0XV0], current.row[Ch0XV1]);
    current.row[Ch1XVD]    = max(current.row[Ch1XV0], current.row[Ch1XV1]) - min(current.row[Ch1XV0], current.row[Ch1XV1]);
    current.row[FunXVD]    = max(current.row[FunXV0], current.row[FunXV1]) - min(current.row[FunXV0], current.row[FunXV1]);
    current.row[Ch0Vpp]    = current.row[Ch0Vmax] - current.row[Ch0Vmin];
    current.row[Ch1Vpp]    = current.row[Ch1Vmax] - current.row[Ch1Vmin];
    current.row[FunVpp]    = current.row[FunVmax] - current.row[FunVmin];
    current.row[Ch0VrmsDc] = sqrt(current.row[Ch0VrmsDc] / dSamples);
    current.row[Ch1VrmsDc] = sqrt(current.row[Ch1VrmsDc] / dSamples);
    current.row[FunVrmsDc] = sqrt(current.row[FunVrmsDc] / dSamples);
    current.row[Ch0VrmsAc] = sqrt(current.row[Ch0VrmsAc] / dSamples);
    current.row[Ch1VrmsAc] = sqrt(current.row[Ch1VrmsAc] / dSamples);
    current.row[FunVrmsAc] = sqrt(current.row[FunVrmsAc] / dSamples);
    if(wndMain.channel01.OscOnOff == 0)
    {
        for(int i = Channel0 + 1; i < Channel1; i++)
        {
            current.row[i] = 0;
        }
    }
    if(wndMain.channel02.OscOnOff == 0)
    {
        for(int i = Channel1 + 1; i < Function; i++)
        {
            current.row[i] = 0;
        }
    }
    if(wndMain.function.OscOnOff == 0)
    {
        for(int i = Function + 1; i < FFTCh0; i++)
        {
            current.row[i] = 0;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // FFT
    ////////////////////////////////////////////////////////////////////////////////
    for(int ch = 0; ch < 3; ch++)
    {
        bool isFunction = (ch == 3) ? true : false;
        iint p = wndMain.horizontal.FFTSize;
        if (!p)
        {
            return;
        }
        if (ch == 0 && wndMain.channel01.FFTOnOff == 0)
        {
            current.row[FFTCh0F0]   = 0;
            current.row[FFTCh0V0db] = 0;
            current.row[FFTCh0F1]   = 0;
            current.row[FFTCh0V1db] = 0;
            current.row[FFTCh0VD]   = 0;
            current.row[FFTCh0FD]   = 0;
            continue;
        }
        if ((ch == 1 && wndMain.channel02.FFTOnOff == 0))
        {
            current.row[FFTCh1F0]   = 0;
            current.row[FFTCh1V0db] = 0;
            current.row[FFTCh1F1]   = 0;
            current.row[FFTCh1V1db] = 0;
            current.row[FFTCh1VD]   = 0;
            current.row[FFTCh1FD]   = 0;
            continue;
        }
        if ((ch == 2 && wndMain.function.FFTOnOff  == 0))
        {
            current.row[FFTFunF0]   = 0;
            current.row[FFTFunV0db] = 0;
            current.row[FFTFunF1]   = 0;
            current.row[FFTFunV1db] = 0;
            current.row[FFTFunVD]   = 0;
            current.row[FFTFunFD]   = 0;
            continue;
        }
        ////////////////////////////////////////////////////////////////////////////////
        // FFT input
        ////////////////////////////////////////////////////////////////////////////////
        iint n = 0;
        for(int j = 0; j < p; j++)
        {
            if(isFunction)
            {
                float ch0 = display.fft0.bytes[j];
                float ch1 = display.fft1.bytes[j];
                fft.aRe[j]   = channelFunction(ch0, ch1, wndMain.function.Type, wndMain);
                fft.aIm[j]   = 0.0;
                fft.aAmpl[j] = 0.0;
                n = min(display.samples, display.samples);
            }
            else
            {
                n = display.samples;
                if(ch == 0) { fft.aRe[j] = display.fft0.bytes[j]; }
                if(ch == 1) { fft.aRe[j] = display.fft1.bytes[j]; }
                fft.aIm[j]   = 0.0;
                fft.aAmpl[j] = 0.0;
            }
        }
        ////////////////////////////////////////////////////////////////////////////////
        // FFT calculate
        ////////////////////////////////////////////////////////////////////////////////
        fftRCalculateAmp(p, &fft.aRe[0], &fft.aIm[0], &fft.aAmpl[0]);
        int szBins = p / 2;
        ////////////////////////////////////////////////////////////////////////////////
        // measure
        ////////////////////////////////////////////////////////////////////////////////
        if(szBins)
        {
            double       time = pOsciloscope->window.horizontal.Capture;
            double   maxHertz = (1.f / time) * 0.5f;
            double freq0 = wndMain.measure.data.pickFFT0.position.getXFreq();
            double freq1 = wndMain.measure.data.pickFFT1.position.getXFreq();
            int x0Bin = 0;
            int x1Bin = 0;
            double x0Freq = 0;
            double x1Freq = 0;
            double x0Amplitude = 0;
            double x1Amplitude = 0;
            if (wndMain.display.fftLogFreq == 0)
            {
                x0Bin = wndMain.measure.data.pickFFT0.position.x * szBins;
                x1Bin = wndMain.measure.data.pickFFT1.position.x * szBins;
            }
            else
            {
                x0Bin = pow(szBins, wndMain.measure.data.pickFFT0.position.x);
                x1Bin = pow(szBins, wndMain.measure.data.pickFFT1.position.x);
            }
            x0Freq = ((double)x0Bin / (double)szBins) * maxHertz;
            x1Freq = ((double)x1Bin / (double)szBins) * maxHertz;
            if(wndMain.display.fftDecibel == 0)
            {
                x0Amplitude = fft.aAmpl[x0Bin];
                x1Amplitude = fft.aAmpl[x1Bin];
            }
            else
            {
                x0Amplitude = 20 * log10(fft.aAmpl[x0Bin] / 1.0); // dB (relative to full scale sine wave)
                x1Amplitude = 20 * log10(fft.aAmpl[x1Bin] / 1.0);
            }
            switch(ch)
            {
                case 0:
                    {
                        current.row[FFTCh0F0]   = x0Freq;
                        current.row[FFTCh0V0db] = x0Amplitude;
                        current.row[FFTCh0F1]   = x1Freq;
                        current.row[FFTCh0V1db] = x1Amplitude;
                        current.row[FFTCh0VD] = max(current.row[FFTCh0V0db], current.row[FFTCh0V1db]) - min(current.row[FFTCh0V0db], current.row[FFTCh0V1db]);
                        current.row[FFTCh0FD] = max(current.row[FFTCh0F0],   current.row[FFTCh0F1])   - min(current.row[FFTCh0F0],   current.row[FFTCh0F1]);
                    }
                    break;
                case 1:
                    {
                        current.row[FFTCh1F0]   = x0Freq;
                        current.row[FFTCh1V0db] = x0Amplitude;
                        current.row[FFTCh1F1]   = x1Freq;
                        current.row[FFTCh1V1db] = x1Amplitude;
                        current.row[FFTCh1VD] = max(current.row[FFTCh1V0db], current.row[FFTCh1V1db]) - min(current.row[FFTCh1V0db], current.row[FFTCh1V1db]);
                        current.row[FFTCh1FD] = max(current.row[FFTCh1F0],   current.row[FFTCh1F1])   - min(current.row[FFTCh1F0],   current.row[FFTCh1F1]);
                    }
                    break;
                case 2:
                    {
                        current.row[FFTFunF0]   = x0Freq;
                        current.row[FFTFunV0db] = x0Amplitude;
                        current.row[FFTFunF1]   = x1Freq;
                        current.row[FFTFunV1db] = x1Amplitude;
                        current.row[FFTFunVD] = max(current.row[FFTFunV0db], current.row[FFTFunV1db]) - min(current.row[FFTFunV0db], current.row[FFTFunV1db]);
                        current.row[FFTFunFD] = max(current.row[FFTFunF0],   current.row[FFTFunF1])   - min(current.row[FFTFunF0],   current.row[FFTFunF1]);
                    }
                    break;
            };
        }
    };
}

void OsciloscopeThreadRenderer::renderAnalog(uint threadId, OsciloscopeThreadData& threadData, float z, uint channelId, uint shadow, SDisplay& display, float captureTime, float captureVolt, uint color, bool invert)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    if(display.samples == 0)
    { return; }
    if(display.samples > SCOPEFUN_DISPLAY)
    { return; }
    // samples
    uint  isamples = display.samples;
    if(!isamples)
    { return; }
    // sample zoom
    double sampleZoom = render.signalZoom;
    // start / end / increment / xgridsize
    uint  start = 0;
    uint  end = isamples - 1;
    uint  increment = wndMain.display.tessalation2d;
    uint  count = 0;
    uint  realcount = 0;
    uint  bLineCount = end - start + 1;
    // TODO: ?
    for(uint point = start; point <= end; point += increment)
    {
        count++;
    }
    // ETS disabled
    bool isETS = false;
    float      xfactor = 1.f;
    float    xposition = (wndMain.horizontal.Position / 100.f) * xfactor;
    // y
    const float yGridMax = 0.5f;
    float yfactor = yGridMax;
    const double yOffset = 0;
    if(invert)
    { yfactor = -yfactor; }
    // begin rendering
    if(isETS)
    {
        pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_POINT, count);
    }
    else
    {
        if(wndMain.display.signalType == 0)
        {
            pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, 2 * count);
        }
        else
        {
            //pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_STRIP, 2 * count);
            pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, count);
        }
    }
    // draw samples
    if(isamples)
    {
        float displaySampleOffset = float(increment) / bLineCount;
        double halfSampleOffset = 0.5 * displaySampleOffset;
        if(isETS)
        {
            // ETS disabled: no offset
            float etsOffset = 0.0f;
            for(uint point = start; point <= end; point += increment)
            {
                uint idx = point;
                float xStart, yStart = 0;
                if(channelId == 0)
                {
                    xStart = (float(point) / float(bLineCount)) + etsOffset;
                    yStart = display.analog0.bytes[idx] * yfactor + float(yOffset);
                }

                if(channelId == 1)
                {
                    xStart = (float(point) / float(bLineCount)) + halfSampleOffset + xposition + etsOffset;
                    yStart = display.analog1.bytes[idx] * yfactor + float(yOffset);
                }

                Vector4 vpoint = Vector4(xStart, yStart, z, 1.f);
                pCanvas3d->bPoint(threadId, vpoint);
                // increment
                xStart += xStart + displaySampleOffset;
            }
        }

        else
        {
            // analog CH1, CH2
            float xstart, xend;
            float ystart, yend;

            for(uint point = start; point <= end; point += increment)
            {
                uint idx0 = point;
                uint idx1 = point + increment;
                if (end == SCOPEFUN_DISPLAY - 1)
                    idx1 = clamp<uint>(point + increment, start, end);  //limit display to final sample

                if(channelId == 0)
                {
                    if (idx0 == start) {
                        ystart = display.analog0.bytes[idx0] * yfactor + float(yOffset);
                    }
                    else {
                        ystart = yend;
                    }
                    yend     = display.analog0.bytes[idx1] * yfactor + float(yOffset);
                    xstart = float(point) / float(bLineCount);
                    xend   = xstart + displaySampleOffset;
                }

                if(channelId == 1)
                {
                    if (idx0 == start) {
                        ystart = display.analog1.bytes[idx0] * yfactor + float(yOffset);

                    }
                    else {
                        ystart = yend;
                    }
                    yend   = display.analog1.bytes[idx1] * yfactor + float(yOffset);

                    xstart = (float(point) / float(bLineCount)) + halfSampleOffset + xposition;
                    xend   = xstart + displaySampleOffset;
                }

                Vector4 vstartH = Vector4(xstart, ystart, z, 1.f);
                Vector4 vendH   = Vector4(xend,   ystart, z, 1.f);

                Vector4 vstartV = Vector4(xend, ystart, z, 1.f);
                Vector4 vendV   = Vector4(xend, yend,   z, 1.f);

                if(wndMain.display.signalType == 0)
                {
                    pCanvas3d->bLine(threadId, vstartH, vendH); //horizontal line
                    pCanvas3d->bLine(threadId, vstartV, vendV); //vertical line
                    realcount++;
                    //continue;
                }
                else
                {
                    pCanvas3d->bLine(threadId, vstartH, vendV); //diagonal line
                    realcount++;
                    /*
                    Vector4 vref0 = Vector4(0.f, 1.f, z, 1.f);
                    Vector4 vref1 = Vector4(1.f, 0.f, z, 1.f);
                    Vector4 vref;
                    if(vecDot3d(vstart, vref0) == 0.000f)
                    {
                        vref = vref1;
                    }
                    else
                    {
                        vref = vref0;
                    }
                    Vector4 vdir = vend - vstart;
                    Vector4 vcross = vecCross3d(vdir, vref);
                    vcross = vecCross3d(vdir, vcross);
                    vcross = vecNormalize3d(vcross);
                    vcross = vcross * Vector4(wndMain.display.signalWidth);
                    Vector4 v0 = vstart + vcross;
                    Vector4 v1 = vstart - vcross;
                    v0.w = v1.w = 1.f;
                    pCanvas3d->bStrip(threadId, v0);
                    pCanvas3d->bStrip(threadId, v1);
                    */
                }
            }
        }
    }
    // end rendering
    if(shadow)
    {
        Vector4 constant = Vector4(wndMain.thermal.heating, 0.f, 0.f, 0.f);
        pCanvas3d->addConstant(threadId, constant);
        pCanvas3d->endBatch(threadId, render.cameraOsc.Final, COLOR_ARGB(255, 255, 255, 255), render.shadowTexture, BLEND_MODE_COPY, CANVAS3D_SHADER_CUSTOM, render.shadowLine3dShader);
    }
    else
    {
        pCanvas3d->endBatch(threadId, render.cameraOsc.Final, color, 0, BLEND_MODE_COPY, wndMain.fftDigital.is(VIEW_SELECT_OSC_3D) ? CANVAS3D_SHADER_DEFAULT_COLOR : CANVAS3D_SHADER_DEFAULT);
    }
}

void OsciloscopeThreadRenderer::renderAnalog3d(uint threadid, OsciloscopeThreadData& threadData, int frameIndex, float z, uint channelId, SDisplay& frame, float captureTime, float captureVolt, uint color, bool invert)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    if(frame.samples == 0 || frame.samples > NUM_SAMPLES)
    {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // samples
    ////////////////////////////////////////////////////////////////////////////////
    uint  isamples = frame.samples;
    ////////////////////////////////////////////////////////////////////////////////
    // start/end/increment/xgridsize
    ////////////////////////////////////////////////////////////////////////////////
    uint  start = 0;
    uint  increment = wndMain.display.tessalation3d;
    increment = pOsciloscope->max3dTesselation(increment, wndMain);
    ////////////////////////////////////////////////////////////////////////////////
    // render
    ////////////////////////////////////////////////////////////////////////////////
    uint end = isamples - 1;
    uint count = 0;
    for(uint point = start; point < end; point += increment)
    {
        count++;
    }
    uint type = wndMain.display.signalType;
    // ETS removed: no per-frame offset
    float   etsOffset = 0.0f;
    float   yGridMax = 0.5f;
    float    yfactor = yGridMax;
    float    xfactor = 1.f;
    float    xposition = (wndMain.horizontal.Position / 100.f) * (xfactor) + etsOffset * xfactor;
    if(invert)
    {
        yfactor = -yfactor;
    }
    double yOffset = 0;
    if(isamples)
    {
        int     i      = 0;
        for(uint point = start; point < end; point += increment)
        {
            uint idx0 = clamp<uint>(point, 0, isamples);
            // x
            float fstart  = (float(point) / NUM_SAMPLES);
            float xstart  = fstart * xfactor + xposition;
            // y
            float ystart = 0;
            if(channelId == 0) { ystart = frame.analog0.bytes[idx0] * yfactor + float(yOffset); }
            if(channelId == 1) { ystart = frame.analog1.bytes[idx0] * yfactor + float(yOffset); }
            if(channelId == 0)
            {
                surfaceFrame0[frameIndex].point[i].pos = Vector4(xstart, ystart, z, 1.f);
            }
            else
            {
                surfaceFrame1[frameIndex].point[i].pos = Vector4(xstart, ystart, z, 1.f);
            }
            i++;
        }
        if(channelId == 0)
        {
            surfaceFrame0[frameIndex].count = i;
        }
        else
        {
            surfaceFrame1[frameIndex].count = i;
        }
    }
}


void OsciloscopeThreadRenderer::renderSurface3d(uint threadId, OsciloscopeThreadData& threadData, int channelId, uint color)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    SurfaceFrame* surface = 0;
    if(channelId ==  0)
    {
        surface = surfaceFrame0;
    }
    if(channelId ==  1)
    {
        surface = surfaceFrame1;
    }
    if(channelId == -1)
    {
        surface = surfaceFrameF;
    }
    uint historyFrameDisplay = threadData.m_historyCount;
    if(historyFrameDisplay < 16)
    {
        int debug = 1;
    }
    if(wndMain.display.solid3d == 0)
    {
        for(uint frame = 0; frame < historyFrameDisplay; frame++)
        {
            uint elCount = max((int)surface[frame].count - 1, 0);
            uint loopCnt = max((int)surface[frame].count,     0);
            pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, elCount);
            for(uint point = 1; point < loopCnt; point++)
            {
                Vector4 p0 = surface[frame].point[point - 1].pos;
                Vector4 p1 = surface[frame].point[point].pos;
                pCanvas3d->bLine(threadId,  p0, p1);
            }
            pCanvas3d->endBatch(threadId,  render.cameraOsc.Final, color, 0, BLEND_MODE_ALPHA, wndMain.display.light3d ? CANVAS3D_SHADER_DEFAULT : CANVAS3D_SHADER_DEFAULT_COLOR);
        }
    }
    else
    {
        for(uint frame = 1; frame < historyFrameDisplay; frame++)
        {
            uint elCount = max(int(2 * surface[frame].count) - 2, 0);
            uint loopCnt = max(int(surface[frame].count), 0);
            pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_TRIANGLE, elCount);
            for(uint point = 1; point < loopCnt; point++)
            {
                Vector4 v[4];
                Vector4 n[4];
                uint iframeM1 = max<uint>(frame - 1, 0);
                uint ipointM1 = max<uint>(point - 1, 0);
                v[0] = surface[iframeM1].point[ipointM1].pos;
                n[0] = surface[iframeM1].point[ipointM1].normal;
                v[1] = surface[iframeM1].point[point].pos;
                n[1] = surface[iframeM1].point[point].normal;
                v[2] = surface[frame].point[point].pos;
                n[2] = surface[frame].point[point].normal;
                v[3] = surface[frame].point[ipointM1].pos;
                n[3] = surface[frame].point[ipointM1].normal;
                int indices[12] = { 0, 2, 1,
                                    2, 0, 3,
                                    2, 0, 1,
                                    0, 2, 3
                                  };
                for(int i = 0; i < 2; i++)
                {
                    pCanvas3d->bTriangleNormal(threadId, v[indices[i * 3 + 0]], v[indices[i * 3 + 1]], v[indices[i * 3 + 2]]);
                }
            }
            pCanvas3d->endBatch(threadId,  render.cameraOsc.Final, color, 0, BLEND_MODE_ALPHA, wndMain.display.light3d ? CANVAS3D_SHADER_NORMAL_COLOR_LIGHT : CANVAS3D_SHADER_NORMAL_COLOR);
        }
    }
}

void OsciloscopeThreadRenderer::renderAnalogFunction(uint threadId, OsciloscopeThreadData& threadData, float z, int function, SDisplay& display, float xCapture, float yCapture0, float yCapture1, uint color, bool invert0, bool invert1)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    if(display.samples == 0)
    { return; }
    if(display.samples > SCOPEFUN_DISPLAY)
    { return; }
    // samples
    uint  isamples = display.samples;
    if(!isamples)
    { return; }
    // sample zoom
    double sampleZoom = render.signalZoom;
    // start / end / increment / xgridsize
    uint  start = 0;
    uint  end = isamples - 1;
    uint  increment = wndMain.display.tessalation2d;
    uint  count = 0;
    uint  realcount = 0;
    uint  bLineCount = end - start + 1;

    for(uint point = start; point <= end; point += increment)
    {
        count++;
    }
    // ETS disabled
    bool isETS = false;
    float      xfactor = 1.f;
    // y
    const float yGridMax = 0.5f;
    float yfactor0 = yGridMax;
    float yfactor1 = yGridMax;
    const double yOffset = 0;
    if(invert0)
    { yfactor0 = -yfactor0; }
    if(invert1)
    { yfactor1 = -yfactor1; }
    // begin rendering
    if(isETS)
    {
        pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_POINT, count);
    }
    else
    {
        if(wndMain.display.signalType == 0)
        {
            pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, 2 * count);
        }
        else
        {
            pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, count);
        }
    }
    // draw samples
    if(isamples)
    {
        float displaySampleOffset = float(increment) / bLineCount;
        double halfSampleOffset = 0.5 * displaySampleOffset;
        double qSampleOffet = 0.25 * displaySampleOffset;
        if(isETS)
        {
            // ETS disabled: no offset
            float etsOffset = 0.0f;
            for(uint point = start; point <= end; point += increment)
            {
                uint idx = point;
                float xStart, yStart, yStart0, yStart1 = 0;
                xStart = (float(point) / float(bLineCount)) + qSampleOffet + etsOffset;
                yStart0 = display.analog0.bytes[idx] * yfactor0 + float(yOffset);
                yStart1 = display.analog1.bytes[idx] * yfactor1 + float(yOffset);
                yStart = channelFunction(yStart0, yStart1, function, wndMain);

                Vector4 vpoint = Vector4(xStart, yStart, z, 1.f);
                pCanvas3d->bPoint(threadId, vpoint);
            }
        }
        else
        {
            // analog CH1, CH2
            float xstart, xend;
            float ystart, ystart0, ystart1, yend, yend0, yend1;

            for(uint point = start; point <= end; point += increment)
            {
                uint idx0 = point;
                uint idx1 = point + increment;
                if (end == SCOPEFUN_DISPLAY - 1)
                    idx1 = clamp<uint>(point + increment, start, end);  //limit display to final sample

                if (idx0 == start) {
                    ystart0 = display.analog0.bytes[idx0] * yfactor0 + float(yOffset);
                    ystart1 = display.analog1.bytes[idx0] * yfactor1 + float(yOffset);
                }
                else {
                    ystart0 = yend0;
                    ystart1 = yend1;
                }
                yend0  = display.analog0.bytes[idx1] * yfactor0 + float(yOffset);
                yend1  = display.analog1.bytes[idx1] * yfactor1 + float(yOffset);

                ystart = channelFunction(ystart0, ystart1, function, wndMain);
                yend   = channelFunction(yend0  , yend1  , function, wndMain);
                xstart = (float(point) / float(bLineCount)) + qSampleOffet;
                xend   = xstart + displaySampleOffset;

                Vector4 vstartH = Vector4(xstart, ystart, z, 1.f);
                Vector4 vendH   = Vector4(xend,   ystart, z, 1.f);

                Vector4 vstartV = Vector4(xend, ystart, z, 1.f);
                Vector4 vendV   = Vector4(xend, yend,   z, 1.f);

                if(wndMain.display.signalType == 0)
                {
                    pCanvas3d->bLine(threadId, vstartH, vendH); //horizontal line
                    pCanvas3d->bLine(threadId, vstartV, vendV); //vertical line
                    realcount++;
                }
                else
                {
                    pCanvas3d->bLine(threadId, vstartH, vendV); //diagonal line
                    realcount++;
                }
            }
        }
    }
    // end rendering
    /*
    if(shadow)
    {
        Vector4 constant = Vector4(wndMain.thermal.heating, 0.f, 0.f, 0.f);
        pCanvas3d->addConstant(threadId, constant);
        pCanvas3d->endBatch(threadId, render.cameraOsc.Final, COLOR_ARGB(255, 255, 255, 255), render.shadowTexture, BLEND_MODE_COPY, CANVAS3D_SHADER_CUSTOM, render.shadowLine3dShader);
    }
    else
    {
        pCanvas3d->endBatch(threadId, render.cameraOsc.Final, color, 0, BLEND_MODE_COPY, wndMain.fftDigital.is(VIEW_SELECT_OSC_3D) ? CANVAS3D_SHADER_DEFAULT_COLOR : CANVAS3D_SHADER_DEFAULT);
    }
    */
    pCanvas3d->endBatch(threadId, render.cameraOsc.Final, color, 0, BLEND_MODE_COPY, wndMain.fftDigital.is(VIEW_SELECT_OSC_3D) ? CANVAS3D_SHADER_DEFAULT_COLOR : CANVAS3D_SHADER_DEFAULT);

}

void OsciloscopeThreadRenderer::renderAnalogFunctionXY(uint threadid, OsciloscopeThreadData& threadData, SDisplay& frame, float xCapture, float yCapture0, float yCapture1, uint color)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    if(!wndMain.function.xyGraph)
    {
        return;
    }
    if(frame.samples == 0)
    {
        return;
    }
    if(frame.samples > NUM_SAMPLES)
    {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // samples
    ////////////////////////////////////////////////////////////////////////////////
    uint  isamples0 = frame.samples;
    if(!isamples0)
    {
        return;
    }
    uint  isamples1 = frame.samples;
    if(!isamples1)
    {
        return;
    }
    if(isamples0 != isamples1)
    {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // start/end/increment/xgridsize
    ////////////////////////////////////////////////////////////////////////////////
    uint  start = 0;
    uint  end = NUM_SAMPLES - 1;
    uint  increment = wndMain.display.tessalation2d;
    ////////////////////////////////////////////////////////////////////////////////
    // render
    ////////////////////////////////////////////////////////////////////////////////
    end = isamples0 - 1;
    uint count = 0;
    for(uint point = start+ increment; point <= end; point += increment)
    {
        uint idx0 = clamp<uint>(point, 0, isamples0);
        uint idx1 = clamp<uint>(point + increment, 0, isamples0);
        count++;
    }
    uint type = wndMain.display.signalType;
    pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE, count);
    float   etsDelta = (1.f / float(NUM_SAMPLES)) / float(render.maxEts);
    // ETS removed: no per-frame offset
    float   etsOffset = 0.0f;
    float   yGridMax   = 0.5f;
    float    yfactor0  = yGridMax;
    float    yfactor1  = yGridMax;
    float    xposition = (wndMain.horizontal.Position / 100.f) + etsOffset;
    float  xCount     = 10.f;
    double yNormalize = double(yCapture0) * double(xCount) / 2.0;
    double yOffset0 = 0;
    double yOffset1 = 0;
    double step0 = 0.f;
    double step1 = 0.f;
    step0 = render.analogChannelYVoltageStep0;
    step1 = render.analogChannelYVoltageStep1;
    float zeroOffset0 = 0.f;
    float zeroOffset1 = 0.f;
    zeroOffset0 = render.analogChannelPositin0 - render.analogChannelOffsets0;
    zeroOffset1 = render.analogChannelPositin1 - render.analogChannelOffsets1;
    float zeroVoltage0 = zeroOffset0 * step0;
    float zeroVoltage1 = zeroOffset1 * step1;
    yOffset0 = (zeroVoltage0 / (wndMain.channel01.Capture * xCount));
    yOffset1 = (zeroVoltage1 / (wndMain.channel02.Capture * xCount));
    if(isamples0 && isamples1)
    {
        for(uint point = start; point <= end; point += increment)
        {
            uint idx0 = clamp<uint>(point, 0, isamples0);
            uint idx1 = clamp<uint>(point + increment, 0, isamples0);
            float x0 = frame.analog0.bytes[idx0] * yfactor0 - float(yOffset0);
            float y0 = frame.analog1.bytes[idx0] * yfactor0 - float(yOffset1);
            float x1 = frame.analog0.bytes[idx1] * yfactor1 - float(yOffset0);
            float y1 = frame.analog1.bytes[idx1] * yfactor1 - float(yOffset1);
            // debug
            float fstart = (float(point) / NUM_SAMPLES);
            float fend = (float(point + increment) / NUM_SAMPLES);
            float xstart = fstart + xposition;
            float xend = fend + xposition;

            Vector4 vstart = Vector4(x0+0.5, y0, 0.f, 1.f);
            Vector4 vend   = Vector4(x1+0.5, y1, 0.f, 1.f);
            pCanvas3d->bLine(threadid, vstart, vend);
        }
    }
    pCanvas3d->endBatch(threadid, render.cameraOsc.Final, color, 0, BLEND_MODE_COPY, CANVAS3D_SHADER_DEFAULT);
}

void OsciloscopeThreadRenderer::renderAnalogFunction3d(uint threadid, OsciloscopeThreadData& threadData, SDisplay& frame, int frameIndex, float z, uint color)
{
    if(frame.samples == 0 || frame.samples > NUM_SAMPLES)
    {
        return;
    }
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    bool invert0 = wndMain.channel01.Invert;
    bool invert1 = wndMain.channel02.Invert;
    float xCapture = wndMain.horizontal.Capture;
    float yCapture = wndMain.channel01.Capture;
    ////////////////////////////////////////////////////////////////////////////////
    // samples
    ////////////////////////////////////////////////////////////////////////////////
    uint  isamples0 = frame.samples;
    if(!isamples0)
    {
        return;
    }
    uint  isamples1 = frame.samples;
    if(!isamples1)
    {
        return;
    }
    if(isamples0 != isamples1)
    {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // start/end/increment/xgridsize
    ////////////////////////////////////////////////////////////////////////////////
    uint  start = 0;
    uint  end = NUM_SAMPLES - 1;
    uint  increment = wndMain.display.tessalation3d;
    increment = pOsciloscope->max3dTesselation(increment, wndMain);
    ////////////////////////////////////////////////////////////////////////////////
    // render
    ////////////////////////////////////////////////////////////////////////////////
    end = isamples0 - 1;
    uint count = 0;
    for(uint point = start+increment; point <= end; point += increment)
    {
        uint idx0 = clamp<uint>(point, 0, isamples0);
        uint idx1 = clamp<uint>(point + increment, 0, isamples0);
        count++;
    }
    uint type = wndMain.display.signalType;
    float   etsDelta = (1.f / float(NUM_SAMPLES)) / float(render.maxEts);
    // ETS removed: no per-frame offset
    float   etsOffset = 0.0f;
    float    yGridMax  = 0.5f;
    float    yfactor0  = yGridMax;
    float    yfactor1  = yGridMax;
    float    xposition = (wndMain.horizontal.Position / 100.f) + etsOffset;
    if(invert0)
    {
        yfactor0 = -yfactor0;
    }
    if(invert1)
    {
        yfactor1 = -yfactor1;
    }
    double yOffset0 = 0;
    double yOffset1 = 0;
    if(isamples0 && isamples1)
    {
        int i = 0;
        for(uint point = start; point <= end; point += increment)
        {
            uint idx0 = clamp<uint>(point, 0, isamples0);
            uint idx1 = clamp<uint>(point + increment, 0, isamples0);
            // y
            float y0 = frame.analog0.bytes[idx0] * yfactor0 + float(yOffset0);
            float y1 = frame.analog1.bytes[idx0] * yfactor1 + float(yOffset1);
            float y      = channelFunction(y0, y1, wndMain.function.Type, wndMain);
            // x
            float fpoint = (float(point) / NUM_SAMPLES);
            float x      = fpoint + xposition;
            // surface
            surfaceFrameF[frameIndex].point[i].pos = Vector4(x, y, z, 1.f);
            i++;
        }
        surfaceFrameF[frameIndex].count = i;
    }
}


void OsciloscopeThreadRenderer::renderMeasureFFT(uint threadId, OsciloscopeThreadData& threadData)
{
    SDisplay&            frame = threadData.m_frame;
    WndMain&           wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    Matrix4x4&   final = render.cameraFFT.Final;
    float FFT0PickX = wndMain.measure.data.pickFFT0.position.x;
    float FFT0PickY = wndMain.measure.data.pickFFT0.position.y;
    float FFT0PickZ = wndMain.measure.data.pickFFT0.position.z;
    float FFT1PickX = wndMain.measure.data.pickFFT1.position.x;
    float FFT1PickY = wndMain.measure.data.pickFFT1.position.y;
    float FFT1PickZ = wndMain.measure.data.pickFFT1.position.z;
    if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        FFT0PickZ = 0.0f;
        FFT1PickZ = 0.0f;
    }
    if(wndMain.measure.data.pickFFT0.active)
    {
        pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, 1);
        pCanvas3d->bLine(threadId,  Vector4(FFT0PickX, yMin, FFT0PickZ, 1), Vector4(FFT0PickX, yMax, FFT0PickZ, 1));
        pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 255, 255, 0), 0, BLEND_MODE_ALPHA);
    }
    if(wndMain.measure.data.pickFFT1.active)
    {
        pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, 1);
        pCanvas3d->bLine(threadId,  Vector4(FFT1PickX, yMin, FFT1PickZ, 1), Vector4(FFT1PickX, yMax, FFT1PickZ, 1));
        pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 0, 255, 255), 0, BLEND_MODE_ALPHA);
    }
    if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        FFT0PickZ = 0.0f;
        FFT1PickZ = 1.0f;
    }
    int indices[12 * 3] =    // bottom
    {
        0, 1, 2,
        0, 2, 3,
        // top
        4, 5, 6,
        4, 6, 7,
        // left
        0, 3, 7,
        0, 7, 4,
        // right
        1, 2, 5,
        5, 2, 6,
        // front
        0, 1, 4,
        4, 1, 5,
        // back
        2, 3, 7,
        7, 2, 6
    };
    ////////////////////////////////////////////////////////////////////////////////
    // X
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.measure.data.pickFFT0.active && wndMain.measure.data.pickFFT1.active)
    {
        if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
        {
            Vector4 v[8];
            v[0] = Vector4(FFT0PickX, yMin, FFT0PickZ, 1);
            v[1] = Vector4(FFT1PickX, yMin, FFT0PickZ, 1);
            v[2] = Vector4(FFT1PickX, yMin, FFT1PickZ, 1);
            v[3] = Vector4(FFT0PickX, yMin, FFT1PickZ, 1);
            v[4] = Vector4(FFT0PickX, yMax, FFT0PickZ, 1);
            v[5] = Vector4(FFT1PickX, yMax, FFT0PickZ, 1);
            v[6] = Vector4(FFT1PickX, yMax, FFT1PickZ, 1);
            v[7] = Vector4(FFT0PickX, yMax, FFT1PickZ, 1);
            pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 12);
            for(int i = 0; i < 12; i++)
            {
                pCanvas3d->bTriangleNormal(threadId, v[indices[i * 3 + 0]], v[indices[i * 3 + 1]], v[indices[i * 3 + 2]]);
            }
            pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);
        }
        else
        {
            pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 2);
            pCanvas3d->bTriangleNormal(threadId,  Vector4(FFT0PickX, yMin, 0, 1), Vector4(FFT0PickX, yMax, 0, 1), Vector4(FFT1PickX, yMax, 0, 1));
            pCanvas3d->bTriangleNormal(threadId,  Vector4(FFT0PickX, yMin, 0, 1), Vector4(FFT1PickX, yMax, 0, 1), Vector4(FFT1PickX, yMin, 0, 1));
            pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);
        }
    }
}

void OsciloscopeThreadRenderer::renderMeasure(uint threadId, OsciloscopeThreadData& threadData)
{
    SDisplay&            frame = threadData.m_frame;
    WndMain&           wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    double           sigZoom = render.signalZoom;
    double           sigPos = render.signalPosition;
    Matrix4x4&   final = render.cameraOsc.Final;
    float pre = wndMain.trigger.Percent / 100.0;
    float X0PickX = wndMain.measure.data.pickX0.position.x;
    float X0PickY = wndMain.measure.data.pickX0.position.y;
    float X0PickZ = wndMain.measure.data.pickX0.position.z;
    float X1PickX = wndMain.measure.data.pickX1.position.x;
    float X1PickY = wndMain.measure.data.pickX1.position.y;
    float X1PickZ = wndMain.measure.data.pickX1.position.z;
    float Y0PickX = wndMain.measure.data.pickY0.position.x;
    float Y0PickY = wndMain.measure.data.pickY0.position.y;
    float Y0PickZ = wndMain.measure.data.pickY0.position.z;
    float Y1PickX = wndMain.measure.data.pickY1.position.x;
    float Y1PickY = wndMain.measure.data.pickY1.position.y;
    float Y1PickZ = wndMain.measure.data.pickY1.position.z;
    if(wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
    {
        X0PickZ = 0.0f;
        X1PickZ = 0.0f;
        Y0PickZ = 0.0f;
        Y1PickZ = 0.0f;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Y
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.measure.data.pickY0.active)
    {
        pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, 1);
        pCanvas3d->bLine(threadId,  Vector4(xMin, Y0PickY, Y0PickZ, 1), Vector4(xMax, Y0PickY, Y0PickZ, 1));
        pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 255, 255, 0), 0, BLEND_MODE_ALPHA);
    }
    if(wndMain.measure.data.pickY1.active)
    {
        pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, 1);
        pCanvas3d->bLine(threadId,  Vector4(xMin, Y1PickY, Y1PickZ, 1), Vector4(xMax, Y1PickY, Y1PickZ, 1));
        pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 0, 255, 255), 0, BLEND_MODE_ALPHA);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // X
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.measure.data.pickX0.active)
    {
        pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, 1);
        pCanvas3d->bLine(threadId,  Vector4(X0PickX, yMin, X0PickZ, 1), Vector4(X0PickX, yMax, X0PickZ, 1));
        pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 255, 255, 0), 0, BLEND_MODE_ALPHA);
    }
    if(wndMain.measure.data.pickX1.active)
    {
        pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_LINE, 1);
        pCanvas3d->bLine(threadId,  Vector4(X1PickX, yMin, X1PickZ, 1), Vector4(X1PickX, yMax, X1PickZ, 1));
        pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(255, 0, 255, 255), 0, BLEND_MODE_ALPHA);
    }
    if(wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
    {
        X0PickZ = 0.0f;
        X1PickZ = 1.0f;
        Y0PickZ = 0.0f;
        Y1PickZ = 1.0f;
    }
    int indices[12 * 3] =  // bottom
    {
        0, 1, 2,
        0, 2, 3,
        // top
        4, 5, 6,
        4, 6, 7,
        // left
        0, 3, 7,
        0, 7, 4,
        // right
        1, 2, 5,
        5, 2, 6,
        // front
        0, 1, 4,
        4, 1, 5,
        // back
        2, 3, 7,
        7, 2, 6
    };
    ////////////////////////////////////////////////////////////////////////////////
    // X
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.measure.data.pickX0.active && wndMain.measure.data.pickX1.active)
    {
        if(wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
        {
            Vector4 v[8];
            v[0] = Vector4(X0PickX,   yMin, X0PickZ, 1);
            v[1] = Vector4(X1PickX,   yMin, X0PickZ, 1);
            v[2] = Vector4(X1PickX,   yMin, X1PickZ, 1);
            v[3] = Vector4(X0PickX,   yMin, X1PickZ, 1);
            v[4] = Vector4(X0PickX,   yMax, X0PickZ, 1);
            v[5] = Vector4(X1PickX,   yMax, X0PickZ, 1);
            v[6] = Vector4(X1PickX,   yMax, X1PickZ, 1);
            v[7] = Vector4(X0PickX,   yMax, X1PickZ, 1);
            pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 12);
            for(int i = 0; i < 12; i++)
            {
                pCanvas3d->bTriangleNormal(threadId,  v[ indices[i * 3 + 0] ], v[ indices[i * 3 + 1] ], v[ indices[i * 3 + 2] ]);
            }
            pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);
        }
        else
        {
            pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 2);
            pCanvas3d->bTriangleNormal(threadId,  Vector4(X0PickX, yMin, 0, 1), Vector4(X0PickX, yMax, 0, 1), Vector4(X1PickX, yMax, 0, 1));
            pCanvas3d->bTriangleNormal(threadId,  Vector4(X0PickX, yMin, 0, 1), Vector4(X1PickX, yMax, 0, 1), Vector4(X1PickX, yMin, 0, 1));
            pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Y
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.measure.data.pickY0.active && wndMain.measure.data.pickY1.active)
    {
        if(wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
        {
            Vector4 v[8];
            v[0] = Vector4(xMin, Y0PickY, Y0PickZ, 1);
            v[1] = Vector4(xMin, Y1PickY, Y0PickZ, 1);
            v[2] = Vector4(xMin, Y1PickY, Y1PickZ, 1);
            v[3] = Vector4(xMin, Y0PickY, Y1PickZ, 1);
            v[4] = Vector4(xMax, Y0PickY, Y0PickZ, 1);
            v[5] = Vector4(xMax, Y1PickY, Y0PickZ, 1);
            v[6] = Vector4(xMax, Y1PickY, Y1PickZ, 1);
            v[7] = Vector4(xMax, Y0PickY, Y1PickZ, 1);
            pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 12);
            for(int i = 0; i < 12; i++)
            {
                pCanvas3d->bTriangleNormal(threadId,  v[indices[i * 3 + 0]], v[indices[i * 3 + 1]], v[indices[i * 3 + 2]]);
            }
            pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);
        }
        else
        {
            pCanvas3d->beginBatch(threadId,  CANVAS3D_BATCH_TRIANGLE, 2);
            pCanvas3d->bTriangleNormal(threadId,  Vector4(xMin, Y0PickY, 0, 1), Vector4(xMax, Y0PickY, 0, 1), Vector4(xMin, Y1PickY, 0, 1));
            pCanvas3d->bTriangleNormal(threadId,  Vector4(xMin, Y1PickY, 0, 1), Vector4(xMax, Y1PickY, 0, 1), Vector4(xMax, Y0PickY, 0, 1));
            pCanvas3d->endBatch(threadId,  final, COLOR_ARGB(64, 128, 128, 128), 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_NORMAL);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderFFTGrid
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderFFTGrid(uint threadid, OsciloscopeThreadData& threadData)
{
    SDisplay&                frame = threadData.m_frame;
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    ////////////////////////////////////////////////////////////////////////////////
    // Grid
    ////////////////////////////////////////////////////////////////////////////////
    if(!wndMain.display.fftGrid)
    {
        return;
    }
    // Horizontal lines
    const int  yDivsLin = 5;
    const float yValMax = 1.25;
    pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE, yDivsLin + 1);
    for(int div = 0; div <= yDivsLin; div++)
    {
        //ToolText::Volt(buffer, 1024, voltDisplay * (yPos + yMax - (charHeight / 2)) * 5.f);
        float yVal     = ((float)div / (float)yDivsLin) * yValMax;  // 0 ... yValMax
        float yPosNorm = (yVal / yValMax) + yMin; // yMin ... yMax
        pCanvas3d->bLine(threadid,  Vector4(xMin, yPosNorm, 0, 1), Vector4(xMax, yPosNorm, 0, 1));
    }
    pCanvas3d->endBatch(threadid,  render.cameraFFT.Final, COLOR_ARGB(255, 25, 25, 25));
    // Vertical lines
    const int xDivsLin = 10;
    float charHeight = pFont->getLineHeight(threadid) / 2.0;
    float minX = render.cameraFFT.View.Pos().x - 0.5f;
    float maxX = render.cameraFFT.View.Pos().x + 0.5f;
    /*
    if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        minX = xMin;
        maxX = xMax;
    }
    */
    FORMAT_BUFFER();
    int xPos = 0;
    float  time = wndMain.horizontal.Capture;
    double maxHertz = (1.f / time) * 0.5f;
    const int logDivsMajor = 6;
    const int logDivsSub = 9;
    Array<float, logDivsMajor * logDivsSub> lines;
    lines.setCount(0);
    // Setup log scale units
    iint szFFT = wndMain.horizontal.FFTSize;
    if(!szFFT)
    {
        return;
    }
    // Vertical lines
    double resolutionFFT = maxHertz / szFFT;
    double xGridMax = log10(maxHertz / (2 * resolutionFFT));
    int divMajor = 0;
    if(wndMain.display.fftLogFreq == 1)
    {
        // log scale time axis
        int multiplierSub = 1;
        double textLastPos = 0;
        for(int i = 0; i < logDivsMajor * logDivsSub; i++)
        {
            double hertz = (2 * resolutionFFT) * multiplierSub * pow(10, divMajor);
            double xGridCurrent = log10(hertz / (2 * resolutionFFT));
            double xGridPos = xGridCurrent / xGridMax;
            double textDelta = xGridPos - textLastPos;
            if (xGridPos > 1.0)
                break;
            // lines position
            lines.pushBack(xGridPos);
            // next grid line
            if (i % logDivsSub == logDivsSub - 1) {
                multiplierSub = 1;
                divMajor++;
            }
            else
                multiplierSub++;
        }
    }
    else
    {
        // linear scale frequency axis
        for(int xDiv = 0; xDiv <= xDivsLin; xDiv++)
        {
            lines.pushBack((float)xDiv / (float)xDivsLin);
        }
    }
    // draw vertical grid lines
    if(lines.getCount())
    {
        pCanvas3d->beginBatch(threadid,  CANVAS3D_BATCH_LINE, lines.getCount());
        for(int i = 0; i < lines.getCount(); i++)
        {
            float xGrid = lines[i];
            pCanvas3d->bLine(threadid, Vector4(xGrid, 0.5f, 0.f, 1.f),  Vector4(xGrid, -0.5f, 0.f, 1.f));
        }
        pCanvas3d->endBatch(threadid, render.cameraFFT.Final, COLOR_ARGB(255, 25, 25, 25));
    }
    // 3D
    if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        float   size = 0.5f;
        float offset = 0.5;
        pCanvas3d->beginBatch(threadid, CANVAS3D_BATCH_LINE, 8);
        pCanvas3d->bLine(threadid, Vector4( size + offset,  size, 0, 1), Vector4( size + offset,  size, 1, 1));
        pCanvas3d->bLine(threadid, Vector4( size + offset, -size, 0, 1), Vector4( size + offset, -size, 1, 1));
        pCanvas3d->bLine(threadid, Vector4(-size + offset,  size, 0, 1), Vector4(-size + offset,  size, 1, 1));
        pCanvas3d->bLine(threadid, Vector4(-size + offset, -size, 0, 1), Vector4(-size + offset, -size, 1, 1));
        pCanvas3d->bLine(threadid, Vector4( size + offset, -size, 1, 1), Vector4( size + offset,  size, 1, 1));
        pCanvas3d->bLine(threadid, Vector4( size + offset,  size, 1, 1), Vector4(-size + offset,  size, 1, 1));
        pCanvas3d->bLine(threadid, Vector4( size + offset, -size, 1, 1), Vector4(-size + offset, -size, 1, 1));
        pCanvas3d->bLine(threadid, Vector4(-size + offset, -size, 1, 1), Vector4(-size + offset,  size, 1, 1));
        pCanvas3d->endBatch(threadid,  render.cameraFFT.Final, COLOR_ARGB(255, 0, 0, 0));
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderFFTAxis
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderFFTAxis(uint threadid, OsciloscopeThreadData& threadData)
{
    SDisplay&                frame = threadData.m_frame;
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    ////////////////////////////////////////////////////////////////////////////////
    // Axis
    ////////////////////////////////////////////////////////////////////////////////
    if(!wndMain.display.fftAxis)
    {
        return;
    }
    pCanvas3d->beginBatch(threadid,  CANVAS3D_BATCH_LINE, 1);
    pCanvas3d->bLine(threadid,  Vector4(xMin, -0.5f, 0.0f, 1.f), Vector4(xMax, -0.5f, 0.f, 1.f));
    pCanvas3d->endBatch(threadid,  render.cameraFFT.Final, COLOR_ARGB(255, 255, 0, 0));
    pCanvas3d->beginBatch(threadid,  CANVAS3D_BATCH_LINE, 1);
    pCanvas3d->bLine(threadid,  Vector4(xMin, 0.5f, 0.f, 1.f),  Vector4(xMin, -0.5f, 0.f, 1.f));
    pCanvas3d->endBatch(threadid, render.cameraFFT.Final, COLOR_ARGB(255, 0, 255, 0));
}

////////////////////////////////////////////////////////////////////////////////
//
// renderFFTUnits
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeThreadRenderer::renderFFTUnits(uint threadid, OsciloscopeThreadData& threadData)
{
    SDisplay&              display = threadData.m_frame;
    MeasureData&              data = threadData.m_window.measure.data;
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    if(display.samples > NUM_SAMPLES)
    { return; }
    iint iSamples = display.samples;
    pFont->setSize(threadid, 0.25f);
    iint szFFT = wndMain.horizontal.FFTSize;
    if(!szFFT)
    {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // freq numbers
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.display.fftUnits)
    {
        int divisions  = 10;
        float charHeight = pFont->getLineHeight(threadid) / 2.0;
        /*
        float      minX = render.cameraFFT.View.Pos().x - 0.5f * render.zoomFFT;
        float      maxX = render.cameraFFT.View.Pos().x + 0.5f * render.zoomFFT;
        */
        float minX = render.cameraFFT.View.Pos().x - 0.5f;
        float maxX = render.cameraFFT.View.Pos().x + 0.5f;
        if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
        {
            minX = xMin;
            maxX = xMax;
        }
        float xDelta    = (maxX - minX);
        float xAdvance  = xDelta / float(divisions);
        FORMAT_BUFFER();
        int xPos = 0;
        float  time = wndMain.horizontal.Capture;
        double maxHertz = (1.f / time) * 0.5f;
        const int logDivsMajor = 6;
        const int logDivsSub = 9;
        // Setup log scale units
        double resolutionFFT = maxHertz / szFFT;
        double xGridMax = log10(maxHertz / (2 * resolutionFFT));
        int divMajor = 0;
        if(wndMain.display.fftLogFreq == 1)
        {
            // log scale frequency axis
            int multiplierSub = 1;
            double textLastPos = 0;
            for(int i = 0; i < logDivsMajor * logDivsSub; i++)
            {
                double hertz = (2 * resolutionFFT) * multiplierSub * pow(10, divMajor);
                double xGridCurrent = log10(hertz / (2 * resolutionFFT));
                double xGridPos = xGridCurrent / xGridMax;
                double textDelta = xGridPos - textLastPos;
                if (xGridPos > 1.0)
                    break;
                // draw units
                if (multiplierSub == 1 || (multiplierSub == 2 && textDelta > 0.08) || xGridPos > 0.99)
                {
                    textLastPos = xGridPos;
                    ToolText::Hertz(formatBuffer, FORMAT_BUFFER_SIZE, hertz);
                    pFont->writeText3d(threadid, render.cameraFFT.Final, xGridPos, -charHeight - 0.5f, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, 0xffffffff, render.fftScaleX, render.fftScaleY);
                }
                // next grid line
                if (i % logDivsSub == logDivsSub - 1) {
                    multiplierSub = 1;
                    divMajor++;
                }
                else
                    multiplierSub++;
            }
        }
        else
        {
            // linear scale frequency axis
            for(float xGrid = minX; xGrid <= maxX; xGrid = xGrid + xAdvance)
            {
                float hertz = float(xGrid) * maxHertz;
                ToolText::Hertz(formatBuffer, FORMAT_BUFFER_SIZE, hertz);
                pFont->writeText3d(threadid, render.cameraFFT.Final, xGrid, -charHeight - 0.5f, 0.f, Vector4(1, 0, 0, 1), Vector4(0, 1, 0, 1), formatBuffer, 0xffffffff, render.fftScaleX, render.fftScaleY);
                xPos++;
            }
        }
        // y units
        // float    voltDisplay = wndMain.channel01.FFTOnOff ? wndMain.channel01.Capture : wndMain.channel02.Capture;
        float  xUnits = xMin - 0.075f;
        double ydelta = 1.f / 5.f;
        int  yDivsLin = 5;
        float yValMax = 1.25;
        char    buffer[1024] = {0};
        if(wndMain.display.fftDecibel == 0)  // linear scale
        {
            for(int div = 0; div <= yDivsLin; div++)
            {
                //ToolText::Volt(buffer, 1024, voltDisplay * (yPos + yMax - (charHeight / 2)) * 5.f);
                float yVal     = ((float)div / (float)yDivsLin) * yValMax;  // 0 ... yValMax
                float yPosNorm = (yVal / yValMax) + yMin; // yMin ... yMax
                FORMAT("%.2f", yVal);
                pFont->writeText3d(threadid, render.cameraFFT.Final, xUnits, yPosNorm + (charHeight), 0, Vector4(1.f, 0.f, 0.f, 1.f), Vector4(0.f, 1.f, 0.f, 1.f), formatBuffer, -1, render.fftScaleX, render.fftScaleY);
            }
        }
        else // log
        {
            for(double yPos = (yMin); yPos <= yMax + ydelta; yPos = yPos + ydelta)
            {
                float yNorm   = (yPos + yMax);
                float dbRange = 10 * wndMain.display.fftDecibel;
                float dBvalue = -1.0 * (1.f - yNorm) * dbRange;
                ToolText::Decibels(buffer, 1024, dBvalue);
                pFont->writeText3d(threadid, render.cameraFFT.Final, xUnits, yPos + (charHeight), 0, Vector4(1.f, 0.f, 0.f, 1.f), Vector4(0.f, 1.f, 0.f, 1.f), buffer, -1, render.fftScaleX, render.fftScaleY);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderFFT
//
////////////////////////////////////////////////////////////////////////////////
double freqFromPosition(int index, int maxIndex, double maxHertz);

void OsciloscopeThreadRenderer::renderFFT(uint threadId, OsciloscopeThreadData& threadData, MeasureData& measure, OsciloscopeFFT& fft, SDisplay& display, bool funtion, float z, uint channelId, uint color)
{
    MeasureData&              data = measure;
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    if(!display.samples)
    {
        return;
    }
    if(display.samples > NUM_SAMPLES)
    {
        return;
    }
    const double yValMax = 1.25;
    iint iFrameSamples = display.captured;
    iint  szFFT = wndMain.horizontal.FFTSize;
    int szFrame = wndMain.horizontal.FrameSize;
    int szBins = szFFT/2; //take single side of FFT spectrum
    if(!szFFT)
    {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // FFT input
    ////////////////////////////////////////////////////////////////////////////////
    for(int i = 0; i < szFFT; i++)
    {
        if(funtion)
        {
            if (i < iFrameSamples) {
                float ch0 = display.fft0.bytes[i];
                float ch1 = display.fft1.bytes[i];
                // TODO: map directly to kiss_fft_cpx*
                fft.aRe[i] = channelFunction(ch0, ch1, wndMain.function.Type, wndMain);
            }
            else {
                fft.aRe[i] = 0;
            }
        }
        else
        {
            if (channelId == 0)
            {
                if (i < iFrameSamples)
                    fft.aRe[i] = display.fft0.bytes[i];
                else
                    fft.aRe[i] = 0;
            }
            if (channelId == 1)
            {
                if (i < iFrameSamples)
                    fft.aRe[i] = display.fft1.bytes[i];
                else
                    fft.aRe[i] = 0;
            }
        }
        fft.aIm[i] = 0.0;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // FFT calculate
    ////////////////////////////////////////////////////////////////////////////////
    fftRCalculateAmp(szFFT, &fft.aRe[0], &fft.aIm[0], &fft.aAmpl[0]);
    ////////////////////////////////////////////////////////////////////////////////
    // Draw
    ////////////////////////////////////////////////////////////////////////////////
    double     time = pOsciloscope->window.horizontal.Capture;
    double  maxFreq = (1.f / time) * 0.5f;
    int    maxIndex = szBins;
    double freq0   = data.pickFFT0.position.getXFreq();
    double freq1   = data.pickFFT1.position.getXFreq();
    double measurePeriod    = 0.f;
    double measureAmplitude = 0.f;
    if(szBins)
    {
        float      delta = wndMain.display.fftWidth;
        float    yfactor = 1.0f;
        uint type = wndMain.display.fftType;
        if(type == 0)
        {
            pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_LINE, szBins);
        }
        else
        {
            pCanvas3d->beginBatch(threadId, CANVAS3D_BATCH_TRIANGLE, 2 * szBins);
        }

        // loop through FFT points
        for(int idx = 0; idx < szBins; idx++)
        {
            // linear scale Y axis (amplitude)
            float amplitude = fft.aAmpl[idx] * yfactor;

            if(wndMain.display.fftDecibel >= 1)
            {
                float dBmin = -10.0 * wndMain.display.fftDecibel;
                float logAmplitude = 20 * log10(amplitude / 1.0); // (relative to full scale sine wave)
                logAmplitude = clamp(logAmplitude, dBmin, 3.f);
                amplitude = 1.0 - (logAmplitude / dBmin);
            }
            double    freq = 0;
            float position = float(idx) / float(szBins);
            if(wndMain.display.fftLogFreq > 0)
            {
                float value = position * float(szBins);
                float log2Val = log2(value);
                float log2Max = log2(szBins);
                position      = log2Val / log2Max;
                freq = freqFromPosition(idx, szBins, maxFreq);
            }
            else
            {
                freq = position * maxFreq;
            }
            if(amplitude >= measureAmplitude)
            {
                measureAmplitude = amplitude;
                measurePeriod    = 1.0 / freq;
            }
            float y0val = 0;
            if(wndMain.display.fftDecibel >= 1)
            {
                y0val = amplitude - 0.5f;
            }
            else
            {
                y0val = (amplitude / yValMax) - 0.5f;
            }
            float      x0pos = position;
            float      x1pos = x0pos - delta;
            float      x2pos = x0pos + delta;
            if(type == 0)
            {
                pCanvas3d->bLine(threadId, Vector4(x0pos, -0.5f, z, 1.f), Vector4(x0pos, y0val, z, 1.f));
            }
            else
            {
                pCanvas3d->bTriangle(threadId,  Vector4(x1pos, y0val, z, 1.f), Vector4(x1pos, -0.5f, z, 1.f), Vector4(x2pos, -0.5f, z, 1.f));
                pCanvas3d->bTriangle(threadId,  Vector4(x1pos, y0val, z, 1.f), Vector4(x1pos, -0.5f, z, 1.f), Vector4(x2pos, y0val, z, 1.f));
            }
        }
        pCanvas3d->endBatch(threadId,  render.cameraFFT.Final, color, 0, BLEND_MODE_ALPHA, CANVAS3D_SHADER_DEFAULT);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// renderSlider
//
////////////////////////////////////////////////////////////////////////////////
void  OsciloscopeSlider::Rectangle(double& rectW, double& rectH, double& rectX, double& rectY, uint width, uint height, double size)
{
    rectW = width - 1;
    rectH = height / 42;
    rectX = 1.f;
    rectY = width * size - rectH / 2;
}

void OsciloscopeSlider::MinMax(double& xMinimum, double& xMaximum, uint width, uint height, double viewX, double size, double zoom)
{
    double rectW = 0;
    double rectH = 0;
    double rectX = 0;
    double rectY = 0;
    Rectangle(rectW, rectH, rectX, rectY, width, height, size);
    double xNormalized = clamp(1.0 - viewX, 0.0, 1.0);
    double sliderWidth = max(rectH / 2.f, width / 2.f * zoom);
    double xPos  = (1.f - xNormalized) * rectW;
    xMinimum = xPos - max(rectH / 2.f, width / 2.f * zoom);
    xMaximum = xPos + max(rectH / 2.f, width / 2.f * zoom);
}

void OsciloscopeThreadRenderer::renderSlider(uint threadId, OsciloscopeThreadData& threadData)
{
    WndMain&               wndMain = threadData.m_window;
    OsciloscopeRenderData&  render = threadData.m_render;
    double posX = (render.sliderPosition);
    if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        posX = 0.5f;
    }
    double sliderRectW = 0;
    double sliderRectH = 0;
    double sliderRectX = 0;
    double sliderRectY = 0;
    double xMinimum = 0;
    double xMaximum = 0;
    double size = render.sliderSize;
    OsciloscopeSlider slider;
    slider.Rectangle(sliderRectW, sliderRectH, sliderRectX, sliderRectY, render.width, render.height, render.sliderSize);
    slider.MinMax(xMinimum, xMaximum, render.width, render.height, posX, render.sliderSize, render.signalZoom);
    // slider
    pCanvas2d->beginBatch(threadId, CANVAS2D_BATCH_RECTANGLE, 1);
    pCanvas2d->bRectangle(threadId, xMinimum + 1, sliderRectY + 1, xMaximum, sliderRectY + sliderRectH - 1);
    pCanvas2d->endBatch(threadId, render.sliderMode == 1 ? render.colorBorder : render.colorGrid);
    // border
    pCanvas2d->beginBatch(threadId, CANVAS2D_BATCH_LINE, 4);
    pCanvas2d->bLine(threadId, sliderRectX, sliderRectY, sliderRectX + sliderRectW, sliderRectY);
    pCanvas2d->bLine(threadId, sliderRectX, sliderRectY + sliderRectH, sliderRectX + sliderRectW, sliderRectY + sliderRectH);
    pCanvas2d->bLine(threadId, sliderRectX, sliderRectY, sliderRectX, sliderRectY + sliderRectH);
    pCanvas2d->bLine(threadId, sliderRectX + sliderRectW, sliderRectY, sliderRectX + sliderRectW, sliderRectY + sliderRectH);
    pCanvas2d->endBatch(threadId, render.colorBorder);
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
