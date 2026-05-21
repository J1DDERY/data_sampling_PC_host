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
#include <scopefun/gui/OsciloskopHardwareGenerator.h>

OsciloskopHardwareGenerator::OsciloskopHardwareGenerator(wxWindow* parent)
    :
    HardwareGenerator(parent)
{
}

void OsciloskopHardwareGenerator::HardwareGeneratorOnInitDialog(wxInitDialogEvent& event)
{
    // TODO: Implement HardwareGeneratorOnInitDialog
    FORMAT_BUFFER();

    m_spinBtnFrequency->SetRange(-1000000, 1000000);
    m_spinBtnFrequency1->SetRange(-1000000, 1000000);
    m_spinBtnPhase1->SetRange(-1000000, 1000000);
    m_spinBtnCh0Volt->SetRange(-1000000, 1000000);
    m_spinBtnCh1Volt->SetRange(-1000000, 1000000);
    m_spinBtnCh0Offset->SetRange(-1000000, 1000000);
    m_spinBtnCh1Offset->SetRange(-1000000, 1000000);
    m_spinBtnGen0SqrDuty->SetRange(-1000000, 1000000);
    m_spinBtnGen1SqrDuty->SetRange(-1000000, 1000000);
    m_spinBtnPhase0->SetRange(-1000000, 1000000);
    m_spinBtnPhase1->SetRange(-1000000, 1000000);

    // freq
    pOsciloscope->window.hardwareGenerator.frequency0 = sfGetGeneratorFrequency0(getHw(), pOsciloscope->settings.getHardware()->generatorFs);
    pOsciloscope->window.hardwareGenerator.frequency1 = sfGetGeneratorFrequency1(getHw(), pOsciloscope->settings.getHardware()->generatorFs);

    // type
    pOsciloscope->window.hardwareGenerator.type0 = sfGetGeneratorType0(getHw());
    pOsciloscope->window.hardwareGenerator.type1 = sfGetGeneratorType1(getHw());

    // voltage
    pOsciloscope->window.hardwareGenerator.voltage0 = sfGetGeneratorVoltage0(getHw());
    pOsciloscope->window.hardwareGenerator.voltage1 = sfGetGeneratorVoltage1(getHw());

    // offset
    pOsciloscope->window.hardwareGenerator.offset0 = sfGetGeneratorOffset0(getHw()) - pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 0);
    pOsciloscope->window.hardwareGenerator.offset1 = sfGetGeneratorOffset1(getHw()) - pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 1);

    // phase
    pOsciloscope->window.hardwareGenerator.phase0 = sfGetGeneratorPhase0(getHw());
    pOsciloscope->window.hardwareGenerator.phase1 = sfGetGeneratorPhase1(getHw());

    // square duty
    pOsciloscope->window.hardwareGenerator.squareDuty0 = sfGetGeneratorSquareDuty0(getHw());
    pOsciloscope->window.hardwareGenerator.squareDuty1 = sfGetGeneratorSquareDuty1(getHw());

    //OnOff
    pOsciloscope->window.hardwareGenerator.onOff0 = sfGetGeneratorOn0(getHw());
    pOsciloscope->window.hardwareGenerator.onOff1 = sfGetGeneratorOn1(getHw());
    if(pOsciloscope->window.hardwareGenerator.onOff0)
    {
        m_buttonOn0->Disable();
        m_buttonOff0->Enable();
    }
    else
    {
        m_buttonOn0->Enable();
        m_buttonOff0->Disable();
    }
    if(pOsciloscope->window.hardwareGenerator.onOff1)
    {
        m_buttonOn1->Disable();
        m_buttonOff1->Enable();
    }
    else
    {
        m_buttonOn1->Enable();
        m_buttonOff1->Disable();
    }

    // generator 0
    FORMAT("%f", pOsciloscope->window.hardwareGenerator.frequency0);
    m_comboBoxType->SetSelection(pOsciloscope->window.hardwareGenerator.type0);
    m_textCtrlFrequency->SetValue(wxString::FromAscii(formatBuffer));
    FORMAT("%.3f", double(pOsciloscope->window.hardwareGenerator.voltage0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep);
    m_textCtrlVoltage0->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderVoltage->SetValue(pOsciloscope->window.hardwareGenerator.voltage0);
    FORMAT("%.3f", double(pOsciloscope->window.hardwareGenerator.offset0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep);
    m_textCtrlOffset0->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderOffset->SetValue(pOsciloscope->window.hardwareGenerator.offset0);
    FORMAT("%.3f", (pOsciloscope->window.hardwareGenerator.squareDuty0 / 2048.0) * 100);
    m_textCtrlSquareDuty0->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderSquareDuty->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty0);
    FORMAT("%.3f", (pOsciloscope->window.hardwareGenerator.phase0 / 16384.0) * 180);
    m_textCtrlPhase0->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderPhase0->SetValue(pOsciloscope->window.hardwareGenerator.phase0);

    // generator 1
    FORMAT("%f", pOsciloscope->window.hardwareGenerator.frequency1);
    m_comboBoxType1->SetSelection(pOsciloscope->window.hardwareGenerator.type1);
    m_textCtrlFrequency1->SetValue(wxString::FromAscii(formatBuffer));
    FORMAT("%.3f", double(pOsciloscope->window.hardwareGenerator.voltage1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep);
    m_textCtrlVoltage1->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderVoltage1->SetValue(pOsciloscope->window.hardwareGenerator.voltage1);
    FORMAT("%.3f", double(pOsciloscope->window.hardwareGenerator.offset1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep);
    m_textCtrlOffset1->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderOffset1->SetValue(pOsciloscope->window.hardwareGenerator.offset1);
    FORMAT("%.3f", (pOsciloscope->window.hardwareGenerator.squareDuty1 / 2048.0) * 100);
    m_textCtrlSquareDuty1->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderSquareDuty1->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty1);
    FORMAT("%.3f", (pOsciloscope->window.hardwareGenerator.phase1 / 16384.0) * 180);
    m_textCtrlPhase1->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderPhase1->SetValue(pOsciloscope->window.hardwareGenerator.phase1);

    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        if(pOsciloscope->window.hardwareGenerator.onOff0)
        {
            m_buttonOn0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOn0->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOff0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOff0->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
        }
        else
        {
            m_buttonOn0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOn0->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOff0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOff0->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
        }
        if(pOsciloscope->window.hardwareGenerator.onOff1)
        {
            m_buttonOn1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOn1->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOff1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOff1->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
        }
        else
        {
            m_buttonOn1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOn1->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOff1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOff1->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////
//
// generator 0
//
///////////////////////////////////////////////////////////////////////////////////////

void OsciloskopHardwareGenerator::m_comboBoxTypeOnCombobox(wxCommandEvent& event)
{
    // TODO: Implement m_comboBoxTypeOnCombobox
    int type =  m_comboBoxType->GetSelection();
    pOsciloscope->window.hardwareGenerator.type0 = type;
    sfSetGeneratorType0(getHw(), (GeneratorType)(type));
    pOsciloscope->transferData();
    /*
    if(type == GENERATOR_CUSTOM)
    {
        pOsciloscope->window.hardwareGenerator.uploadAwg(0);
    }*/
}

void OsciloskopHardwareGenerator::m_textCtrlFrequencyOnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlFrequencyOnTextEnter
    double freq = atof(m_textCtrlFrequency->GetValue().ToAscii().data());
    pOsciloscope->window.hardwareGenerator.frequency0 = freq;
    sfSetGeneratorFrequency0(getHw(), freq, pOsciloscope->settings.getHardware()->generatorFs);
    m_textCtrlFrequency->SetValue( pFormat->doubleToString(sfGetGeneratorFrequency0(getHw(), pOsciloscope->settings.getHardware()->generatorFs)));
    pOsciloscope->transferData();
}

void OsciloskopHardwareGenerator::m_textCtrlVoltage0OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlVoltage0OnTextEnter
    pOsciloscope->window.hardwareGenerator.voltage0 = pFormat->stringToFloat(m_textCtrlVoltage0->GetValue().ToAscii().data()) / pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    sfSetGeneratorVoltage0(getHw(), pOsciloscope->window.hardwareGenerator.voltage0);
    pOsciloscope->transferData();
    m_sliderVoltage->SetValue(pOsciloscope->window.hardwareGenerator.voltage0);
}

void OsciloskopHardwareGenerator::m_sliderVoltageOnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderVoltageOnScroll
    float voltage = double(m_sliderVoltage->GetValue()) * pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->window.hardwareGenerator.voltage0 = m_sliderVoltage->GetValue();
    sfSetGeneratorVoltage0(getHw(), m_sliderVoltage->GetValue());
    pOsciloscope->transferData();
    m_textCtrlVoltage0->SetValue(pFormat->floatToString(voltage));
}

