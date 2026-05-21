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
#include <scopefun/gui/OsciloskopMeasure.h>


class MouseLost : public wxMouseCaptureLostEvent
{

} tmp;

OsciloskopMeasure::OsciloskopMeasure(wxWindow* parent)
    :
    Measure(parent)
{
    pOsciloscope->scrollThread = false;
    wxString aStringRow[Last1] =
    {
        // channel 1
        _("Channel 1"),
        _("Vavg"),
        _("Vrms-dc"),
        _("Vrms-ac"),
        _("Vmin"),
        _("Vmax"),
        _("Vpp"),
        _("Tperiod"),
        _("Freq"),
        _("Surface"),
        _("Channel 1 - X"),
        _("V0"),
        _("V1"),
        _("dV"),
        _("T0"),
        _("T1"),
        _("dT"),
        _("dTInv"),
        _("Channel 1 - Y"),
        _("V0"),
        _("V1"),
        _("dV"),
        // channel 2
        _("Channel 2"),
        _("Vavg"),
        _("Vrms-dc"),
        _("Vrms-ac"),
        _("Vmin"),
        _("Vmax"),
        _("Vpp"),
        _("Tperiod"),
        _("Freq"),
        _("Surface"),
        _("Channel 2 - X"),
        _("V0"),
        _("V1"),
        _("dV"),
        _("T0"),
        _("T1"),
        _("dT"),
        _("dTInv"),
        _("Channel 2 - Y"),
        _("V1"),
        _("V2"),
        _("dV"),
        // Function
        _("Function"),
        _("Vavg"),
        _("Vrms - dc"),
        _("Vrms - ac"),
        _("Vmin"),
        _("Vmax"),
        _("Vpp"),
        _("Tperiod"),
        _("Freq"),
        _("Surface"),
        _("Function - X"),
        _("V0"),
        _("V1"),
        _("dV"),
        _("T0"),
        _("T1"),
        _("dT"),
        _("dTInv"),
        _("Function - Y"),
        _("V0"),
        _("V1"),
        _("dV"),
        // FFT
        _("FFT Channel 1"),
        _("V0"),
        _("V1"),
        _("dV"),
        _("F0"),
        _("F1"),
        _("dF"),
        _("FFT Channel 2"),
        _("V0"),
        _("V1"),
        _("dV"),
        _("F0"),
        _("F1"),
        _("dF"),
        _("FFT Function"),
        _("V0"),
        _("V1"),
        _("dV"),
        _("F0"),
        _("F1"),
        _("dF"),
        // digital
        _("Digital X0"),
        _("X0Bit0"),
        _("X0Bit1"),
        _("X0Bit2"),
        _("X0Bit3"),
        _("X0Bit4"),
        _("X0Bit5"),
        _("X0Bit6"),
        _("X0Bit7"),
        _("X0Bit8"),
        _("X0Bit9"),
        _("X0Bit10"),
        _("X0Bit11"),
        _("Digital X1"),
        _("X1Bit0"),
        _("X1Bit1"),
        _("X1Bit2"),
        _("X1Bit3"),
        _("X1Bit4"),
        _("X1Bit5"),
        _("X1Bit6"),
        _("X1Bit7"),
        _("X1Bit8"),
        _("X1Bit9"),
        _("X1Bit10"),
        _("X1Bit11"),
    };
    for(int i = 0; i < Last1; i++)
    {
        aStringR[i] = aStringRow[i];
    }

    // FFT Amplitude log scale units
    wxString aStringCol[Last1] =
    {
        // channel 1
        _("-"),//_("Channel 1"),
        _("V"),//_("Vavg"),
        _("V"),//_("Vrms - dc"),
        _("V"),//_("Vrms - ac"),
        _("V"),//_("Vmin"),
        _("V"),//_("Vmax"),
        _("V"),//_("Vpp"),
        _("s"),//_("Tperiod"),
        _("Hz"),//_("Freq"),
        _("Vs"),//_("Surface"),
        _("-"),//_("Channel 1 - X pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        _("s"),//_("T1"),
        _("s"),//_("T2"),
        _("s"),//_("dT"),
        _("Hz"),//_("dTInv"),
        _("-"),//_("Channel 1 - Y pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        // channel 2
        _("-"),//_("Channel 2"),
        _("V"),//_("Vavg"),
        _("V"),//_("Vrms - dc"),
        _("V"),//_("Vrms - ac"),
        _("V"),//_("Vmin"),
        _("V"),//_("Vmax"),
        _("V"),//_("Vpp"),
        _("s"),//_("Tperiod"),
        _("Hz"),//_("Freq"),
        _("Vs"),//_("Surface"),
        _("-"),//_("Channel 2 - X pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        _("s"),//_("T1"),
        _("s"),//_("T2"),
        _("s"),//_("dT"),
        _("Hz"),//_("dTInv"),
        _("-"),//_("Channel 2 - Y pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        // Function
        _("-"),//_("Function"),
        _("V"),//_("Vavg"),
        _("V"),//_("Vrms - dc"),
        _("V"),//_("Vrms - ac"),
        _("V"),//_("Vmin"),
        _("V"),//_("Vmax"),
        _("V"),//_("Vpp"),
        _("s"),//_("Tperiod"),
        _("Hz"),//_("Freq"),
        _("Vs"),//_("Surface"),
        _("-"),//_("Function - X pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        _("s"),//_("T1"),
        _("s"),//_("T2"),
        _("s"),//_("dT"),
        _("Hz"),//_("dTInv"),
        _("-"),//_("Function - Y pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        // FFT
        _("-"),//_("FFTCh0"),
        _("dB"),//_("Ch0V0"),
        _("dB"),//_("Ch0V1"),
        _("dB"),//_("Ch0VD"),
        _("Hz"),//_("Ch0F0"),
        _("Hz"),//_("Ch0F1"),
        _("Hz"),//_("Ch0FD"),
        _("-"),//_("FFTCh1"),
        _("dB"),//_("Ch1V0"),
        _("dB"),//_("Ch1V1"),
        _("dB"),//_("Ch1VD"),
        _("Hz"),//_("Ch1F0"),
        _("Hz"),//_("Ch1F1"),
        _("Hz"),//_("Ch1FD"),
        _("-"),//_("FFTFun"),
        _("dB"),//_("FunV0"),
        _("dB"),//_("FunV1"),
        _("dB"),//_("FunVD"),
        _("Hz"),//_("FunF0"),
        _("Hz"),//_("FunF1"),
        _("Hz"),//_("FunFD"),
        // digital
        _("-"),//_("DigitalX1"),
        _(""),//_("DigitalX1Bit0"),
        _(""),//_("DigitalX1Bit1"),
        _(""),//_("DigitalX1Bit2"),
        _(""),//_("DigitalX1Bit3"),
        _(""),//_("DigitalX1Bit4"),
        _(""),//_("DigitalX1Bit5"),
        _(""),//_("DigitalX1Bit6"),
        _(""),//_("DigitalX1Bit7"),
        _(""),//_("DigitalX1Bit8"),
        _(""),//_("DigitalX1Bit9"),
        _(""),//_("DigitalX1Bit10"),
        _(""),//_("DigitalX1Bit11"),
        _("-"),//_("DigitalX2"),
        _(""),//_("DigitalX2Bit0"),
        _(""),//_("DigitalX2Bit1"),
        _(""),//_("DigitalX2Bit2"),
        _(""),//_("DigitalX2Bit3"),
        _(""),//_("DigitalX2Bit4"),
        _(""),//_("DigitalX2Bit5"),
        _(""),//_("DigitalX2Bit6"),
        _(""),//_("DigitalX2Bit7"),
        _(""),//_("DigitalX2Bit8"),
        _(""),//_("DigitalX2Bit9"),
        _(""),//_("DigitalX2Bit10"),
        _(""),//_("DigitalX2Bit11")
    };
    // FFT Amplitude linear scale units
    wxString aStringColLin[Last1] =
    {
        // channel 1
        _("-"),//_("Channel 1"),
        _("V"),//_("Vavg"),
        _("V"),//_("Vrms - dc"),
        _("V"),//_("Vrms - ac"),
        _("V"),//_("Vmin"),
        _("V"),//_("Vmax"),
        _("V"),//_("Vpp"),
        _("s"),//_("Tperiod"),
        _("Hz"),//_("Freq"),
        _("Vs"),//_("Surface"),
        _("-"),//_("Channel 1 - X pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        _("s"),//_("T1"),
        _("s"),//_("T2"),
        _("s"),//_("dT"),
        _("Hz"),//_("dTInv"),
        _("-"),//_("Channel 1 - Y pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        // channel 2
        _("-"),//_("Channel 2"),
        _("V"),//_("Vavg"),
        _("V"),//_("Vrms - dc"),
        _("V"),//_("Vrms - ac"),
        _("V"),//_("Vmin"),
        _("V"),//_("Vmax"),
        _("V"),//_("Vpp"),
        _("s"),//_("Tperiod"),
        _("Hz"),//_("Freq"),
        _("Vs"),//_("Surface"),
        _("-"),//_("Channel 2 - X pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        _("s"),//_("T1"),
        _("s"),//_("T2"),
        _("s"),//_("dT"),
        _("Hz"),//_("dTInv"),
        _("-"),//_("Channel 2 - Y pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        // Function
        _("-"),//_("Function"),
        _("V"),//_("Vavg"),
        _("V"),//_("Vrms - dc"),
        _("V"),//_("Vrms - ac"),
        _("V"),//_("Vmin"),
        _("V"),//_("Vmax"),
        _("V"),//_("Vpp"),
        _("s"),//_("Tperiod"),
        _("Hz"),//_("Freq"),
        _("Vs"),//_("Surface"),
        _("-"),//_("Function - X pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        _("s"),//_("T1"),
        _("s"),//_("T2"),
        _("s"),//_("dT"),
        _("Hz"),//_("dTInv"),
        _("-"),//_("Function - Y pick"),
        _("V"),//_("V1"),
        _("V"),//_("V2"),
        _("V"),//_("dV"),
        // FFT
        _("-"),//_("FFTCh0"),
        _(""),//_("Ch0V0"),
        _(""),//_("Ch0V1"),
        _(""),//_("Ch0VD"),
        _("Hz"),//_("Ch0F0"),
        _("Hz"),//_("Ch0F1"),
        _("Hz"),//_("Ch0FD"),
        _("-"),//_("FFTCh1"),
        _(""),//_("Ch1V0"),
        _(""),//_("Ch1V1"),
        _(""),//_("Ch1VD"),
        _("Hz"),//_("Ch1F0"),
        _("Hz"),//_("Ch1F1"),
        _("Hz"),//_("Ch1FD"),
        _("-"),//_("FFTFun"),
        _(""),//_("FunV0"),
        _(""),//_("FunV1"),
        _(""),//_("FunVD"),
        _("Hz"),//_("FunF0"),
        _("Hz"),//_("FunF1"),
        _("Hz"),//_("FunFD"),
        // digital
        _("-"),//_("DigitalX1"),
        _(""),//_("DigitalX1Bit0"),
        _(""),//_("DigitalX1Bit1"),
        _(""),//_("DigitalX1Bit2"),
        _(""),//_("DigitalX1Bit3"),
        _(""),//_("DigitalX1Bit4"),
        _(""),//_("DigitalX1Bit5"),
        _(""),//_("DigitalX1Bit6"),
        _(""),//_("DigitalX1Bit7"),
        _(""),//_("DigitalX1Bit8"),
        _(""),//_("DigitalX1Bit9"),
        _(""),//_("DigitalX1Bit10"),
        _(""),//_("DigitalX1Bit11"),
        _("-"),//_("DigitalX2"),
        _(""),//_("DigitalX2Bit0"),
        _(""),//_("DigitalX2Bit1"),
        _(""),//_("DigitalX2Bit2"),
        _(""),//_("DigitalX2Bit3"),
        _(""),//_("DigitalX2Bit4"),
        _(""),//_("DigitalX2Bit5"),
        _(""),//_("DigitalX2Bit6"),
        _(""),//_("DigitalX2Bit7"),
        _(""),//_("DigitalX2Bit8"),
        _(""),//_("DigitalX2Bit9"),
        _(""),//_("DigitalX2Bit10"),
        _(""),//_("DigitalX2Bit11")
    };
    for(int i = 0; i < Last1; i++)
    {
        aStringC[i]    = aStringCol[i];
        aStringCLin[i] = aStringColLin[i];
    }

    SDL_memset(aHideCount, 0, Last1*sizeof(int) );
    aHideCount[Channel0]   = Ch0XCursor - Channel0   - 1;
    aHideCount[Channel1]   = Ch1XCursor - Channel1   - 1;
    aHideCount[Function]   = FunXCursor - Function   - 1;
    aHideCount[Ch0XCursor] = Ch0YCursor - Ch0XCursor - 1;
    aHideCount[Ch0YCursor] = Channel1   - Ch0YCursor - 1;
    aHideCount[Ch1XCursor] = Ch1YCursor - Ch1XCursor - 1;
    aHideCount[Ch1YCursor] = Function   - Ch1YCursor - 1;
    aHideCount[FunXCursor] = FunYCursor - FunXCursor - 1;
    aHideCount[FunYCursor] = FFTCh0     - FunYCursor - 1;
    aHideCount[FFTCh0]     = FFTCh1     - FFTCh0     - 1;
    aHideCount[FFTCh1]     = FFTFun     - FFTCh1     - 1;
    aHideCount[FFTFun]     = DigitalX0  - FFTFun     - 1;
    aHideCount[DigitalX0]  = DigitalX1  - DigitalX0  - 1;
    aHideCount[DigitalX1]  = Last1      - DigitalX1  - 1;
    recursive = 0;
}

