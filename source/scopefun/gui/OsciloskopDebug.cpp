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
#include <scopefun/gui/OsciloskopDebug.h>


OsciloskopDebug::OsciloskopDebug(wxWindow* parent)
    :
    Debug(parent)
{
    SetSize(500, 500);

    Connect(wxEVT_CLOSE_WINDOW, wxActivateEventHandler(OsciloskopDebug::OnDestroy));

    if (!isFileWritable())
       m_buttonSave->Disable();

    // Start refresh timer (1 second interval)
    m_refreshTimer.SetOwner(this);
    Connect(m_refreshTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler(OsciloskopDebug::OnTimerUpdate));
    m_refreshTimer.Start(1000);
}

OsciloskopDebug::~OsciloskopDebug()
{
    m_refreshTimer.Stop();
}

void OsciloskopDebug::ThermalOnActivate(wxActivateEvent& event)
{
   /* TODO: erase Lua
   if (m_script && m_script->m_luaScript)
   {
      m_script->Reload();
      m_Lua->SetValue((const char*)m_script->m_luaScript);
   }
   */
}

void OsciloskopDebug::OnDestroy(wxActivateEvent& event)
{
    m_refreshTimer.Stop();
    Hide();
}

void OsciloskopDebug::Clear()
{
    if (m_textCtrlOutput)
        m_textCtrlOutput->Clear();
}

void OsciloskopDebug::AppendText(const char* str)
{
    if (m_textCtrlOutput)
        m_textCtrlOutput->AppendText(str);
}

void OsciloskopDebug::OnTimerUpdate(wxTimerEvent& event)
{
    if (!pOsciloscope || !IsShown())
        return;

    char buf[2048] = {0};
    int frames  = SDL_AtomicGet(&pOsciloscope->debugFrameCount);
    int healthy = SDL_AtomicGet(&pOsciloscope->captureWatchdogHealthy);
    int stuck   = SDL_AtomicGet(&pOsciloscope->debugStuckCount);
    int signal  = SDL_AtomicGet(&pOsciloscope->signalMode);
    int fps     = pTimer ? pTimer->getFps(TIMER_CAPTURE) : 0;

    const char* modeStr = "?";
    switch (signal) {
        case 0: modeStr = "PLAY";    break;
        case 1: modeStr = "PAUSE";   break;
        case 2: modeStr = "CAPTURE"; break;
        case 3: modeStr = "CLEAR";   break;
        default: modeStr = "?";      break;
    }

    macroString(buf, 2048,
        "=== ScopeFun Debug ===\n"
        "Status:      %s\n"
        "SignalMode:  %s\n"
        "Frames:      %d\n"
        "Capture FPS: %d\n"
        "Stuck count: %d\n"
        "Thread:      %s\n"
        "=====================\n",
        healthy ? "OK" : "STUCK!",
        modeStr,
        frames,
        fps,
        stuck,
        healthy ? "running" : "recovered"
    );

    Clear();
    AppendText(buf);
}
void OsciloskopDebug::m_buttonStartOnButtonClick( wxCommandEvent& event )
{
    /* TODO: erase Lua
    if (m_script)
    {
      m_script->Run();
      m_buttonStart->Disable();
      m_buttonStop->Enable();
      m_buttonUpload->Enable();
    }
    event.Skip();
    */
}

void OsciloskopDebug::m_buttonStopOnButtonClick( wxCommandEvent& event )
{
   /* TODO: erase Lua
   if (m_script)
   {
      SDL_CreateThread(LuaStopScript, "LuaStopScript", this);
   }
   event.Skip();
   */
}

void OsciloskopDebug::m_buttonSaveOnButtonClick( wxCommandEvent& event )
{
   // TODO: erase Lua
   //fileSaveString(m_script->m_fileName.asChar(), m_Lua->GetValue());
}

void OsciloskopDebug::m_buttonLuaOnButtonClick( wxCommandEvent& event )
{
    /*TODO: erase Lua
   m_Lua->SetValue((const char*)m_script->m_luaScript);
   if(isFileWritable())
      m_buttonSave->Enable();
   m_buttonLua->Disable();
   m_buttonHelp->Enable();
   event.Skip();
   */
}

void OsciloskopDebug::m_buttonHelpOnButtonClick( wxCommandEvent& event )
{
    /* TODO: erase Lua
   m_Lua->SetValue(pOsciloscope->m_callback.GetHelp());
   m_buttonSave->Disable();
   m_buttonLua->Enable();
   m_buttonHelp->Disable();
   event.Skip();
   */
}


void OsciloskopDebug::m_buttonUploadOnButtonClick(wxCommandEvent& event)
{
    /* TODO: erase Lua
   pOsciloscope->thread.setGeneratorData(&getCtx()->generator);
   pOsciloscope->thread.function(afUploadGenerator);
   pOsciloscope->transferData();
   event.Skip();
   */
}

void OsciloskopDebug::SetText(std::string str)
{
   m_textCtrlOutput->SetValue(str.c_str());
}

void OsciloskopDebug::ThreadStop()
{
    /* TODO: erase Lua
   m_script->Stop();
   m_buttonStart->Enable();
   m_buttonStop->Disable();
   m_buttonUpload->Disable();
   */
}