void OsciloskopHardwareGenerator::m_textCtrlOffset0OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlOffset0OnTextEnter
    pOsciloscope->window.hardwareGenerator.offset0 = pFormat->stringToFloat(m_textCtrlOffset0->GetValue().ToAscii().data()) / pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->window.hardwareGenerator.offset0 = clamp(pOsciloscope->window.hardwareGenerator.offset0, -2048, 2047);
    sfSetGeneratorOffset0(getHw(), pOsciloscope->window.hardwareGenerator.offset0 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 0));
    pOsciloscope->transferData();
    m_sliderOffset->SetValue(pOsciloscope->window.hardwareGenerator.offset0);
    m_textCtrlOffset0->SetValue(pFormat->floatToString(pOsciloscope->window.hardwareGenerator.offset0 * pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
}

void OsciloskopHardwareGenerator::m_sliderOffsetOnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderOffsetOnScroll
    float voltage = double(m_sliderOffset->GetValue()) * pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->window.hardwareGenerator.offset0 = m_sliderOffset->GetValue();
    sfSetGeneratorOffset0(getHw(), m_sliderOffset->GetValue() + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 0));
    pOsciloscope->transferData();
    m_textCtrlOffset0->SetValue(pFormat->floatToString(voltage));
}

void OsciloskopHardwareGenerator::m_textCtrlSquareDuty0OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlSquareDuty0OnTextEnter
    pOsciloscope->window.hardwareGenerator.squareDuty0 = clamp((int)(pFormat->stringToFloat(m_textCtrlSquareDuty0->GetValue().ToAscii().data()) * 2048 / 100), 0, 2047);
    m_textCtrlSquareDuty0->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.squareDuty0 / 2048.0) * 100));
    sfSetGeneratorSquareDuty0(getHw(), pOsciloscope->window.hardwareGenerator.squareDuty0);
    pOsciloscope->transferData();
    m_sliderSquareDuty->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty0);
}