void OsciloskopMeasure::onScrollWinMeasure(wxScrollWinEvent& evt)
{
    if(evt.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK)
    {
        pOsciloscope->scrollThread = true;
    }
    if(evt.GetEventType() == wxEVT_SCROLLWIN_THUMBRELEASE)
    {
        pOsciloscope->scrollThread = false;
    }
}

void DoubleToString(char* buffer, int size, double value);

double freqFromPosition(int index, int maxIndex, double maxHertz)
{
    double ind = double(index);
    double indMax = double(maxIndex);
    double  log2Val = log2(ind);
    double  log2Max = log2(indMax);
    double position = log2Val / log2Max;
    double hertz = pow(indMax, position);
    double norm = hertz / indMax;
    double freq = norm * maxHertz;
    return freq;
}

double getXScroll(wxSlider* slider)
{
    double time   = pOsciloscope->window.horizontal.Capture * pOsciloscope->window.horizontal.FrameSize;
    double xTime  = (double(slider->GetValue()) / double(slider->GetMax())) * time;
    xTime -= time * double(pOsciloscope->window.trigger.Percent / 100.0);
    return xTime;
}

double getYScroll(wxSlider* slider, wxChoice* choice)
{
    double capture = 0.0;
    if(choice->GetSelection() == 0)
    {
        capture = pOsciloscope->window.channel01.Capture;
    }
    if(choice->GetSelection() == 1)
    {
        capture = pOsciloscope->window.channel02.Capture;
    }
    double slVal = slider->GetValue();
    double slRange = double(slider->GetMax()) - double(slider->GetMin());
    return (slVal / slRange) * capture * pOsciloscope->grid.yCount;
}

