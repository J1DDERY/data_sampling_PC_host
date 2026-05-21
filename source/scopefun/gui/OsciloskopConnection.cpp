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
#include <scopefun/gui/OsciloskopConnection.h>

OsciloskopConnection::OsciloskopConnection(wxWindow* parent)
    :
    Connection(parent)
{
    if(m_choiceConnection->GetSelection() == 0)
    {
        m_buttonConnect->Disable();
        m_buttonDisconnect->Disable();
        m_textCtrlIP->Disable();
        m_textCtrlPort->Disable();
        m_checkBoxConnected->Disable();
    }
}

void OsciloskopConnection::m_choiceConnectionOnChoice(wxCommandEvent& event)
{
    // TODO: Implement m_choiceConnectionOnChoice
    if(m_choiceConnection->GetSelection() == 0)
    {
        m_buttonConnect->Disable();
        m_buttonDisconnect->Disable();
        m_textCtrlIP->Disable();
        m_textCtrlPort->Disable();
        m_checkBoxConnected->Disable();
    }
    else
    {
        m_buttonConnect->Enable();
        m_buttonDisconnect->Enable();
        m_textCtrlIP->Enable();
        m_textCtrlPort->Enable();
        m_checkBoxConnected->Enable();
    }
}

void OsciloskopConnection::m_textCtrlIPOnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlIPOnTextEnter
}

void OsciloskopConnection::m_textCtrlPortOnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlPortOnTextEnter
}

void OsciloskopConnection::m_checkBoxConnectedOnCheckBox(wxCommandEvent& event)
{
    // TODO: Implement m_checkBoxConnectedOnCheckBox
}

void OsciloskopConnection::m_buttonConnectOnButtonClick(wxCommandEvent& event)
{
}

void OsciloskopConnection::m_buttonDisconnectOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonDisconnectOnButtonClick
    // disconnect first, because mode change has no affect then ...
    // automatic mode change
    pOsciloscope->window.horizontal.Mode = SIGNAL_MODE_PAUSE;
    SDL_AtomicSet(&pOsciloscope->signalMode, SIGNAL_MODE_PAUSE);
    wxApp* app = (wxApp*)wxApp::GetInstance();
    if(app)
    {
        OsciloskopOsciloskop* osc = (OsciloskopOsciloskop*)app->GetTopWindow();
        if(osc)
        {
            osc->setTimeMode(SIGNAL_MODE_PAUSE);
        }
    }
}

void OsciloskopConnection::m_buttonOkOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonOkOnButtonClick
    Hide();
}

void OsciloskopConnection::m_buttonDefaultOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonDefaultOnButtonClick
}

void OsciloskopConnection::m_buttonCancelOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonCancelOnButtonClick
    Hide();
}

void OsciloskopConnection::setConnectedCheckBox(int value)
{
    m_checkBoxConnected->SetValue(value);
}