void OsciloskopHardwareGenerator::m_sliderSquareDutyOnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderSquareDutyOnScroll
    pOsciloscope->window.hardwareGenerator.squareDuty0 = m_sliderSquareDuty->GetValue();
    sfSetGeneratorSquareDuty0(getHw(), m_sliderSquareDuty->GetValue());
    pOsciloscope->transferData();
    m_textCtrlSquareDuty0->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.squareDuty0 / 2048.0) * 100));
}

void OsciloskopHardwareGenerator::m_textCtrlPhase0OnTextEnter(wxCommandEvent& event)
{
    float phaseEntered = pFormat->stringToFloat(m_textCtrlPhase0->GetValue().ToAscii().data());
    pOsciloscope->window.hardwareGenerator.phase0 = clamp((phaseEntered / 180.f) * 16384.f, -16384.f, 16383.f);
    m_textCtrlPhase0->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.phase0 / 16384.0) * 180));
    sfSetGeneratorPhase0(getHw(), pOsciloscope->window.hardwareGenerator.phase0);
    pOsciloscope->transferData();
    m_sliderPhase0->SetValue(pOsciloscope->window.hardwareGenerator.phase0);
}

void OsciloskopHardwareGenerator::m_sliderPhase0OnScroll(wxScrollEvent& event)
{
    pOsciloscope->window.hardwareGenerator.phase0 = m_sliderPhase0->GetValue();
    sfSetGeneratorPhase0(getHw(), pOsciloscope->window.hardwareGenerator.phase0);
    pOsciloscope->transferData();
    m_textCtrlPhase0->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.phase0 / 16384.0) * 180));
}

void OsciloskopHardwareGenerator::m_textCtrlPhase1OnTextEnter(wxCommandEvent& event)
{
    float phaseEntered = pFormat->stringToFloat(m_textCtrlPhase1->GetValue().ToAscii().data());
    pOsciloscope->window.hardwareGenerator.phase1 = clamp((phaseEntered / 180.f) * 16384.f, -16384.f, 16383.f);
    m_textCtrlPhase1->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.phase1 / 16384.0) * 180));
    sfSetGeneratorPhase1(getHw(), pOsciloscope->window.hardwareGenerator.phase1);
    pOsciloscope->transferData();
    m_sliderPhase1->SetValue(pOsciloscope->window.hardwareGenerator.phase1);
}