int setYScroll(double yVolt, wxChoice* choice)
{
    double capture = 0.0;
    if(choice->GetSelection() == 0)
    {
        capture = pOsciloscope->window.channel01.Capture;
    }
    if(choice->GetSelection() == 1)
    {
        capture = pOsciloscope->window.channel02.Capture;
    }
    double slRange = 0;
    return capture * pOsciloscope->grid.yCount;
}

double getFFTScroll(wxSlider* slider)
{
    double       time = pOsciloscope->window.horizontal.Capture;
    double   maxHertz = (1.f / time) * 0.5f;
    iint           p = pOsciloscope->window.horizontal.FFTSize;
    int        count = p / 2 + 1;
    int    maxIndex = count - 1;
    int selection = double(slider->GetValue()) / double(slider->GetMax()) * maxIndex;
    return freqFromPosition(selection, maxIndex, maxHertz);
}


double getFFTSpin(double oldFreq)
{
    double       time = pOsciloscope->window.horizontal.Capture;
    double   maxHertz = (1.f / time) * 0.5f;
    iint           p = pOsciloscope->window.horizontal.FFTSize;
    int        count = p / 2 + 1;
    double linearSpin = maxHertz / double(count);
    if(pOsciloscope->window.display.fftLogFreq == 0)
    {
        return linearSpin;
    }
    else
    {
        int maxIndex = count - 1;
        int index = 0;
        for(int i = 0; i < maxIndex; i++)
        {
            double freq = freqFromPosition(i, maxIndex, maxHertz);
            if(freq >= oldFreq)
            {
                index = i + 1;
                break;
            }
        }
        double newFreq = freqFromPosition(index, maxIndex, maxHertz);
        return max(newFreq, oldFreq) - min(newFreq, oldFreq);
    }
}


double mulXFromIndex(int index)
{
    switch(index)
    {
        case 0:
            return KILO;
        case 1:
            return 1.f;
        case 2:
            return MILI;
        case 3:
            return MICRO;
    };
    return 1.f;
}

double mulFreqFromIndex(int index)
{
    switch(index)
    {
        case 0:
            return 1.f;
        case 1:
            return KILO;
        case 2:
            return MEGA;
    };
    return 1.f;
}

#define MAX_FILTER 256

class ListModel1 : public wxDataViewListStore
{
public:
    wxDataViewListCtrl*     m_list;
    Array<uint, MAX_FILTER> m_row2ID;
    Array<uint, MAX_FILTER> m_ID2Row;
public:
    ListModel1()
    {
        m_list = 0;
    }

    virtual bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const
    {
        wxColour color;
        color.Set(128, 128, 128);
        int rowIndex = m_list->ItemToRow(item);
        int row      = -1;
        if(rowIndex >= 0 && rowIndex < m_row2ID.getCount())
        {
            row = m_row2ID[rowIndex];
        }
        if(row == Channel0 || row == Ch0XCursor || row == Ch0YCursor || row == Channel1 || row == Ch1XCursor || row == Ch1YCursor || row == Function || row == FunXCursor || row == FunYCursor || row == FFTCh0 || row == FFTCh1 || row == FFTFun || row == DigitalX0 || row == DigitalX1)
        {
            attr.SetBackgroundColour(color);
        }
        return false;
    }
} Model1;

void OsciloskopMeasure::insertRow(int row, int id)
{
    wxVector<wxVariant> temp;
    if(id == Channel0 || id == Ch0XCursor || id == Ch0YCursor || id == Channel1 || id == Ch1XCursor || id == Ch1YCursor || id == Function || id == FunXCursor || id == FunYCursor || id == FFTCh0 || id == FFTCh1 || id == FFTFun || id == DigitalX0 || id == DigitalX1)
    {
        temp.push_back(wxVariant("-"));
        temp.push_back(wxVariant(aStringR[id]));
        temp.push_back(wxVariant("0"));
        temp.push_back(wxVariant("0"));
        temp.push_back(wxVariant("0"));
        temp.push_back(wxVariant("0"));
    }
    else
    {
        temp.push_back(wxVariant(""));
        temp.push_back(wxVariant(aStringR[id]));
        temp.push_back(wxVariant("0"));
        temp.push_back(wxVariant("0"));
        temp.push_back(wxVariant("0"));
        temp.push_back(wxVariant("0"));
    }
    m_dataViewListCtrl1->InsertItem(row, temp);
}

void OsciloskopMeasure::removeRow(int row)
{
    int maxRow = m_dataViewListCtrl1->GetItemCount();
    if(row >= 0 && row < maxRow)
    {
        m_dataViewListCtrl1->DeleteItem(row);
    }
}

void OsciloskopMeasure::DisplayOnInitDialog(wxInitDialogEvent& event)
{
    pOsciloscope->window.measure.uiOpen = 1;
    m_spinBtnX0->SetRange(-1000000, 1000000);
    m_spinBtnX0->SetRange(-1000000, 1000000);
    m_spinBtnX1->SetRange(-1000000, 1000000);
    m_spinBtnX1->SetRange(-1000000, 1000000);
    m_spinBtnY0->SetRange(-1000000, 1000000);
    m_spinBtnY0->SetRange(-1000000, 1000000);
    m_spinBtnY1->SetRange(-1000000, 1000000);
    m_spinBtnY1->SetRange(-1000000, 1000000);
    m_spinBtnFFT0->SetRange(-1000000, 1000000);
    m_spinBtnFFT0->SetRange(-1000000, 1000000);
    m_spinBtnFFT1->SetRange(-1000000, 1000000);
    m_spinBtnFFT1->SetRange(-1000000, 1000000);
    m_checkBoxAutoClear->SetValue(pOsciloscope->window.measure.automaticDataClear);
    m_checkBoxAutoClearTrigger->SetValue(pOsciloscope->window.measure.automaticDataClearTrigger);
    m_checkBoxAutoClearTrigger->Hide();
    m_checkBoxAutoClearTrigger->Disable();
    pTimer->init(TIMER_MEASURE);
    pOsciloscope->window.measure.data.column[MEASURE_MINIMUM].SetValue(MAX_DOUBLE);
    pOsciloscope->window.measure.data.column[MEASURE_MAXIMUM].SetValue(-MAX_DOUBLE);
    if(!Model1.m_list)
    {
        Model1.m_list  = m_dataViewListCtrl1;
        Model1.m_row2ID.clear();
        m_dataViewListCtrl1->AssociateModel(&Model1);
        #ifndef PLATFORM_MAC
        m_dataViewListCtrl1->SetDoubleBuffered(true);
        #endif
        m_dataViewListCtrl1->AppendTextColumn(wxString("/"));
        m_dataViewListCtrl1->AppendTextColumn(wxString("Value"));
        m_dataViewListCtrl1->AppendTextColumn(wxString("Current"));
        m_dataViewListCtrl1->AppendTextColumn(wxString("Average"));
        m_dataViewListCtrl1->AppendTextColumn(wxString("Minimum"));
        m_dataViewListCtrl1->AppendTextColumn(wxString("Maximum"));
        int formwidth  = 500;
        int formheight = 500;
        GetClientSize(&formwidth, &formheight);
        formwidth  = formwidth * 10 / 65;
        formheight /= 42;
        m_dataViewListCtrl1->SetSize(formwidth * 7, formheight * Last1);
        m_dataViewListCtrl1->GetColumn(0)->SetWidth(formwidth);
        m_dataViewListCtrl1->GetColumn(0)->SetAlignment(wxALIGN_CENTER);
        m_dataViewListCtrl1->GetColumn(1)->SetWidth(formwidth);
        m_dataViewListCtrl1->GetColumn(1)->SetAlignment(wxALIGN_CENTER);
        m_dataViewListCtrl1->GetColumn(2)->SetWidth(formwidth);
        m_dataViewListCtrl1->GetColumn(2)->SetAlignment(wxALIGN_CENTER);
        m_dataViewListCtrl1->GetColumn(3)->SetWidth(formwidth);
        m_dataViewListCtrl1->GetColumn(3)->SetAlignment(wxALIGN_CENTER);
        m_dataViewListCtrl1->GetColumn(4)->SetWidth(formwidth);
        m_dataViewListCtrl1->GetColumn(4)->SetAlignment(wxALIGN_CENTER);
        m_dataViewListCtrl1->GetColumn(5)->SetWidth(formwidth);
        m_dataViewListCtrl1->GetColumn(5)->SetAlignment(wxALIGN_CENTER);
        m_dataViewListCtrl1->SetRowHeight(formheight);
        for(int i = 0; i < Last1; i++)
        {
            insertRow(i, i);
            Model1.m_row2ID.pushBack(i);
            Model1.m_ID2Row.pushBack(i);
        }
    }
}

void OsciloskopMeasure::m_checkBoxAutoClearOnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.measure.automaticDataClear = m_checkBoxAutoClear->GetValue();
}

void OsciloskopMeasure::m_checkBoxAutoClearTriggerOnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.measure.automaticDataClearTrigger = m_checkBoxAutoClear->GetValue();
}

void OsciloskopMeasure::m_buttonHistoryClearOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.measure.data.Clear();
    pOsciloscope->window.measure.data.column[MEASURE_MINIMUM].SetValue(MAX_DOUBLE);
    pOsciloscope->window.measure.data.column[MEASURE_MAXIMUM].SetValue(-MAX_DOUBLE);
}