void OsciloskopHardwareGenerator::m_sliderPhase1OnScroll(wxScrollEvent& event)
{
    pOsciloscope->window.hardwareGenerator.phase1 = m_sliderPhase1->GetValue();
    sfSetGeneratorPhase1(getHw(), pOsciloscope->window.hardwareGenerator.phase1);
    pOsciloscope->transferData();
    m_textCtrlPhase1->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.phase1 / 16384.0) * 180));
}

void OsciloskopHardwareGenerator::m_filePicker1OnFileChanged(wxFileDirPickerEvent& event)
{
    // TODO: Implement m_filePicker1OnFileChanged
    pOsciloscope->window.hardwareGenerator.loadCustomAwg(0, m_filePicker1->GetFileName().GetFullPath().ToAscii().data());
}

void OsciloskopHardwareGenerator::m_buttonCustomFileOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.hardwareGenerator.loadCustomAwg(0, m_filePicker1->GetFileName().GetFullPath().ToAscii().data());
    pOsciloscope->window.hardwareGenerator.uploadAwg(0);
}

void OsciloskopHardwareGenerator::m_buttonOn0OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.hardwareGenerator.onOff0 = 1;
    sfSetGeneratorVoltage0(getHw(), pOsciloscope->window.hardwareGenerator.voltage0);
    sfSetGeneratorFrequency0(getHw(), pOsciloscope->window.hardwareGenerator.frequency0, pOsciloscope->settings.getHardware()->generatorFs);
    sfSetGeneratorSquareDuty0(getHw(), pOsciloscope->window.hardwareGenerator.squareDuty0);
    sfSetGeneratorOffset0(getHw(), pOsciloscope->window.hardwareGenerator.offset0 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 0));
    sfSetGeneratorPhase0(getHw(), pOsciloscope->window.hardwareGenerator.phase0);
    sfSetGeneratorOn0(getHw(), 1);
    pOsciloscope->transferData();
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOn0->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff0->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
    }
    m_buttonOn0->Disable();
    m_buttonOff0->Enable();
}

void OsciloskopHardwareGenerator::m_buttonOff0OnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_radioBtnOffOnRadioButton
    pOsciloscope->window.hardwareGenerator.onOff0 = 0;
    sfSetGeneratorOn0(getHw(), 0);
    pOsciloscope->transferData();
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOn0->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff0->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
    }
    m_buttonOn0->Enable();
    m_buttonOff0->Disable();
}

///////////////////////////////////////////////////////////////
// awg0 - spin
///////////////////////////////////////////////////////////////

void OsciloskopHardwareGenerator::m_spinBtnCh0VoltOnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.voltage0++;
    pOsciloscope->window.hardwareGenerator.voltage0 = min(pOsciloscope->window.hardwareGenerator.voltage0, 2047);
    m_textCtrlVoltage0->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.voltage0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderVoltage->SetValue(pOsciloscope->window.hardwareGenerator.voltage0);
    sfSetGeneratorVoltage0(getHw(), pOsciloscope->window.hardwareGenerator.voltage0);
    pOsciloscope->transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh0VoltOnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.voltage0--;
    pOsciloscope->window.hardwareGenerator.voltage0 = max(pOsciloscope->window.hardwareGenerator.voltage0, 0);
    m_textCtrlVoltage0->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.voltage0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderVoltage->SetValue(pOsciloscope->window.hardwareGenerator.voltage0);
    sfSetGeneratorVoltage0(getHw(), pOsciloscope->window.hardwareGenerator.voltage0);
    pOsciloscope->transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh0OffsetOnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.offset0++;
    pOsciloscope->window.hardwareGenerator.offset0 = min(pOsciloscope->window.hardwareGenerator.offset0, 2047);
    m_textCtrlOffset0->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.offset0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderOffset->SetValue(pOsciloscope->window.hardwareGenerator.offset0);
    sfSetGeneratorOffset0(getHw(), pOsciloscope->window.hardwareGenerator.offset0 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 0));
    pOsciloscope->transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh0OffsetOnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.offset0--;
    pOsciloscope->window.hardwareGenerator.offset0 = max(pOsciloscope->window.hardwareGenerator.offset0, -2048);
    m_textCtrlOffset0->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.offset0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderOffset->SetValue(pOsciloscope->window.hardwareGenerator.offset0);
    sfSetGeneratorOffset0(getHw(), pOsciloscope->window.hardwareGenerator.offset0 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 0));
    pOsciloscope->transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnGen0SqrDutyOnSpinUp(wxSpinEvent& event)
{
    if (pOsciloscope->window.hardwareGenerator.squareDuty0 < 2047)
        pOsciloscope->window.hardwareGenerator.squareDuty0++;
    sfSetGeneratorSquareDuty0(getHw(), pOsciloscope->window.hardwareGenerator.squareDuty0);
    pOsciloscope->transferData();
    m_sliderSquareDuty->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty0);
    m_textCtrlSquareDuty0->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.squareDuty0 / 2048.0) * 100));
}