void OsciloskopMeasure::CopyToClipBoard(wxDataViewListCtrl* list)
{
    if(wxTheClipboard->Open())
    {
        wxString text;
        text.Append("ScopeFun Oscilloscope Measure Data\n");
        for(uint r = 0; r < list->GetStore()->GetCount(); r++)
        {
            for(uint c = 1; c < list->GetColumnCount(); c++)
            {
                wxVariant value;
                list->GetValue(value, r, c);
                wxString output = value.GetString();
                while(output.length() < 16)
                {
                    size_t len = output.length();
                    if(len % 2)
                    {
                        output.Prepend(" ");
                    }
                    else
                    {
                        output.Append(" ");
                    }
                }
                text.Append(output);
                text.Append(_T("|"));
            }
            text.Append(_T("\n"));
        }
        wxTheClipboard->SetData(new wxTextDataObject(text));
        wxTheClipboard->Close();
    }
}

void OsciloskopMeasure::m_buttonCopyOnButtonClick(wxCommandEvent& event)
{
    CopyToClipBoard(m_dataViewListCtrl1);
}


void OsciloskopMeasure::m_buttonClearXOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.measure.data.pickX0.clear();
    pOsciloscope->window.measure.data.pickX1.clear();
    pOsciloscope->window.measure.data.pickX0.updateUI = true;
    pOsciloscope->window.measure.data.pickX1.updateUI = true;
}

void OsciloskopMeasure::m_buttonClearYOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.measure.data.pickY0.clear();
    pOsciloscope->window.measure.data.pickY1.clear();
    pOsciloscope->window.measure.data.pickY0.updateUI = true;
    pOsciloscope->window.measure.data.pickY1.updateUI = true;
}

void OsciloskopMeasure::m_buttonClearFFTOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.measure.data.pickFFT0.clear();
    pOsciloscope->window.measure.data.pickFFT1.clear();
    pOsciloscope->window.measure.data.pickFFT0.updateUI = true;
    pOsciloscope->window.measure.data.pickFFT1.updateUI = true;
    pOsciloscope->window.measure.data.pickFFT0.position.setXFreq(0.0);
    pOsciloscope->window.measure.data.pickFFT1.position.setXFreq(0.0);
}

void OsciloskopMeasure::m_buttonPickX0OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.measure.data.pickX0.onActivate();
}

void OsciloskopMeasure::m_textCtrlX0OnTextEnter(wxCommandEvent& event)
{
    double time = pFormat->stringToDouble(m_textCtrlX0->GetValue().data().AsChar());
    double  mul = mulXFromIndex(m_choiceX0->GetSelection());
    pOsciloscope->window.measure.data.pickX0.position.setXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom, time * mul);
    m_sliderX0->SetValue(pOsciloscope->window.measure.data.pickX0.position.x * 1000);
}

void OsciloskopMeasure::m_spinBtnX0OnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.measure.data.pickX0.position.xTime -= pOsciloscope->window.horizontal.Capture;
    pOsciloscope->window.measure.data.pickX0.position.setXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom, pOsciloscope->window.measure.data.pickX0.position.xTime);
    pOsciloscope->window.measure.data.pickX0.updateUI = true;
    m_sliderX0->SetValue(pOsciloscope->window.measure.data.pickX0.position.x * 1000);
}

void OsciloskopMeasure::m_spinBtnX0OnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.measure.data.pickX0.position.xTime += pOsciloscope->window.horizontal.Capture;
    pOsciloscope->window.measure.data.pickX0.position.setXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom, pOsciloscope->window.measure.data.pickX0.position.xTime);
    pOsciloscope->window.measure.data.pickX0.updateUI = true;
    m_sliderX0->SetValue(pOsciloscope->window.measure.data.pickX0.position.x * 1000);
}

void OsciloskopMeasure::m_sliderX0OnScroll(wxScrollEvent& event)
{
    pOsciloscope->window.measure.data.pickX0.position.xTime = getXScroll(m_sliderX0);
    pOsciloscope->window.measure.data.pickX0.position.setXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom, pOsciloscope->window.measure.data.pickX0.position.xTime);
    pOsciloscope->window.measure.data.pickX0.updateUI = true;
}

void OsciloskopMeasure::m_buttonPickX1OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.measure.data.pickX1.onActivate();
}

void OsciloskopMeasure::m_textCtrlX1OnTextEnter(wxCommandEvent& event)
{
    double  time = pFormat->stringToDouble(m_textCtrlX1->GetValue().data().AsChar());
    double  mul = mulXFromIndex(m_choiceX1->GetSelection());
    pOsciloscope->window.measure.data.pickX1.position.setXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom, time * mul);
    m_sliderX1->SetValue(pOsciloscope->window.measure.data.pickX1.position.x * 1000);
}

void OsciloskopMeasure::m_spinBtnX1OnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.measure.data.pickX1.position.xTime -= pOsciloscope->window.horizontal.Capture;
    pOsciloscope->window.measure.data.pickX1.position.setXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom, pOsciloscope->window.measure.data.pickX1.position.xTime);
    pOsciloscope->window.measure.data.pickX1.updateUI = true;
    m_sliderX1->SetValue(pOsciloscope->window.measure.data.pickX1.position.x * 1000);
}

void OsciloskopMeasure::m_spinBtnX1OnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.measure.data.pickX1.position.xTime += pOsciloscope->window.horizontal.Capture;
    pOsciloscope->window.measure.data.pickX1.position.setXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom,pOsciloscope->window.measure.data.pickX1.position.xTime);
    pOsciloscope->window.measure.data.pickX1.updateUI = true;
    m_sliderX1->SetValue(pOsciloscope->window.measure.data.pickX1.position.x * 1000);
}

void OsciloskopMeasure::m_sliderX1OnScroll(wxScrollEvent& event)
{
    pOsciloscope->window.measure.data.pickX1.position.xTime = getXScroll(m_sliderX1);
    pOsciloscope->window.measure.data.pickX1.position.setXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom,pOsciloscope->window.measure.data.pickX1.position.xTime);
    pOsciloscope->window.measure.data.pickX1.updateUI = true;
}

void OsciloskopMeasure::m_buttonPickY0OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.measure.data.pickY0.onActivate();
}

void OsciloskopMeasure::m_textCtrlY0OnTextEnter(wxCommandEvent& event)
{
    double volt = pFormat->stringToDouble(m_textCtrlY0->GetValue().data().AsChar());
    pOsciloscope->window.measure.data.pickY0.position.setYVolt(volt, m_choiceY0->GetSelection());
    m_sliderY0->SetValue(pOsciloscope->window.measure.data.pickY0.position.y * 1000);
}