void OsciloskopHardwareGenerator::m_spinBtnGen0SqrDutyOnSpinDown(wxSpinEvent& event)
{
    if (pOsciloscope->window.hardwareGenerator.squareDuty0 > 0)
        pOsciloscope->window.hardwareGenerator.squareDuty0--;
    sfSetGeneratorSquareDuty0(getHw(), pOsciloscope->window.hardwareGenerator.squareDuty0);
    pOsciloscope->transferData();
    m_sliderSquareDuty->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty0);
    m_textCtrlSquareDuty0->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.squareDuty0 / 2048.0) * 100));
}

void OsciloskopHardwareGenerator::m_spinBtnPhase0OnSpinDown(wxSpinEvent& event)
{
    if (pOsciloscope->window.hardwareGenerator.phase0 > -16384)
        pOsciloscope->window.hardwareGenerator.phase0--;
    sfSetGeneratorPhase0(getHw(), pOsciloscope->window.hardwareGenerator.phase0);
    pOsciloscope->transferData();
    m_sliderPhase0->SetValue(pOsciloscope->window.hardwareGenerator.phase0);
    m_textCtrlPhase0->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.phase0 / 16384.0) * 180));
}

void OsciloskopHardwareGenerator::m_spinBtnPhase0OnSpinUp(wxSpinEvent& event)
{
    if (pOsciloscope->window.hardwareGenerator.phase0 < 16383)
        pOsciloscope->window.hardwareGenerator.phase0++;
    sfSetGeneratorPhase0(getHw(), pOsciloscope->window.hardwareGenerator.phase0);
    pOsciloscope->transferData();
    m_sliderPhase0->SetValue(pOsciloscope->window.hardwareGenerator.phase0);
    m_textCtrlPhase0->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.phase0 / 16384.0) * 180));
}

void OsciloskopHardwareGenerator::m_spinBtnPhase1OnSpinDown(wxSpinEvent& event)
{
    if (pOsciloscope->window.hardwareGenerator.phase1 > -16384)
        pOsciloscope->window.hardwareGenerator.phase1--;
    sfSetGeneratorPhase1(getHw(), pOsciloscope->window.hardwareGenerator.phase1);
    pOsciloscope->transferData();
    m_sliderPhase1->SetValue(pOsciloscope->window.hardwareGenerator.phase1);
    m_textCtrlPhase1->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.phase1 / 16384.0) * 180));
}

void OsciloskopHardwareGenerator::m_spinBtnPhase1OnSpinUp(wxSpinEvent& event)
{
    if (pOsciloscope->window.hardwareGenerator.phase1 < 16383)
        pOsciloscope->window.hardwareGenerator.phase1++;
    sfSetGeneratorPhase1(getHw(), pOsciloscope->window.hardwareGenerator.phase1);
    pOsciloscope->transferData();
    m_sliderPhase1->SetValue(pOsciloscope->window.hardwareGenerator.phase1);
    m_textCtrlPhase1->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.phase1 / 16384.0) * 180));
}

void OsciloskopHardwareGenerator::m_spinBtnFrequencyOnSpinUp(wxSpinEvent& event)
{
    //determine the minimum frequency increase
    uint GeneratorDelta0 = sfGetGeneratorDelta0(getHw());
    if (GeneratorDelta0 < 4294967295)
        GeneratorDelta0++;
    sfSetGeneratorDelta0(getHw(),GeneratorDelta0);
    pOsciloscope->transferData();
    double freq0 = sfGetGeneratorFrequency0(getHw(), pOsciloscope->settings.getHardware()->generatorFs);
    pOsciloscope->window.hardwareGenerator.frequency0 = freq0;
    m_textCtrlFrequency->SetValue(pFormat->doubleToString(freq0));
}

void OsciloskopHardwareGenerator::m_spinBtnFrequencyOnSpinDown(wxSpinEvent& event)
{
    //determine the minimum frequency increase
    uint GeneratorDelta0 = sfGetGeneratorDelta0(getHw());
    if (GeneratorDelta0 > 0)
        GeneratorDelta0--;
    sfSetGeneratorDelta0(getHw(),GeneratorDelta0);
    pOsciloscope->transferData();
    double freq0 = sfGetGeneratorFrequency0(getHw(), pOsciloscope->settings.getHardware()->generatorFs);
    pOsciloscope->window.hardwareGenerator.frequency0 = freq0;
    m_textCtrlFrequency->SetValue(pFormat->doubleToString(freq0));
}

///////////////////////////////////////////////////////////////////////////////////////
//
// generator 1
//
///////////////////////////////////////////////////////////////////////////////////////
void OsciloskopHardwareGenerator::m_comboBoxType1OnChoice(wxCommandEvent& event)
{
    // TODO: Implement m_comboBoxType1OnChoice
    int type =  m_comboBoxType1->GetSelection();
    pOsciloscope->window.hardwareGenerator.type1 = type;
    sfSetGeneratorType1(getHw(), (GeneratorType)type);
    pOsciloscope->transferData();
    /*
    if(type == GENERATOR_CUSTOM)
    {
        pOsciloscope->window.hardwareGenerator.uploadAwg(1);
    }*/
}

void OsciloskopHardwareGenerator::m_textCtrlFrequency1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlFrequency1OnTextEnter
    double freq = atof(m_textCtrlFrequency1->GetValue().ToAscii().data());
    pOsciloscope->window.hardwareGenerator.frequency1 = freq;
    sfSetGeneratorFrequency1(getHw(), freq, pOsciloscope->settings.getHardware()->generatorFs);
    m_textCtrlFrequency1->SetValue(pFormat->doubleToString(sfGetGeneratorFrequency1(getHw(), pOsciloscope->settings.getHardware()->generatorFs)));
    pOsciloscope->transferData();
}

void OsciloskopHardwareGenerator::m_textCtrlVoltage1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlVoltage1OnTextEnter
    pOsciloscope->window.hardwareGenerator.voltage1 = pFormat->stringToFloat(m_textCtrlVoltage1->GetValue().ToAscii().data()) / pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    sfSetGeneratorVoltage1(getHw(), pOsciloscope->window.hardwareGenerator.voltage1);
    pOsciloscope->transferData();
    m_sliderVoltage1->SetValue(pOsciloscope->window.hardwareGenerator.voltage1);
}

void OsciloskopHardwareGenerator::m_sliderVoltage1OnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderVoltage1OnScroll
    float voltage = double(m_sliderVoltage1->GetValue()) * pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->window.hardwareGenerator.voltage1 = m_sliderVoltage1->GetValue();
    sfSetGeneratorVoltage1(getHw(), m_sliderVoltage1->GetValue());
    pOsciloscope->transferData();
    m_textCtrlVoltage1->SetValue(pFormat->floatToString(voltage));
}

void OsciloskopHardwareGenerator::m_textCtrlOffset1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlOffset1OnTextEnter
    pOsciloscope->window.hardwareGenerator.offset1 = pFormat->stringToFloat(m_textCtrlOffset1->GetValue().ToAscii().data()) / pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->window.hardwareGenerator.offset1 = clamp(pOsciloscope->window.hardwareGenerator.offset1, -2048, 2047);
    sfSetGeneratorOffset1(getHw(), pOsciloscope->window.hardwareGenerator.offset1 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 1));
    pOsciloscope->transferData();
    m_sliderOffset1->SetValue(pOsciloscope->window.hardwareGenerator.offset1);
    m_textCtrlOffset1->SetValue(pFormat->floatToString(pOsciloscope->window.hardwareGenerator.offset1 * pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
}

void OsciloskopHardwareGenerator::m_sliderOffset1OnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderOffset1OnScroll
    float voltage = double(m_sliderOffset1->GetValue()) * pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->window.hardwareGenerator.offset1 = m_sliderOffset1->GetValue();
    sfSetGeneratorOffset1(getHw(), m_sliderOffset1->GetValue() + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 1));
    pOsciloscope->transferData();
    m_textCtrlOffset1->SetValue(pFormat->floatToString(voltage));
}