void OsciloskopMeasure::m_choiceY0OnChoice(wxCommandEvent& event)
{
    m_textCtrlY0OnTextEnter(event);
}

void OsciloskopMeasure::m_spinBtnY0OnSpinDown(wxSpinEvent& event)
{
    double spin = pOsciloscope->grid.yCount;
    switch(m_choiceY0->GetSelection())
    {
        case 0:
            spin *= pOsciloscope->window.channel01.Capture;
            break;
        case 1:
            spin *= pOsciloscope->window.channel02.Capture;
            break;
    };
    spin /= 511.0;
    pOsciloscope->window.measure.data.pickY0.position.yVolt -= spin;
    pOsciloscope->window.measure.data.pickY0.position.setYVolt(pOsciloscope->window.measure.data.pickY0.position.yVolt, m_choiceY0->GetSelection());
    pOsciloscope->window.measure.data.pickY0.updateUI = true;
    m_sliderY0->SetValue(pOsciloscope->window.measure.data.pickY0.position.y * 1000);
}

void OsciloskopMeasure::m_spinBtnY0OnSpinUp(wxSpinEvent& event)
{
    double spin = pOsciloscope->grid.yCount;
    switch(m_choiceY0->GetSelection())
    {
        case 0:
            spin *= pOsciloscope->window.channel01.Capture;
            break;
        case 1:
            spin *= pOsciloscope->window.channel02.Capture;
            break;
    };
    spin /= 511.0;
    pOsciloscope->window.measure.data.pickY0.position.yVolt += spin;
    pOsciloscope->window.measure.data.pickY0.position.setYVolt(pOsciloscope->window.measure.data.pickY0.position.yVolt, m_choiceY0->GetSelection());
    pOsciloscope->window.measure.data.pickY0.updateUI = true;
    m_sliderY0->SetValue(pOsciloscope->window.measure.data.pickY0.position.y * 1000);
}

void OsciloskopMeasure::m_sliderY0OnScroll(wxScrollEvent& event)
{
    double yVolt = getYScroll(m_sliderY0, m_choiceY0);
    pOsciloscope->window.measure.data.pickY0.position.yVolt = yVolt;
    pOsciloscope->window.measure.data.pickY0.position.setYVolt(yVolt, m_choiceY0->GetSelection());
    pOsciloscope->window.measure.data.pickY0.updateUI = true;
}

void OsciloskopMeasure::m_buttonPickY1OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.measure.data.pickY1.onActivate();
}

void OsciloskopMeasure::m_textCtrlY1OnTextEnter(wxCommandEvent& event)
{
    double volt = pFormat->stringToDouble(m_textCtrlY1->GetValue().data().AsChar());
    pOsciloscope->window.measure.data.pickY1.position.setYVolt(volt, m_choiceY1->GetSelection());
    m_sliderY1->SetValue(pOsciloscope->window.measure.data.pickY1.position.y * 1000);
}

void OsciloskopMeasure::m_choiceY1OnChoice(wxCommandEvent& event)
{
    m_textCtrlY1OnTextEnter(event);
}

void OsciloskopMeasure::m_spinBtnY1OnSpinDown(wxSpinEvent& event)
{
    double spin = pOsciloscope->grid.yCount;
    switch(m_choiceY1->GetSelection())
    {
        case 0:
            spin *= pOsciloscope->window.channel01.Capture;
            break;
        case 1:
            spin *= pOsciloscope->window.channel02.Capture;
            break;
    };
    spin /= 511.0;
    pOsciloscope->window.measure.data.pickY1.position.yVolt -= spin;
    pOsciloscope->window.measure.data.pickY1.position.setYVolt(pOsciloscope->window.measure.data.pickY1.position.yVolt, m_choiceY1->GetSelection());
    pOsciloscope->window.measure.data.pickY1.updateUI = true;
    m_sliderY1->SetValue(pOsciloscope->window.measure.data.pickY1.position.y * 1000);
}

void OsciloskopMeasure::m_spinBtnY1OnSpinUp(wxSpinEvent& event)
{
    double spin = pOsciloscope->grid.yCount;
    switch(m_choiceY1->GetSelection())
    {
        case 0:
            spin *= pOsciloscope->window.channel01.Capture;
            break;
        case 1:
            spin *= pOsciloscope->window.channel02.Capture;
            break;
    };
    spin /= 511.0;
    pOsciloscope->window.measure.data.pickY1.position.yVolt += spin;
    pOsciloscope->window.measure.data.pickY1.position.setYVolt(pOsciloscope->window.measure.data.pickY1.position.yVolt, m_choiceY1->GetSelection());
    pOsciloscope->window.measure.data.pickY1.updateUI = true;
    m_sliderY1->SetValue(pOsciloscope->window.measure.data.pickY1.position.y * 1000);
}

void OsciloskopMeasure::m_sliderY1OnScroll(wxScrollEvent& event)
{
    pOsciloscope->window.measure.data.pickY1.position.yVolt = getYScroll(m_sliderY1, m_choiceY1);
    pOsciloscope->window.measure.data.pickY1.position.setYVolt(pOsciloscope->window.measure.data.pickY1.position.yVolt, m_choiceY1->GetSelection());
    pOsciloscope->window.measure.data.pickY1.updateUI = true;
}

void OsciloskopMeasure::m_buttonPickFFT0OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.measure.data.pickFFT0.onActivate();
}

void OsciloskopMeasure::m_textCtrlFFT0OnTextEnter(wxCommandEvent& event)
{
    double freq = pFormat->stringToDouble(m_textCtrlFFT0->GetValue().data().AsChar());
    double  mul = mulFreqFromIndex(m_choiceFFT0->GetSelection());
    pOsciloscope->window.measure.data.pickFFT0.position.setXFreq(freq * mul);
    m_sliderFFT0->SetValue(pOsciloscope->window.measure.data.pickFFT0.position.x * 1000);
}

void OsciloskopMeasure::m_spinBtnFFT0OnSpinDown(wxSpinEvent& event)
{
    double deltaFreq = getFFTSpin(pOsciloscope->window.measure.data.pickFFT0.position.xFreq);
    pOsciloscope->window.measure.data.pickFFT0.position.xFreq -= deltaFreq;
    pOsciloscope->window.measure.data.pickFFT0.position.setXFreq(pOsciloscope->window.measure.data.pickFFT0.position.xFreq);
    pOsciloscope->window.measure.data.pickFFT0.updateUI = true;
    m_sliderFFT0->SetValue(pOsciloscope->window.measure.data.pickFFT0.position.x * 1000);
}