void OsciloskopHardwareGenerator::m_textCtrlSquareDuty1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlSquareDuty1OnTextEnter
    pOsciloscope->window.hardwareGenerator.squareDuty1 = clamp((int)(pFormat->stringToFloat(m_textCtrlSquareDuty1->GetValue().ToAscii().data()) * 2048 / 100), 0, 2047);
    m_textCtrlSquareDuty1->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.squareDuty1 / 2048.0) * 100));
    sfSetGeneratorSquareDuty1(getHw(), pOsciloscope->window.hardwareGenerator.squareDuty1);
    pOsciloscope->transferData();
    m_sliderSquareDuty1->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty1);
}

void OsciloskopHardwareGenerator::m_sliderSquareDuty1OnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderSquareDuty1OnScroll
    pOsciloscope->window.hardwareGenerator.squareDuty1 = m_sliderSquareDuty1->GetValue();
    sfSetGeneratorSquareDuty1(getHw(), m_sliderSquareDuty1->GetValue());
    pOsciloscope->transferData();
    m_textCtrlSquareDuty1->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.squareDuty1 / 2048.0) * 100));
}

void OsciloskopHardwareGenerator::m_filePicker11OnFileChanged(wxFileDirPickerEvent& event)
{
    // TODO: Implement m_filePicker11OnFileChanged
    pOsciloscope->window.hardwareGenerator.loadCustomAwg(1, m_filePicker11->GetFileName().GetFullPath().ToAscii().data());
}

void OsciloskopHardwareGenerator::m_buttonCustomFile1OnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonCustomFile1OnButtonClick
    pOsciloscope->window.hardwareGenerator.loadCustomAwg(1, m_filePicker11->GetFileName().GetFullPath().ToAscii().data());
    pOsciloscope->window.hardwareGenerator.uploadAwg(1);
}

void OsciloskopHardwareGenerator::m_buttonOn1OnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_radioBtnOn1OnRadioButton
    pOsciloscope->window.hardwareGenerator.onOff1 = 1;
    sfSetGeneratorVoltage1(getHw(), pOsciloscope->window.hardwareGenerator.voltage1);
    sfSetGeneratorFrequency1(getHw(), pOsciloscope->window.hardwareGenerator.frequency1, pOsciloscope->settings.getHardware()->generatorFs);
    sfSetGeneratorSquareDuty1(getHw(), pOsciloscope->window.hardwareGenerator.squareDuty1);
    sfSetGeneratorOffset1(getHw(), pOsciloscope->window.hardwareGenerator.offset1 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 1));
    sfSetGeneratorPhase1(getHw(), pOsciloscope->window.hardwareGenerator.phase1);
    sfSetGeneratorOn1(getHw(), 1);
    pOsciloscope->transferData();
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOn1->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff1->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
    }
    m_buttonOn1->Disable();
    m_buttonOff1->Enable();
}

void OsciloskopHardwareGenerator::m_buttonOff1OnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_radioBtnOff1OnRadioButton
    pOsciloscope->window.hardwareGenerator.onOff1 = 0;
    sfSetGeneratorOn1(getHw(), 0);
    pOsciloscope->transferData();
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOn1->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff1->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
    }
    m_buttonOn1->Enable();
    m_buttonOff1->Disable();
}

///////////////////////////////////////////////////////////////
// awg1 - spin
///////////////////////////////////////////////////////////////