void OsciloskopMeasure::m_spinBtnFFT0OnSpinUp(wxSpinEvent& event)
{
    double deltaFreq = getFFTSpin(pOsciloscope->window.measure.data.pickFFT0.position.xFreq);
    pOsciloscope->window.measure.data.pickFFT0.position.xFreq += deltaFreq;
    pOsciloscope->window.measure.data.pickFFT0.position.setXFreq(pOsciloscope->window.measure.data.pickFFT0.position.xFreq);
    pOsciloscope->window.measure.data.pickFFT0.updateUI = true;
    m_sliderFFT0->SetValue(pOsciloscope->window.measure.data.pickFFT0.position.x * 1000);
}

void OsciloskopMeasure::m_sliderFFT0OnScroll(wxScrollEvent& event)
{
    pOsciloscope->window.measure.data.pickFFT0.position.xFreq = getFFTScroll(m_sliderFFT0);
    pOsciloscope->window.measure.data.pickFFT0.position.setXFreq(pOsciloscope->window.measure.data.pickFFT0.position.xFreq);
    pOsciloscope->window.measure.data.pickFFT0.updateUI = true;
}

void OsciloskopMeasure::m_buttonPickFFT1OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.measure.data.pickFFT1.onActivate();
}

void OsciloskopMeasure::m_textCtrlFFT1OnTextEnter(wxCommandEvent& event)
{
    double freq = pFormat->stringToDouble(m_textCtrlFFT1->GetValue().data().AsChar());
    double  mul = mulFreqFromIndex(m_choiceFFT1->GetSelection());
    pOsciloscope->window.measure.data.pickFFT1.position.setXFreq(freq * mul);
    m_sliderFFT1->SetValue(pOsciloscope->window.measure.data.pickFFT1.position.x * 1000);
}

void OsciloskopMeasure::m_spinBtnFFT1OnSpinDown(wxSpinEvent& event)
{
    double deltaFreq = getFFTSpin(pOsciloscope->window.measure.data.pickFFT1.position.xFreq);
    pOsciloscope->window.measure.data.pickFFT1.position.xFreq -= deltaFreq;
    pOsciloscope->window.measure.data.pickFFT1.position.setXFreq(pOsciloscope->window.measure.data.pickFFT1.position.xFreq);
    pOsciloscope->window.measure.data.pickFFT1.updateUI = true;
    m_sliderFFT1->SetValue(pOsciloscope->window.measure.data.pickFFT1.position.x * 1000);
}

void OsciloskopMeasure::m_spinBtnFFT1OnSpinUp(wxSpinEvent& event)
{
    double deltaFreq = getFFTSpin(pOsciloscope->window.measure.data.pickFFT1.position.xFreq);
    pOsciloscope->window.measure.data.pickFFT1.position.xFreq += deltaFreq;
    pOsciloscope->window.measure.data.pickFFT1.position.setXFreq(pOsciloscope->window.measure.data.pickFFT1.position.xFreq);
    pOsciloscope->window.measure.data.pickFFT1.updateUI = true;
    m_sliderFFT1->SetValue(pOsciloscope->window.measure.data.pickFFT1.position.x * 1000);
}

void OsciloskopMeasure::m_sliderFFT1OnScroll(wxScrollEvent& event)
{
    pOsciloscope->window.measure.data.pickFFT1.position.xFreq = getFFTScroll(m_sliderFFT1);
    pOsciloscope->window.measure.data.pickFFT1.position.setXFreq(pOsciloscope->window.measure.data.pickFFT1.position.xFreq);
    pOsciloscope->window.measure.data.pickFFT1.updateUI = true;
}

void OsciloskopMeasure::m_ItemActivated( wxDataViewEvent& event )
{
// TODO: Implement m_ItemActivated
   SelectionChanged(event);
}

void OsciloskopMeasure::m_SelectionChanged1(wxDataViewEvent& event)
{
   // TODO: Implement m_ItemStartEditing
}

void OsciloskopMeasure::m_ItemStartEditing( wxDataViewEvent& event )
{
// TODO: Implement m_ItemStartEditing
}

void OsciloskopMeasure::m_OnLeftDown( wxMouseEvent& event )
{
    // TODO: Implement m_OnLeftDown
}

void OsciloskopMeasure::m_dataViewListCtrl1OnMouseEvents( wxMouseEvent& event )
{
    // TODO: Implement m_dataViewListCtrl1OnMouseEvents
}

void OsciloskopMeasure::m_buttonOkOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonOkOnButtonClick
    Hide();
}

void OsciloskopMeasure::m_buttonDefaultOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonDefaultOnButtonClick
    pOsciloscope->window.measure.Default();
    wxInitDialogEvent initevent;
    DisplayOnInitDialog(initevent);
}

void OsciloskopMeasure::m_buttonCancelOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonCancelOnButtonClick
    Hide();
}


void OsciloskopMeasure::setTextCtrlFreq(wxTextCtrl* ctr, wxChoice* choice, double value)
{
    int index = multiplyerIndexFromValue(value);
    double mul = multiplyerDoubleFromValue(value);
    if(choice)
    {
        switch(index)
        {
            case 0:
                choice->SetSelection(0);
                break;
            case 1:
                choice->SetSelection(1);
                break;
            case 2:
                choice->SetSelection(2);
                break;
        };
    }
    ctr->SetValue(pFormat->doubleToString(value * mul));
}

float secondsToRange(float s)
{
    float capture = pOsciloscope->window.horizontal.Capture;
    float range = float(NUM_SAMPLES) * capture / 2.f;
    return (s / range) * 0.5f;
}

float rangeToSeconds(float range)
{
    float xCount = pOsciloscope->grid.xCount;
    float capture = pOsciloscope->window.horizontal.Capture;
    return xCount * range * capture * (float(NUM_SAMPLES) / xCount);
}