void OsciloskopHardwareGenerator::m_spinBtnCh1VoltOnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.voltage1++;
    pOsciloscope->window.hardwareGenerator.voltage1 = min(pOsciloscope->window.hardwareGenerator.voltage1, 2047);
    m_textCtrlVoltage1->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.voltage1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderVoltage1->SetValue(pOsciloscope->window.hardwareGenerator.voltage1);
    sfSetGeneratorVoltage1(getHw(), pOsciloscope->window.hardwareGenerator.voltage1);
    pOsciloscope->transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh1VoltOnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.voltage1--;
    pOsciloscope->window.hardwareGenerator.voltage1 = max(pOsciloscope->window.hardwareGenerator.voltage1, 0);
    m_textCtrlVoltage1->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.voltage1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderVoltage1->SetValue(pOsciloscope->window.hardwareGenerator.voltage1);
    sfSetGeneratorVoltage1(getHw(), pOsciloscope->window.hardwareGenerator.voltage1);
    pOsciloscope->transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh1OffsetOnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.offset1++;
    pOsciloscope->window.hardwareGenerator.offset1 = min(pOsciloscope->window.hardwareGenerator.offset1, 2047);
    m_textCtrlOffset1->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.offset1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderOffset1->SetValue(pOsciloscope->window.hardwareGenerator.offset1);
    sfSetGeneratorOffset1(getHw(), pOsciloscope->window.hardwareGenerator.offset1 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 1));
    pOsciloscope->transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh1OffsetOnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.offset1--;
    pOsciloscope->window.hardwareGenerator.offset1 = max(pOsciloscope->window.hardwareGenerator.offset1, -2048);
    m_textCtrlOffset1->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.offset1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderOffset1->SetValue(pOsciloscope->window.hardwareGenerator.offset1);
    sfSetGeneratorOffset1(getHw(), pOsciloscope->window.hardwareGenerator.offset1 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 1));
    pOsciloscope->transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnGen1SqrDutyOnSpinUp(wxSpinEvent& event)
{
    if (pOsciloscope->window.hardwareGenerator.squareDuty1 < 2047)
        pOsciloscope->window.hardwareGenerator.squareDuty1++;
    pOsciloscope->window.hardwareGenerator.squareDuty1++;
    sfSetGeneratorSquareDuty1(getHw(), pOsciloscope->window.hardwareGenerator.squareDuty1);
    pOsciloscope->transferData();
    m_sliderSquareDuty1->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty1);
    m_textCtrlSquareDuty1->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.squareDuty1 / 2048.0) * 100));
}

void OsciloskopHardwareGenerator::m_spinBtnGen1SqrDutyOnSpinDown(wxSpinEvent& event)
{
    if (pOsciloscope->window.hardwareGenerator.squareDuty1 > 0)
        pOsciloscope->window.hardwareGenerator.squareDuty1--;
    sfSetGeneratorSquareDuty1(getHw(), pOsciloscope->window.hardwareGenerator.squareDuty1);
    pOsciloscope->transferData();
    m_sliderSquareDuty1->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty1);
    m_textCtrlSquareDuty1->SetValue(pFormat->floatToString((pOsciloscope->window.hardwareGenerator.squareDuty1 / 2048.0) * 100));
}

void OsciloskopHardwareGenerator::m_spinBtnFrequency1OnSpinUp(wxSpinEvent& event)
{
    //determine the minimum frequency increase
    uint GeneratorDelta1 = sfGetGeneratorDelta1(getHw());
    if (GeneratorDelta1 < 4294967295)
        GeneratorDelta1++;
    sfSetGeneratorDelta1(getHw(),GeneratorDelta1);
    pOsciloscope->transferData();
    double freq1 = sfGetGeneratorFrequency1(getHw(), pOsciloscope->settings.getHardware()->generatorFs);
    pOsciloscope->window.hardwareGenerator.frequency1 = freq1;
    m_textCtrlFrequency1->SetValue(pFormat->doubleToString(freq1));
}

void OsciloskopHardwareGenerator::m_spinBtnFrequency1OnSpinDown(wxSpinEvent& event)
{
    //determine the minimum frequency increase
    uint GeneratorDelta1 = sfGetGeneratorDelta1(getHw());
    if (GeneratorDelta1 > 0)
        GeneratorDelta1--;
    sfSetGeneratorDelta1(getHw(),GeneratorDelta1);
    pOsciloscope->transferData();
    double freq1 = sfGetGeneratorFrequency1(getHw(), pOsciloscope->settings.getHardware()->generatorFs);
    pOsciloscope->window.hardwareGenerator.frequency1 = freq1;
    m_textCtrlFrequency1->SetValue(pFormat->doubleToString(freq1));
}
////////////////////////////////////////////////////////////////////////////////////////////
//
// Ok / Default  / Cancel
//
////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopHardwareGenerator::m_buttonOkOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonOkOnButtonClick
    Hide();
}

void OsciloskopHardwareGenerator::m_buttonDefaultOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonDefaultOnButtonClick
    pOsciloscope->window.hardwareGenerator.Default();
    wxInitDialogEvent e = { 0 };
    HardwareGeneratorOnInitDialog(e);
    wxCommandEvent ce = { 0 };
    m_buttonOn0OnButtonClick(ce);
    m_buttonOn1OnButtonClick(ce);
}

void OsciloskopHardwareGenerator::m_buttonCancelOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonCancelOnButtonClick
    Hide();
}