void OsciloskopMeasure::SelectionChanged(wxDataViewEvent& event)
{
    if (recursive)
    {
      event.Skip();
      return;
    }
    recursive = 1;
    int    row = m_dataViewListCtrl1->ItemToRow(event.GetItem());
    int maxRow = m_dataViewListCtrl1->GetItemCount();
    if(row >= 0 && row < maxRow )
    {
        wxVariant showHide;
        m_dataViewListCtrl1->GetValue(showHide, row, 0);
        if(showHide == wxVariant(_T("[ ]")))
        {
            showHide = wxVariant("[X]");
            m_dataViewListCtrl1->SetValue(showHide, row, 0);
        }
        else if(showHide == wxVariant(_T("[X]")))
        {
            showHide = wxVariant("[ ]");
            m_dataViewListCtrl1->SetValue(showHide, row, 0);
        }
        else if(showHide == wxVariant(_T("-")))
        {
            // hide
            showHide = _T("+");
            int id    = Model1.m_row2ID[row];
            int count = aHideCount[id];
            for(int i = 0; i < count; i++)
            {
                removeRow(row + 1);
                Model1.m_row2ID.remove(row + 1);
            }
            m_dataViewListCtrl1->SetValue(showHide, row, 0);
        }
        else if(showHide == wxVariant(_T("+")))
        {
            // show
            showHide = _T("-");
            int    id = Model1.m_row2ID[row];
            int count = aHideCount[id];
            for (int i = 0; i < count; i++)
            {
               insertRow(row + i + 1, id + i + 1);
               Model1.m_row2ID.insert(row + i + 1, id + i + 1 );
            }
            m_dataViewListCtrl1->SetValue(showHide, row, 0);
        }
    }
    recursive = 0;
    event.Skip();
}


void DoubleToString(char* buffer, int size, double value, char* unit)
{
    FORMAT_BUFFER();
    double cmp = fabs(value);
    if     (value >=  MAX_DOUBLE / 100) FORMAT("max %s", unit)
    else if(value <= -MAX_DOUBLE / 100) FORMAT("min %s", unit)
    else if(value == 0.0) FORMAT("0 %s",  unit)
    else if(cmp < 100 * DOUBLE_GIGA  && cmp >= 100 * DOUBLE_MEGA)  FORMAT("%.3f G%s", value / DOUBLE_GIGA, unit)
    else if(cmp < 100 * DOUBLE_MEGA  && cmp >= 100 * DOUBLE_KILO)  FORMAT("%.3f M%s", value / DOUBLE_MEGA, unit)
    else if(cmp < 100 * DOUBLE_KILO  && cmp >= 100)                FORMAT("%.3f k%s", value / DOUBLE_KILO, unit)
    else if(cmp < 100 && cmp >= 0.1)                               FORMAT("%.3f %s",  value, unit)
    else if(cmp < 100 * DOUBLE_MILI  && cmp >= 100 * DOUBLE_MICRO) FORMAT("%.3f m%s", value * DOUBLE_KILO, unit)
    else if(cmp < 100 * DOUBLE_MICRO && cmp >= 100 * DOUBLE_NANO)  FORMAT("%.3f u%s", value * DOUBLE_MEGA, unit)
    else if(cmp < 100 * DOUBLE_NANO  && cmp >= 100 * DOUBLE_PIKO)  FORMAT("%.3f n%s", value * DOUBLE_GIGA, unit)
    else                                                           FORMAT("%.3f n%s", value * DOUBLE_GIGA, unit);
    memcpy(buffer, formatBuffer, FORMAT_BUFFER_SIZE);
}

void OsciloskopMeasure::setCell(int r, int i, MeasureHistory his)
{
    int col = (int)his + 2;
    EValueGrid1 type = (EValueGrid1)r;
    double dValue = pOsciloscope->window.measure.data.column[his].Row(type);
    wxVariant value;
    FORMAT_BUFFER();
    DoubleToString(formatBuffer, FORMAT_BUFFER_SIZE, dValue, aStringC[type].char_str().data());
    value = formatBuffer;
    int row = r;
    if(!(row == Channel0 || row == Ch0XCursor || row == Ch0YCursor || row == Channel1 || row == Ch1XCursor || row == Ch1YCursor || row == Function || row == FunXCursor || row == FunYCursor || row == FFTCh0 || row == FFTCh1 || row == FFTFun || row == DigitalX0 || row == DigitalX1))
    {
        if (row < DigitalX0)
        {
            m_dataViewListCtrl1->SetValue(value, i, col);
        }
        else
        {
            // digital
            if (dValue != 0)
            {
                m_dataViewListCtrl1->SetValue("1", i, col);
            }
            else
            {
                m_dataViewListCtrl1->SetValue("0", i, col);
            }
        }

    }
    else
    {
        SDL_memset(formatBuffer, 0, FORMAT_BUFFER_SIZE);
        value = formatBuffer;
        m_dataViewListCtrl1->SetValue(value, i, col);
    }
}

void OsciloskopMeasure::setHistory(MeasureHistory type)
{
    MeasureChannelData& his = pOsciloscope->window.measure.data.column[type];
    for(int i = 0; i < Model1.m_row2ID.getCount(); i++)
    {
        setCell(Model1.m_row2ID[i], i, type);
    }
}

void OsciloskopMeasure::PopulateWithData()
{
    setHistory(MEASURE_CURRENT);
    setHistory(MEASURE_AVERAGE);
    setHistory(MEASURE_MINIMUM);
    setHistory(MEASURE_MAXIMUM);
    m_dataViewListCtrl1->Refresh();
}


////////////////////////////////////////////////////////////////////////////////////////
// setTextCtrl
////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopMeasure::setTextCtrlY(wxTextCtrl* ctr, double value)
{
    double mul = multiplyerDoubleFromValue(value);
    ctr->SetValue(pFormat->doubleToString(value * mul));
}

void OsciloskopMeasure::setTextCtrlX(wxTextCtrl* ctr, wxChoice* choice, double value)
{
    int index = multiplyerIndexFromValue(value);
    double mul = multiplyerDoubleFromValue(value);
    if(choice)
    {
        switch(index)
        {
            case 1:
                choice->SetSelection(0);
                break;
            case 0:
                choice->SetSelection(1);
                break;
            case -1:
                choice->SetSelection(2);
                break;
            case -2:
                choice->SetSelection(3);
                break;
            case -3:
                choice->SetSelection(4);
                break;
        };
    }
    ctr->SetValue(pFormat->doubleToString(value * mul));
}

////////////////////////////////////////////////////////////////////////////////////////
// setSlider
////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopMeasure::setSliderX(wxSlider* slider, int value)
{
    slider->SetValue(value);
}
