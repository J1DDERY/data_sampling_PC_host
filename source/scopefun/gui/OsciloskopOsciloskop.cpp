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
#include <scopefun/gui/OsciloskopOsciloskop.h>

OsciloskopOsciloskop::OsciloskopOsciloskop(wxWindow* parent)
:
Osciloskop(parent)
{

}

wxMenu* OsciloskopOsciloskop::getMenuSafe(int preferredIndex)
{
    wxMenuBar* mb = this->GetMenuBar();
    if(!mb) return nullptr;
    int cnt = mb->GetMenuCount();
    if(preferredIndex < cnt) return mb->GetMenu(preferredIndex);
    if(cnt > 0) return mb->GetMenu(cnt - 1);
    return nullptr;
}

void OsciloskopOsciloskop::onActivate(wxActivateEvent& event)
{
    if(once)
    {
        once = 0;

        #ifndef _DEBUG
        // Safe remove: menu may not exist after GUI simplification
        {
            wxMenuBar* mb = this->GetMenuBar();
            if(mb)
            {
                int cnt = mb->GetMenuCount();
                for(int i = 0; i < cnt; ++i)
                {
                    wxString lbl = mb->GetMenuLabel(i);
                    wxMenu* m = mb->GetMenu(i);
                    if(m)
                    {
                        wxMenuItemList::const_iterator it;
                        for(it = m->GetMenuItems().begin(); it != m->GetMenuItems().end(); ++it)
                        {
                            wxMenuItem* item = *it;
                            if(item && item->GetItemLabel() == wxString("Software Simulator"))
                            {
                                m->Remove(item->GetId());
                                goto removed_software_simulator;
                            }
                        }
                    }
                }
            }
        }
        removed_software_simulator:
        #endif

        m_spinBtnXPos->SetRange(-1000000, 1000000);
        m_spinBtnTrigPre->SetRange(-1000000, 1000000);
        m_spinBtnTrigHis->SetRange(-1000000, 1000000);
        m_spinBtnTrigLevel->SetRange(-1000000, 1000000);
        m_spinBtnTrigHoldoff->SetRange(-1000000, 1000000);
        m_spinBtnFrameHistory->SetRange(-1000000, 1000000);
        m_spinBtnCh0YPos->SetRange(-1000000, 1000000);
        m_spinBtnCh1YPos->SetRange(-1000000, 1000000);

        m_sliderTimeFrame->SetValue(0);

        wxSpinEvent evtspin;
        m_spinBtnTrigPreOnSpinDown(evtspin);

        if (!isFileWritable())
        {
           wxMessageDialog msgBox(this, "The current user does not have write permissions to software install path. Some features will be disabled.", wxT("Info"), wxOK | wxCENTRE);
           int ret = msgBox.ShowModal();
        }

        wxMenu* menu = new wxMenu();
        // TODO: erase LUA Script menu
        /*
         GetMenuBar()->Insert(6, menu, "Script");
#if defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
        String scriptPath = GetOscDataFolder().GetFullPath().data().AsChar();
#else
        String scriptPath = GetOscDataFolder().GetCwd().data().AsChar();
#endif
        String helpPath  = scriptPath;
               helpPath += "/script/scopefunapi.help";
        scriptPath += "/script/*.lua";
        wxString f = wxFindFirstFile(scriptPath.asChar());
        for(int i = 0; i < SCOPEFUN_MAX_SCRIPT; i++)
        {
            if(f.empty())
            { break; }
            pOsciloscope->m_callback.Add(f.data().AsChar(), LuaMsg );
            String fileName = f.data().AsChar();
            int pos = fileName.pos("/Script/");
            fileName.remove(0, pos + 1);
            wxMenu* menu = getMenuSafe(6);
            wxMenuItem* menuItem = menu->AppendCheckItem(wxID_ANY, fileName.asChar(), wxEmptyString);
            Connect(menuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OsciloskopOsciloskop::MenuScriptSelection), (wxObject*)pOsciloscope->m_callback.Get(i), this);
            f = wxFindNextFile();
        }
        pOsciloscope->m_callback.SetHelp(helpPath);
        */
        pTimer->init(TIMER_MAIN_THREAD);
        ////////////////////////////////////////////////////////////////////////////////////////
        // version
        ////////////////////////////////////////////////////////////////////////////////////////
        wxCommandEvent version2;
        m_comboBoxTimeCapture->Clear();
        for (int i = 0; i < t2cLast; i++)
        {
           m_comboBoxTimeCapture->Append(captureTimeToStr(i));
        }
        m_comboBoxTimeCapture->SetSelection(sfGetXRange(getHw()));
        ////////////////////////////////////////////////////////////////////////////////////////
        // colors
        ////////////////////////////////////////////////////////////////////////////////////////
        setColors(this);
        setColors(pStorage);
        setColors(pConnection);
        setColors(pMeasure);
        setColors(pDebug);
        setColors(pInfo);
        setColors(pDisplay);
        setColors(pThermal);
        setColors(pThermal);
        pTimer->init(TIMER_USERINTERFACE);
        if(pOsciloscope->settings.getSettings()->windowDebug == 0)
        {
            m_menu7->Remove(m_menuItemDebug);
        }
        ////////////////////////////////////////////////////////////////////////////////////////
        // license
        ////////////////////////////////////////////////////////////////////////////////////////
        int agree = checkAgree();
        if(!agree)
        {
            wxCommandEvent nullEvent;
            m_menuItemInfoOnMenuSelection(nullEvent);
        }
        ////////////////////////////////////////////////////////////////////////////////////////
        // icon
        ////////////////////////////////////////////////////////////////////////////////////////
        wxIconBundle icoBundle;
        wxIcon icon16(xpm16_xpm);
        wxIcon icon32(xpm32_xpm);
        wxIcon icon64(xpm64_xpm);
        wxIcon icon96(xpm96_xpm);
        wxIcon icon128(xpm128_xpm);
        wxIcon icon256(xpm256_xpm);
        wxIcon icon512(xpm512_xpm);
        icoBundle.AddIcon(icon16);
        icoBundle.AddIcon(icon32);
        icoBundle.AddIcon(icon64);
        icoBundle.AddIcon(icon96);
        icoBundle.AddIcon(icon128);
        icoBundle.AddIcon(icon512);
        SetIcons(icoBundle);
        m_comboBoxTimeCaptureOnCombobox(version2);
        wxCommandEvent evt;
        m_textCtrlTimeFrameSize->SetValue(pFormat->integerToString(10000));

        uint sampleSize = atoi(m_textCtrlTimeFrameSize->GetValue().ToAscii().data());
        sfSetSampleSize(getHw(), sampleSize);
        pOsciloscope->window.horizontal.FrameSize = sfGetSampleSize(getHw());
        SDL_AtomicSet(&pOsciloscope->m_captureBuffer.m_frameSize, sampleSize*4 + SCOPEFUN_FRAME_HEADER);

        m_textCtrlTimeFrameSize->SetValue(wxString::FromAscii(pFormat->integerToString(sfGetSampleSize(getHw()))));

        bool writeOk = isFileWritable();
        if(!writeOk)
        // disable items that require disk write permissions
        {
            wxMenuItem* item3 = m_menu5->FindItemByPosition(3); // Autocalibrate
            if(item3)
            { item3->Enable(false); }
            wxMenuItem* item5 = m_menu5->FindItemByPosition(5); // Write Calibration
            if(item5)
            { item5->Enable(false); }
            wxMenuItem* item6 = m_menu5->FindItemByPosition(6); // Read Calibration
            if(item6)
            { item6->Enable(false); }
        }
        // TODO: WindowDebug (Hide Factory Items)
        if(pOsciloscope->settings.getSettings()->windowDebug != 2)
        {
            m_menu5->Remove(m_menuItemWriteCertificate);
            m_menu5->Remove(m_separator4111);
            m_menu5->Remove(m_menuItemLockCertificate);
            m_menu5->Remove(m_separator411);
            m_menu5->Remove(m_menuItemResetEEPROM);
            m_menu5->Remove(m_menuItemFactoryWriteEEPROM);
            m_menu5->Remove(m_menuItemEraseECalibration);
            m_menu5->Remove(m_menuItemFactoryLoadFX3Ram);
        }

        SetupUI(pOsciloscope->window);
    }
}

void OsciloskopOsciloskop::onActivateApp(wxActivateEvent& event)
{
   // TODO: Implement onActivateApp
}

void OsciloskopOsciloskop::onClose(wxCloseEvent& event)
{
    // threads exit
    pOsciloscope->exitThreads();

    // exit
    Destroy();
}

void OsciloskopOsciloskop::OnIdle(wxIdleEvent& event)
{
    ////////////////////////////////////////////////////////////////////////////////
    // timer
    ////////////////////////////////////////////////////////////////////////////////
    pTimer->deltaTime(TIMER_USERINTERFACE);
    timer += pTimer->getDelta(TIMER_USERINTERFACE);

    ////////////////////////////////////////////////////////////////////////////////
    // user interface
    ////////////////////////////////////////////////////////////////////////////////
    if(timer > 0.1)
    {
        double idleTimer = timer;
        timer = 0;

        ////////////////////////////////////////////////////////////////////////////////
        // undo / redo
        ////////////////////////////////////////////////////////////////////////////////
        m_buttonRedo->Enable(pOsciloscope->isRedoActive());
        m_buttonUndo->Enable(pOsciloscope->isUndoActive());

        ////////////////////////////////////////////////////////////////////////////////
        // reset gui
        ////////////////////////////////////////////////////////////////////////////////
        if(SDL_AtomicGet(&pOsciloscope->calibrate.resetUI) > 0)
        {
            // frame
            pOsciloscope->window.horizontal.Frame = 0;
            SDL_AtomicSet(&pOsciloscope->m_captureBuffer.m_frameIndex, 0);
            // mode
            pOsciloscope->window.horizontal.Mode = SIGNAL_MODE_PAUSE;
            SDL_AtomicSet(&pOsciloscope->signalMode, SIGNAL_MODE_PAUSE);
            // clear render target
            SDL_AtomicSet(&pOsciloscope->clearRenderTarget, 1);
            // user interface
            setupUI(pOsciloscope->window);
            // transfer
            pOsciloscope->transferData();
            // exit
            SDL_AtomicSet(&pOsciloscope->calibrate.resetUI, 0);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // message box
        ////////////////////////////////////////////////////////////////////////////////
        if(pOsciloscope->calibrate.active)
        {
            switch(pOsciloscope->calibrate.messageBox)
            {
                case acmbStart:
                case acmbChannel00:
                case acmbChannel01:
                case acmbChannel10:
                case acmbChannel11:
                case acmbEnd:
                    {
                        wxString message;
                        if(pOsciloscope->calibrate.messageBox == acmbChannel00)
                        {
                            message = wxT("Cable connect generator 1 (AWG1) with channel 1 (CH1). Then click OK to proceed.");
                        }
                        if(pOsciloscope->calibrate.messageBox == acmbChannel01)
                        {
                            message = wxT("Cable connect generator 1 (AWG1) with channel 2 (CH2). Then click OK to proceed.");
                        }
                        if(pOsciloscope->calibrate.messageBox == acmbChannel10)
                        {
                            message = wxT("Cable connect generator 2 (AWG2) with channel 1 (CH1). Then click OK to proceed.");
                        }
                        if(pOsciloscope->calibrate.messageBox == acmbChannel11)
                        {
                            message = wxT("Cable connect generator 2 (AWG2) with channel 2 (CH2). Then click OK to proceed.");
                        }
                        if(pOsciloscope->calibrate.messageBox == acmbStart)
                        {
                            message = wxT("This action will start automatic calibration. Disconnect all channels and generators. Click OK to proceed or Cancel to abort.");
                        }
                        if(pOsciloscope->calibrate.messageBox == acmbEnd)
                        {
                            message = wxT("Automatic calibration was successful. Click OK to save calibration results to file.");
                        }
                        pOsciloscope->calibrate.messageBox = acmbNone;
                        wxMessageDialog msgBox(this, message, wxT("Automatic Calibration"), wxCANCEL | wxOK | wxCENTRE);
                        int ret = msgBox.ShowModal();
                        if(ret == wxID_OK)
                        {
                            pOsciloscope->calibrate.messageBox = acmbOk;
                        }
                        if(ret == wxID_CANCEL)
                        {
                            pOsciloscope->calibrate.messageBox = acmbCancel;
                        }
                    }
                    break;
            };
        }

        ////////////////////////////////////////////////////////////////////////////////
        // usb
        ////////////////////////////////////////////////////////////////////////////////
        static int open = 0;

        int isFpga   = pOsciloscope->thread.isFpga();
        m_checkBox261->SetValue(isFpga);
        int isOpen   = pOsciloscope->thread.isOpen();
        m_checkBox26->SetValue(isOpen);
        if (isFpga) {
            open = 1;
            //if (pOsciloscope->window.horizontal.Mode != SIGNAL_MODE_CAPTURE)
                m_buttonCapture->Enable();
            //OnAutoDetectUsb();
        }
        else
        {
            open = 0;
            m_buttonCapture->Disable();
        }

        ////////////////////////////////////////////////////////////////////////////////
        // trigger LED
        ////////////////////////////////////////////////////////////////////////////////
        if(pOsciloscope->display.attr.getCount())
        {
            if(pOsciloscope->display.attr.getCount())
            {
                byte attr = pOsciloscope->display.attr[0];
                if(attr & FRAME_ATTRIBUTE_TRIGGERED_LED)
                {
                    m_radioBtnTriggered->SetValue(true);
                }
                else
                {
                    m_radioBtnTriggered->SetValue(false);
                }
            }
        }



        ////////////////////////////////////////////////////////////////////////////////
        // frame index
        ////////////////////////////////////////////////////////////////////////////////
        int mode = SDL_AtomicGet(&pOsciloscope->signalMode);
        if( mode == SignalMode::SIGNAL_MODE_CAPTURE  ||
            mode == SignalMode::SIGNAL_MODE_CAPTURE ||
            mode == SignalMode::SIGNAL_MODE_PLAY )
        {
            int frameIndex = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameIndex);
            int frameStart = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameStart);
            int frameEnd   = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameEnd);
            int frameCount = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameCount);
            int sliderPos  = clamp<int>(frameEnd,              0, frameCount-1);
            int sliderMax  = clamp<int>(frameEnd - frameStart, 0, frameCount-1);
            m_sliderTimeFrame->SetMax(sliderMax);
            m_sliderTimeFrame->SetValue(sliderPos);
            m_textCtrlTimeFrame->SetValue(wxString::FromAscii(pFormat->integerToString(sliderPos)));
            //TODO: Digital loop counter
            //m_textCtrlDigitalLoopCnt->SetValue(pFormat->floatToString((float)window.digitalSetup.loopCnt));
        }

        ////////////////////////////////////////////////////////////////////////////////
        // measure
        ////////////////////////////////////////////////////////////////////////////////
        pTimer->deltaTime(TIMER_MEASURE);
        if(pMeasure && pMeasure->IsShown() && pOsciloscope->scrollThread == false)
        {
            pMeasure->PopulateWithData();
        }
        if(pMeasure && pMeasure->IsShown())
        {
            if(pOsciloscope->window.measure.data.pickX0.updateUI)
            {
                pOsciloscope->window.measure.data.pickX0.updateUI = false;
                double xTime = pOsciloscope->window.measure.data.pickX0.position.getXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom);
                pMeasure->setTextCtrlX(pMeasure->m_textCtrlX0, pMeasure->m_choiceX0, xTime);
                pMeasure->m_sliderX0->SetValue(pOsciloscope->window.measure.data.pickX0.position.x * 1000);
            }
            if(pOsciloscope->window.measure.data.pickX1.updateUI)
            {
                pOsciloscope->window.measure.data.pickX1.updateUI = false;
                double xTime = pOsciloscope->window.measure.data.pickX1.position.getXTime(pOsciloscope->signalPosition, pOsciloscope->signalZoom);
                pMeasure->setTextCtrlX(pMeasure->m_textCtrlX1, pMeasure->m_choiceX1, xTime);
                pMeasure->m_sliderX1->SetValue(pOsciloscope->window.measure.data.pickX1.position.x * 1000);
            }
            if(pOsciloscope->window.measure.data.pickY0.updateUI)
            {
                pOsciloscope->window.measure.data.pickY0.updateUI = false;
                double y0Volt = pOsciloscope->window.measure.data.pickY0.position.getYVolt(pMeasure->m_choiceY0->GetSelection());
                pMeasure->setTextCtrlY(pMeasure->m_textCtrlY0, y0Volt);
                pMeasure->m_sliderY0->SetValue(pOsciloscope->window.measure.data.pickY0.position.y * 1000);
            }
            if(pOsciloscope->window.measure.data.pickY1.updateUI)
            {
                pOsciloscope->window.measure.data.pickY1.updateUI = false;
                double y1Volt = pOsciloscope->window.measure.data.pickY1.position.getYVolt(pMeasure->m_choiceY1->GetSelection());
                pMeasure->setTextCtrlY(pMeasure->m_textCtrlY1, y1Volt);
                pMeasure->m_sliderY1->SetValue(pOsciloscope->window.measure.data.pickY1.position.y * 1000);
            }
            if(pOsciloscope->window.measure.data.pickFFT0.updateUI)
            {
                pOsciloscope->window.measure.data.pickFFT0.updateUI = false;
                double FFT0Freq = pOsciloscope->window.measure.data.pickFFT0.position.getXFreq();
                pMeasure->setTextCtrlFreq(pMeasure->m_textCtrlFFT0, pMeasure->m_choiceFFT0, FFT0Freq);
                pMeasure->m_sliderFFT0->SetValue(pOsciloscope->window.measure.data.pickFFT0.position.x * 1000);
            }
            if(pOsciloscope->window.measure.data.pickFFT1.updateUI)
            {
                pOsciloscope->window.measure.data.pickFFT1.updateUI = false;
                double FFT1Freq = pOsciloscope->window.measure.data.pickFFT1.position.getXFreq();
                pMeasure->setTextCtrlFreq(pMeasure->m_textCtrlFFT1, pMeasure->m_choiceFFT1, FFT1Freq);
                pMeasure->m_sliderFFT1->SetValue(pOsciloscope->window.measure.data.pickFFT1.position.x * 1000);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        // debug
        ////////////////////////////////////////////////////////////////////////////////

        /*if(pDebug && pDebug->IsShown())
        {
            if(SDL_AtomicGet(&pOsciloscope->signalMode) != SIGNAL_MODE_PAUSE)
            {
                pDebug->Clear();
                int count = min(32, pOsciloscope->display.debug.getCount());
                FORMAT_BUFFER();
                for(int i = 0; i < count / 2; i++)
                {
                    ushort wordToPrint = (ushort(pOsciloscope->display.debug[2 * i]) << 8) | pOsciloscope->display.debug[2 * i + 1];
                    FORMAT("%d HEX:%04x DEC:%04d \n", i, wordToPrint, wordToPrint);
                    pDebug->AppendText(formatBuffer);
                }
            }
        }*/

    }

    ////////////////////////////////////////////////////////////////////////////////
    // render
    ////////////////////////////////////////////////////////////////////////////////
    pOsciloscope->Render();
    SDL_Delay(9);

    ////////////////////////////////////////////////////////////////////////////////
    // continue loop
    ////////////////////////////////////////////////////////////////////////////////
    event.Skip();
    event.RequestMore();
}

void OsciloskopOsciloskop::OnSetFocus(wxFocusEvent& event)
{
// TODO: Implement OnSetFocus
}

void OsciloskopOsciloskop::OnSize(wxSizeEvent& event)
{
    this->Layout();
    this->Refresh();
}

// open .osc file
void OsciloskopOsciloskop::m_menuItem1OnMenuSelection(wxCommandEvent& event)
{
   wxFileDialog* LoadDialog = new wxFileDialog(this, _("Load File As _?"), wxEmptyString, wxEmptyString, _("*.osc"), wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);
   // Creates a Load Dialog with 4 file types
   int show = LoadDialog->ShowModal();
   if (show == wxID_OK)   // If the user clicked "OK"
   {
      // file
      String file = LoadDialog->GetPath().ToAscii().data();
      LoadDialog->Destroy();
      //delete LoadDialog;

      // show dialog
      wxProgressDialog* progressDlg = new wxProgressDialog(_("Loading File"), _("in progress..."), 100, this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT);
      progressDlg->Show(true);
      // run thread
      pOsciloscope->loadFromFile(file.asChar());
      // loop
      int active = 1;
      while (active)
      {
         int progress = pOsciloscope->m_captureBuffer.getProgress();
         active = pOsciloscope->m_captureBuffer.isActive();
         progressDlg->Update(progress);
         if (progressDlg->WasCancelled())
         {
            pOsciloscope->m_captureBuffer.disable();
         }

         pOsciloscope->Render();
      }
      // destroy
      progressDlg->Destroy();
      //delete progressDlg;
   }
   else
   {
      LoadDialog->Destroy();
      //delete LoadDialog;
   }
   int frameSamples = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameSamples);
   m_textCtrlTimeFrameSize->SetValue(wxString::FromAscii(pFormat->integerToString(frameSamples)));
}

// save .osc file
void OsciloskopOsciloskop::m_menuItem2OnMenuSelection(wxCommandEvent& event)
{
   wxFileDialog* SaveDialog = new wxFileDialog(this, _("Save File As _?"), wxEmptyString, wxEmptyString, _("*.osc"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

   // Creates a Save Dialog with 4 file types
   int show = SaveDialog->ShowModal();
   if (show == wxID_OK)   // If the user clicked "OK"
   {
      // file
      String file = SaveDialog->GetPath().ToAscii().data();
      #if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
            file.add(".osc");
      #endif
      SaveDialog->Destroy();
      //delete SaveDialog;

      // show dialog
      wxProgressDialog* progressDlg = new wxProgressDialog(_("Saving File"), _("in progress..."), 100, this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT);
      progressDlg->Show(true);
      // run thread
      pOsciloscope->saveToFile(file.asChar());
      // loop
      int active = 1;
      while (active)
      {
         int progress = pOsciloscope->m_captureBuffer.getProgress();
         active = pOsciloscope->m_captureBuffer.isActive();
         progressDlg->Update(progress);
         if (progressDlg->WasCancelled())
         {
            pOsciloscope->m_captureBuffer.disable();
         }

         pOsciloscope->Render();
      }
      // destroy
      progressDlg->Destroy();
      //delete progressDlg;
   }
   else
   {
      SaveDialog->Destroy();
      //delete SaveDialog;
   }
}

void OsciloskopOsciloskop::m_menuItem3OnMenuSelection(wxCommandEvent& event)
{
    Close(true);
}

void OsciloskopOsciloskop::m_menuItem20OnMenuSelection(wxCommandEvent& event)
{
    pOsciloscope->window.fftDigital.raise(VIEW_SELECT_OSC_3D);
    pOsciloscope->window.fftDigital.lower(VIEW_SELECT_OSC_2D);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(0)->Check(1);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Check(0);
    pOsciloscope->oscCameraSetup(1);
}

void OsciloskopOsciloskop::m_menuItem21OnMenuSelection(wxCommandEvent& event)
{
    pOsciloscope->window.fftDigital.lower(VIEW_SELECT_OSC_3D);
    pOsciloscope->window.fftDigital.raise(VIEW_SELECT_OSC_2D);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(0)->Check(0);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(1)->Check(1);
    pOsciloscope->oscCameraSetup(0);
}

void OsciloskopOsciloskop::m_menuItem22OnMenuSelection(wxCommandEvent& event)
{
    pOsciloscope->window.fftDigital.raise(VIEW_SELECT_FFT_3D);
    pOsciloscope->window.fftDigital.lower(VIEW_SELECT_FFT_2D);
    pOsciloscope->window.fftDigital.lower(VIEW_SELECT_DIGITAL);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(2)->Check(1);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(3)->Check(0);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(4)->Check(0);
    pOsciloscope->fftCameraSetup(1);
}

void OsciloskopOsciloskop::m_menuItem23OnMenuSelection(wxCommandEvent& event)
{
    pOsciloscope->window.fftDigital.lower(VIEW_SELECT_FFT_3D);
    pOsciloscope->window.fftDigital.raise(VIEW_SELECT_FFT_2D);
    pOsciloscope->window.fftDigital.lower(VIEW_SELECT_DIGITAL);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(2)->Check(0);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(3)->Check(1);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(4)->Check(0);
    pOsciloscope->fftCameraSetup(0);
}

void OsciloskopOsciloskop::m_menuItem6OnMenuSelection(wxCommandEvent& event)
{
    pOsciloscope->window.fftDigital.lower(VIEW_SELECT_FFT_3D);
    pOsciloscope->window.fftDigital.lower(VIEW_SELECT_FFT_2D);
    pOsciloscope->window.fftDigital.raise(VIEW_SELECT_DIGITAL);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(2)->Check(0);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(3)->Check(0);
    GetMenuBar()->GetMenu(1)->FindItemByPosition(4)->Check(1);
    pOsciloscope->fftCameraSetup(0);
}

void OsciloskopOsciloskop::m_menuItem8OnMenuSelection(wxCommandEvent& event)
{
    if(!pDisplay)
    {
        pDisplay = new OsciloskopDisplay(this);
    }
    pDisplay->Show();
}

void OsciloskopOsciloskop::m_menuItem9OnMenuSelection(wxCommandEvent& event)
{
    if(!pThermal)
    {
        pThermal = new OsciloskopThermal(this);
    }
    pThermal->Show();
}

void OsciloskopOsciloskop::m_menuItemSoftwareOnMenuSelection(wxCommandEvent& event)
{
}

void OsciloskopOsciloskop::m_menuItem11OnMenuSelection(wxCommandEvent& event)
{
}

void OsciloskopOsciloskop::m_menuItemReadEEPROMOnMenuSelection(wxCommandEvent& event)
{
    // pause
    wxCommandEvent evt;
    m_buttonPauseOnButtonClick(evt);
    // firmware id
    String  ID;
    SEeprom eeprom;
    int     size   = 0;
    int     offset = 0;
    memset(eeprom.data.bytes, 0x00, sizeof(SEeprom));
    pOsciloscope->thread.readFirmwareIDFromEEPROM(pOsciloscope->settings.getHardware());
    pOsciloscope->thread.getEEPROM(&eeprom, &size, &offset);
    ID = (char*)&eeprom.data.bytes[0];
    // data
    memset(eeprom.data.bytes, 0x00, sizeof(SEeprom));
    pOsciloscope->thread.readUsbFromEEPROM(pOsciloscope->settings.getHardware(), (16 * EEPROM_BYTE_COUNT));
    pOsciloscope->thread.getEEPROM(&eeprom, &size, &offset);
    // debug
    wxCommandEvent et;
    m_menuItemDebugOnMenuSelection(et);
    if(pDebug && pDebug->IsShown())
    {
        pDebug->Clear();
        FORMAT_BUFFER();
        FORMAT("%s \n", (char*)ID.asChar());
        pDebug->AppendText(wxString::FromAscii(formatBuffer));
        int count = min(16, size / EEPROM_BYTE_COUNT);
        for(int i = 0; i < count; i++)
        {
            for(int j = 0; j < EEPROM_BYTE_COUNT; j++)
            {
                byte byteToPrint = eeprom.data.bytes[i * EEPROM_BYTE_COUNT + j];
                FORMAT("%02x ", byteToPrint);
                pDebug->AppendText(wxString::FromAscii(formatBuffer));
            }
            pDebug->AppendText(wxString::FromAscii("\n"));
        }
    }
}

void OsciloskopOsciloskop::m_menuItemWriteEEPROMOnMenuSelection(wxCommandEvent& event)
{
    pOsciloscope->thread.writeUsbToEEPROM(pOsciloscope->settings.getHardware(), 0);
}

void OsciloskopOsciloskop::m_menuItem12OnMenuSelection(wxCommandEvent& event)
{
    pOsciloscope->AutoCalibrate();
}

void OsciloskopOsciloskop::m_menuItemWriteCalibrateOnMenuSelection(wxCommandEvent& event)
{
    pOsciloscope->settings.load();
    pOsciloscope->thread.writeCalibrateSettingsToEEPROM(pOsciloscope->settings.getHardware());
}

void OsciloskopOsciloskop::m_menuItemReadCalibrateOnMenuSelection(wxCommandEvent& event)
{
    // pause
    wxCommandEvent evt;
    m_buttonPauseOnButtonClick(evt);
    // read
    SEeprom eeprom;
    int     size = 0;
    int     offset = 0;
    memset(eeprom.data.bytes, 0xdd, sizeof(SEeprom));
    //read calibration from EEPROM
    if (pOsciloscope->thread.readCalibrateSettingsFromEEPROM(pOsciloscope->settings.getHardware())== SCOPEFUN_SUCCESS)
    {
        pOsciloscope->thread.getEEPROM(&eeprom, &size, &offset);
        // get current OscHardware struct from RAM
        OscHardware* oscHw = pOsciloscope->settings.getHardware();
        // copy OscHardware from EEPROM to OscHardware in RAM
        SDL_memcpy(oscHw, &eeprom, size);
        // save calibration from OscHardware to file
        oscHw->saveCalibrationToFile();

        // show calibration data in debug window
        wxCommandEvent et;
        m_menuItemDebugOnMenuSelection(et);
        if (pDebug && pDebug->IsShown())
        {
            pDebug->Clear();
            // create
            cJSON* jsonRoot = cJSON_CreateObject();
            // calibrated
            cJSON* jsonCalibratedNormal = cJSON_CreateObject();
            cJSON_AddItemToObject(jsonRoot, "calibratedNormal", jsonCalibratedNormal);
            oscHw->saveCalibrated(jsonCalibratedNormal, ctNormal);
            cJSON* jsonCalibrated500Mhz = cJSON_CreateObject();
            cJSON_AddItemToObject(jsonRoot, "calibrated500Mhz", jsonCalibrated500Mhz);
            oscHw->saveCalibrated(jsonCalibrated500Mhz, ct500Mhz);
            // save
            setlocale(LC_ALL, "C");
            pDebug->AppendText("Calibration from EEPROM:\n");
            char* jsonString = cJSON_Print(jsonRoot);
            // show text
            pDebug->AppendText(jsonString);
            pDebug->AppendText("\n");
       }
    }
}

void OsciloskopOsciloskop::m_menuItemWriteCertificateOnMenuSelection(wxCommandEvent& event)
{
    m_buttonConnectOnButtonClick(event);
    pOsciloscope->thread.wait();
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Clipboard data will be written to EEPROM.", "Please copy data to clipboard before proceeding.", 0);
    if(wxTheClipboard->Open())
    {
        if(wxTheClipboard->IsSupported(wxDF_TEXT))
        {
            wxTextDataObject data;
            if(wxTheClipboard->GetData(data))
            {
                wxString string = data.GetText();
                char* src = (char*)string.To8BitData().data();
                cJSON_Minify(src);
                size_t size = SDL_strlen(src);
                if(size > 256)
                {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Clipboard data is too large.", " Max. 256 bytes can be written.", 0);
                }
                else
                {
                    cJSON* json = cJSON_Parse(src);
                    bool isValid = json && json->child && json->child[0].next;
                    if(isValid && SDL_memcmp(json->child[0].next->valuestring, ((UsbContext*)getCtx()->usb)->serialBuffer, ((UsbContext*)getCtx()->usb)->serialBufferSize) == 0)
                    {
                        char zeroAll[256] = { 0 };
                        int ret = usbFx3WriteLockableEEPROM((UsbContext*)getCtx()->usb, (unsigned char*)zeroAll, 256, 0);
                        if(ret == PUREUSB_SUCCESS)
                        {
                            int ret = usbFx3WriteLockableEEPROM((UsbContext*)getCtx()->usb, (unsigned char*)src, size, 0);
                            if(ret == PUREUSB_SUCCESS)
                            {
                                char verify[256] = { 0 };
                                ret = usbFx3ReadLockableEEPROM((UsbContext*)getCtx()->usb, (unsigned char*)verify, size, 0);
                                if(ret == PUREUSB_SUCCESS)
                                {
                                    if(SDL_memcmp((unsigned char*)src, verify, size) == 0)
                                    {
                                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Clipboard data written and verified that all written bits match original.", verify, 0);
                                    }
                                    else
                                    {
                                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Verifying written bits failed.", verify, 0);
                                    }
                                }
                                else
                                {
                                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Reading written bits failed.", "failed", 0);
                                }
                            }
                            else
                            {
                                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Writing bits failed.", "failed", 0);
                            }
                        }
                    }
                    else
                    {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Clipboard data serial id does not match usb.", "Serial id is incorrect.", 0);
                    }
                }
            }
        }
        wxTheClipboard->Close();
    }
}

void OsciloskopOsciloskop::m_menuItemReadCertificateOnMenuSelection(wxCommandEvent& event)
{
    m_buttonConnectOnButtonClick(event);
    pOsciloscope->thread.wait();
    char verify[257] = { 0 };
    int ret = usbFx3ReadLockableEEPROM((UsbContext*)getCtx()->usb, (unsigned char*)verify, 256, 0);
    if(ret == PUREUSB_SUCCESS)
    {
       //if (wxTheClipboard->IsOpened())
       //   wxTheClipboard->Close();
        if(wxTheClipboard->Open())
        {
            wxString string = wxString::From8BitData(verify);
            wxTextDataObject* data = new wxTextDataObject(string);
            wxTheClipboard->SetData(data);
            wxTheClipboard->Flush();
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Certificate data was read and written to clipboard.", string.data().AsChar(), 0);
            wxTheClipboard->Close();
        }
    }
}

void OsciloskopOsciloskop::m_menuItemLockCertificateOnMenuSelection(wxCommandEvent& event)
{
    const SDL_MessageBoxButtonData buttons[] =
    {
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "no" },
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "yes" },
    };
    const SDL_MessageBoxColorScheme colorScheme =
    {
        { /* .colors (.r, .g, .b) */
            /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
            { 255,   0,   0 },
            /* [SDL_MESSAGEBOX_COLOR_TEXT] */
            {   0, 255,   0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
            { 255, 255,   0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
            {   0,   0, 255 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
            { 255,   0, 255 }
        }
    };
    const SDL_MessageBoxData messageboxdata =
    {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        NULL, /* .window */
        "Data will be locked permanently.", /* .title */
        "This action cannot be undone.", /* .message */
        SDL_arraysize(buttons), /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
    };
    int buttonid = 0;
    int ret = SDL_ShowMessageBox(&messageboxdata, &buttonid);
    if(buttonid == SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT)
    {
        m_buttonConnectOnButtonClick(event);
        pOsciloscope->thread.wait();
        int ret = usbFx3LockLockableEEPROM((UsbContext*)getCtx()->usb);
        if(ret == PUREUSB_SUCCESS)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Data locked.", "permanently", 0);
        }
    }
}

void OsciloskopOsciloskop::m_menuItemResetEEPROMOnMenuSelection(wxCommandEvent& event)
{
    pOsciloscope->thread.eraseEEPROM(pOsciloscope->settings.getHardware());
}

void OsciloskopOsciloskop::m_menuItemFactoryWriteEEPROMOnMenuSelection(wxCommandEvent& event)
{
    pOsciloscope->thread.writeUsbToEEPROM(pOsciloscope->settings.getHardware(), 1);
}

void OsciloskopOsciloskop::m_menuItemEraseECalibrationOnMenuSelection(wxCommandEvent& event)
{
    pOsciloscope->thread.eraseCalibrationEEPROM(pOsciloscope->settings.getHardware());
}

void OsciloskopOsciloskop::m_menuItemFactoryLoadFX3RamOnMenuSelection(wxCommandEvent& event)
{
    // debug text
    String str_debug = "Firmware upload";
    // open debug window
    wxCommandEvent et;
    m_menuItemDebugOnMenuSelection(et);
    if(pDebug && pDebug->IsShown())
    {
        // show text
        FORMAT_BUFFER();
        FORMAT("%s ", (char*)str_debug.asChar());
        //pDebug->Clear();
        pDebug->AppendText(wxString::FromAscii(formatBuffer));
        int iret = pOsciloscope->thread.writeUsbToRam(pOsciloscope->settings.getHardware(), 1);
        if(iret == SCOPEFUN_SUCCESS) {
            str_debug = "OK.";
        }
        else {
            str_debug = "failed.\n";
        }
        FORMAT("%s", (char*)str_debug.asChar());
        pDebug->AppendText(wxString::FromAscii(formatBuffer));
        // read scopefun serial number
        if (pOsciloscope->thread.isOpen() and (((UsbContext*)getCtx()->usb)->vendorId != CYPRESS_VID))
        {
            pDebug->AppendText(" S/N: ");
            pDebug->AppendText(((UsbContext*)getCtx()->usb)->serialBuffer);
            pDebug->AppendText("\n");
        }
    }
}

void OsciloskopOsciloskop::m_menuItem15OnMenuSelection(wxCommandEvent& event)
{
    if(!pMeasure)
    {
        pMeasure = new OsciloskopMeasure(this);
    }
    pMeasure->Show();
}

void OsciloskopOsciloskop::m_menuItemDebugOnMenuSelection(wxCommandEvent& event)
{
    if(!pDebug)
    {
        pDebug = new OsciloskopDebug(this);
    }
    pDebug->Show();
}

void OsciloskopOsciloskop::m_menuItemInfoOnMenuSelection(wxCommandEvent& event)
{
    if(!pInfo)
    {
        pInfo = new OsciloskopInfo(this);
    }
    pInfo->Show();
}

void OsciloskopOsciloskop::m_buttonConnectOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->thread.openUSB(pOsciloscope->settings.getHardware(), 0);
    if (pOsciloscope->thread.isOpen())
        pOsciloscope->thread.useEepromCalibration(pOsciloscope->settings.getHardware());
    // Send full hardware configuration after connect
    pOsciloscope->setupControl(pOsciloscope->window);
    pOsciloscope->transferData();
    m_comboBoxCh0CaptureOnCombobox(event);
    m_comboBoxCh1CaptureOnCombobox(event);
}

void OsciloskopOsciloskop::m_buttonDisconnectOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->thread.function(afCloseUsb);
    SDL_AtomicSet(&pOsciloscope->signalMode, SIGNAL_MODE_PAUSE);
    pOsciloscope->window.horizontal.Mode = SIGNAL_MODE_PAUSE;
    // must sync close button usbClose and usbCapture thread
    // current fix is that we just wait a second which should be enough for the capture thread to exit capture state in most cases
    SDL_Delay(1000);
    m_buttonPauseOnButtonClick(event);
}

void OsciloskopOsciloskop::m_buttonResetOnButtonClick(wxCommandEvent& event)
{
    usbFx3Reset((UsbContext*)pOsciloscope->ctx->sf.usb);
}

void OsciloskopOsciloskop::m_buttonUndoOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->transferUndo();
    event.Skip();
}

void OsciloskopOsciloskop::m_buttonRedoOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->transferRedo();
    event.Skip();
}

void OsciloskopOsciloskop::m_comboBoxTimeControlOnCombobox(wxCommandEvent& event)
{
    pOsciloscope->window.horizontal.Control = m_comboBoxTimeControl->GetSelection();
    sfSetControl(getHw(), pOsciloscope->window.horizontal.Control);
    pOsciloscope->transferData();
    if(sfGetControl(getHw()) == 3)
    {
        usbFx3Reset((UsbContext*)pOsciloscope->ctx->sf.usb);
    }
}

void OsciloskopOsciloskop::m_comboBoxTimeCaptureOnCombobox(wxCommandEvent& event)
{
    pOsciloscope->window.horizontal.Capture = captureTimeFromEnum(m_comboBoxTimeCapture->GetSelection());
    SDL_AtomicSet(&pOsciloscope->threadCapture, m_comboBoxTimeCapture->GetSelection());
    sfSetXRange(getHw(), m_comboBoxTimeCapture->GetSelection());
    pOsciloscope->transferData();
    /*
    pOsciloscope->window.horizontal.Display = pOsciloscope->window.horizontal.Capture;
    uint  multiEnum = multiplyerFromValue(pOsciloscope->window.horizontal.Display);
    float multiFloat = multiplyerFromEnum(multiEnum);
    */
    pOsciloscope->window.measure.ClearCapture();
    pOsciloscope->window.measure.data.column[MEASURE_MINIMUM].SetValue(MAX_DOUBLE);
    pOsciloscope->window.measure.data.column[MEASURE_MAXIMUM].SetValue(-MAX_DOUBLE);
}

void OsciloskopOsciloskop::m_checkBoxETSOnCheckBox(wxCommandEvent& event)
{
    // ETS support removed: always disable
    pOsciloscope->window.horizontal.ETS = 0;
    pOsciloscope->transferData();
    SDL_AtomicSet(&pOsciloscope->clearRenderTarget,1);
}

void OsciloskopOsciloskop::m_checkBoxFullOnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.horizontal.Full = m_checkBoxFull->GetValue();
}

void OsciloskopOsciloskop::m_textCtrlTimePositionOnTextEnter(wxCommandEvent& event)
{
    SDL_AtomicSet(&pOsciloscope->clearThermal, 1);
    pOsciloscope->window.horizontal.Position = pFormat->stringToFloat(m_textCtrlTimePosition->GetValue().ToAscii().data());
    m_sliderTimePosition->SetValue(pOsciloscope->window.horizontal.Position);
}

void OsciloskopOsciloskop::m_spinBtnXPosOnSpinDown(wxSpinEvent& event)
{
    m_sliderTimePosition->SetValue(m_sliderTimePosition->GetValue() - 1);
    wxScrollEvent e;
    m_sliderTimePositionOnScroll(e);
}

void OsciloskopOsciloskop::m_spinBtnXPosOnSpinUp(wxSpinEvent& event)
{
    m_sliderTimePosition->SetValue(m_sliderTimePosition->GetValue() + 1);
    wxScrollEvent e;
    m_sliderTimePositionOnScroll(e);
}

void OsciloskopOsciloskop::m_sliderTimePositionOnScroll(wxScrollEvent& event)
{
    SDL_AtomicSet(&pOsciloscope->clearThermal, 1);
    pOsciloscope->window.horizontal.Position = m_sliderTimePosition->GetValue();
    m_textCtrlTimePosition->SetValue(wxString::FromAscii(pFormat->floatToString(pOsciloscope->window.horizontal.Position)));
}

void OsciloskopOsciloskop::m_textCtrlTimeFrameOnTextEnter(wxCommandEvent& event)
{
    int frameCount = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameCount);
    int frameIndex = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameIndex);
    int frameStart = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameStart);

    pOsciloscope->window.horizontal.Frame = pFormat->stringToFloat(m_textCtrlTimeFrame->GetValue().ToAscii().data());
    pOsciloscope->window.horizontal.Frame = clamp<int>(pOsciloscope->window.horizontal.Frame, 0, frameCount - 1);
                            int  newIndex = frameStart + pOsciloscope->window.horizontal.Frame;
    SDL_AtomicSet(&pOsciloscope->m_captureBuffer.m_frameIndex, newIndex);

    m_textCtrlTimeFrame->SetValue(pFormat->integerToString(pOsciloscope->window.horizontal.Frame));
    m_sliderTimeFrame->SetValue(pOsciloscope->window.horizontal.Frame);
    SDL_AtomicSet(&pOsciloscope->clearRenderTarget, 1);
}

void OsciloskopOsciloskop::m_spinBtnFrameHistoryOnSpinDown(wxSpinEvent& event)
{
    m_sliderTimeFrame->SetValue(m_sliderTimeFrame->GetValue() - 1);
    wxScrollEvent e;
    m_sliderTimeFrameOnScroll(e);
}

void OsciloskopOsciloskop::m_spinBtnFrameHistoryOnSpinUp(wxSpinEvent& event)
{
    m_sliderTimeFrame->SetValue(m_sliderTimeFrame->GetValue() + 1);
    wxScrollEvent e;
    m_sliderTimeFrameOnScroll(e);
}

void OsciloskopOsciloskop::m_sliderTimeFrameOnScroll(wxScrollEvent& event)
{
    int frameCount  = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameCount);
    int frameIndex  = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameIndex);
    int frameStart  = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameStart);
    int frameSlider = m_sliderTimeFrame->GetValue();
    int  newIndex   = frameStart + frameSlider;
    SDL_AtomicSet(&pOsciloscope->m_captureBuffer.m_frameIndex, newIndex );

    // ui
    m_textCtrlTimeFrame->SetValue(wxString::FromAscii(pFormat->floatToString(frameSlider)));

    // render
    SDL_AtomicSet(&pOsciloscope->clearRenderTarget, 1);
}

void OsciloskopOsciloskop::m_buttonPlayOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.horizontal.Mode = SIGNAL_MODE_PLAY;
    SDL_AtomicSet(&pOsciloscope->signalMode, SIGNAL_MODE_PLAY);
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        SetButtonColors();
        m_buttonPlay->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonPlay->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
    }
    pOsciloscope->UndoRedoOnOff(0);
    if(event.GetClientData() == 0)
    { pOsciloscope->transferData(); }
    pOsciloscope->UndoRedoOnOff(1);
    m_buttonPause->Enable();
    m_buttonPlay->Disable();
    if (pOsciloscope->thread.isFpga())
        m_buttonCapture->Enable();
    m_buttonClear->Enable();
}

void OsciloskopOsciloskop::m_buttonPauseOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.horizontal.Mode = SIGNAL_MODE_PAUSE;
    SDL_AtomicSet(&pOsciloscope->signalMode, SIGNAL_MODE_PAUSE);
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        SetButtonColors();
        m_buttonPause->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonPause->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
    }
    pOsciloscope->UndoRedoOnOff(0);
    if(event.GetClientData() == 0)
    { pOsciloscope->transferData(); }
    pOsciloscope->UndoRedoOnOff(1);
    m_buttonPause->Disable();
    m_buttonPlay->Enable();
    if (pOsciloscope->thread.isFpga())
        m_buttonCapture->Enable();
    m_buttonClear->Enable();
}

void OsciloskopOsciloskop::m_buttonCaptureOnButtonClick(wxCommandEvent& event)
{
    int isFpga = pOsciloscope->thread.isFpga();
    int isOpen = pOsciloscope->thread.isOpen();
    if (isOpen && isFpga)
    {
        int frameIndex = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameIndex);
        int frameStart = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameStart);
        int frameEnd   = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameEnd);
        int frameCount = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameCount);
        SDL_AtomicSet(&pOsciloscope->m_captureBuffer.m_frameIndex,frameEnd);

        pOsciloscope->window.horizontal.Mode = SIGNAL_MODE_CAPTURE;
        SDL_AtomicSet(&pOsciloscope->signalMode, SIGNAL_MODE_CAPTURE);
        if (!pOsciloscope->settings.getColors()->windowDefault)
        {
            SetButtonColors();
            m_buttonCapture->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonCapture->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
        }
        pOsciloscope->UndoRedoOnOff(0);
        if (event.GetClientData() == 0)
        {
            //wxCommandEvent event;
            m_comboBoxCh0CaptureOnCombobox(event);
            m_comboBoxCh1CaptureOnCombobox(event);
            //pOsciloscope->transferData();
        }
        pOsciloscope->UndoRedoOnOff(1);
        m_buttonPause->Enable();
        m_buttonPlay->Enable();
        m_buttonCapture->Disable();
        m_buttonClear->Enable();
    }
}

void OsciloskopOsciloskop::m_buttonClearOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.horizontal.Mode = SIGNAL_MODE_CLEAR;
    SDL_AtomicSet(&pOsciloscope->signalMode, SIGNAL_MODE_CLEAR);
    SDL_AtomicSet(&pOsciloscope->m_captureBuffer.m_frameClear, 1);
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        SetButtonColors();
        m_buttonClear->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonClear->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
    }
    pOsciloscope->UndoRedoOnOff(0);
    if(event.GetClientData() == 0)
    { pOsciloscope->transferData(); }
    pOsciloscope->UndoRedoOnOff(1);
    m_buttonPause->Enable();
    m_buttonPlay->Enable();
    if (pOsciloscope->thread.isFpga())
        m_buttonCapture->Enable();
    m_buttonClear->Disable();
}

void OsciloskopOsciloskop::m_textCtrlTimeFrameSizeOnTextEnter(wxCommandEvent& event)
{
    // hw
    sfSetSampleSize(getHw(), atoi(m_textCtrlTimeFrameSize->GetValue().ToAscii().data()));
    sfSetTriggerPre(getHw(), pOsciloscope->window.trigger.Percent);
    uint frameSizeBytes = sfGetFrameSize(getHw());

    // capture
    SDL_AtomicSet(&pOsciloscope->m_captureBuffer.m_frameSize,  frameSizeBytes);
    SDL_AtomicSet(&pOsciloscope->m_captureBuffer.m_frameCount, max<ularge>(1,pOsciloscope->m_captureBuffer.m_dataMax / ularge(frameSizeBytes) ) );

    // ui
    //pOsciloscope->window.horizontal.FrameSize = ((frameSizeBytes-SCOPEFUN_FRAME_HEADER)/4);
    pOsciloscope->window.horizontal.FrameSize = sfGetSampleSize(getHw());
    m_textCtrlTimeFrameSize->SetValue(wxString::FromAscii(pFormat->integerToString(sfGetSampleSize(getHw()))));

    // transfer
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_textCtrlTimeFFTSizeOnTextEnter(wxCommandEvent& event)
{
    iint fftsize = atoi(m_textCtrlTimeFFTSize->GetValue().ToAscii().data());
    fftsize = min(fftsize, NUM_FFT);
    pOsciloscope->window.horizontal.FFTSize = fftsize;
    m_textCtrlTimeFFTSize->SetValue(wxString::FromAscii(pFormat->integerToString(fftsize)));
}

void OsciloskopOsciloskop::m_comboBoxCh0CaptureOnCombobox(wxCommandEvent& event)
{
    float time = pOsciloscope->window.horizontal.Capture;
    float captureOld = pOsciloscope->window.channel01.Capture;
    double oldTriggerVoltagePerSteps = pOsciloscope->getTriggerVoltagePerStep();
    pOsciloscope->window.channel01.Capture = captureVoltFromEnum(m_comboBoxCh0Capture->GetSelection());
    pOsciloscope->window.channel01.Scale   = pFormat->stringToFloat(m_textCtrlCh0Scale->GetValue().ToAscii().data());
    pOsciloscope->window.channel01.Display = pOsciloscope->window.channel01.Capture;
    uint capture0 = m_comboBoxCh0Capture->GetSelection();
    sfSetYRangeScaleA(getHw(), pOsciloscope->getAttr(capture0), pOsciloscope->getGain(0, capture0));
    // yposition fix
    float captureNew       = pOsciloscope->window.channel01.Capture;
    double slidePos        = m_sliderCh0Position->GetValue();
    double slidePosOffset  = -slidePos + pOsciloscope->settings.getHardware()->getAnalogOffset(time, 0, captureNew);
    pOsciloscope->window.channel01.YPosition = -slidePos * pOsciloscope->settings.getHardware()->getAnalogStep(time, 0, captureNew);
    m_textCtrlCh0Position->SetValue(pFormat->floatToString(pOsciloscope->window.channel01.YPosition));
    sfSetYPositionA(getHw(), slidePosOffset);
    float volt;
    uint  unit;
    _setYDisplay(volt, unit, (VoltageCapture)m_comboBoxCh0Capture->GetSelection());
    double newTriggerVoltagePerStep = pOsciloscope->getTriggerVoltagePerStep();
    RecalculateTriggerPosition(oldTriggerVoltagePerSteps, newTriggerVoltagePerStep);
    // transfer
    pOsciloscope->transferData();
    pOsciloscope->window.measure.ClearCapture();
    pOsciloscope->window.measure.data.column[MEASURE_MINIMUM].SetValue(MAX_DOUBLE);
    pOsciloscope->window.measure.data.column[MEASURE_MAXIMUM].SetValue(-MAX_DOUBLE);
}

void OsciloskopOsciloskop::m_textCtrlCh0ScaleOnTextEnter(wxCommandEvent& event)
{
    pOsciloscope->window.channel01.Scale = pFormat->stringToFloat(m_textCtrlCh0Scale->GetValue().ToAscii().data());
    uint capture0 = m_comboBoxCh0Capture->GetSelection();
    sfSetYRangeScaleA(getHw(), pOsciloscope->getAttr(capture0), pOsciloscope->getGain(0, capture0));
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_choiceCh0ACDCOnChoice(wxCommandEvent& event)
{
    pOsciloscope->window.channel01.AcDc = m_choiceCh0ACDC->GetSelection();
    sfSetAnalogSwitchBit(getHw(), CHANNEL_A_ACDC, m_choiceCh0ACDC->GetSelection());
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_checkBoxCh0InvertOnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.channel01.Invert = m_checkBoxCh0Invert->GetValue();
}

void OsciloskopOsciloskop::m_checkBoxCh0GroundOnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.channel01.Ground = m_checkBoxCh0Ground->GetValue();
    sfSetAnalogSwitchBit(getHw(), CHANNEL_A_GROUND, m_checkBoxCh0Ground->GetValue());
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_checkBoxSignal1OnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.channel01.OscOnOff = m_checkBoxSignal1->IsChecked() ? 1 : 0;
}

void OsciloskopOsciloskop::m_checkBoxFFT1OnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.channel01.FFTOnOff = m_checkBoxFFT1->IsChecked() ? 1 : 0;
}

void OsciloskopOsciloskop::m_checkBoxAvg01OnCheckBox(wxCommandEvent& event)
{
    int avg = sfGetAverage(getHw());
    if(m_checkBoxAvg01->IsChecked()) { avg |=  512; }
    else                             { avg &= ~512; }
    sfSetAverage(getHw(), avg);
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_toggleBtnDigitalPatternOutputEnableOnToggle(wxCommandEvent& event)
{
}

void OsciloskopOsciloskop::m_comboBoxDigitalPatternModeOnCombobox(wxCommandEvent& event)
{
}

void OsciloskopOsciloskop::m_buttonDigitalPatternRestartOnButtonClick(wxCommandEvent& event)
{
}

void OsciloskopOsciloskop::m_checkBoxDigitalPatternRestartOnUploadOnCheckBox(wxCommandEvent& event)
{
}

void OsciloskopOsciloskop::m_textCtrlCh0PositionOnTextEnter(wxCommandEvent& event)
{
    float time = pOsciloscope->window.horizontal.Capture;
    float capture = pOsciloscope->window.channel01.Capture;
    // voltage
    pOsciloscope->window.channel01.YPosition = pFormat->stringToDouble(m_textCtrlCh0Position->GetValue().ToAscii().data());
    // step
    double steps = double(pOsciloscope->window.channel01.YPosition) / pOsciloscope->settings.getHardware()->getAnalogStep(time, 0, capture);
    sfSetYPositionA(getHw(), steps + pOsciloscope->settings.getHardware()->getAnalogOffset(time, 0, capture));
    m_sliderCh0Position->SetValue(-steps);
    // transfer
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_spinBtnCh0YPosOnSpinDown(wxSpinEvent& event)
{
    // position
    int ypos = sfGetYPositionA(getHw());
    sfSetYPositionA(getHw(), ypos + 1);
    pOsciloscope->transferData();
    // ypos, step and offset
    ypos = sfGetYPositionA(getHw());
    float  time    = pOsciloscope->window.horizontal.Capture;
    float  capture = pOsciloscope->window.channel01.Capture;
    double step    = pOsciloscope->settings.getHardware()->getAnalogStep(time, 0, capture);
    int    offset  = pOsciloscope->settings.getHardware()->getAnalogOffset(time, 0, capture);
    // update ui
    m_sliderCh0Position->SetValue(-(ypos - offset));
    m_textCtrlCh0Position->SetValue(pFormat->doubleToString(step * double(ypos - offset)));
}

void OsciloskopOsciloskop::m_spinBtnCh0YPosOnSpinUp(wxSpinEvent& event)
{
    // position
    int ypos = sfGetYPositionA(getHw());
    sfSetYPositionA(getHw(), ypos - 1);
    pOsciloscope->transferData();
    // ypos, step and offset
    ypos = sfGetYPositionA(getHw());
    float  time    = pOsciloscope->window.horizontal.Capture;
    float  capture = pOsciloscope->window.channel01.Capture;
    double step    = pOsciloscope->settings.getHardware()->getAnalogStep(time, 0, capture);
    int    offset  = pOsciloscope->settings.getHardware()->getAnalogOffset(time, 0, capture);
    // update ui
    m_sliderCh0Position->SetValue(-(ypos - offset));
    m_textCtrlCh0Position->SetValue(pFormat->doubleToString(step * double(ypos - offset)));
}

void OsciloskopOsciloskop::m_sliderCh0PositionOnScroll(wxScrollEvent& event)
{
    float time    = pOsciloscope->window.horizontal.Capture;
    float capture = pOsciloscope->window.channel01.Capture;
    // steps
    sfSetYPositionA(getHw(), -m_sliderCh0Position->GetValue() + pOsciloscope->settings.getHardware()->getAnalogOffset(time, 0, capture));
    pOsciloscope->transferData();
    // voltage
    pOsciloscope->window.channel01.YPosition = double(-m_sliderCh0Position->GetValue()) * pOsciloscope->settings.getHardware()->getAnalogStep(time, 0, capture);
    m_textCtrlCh0Position->SetValue(pFormat->doubleToString(pOsciloscope->window.channel01.YPosition));
}

void OsciloskopOsciloskop::m_comboBoxCh1CaptureOnCombobox(wxCommandEvent& event)
{
    float captureOld = pOsciloscope->window.channel02.Capture;
    float time = pOsciloscope->window.horizontal.Capture;
    double oldTriggerVoltagePerStep = pOsciloscope->getTriggerVoltagePerStep();
    pOsciloscope->window.channel02.Capture = captureVoltFromEnum(m_comboBoxCh1Capture->GetSelection());
    pOsciloscope->window.channel02.Scale   = pFormat->stringToFloat(m_textCtrlCh1Scale->GetValue().ToAscii().data());
    pOsciloscope->window.channel02.Display = pOsciloscope->window.channel02.Capture;
    uint capture1 = m_comboBoxCh1Capture->GetSelection();
    sfSetYRangeScaleB(getHw(), pOsciloscope->getAttr(capture1), pOsciloscope->getGain(1, capture1));
    //
    float volt;
    uint  unit;
    _setYDisplay(volt, unit, (VoltageCapture)m_comboBoxCh1Capture->GetSelection());
    // yposition fix
    float captureNew = pOsciloscope->window.channel02.Capture;
    double slidePos = m_sliderCh1Position->GetValue();
    double slidePosOffset = -slidePos + pOsciloscope->settings.getHardware()->getAnalogOffset(time, 1, captureNew);
    pOsciloscope->window.channel02.YPosition = -slidePos * pOsciloscope->settings.getHardware()->getAnalogStep(time, 1, captureNew);
    m_textCtrlCh1Position->SetValue(pFormat->floatToString(pOsciloscope->window.channel02.YPosition));
    sfSetYPositionB(getHw(), slidePosOffset);
    //
    double newTriggerVoltagePerStep = pOsciloscope->getTriggerVoltagePerStep();
    RecalculateTriggerPosition(oldTriggerVoltagePerStep, newTriggerVoltagePerStep);
    pOsciloscope->transferData();
    pOsciloscope->window.measure.ClearCapture();
    pOsciloscope->window.measure.data.column[MEASURE_MINIMUM].SetValue(MAX_DOUBLE);
    pOsciloscope->window.measure.data.column[MEASURE_MAXIMUM].SetValue(-MAX_DOUBLE);
}

void OsciloskopOsciloskop::m_textCtrlCh1ScaleOnTextEnter(wxCommandEvent& event)
{
    pOsciloscope->window.channel02.Scale = pFormat->stringToFloat(m_textCtrlCh1Scale->GetValue().ToAscii().data());
    uint capture1 = m_comboBoxCh1Capture->GetSelection();
    sfSetYRangeScaleB(getHw(), pOsciloscope->getAttr(capture1), pOsciloscope->getGain(1, capture1));
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_choiceCh1ACDCOnChoice(wxCommandEvent& event)
{
    pOsciloscope->window.channel02.AcDc = m_choiceCh1ACDC->GetSelection();
    sfSetAnalogSwitchBit(getHw(), CHANNEL_B_ACDC, m_choiceCh1ACDC->GetSelection());
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_checkBoxCh1InvertOnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.channel02.Invert = m_checkBoxCh1Invert->GetValue();
}

void OsciloskopOsciloskop::m_checkBoxCh1GroundOnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.channel02.Ground = m_checkBoxCh1Ground->GetValue();
    sfSetAnalogSwitchBit(getHw(), CHANNEL_B_GROUND, m_checkBoxCh1Ground->GetValue());
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_checkBoxSignal2OnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.channel02.OscOnOff = m_checkBoxSignal2->IsChecked() ? 1 : 0;
}

void OsciloskopOsciloskop::m_checkBoxFFT2OnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.channel02.FFTOnOff = m_checkBoxFFT2->IsChecked() ? 1 : 0;
}

void OsciloskopOsciloskop::m_checkBoxAvg02OnCheckBox(wxCommandEvent& event)
{
    int avg = sfGetAverage(getHw());
    if(m_checkBoxAvg02->IsChecked()) { avg |=  256; }
    else                             { avg &= ~256; }
    sfSetAverage(getHw(), avg);
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_textCtrlCh1PositionOnTextEnter(wxCommandEvent& event)
{
    float time = pOsciloscope->window.horizontal.Capture;
    float capture = pOsciloscope->window.channel02.Capture;
    // voltage
    pOsciloscope->window.channel02.YPosition = pFormat->stringToDouble(m_textCtrlCh1Position->GetValue().ToAscii().data());
    // step
    double steps = double(pOsciloscope->window.channel02.YPosition) / pOsciloscope->settings.getHardware()->getAnalogStep(time, 1, capture);
    sfSetYPositionB(getHw(), steps + pOsciloscope->settings.getHardware()->getAnalogOffset(time, 1, capture));
    m_sliderCh1Position->SetValue(-steps);
    // transfer
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_spinBtnCh1YPosOnSpinDown(wxSpinEvent& event)
{
    // position
    int ypos = sfGetYPositionB(getHw());
    sfSetYPositionB(getHw(), ypos + 1);
    pOsciloscope->transferData();
    // ypos, step and offset
    ypos = sfGetYPositionB(getHw());
    float  time    = pOsciloscope->window.horizontal.Capture;
    float  capture = pOsciloscope->window.channel02.Capture;
    double step    = pOsciloscope->settings.getHardware()->getAnalogStep(time, 1, capture);
    int    offset  = pOsciloscope->settings.getHardware()->getAnalogOffset(time, 1, capture);
    // update ui
    m_sliderCh1Position->SetValue(-(ypos - offset));
    m_textCtrlCh1Position->SetValue(pFormat->doubleToString(step * double(ypos - offset)));
}

void OsciloskopOsciloskop::m_spinBtnCh1YPosOnSpinUp(wxSpinEvent& event)
{
    // position
    int ypos = sfGetYPositionB(getHw());
    sfSetYPositionB(getHw(), ypos - 1);
    pOsciloscope->transferData();
    // ypos, step and offset
    ypos = sfGetYPositionB(getHw());
    float  time    = pOsciloscope->window.horizontal.Capture;
    float  capture = pOsciloscope->window.channel02.Capture;
    double step    = pOsciloscope->settings.getHardware()->getAnalogStep(time, 1, capture);
    int    offset  = pOsciloscope->settings.getHardware()->getAnalogOffset(time, 1, capture);
    // update ui
    m_sliderCh1Position->SetValue(-(ypos - offset));
    m_textCtrlCh1Position->SetValue(pFormat->doubleToString(step * double(ypos - offset)));
}

void OsciloskopOsciloskop::m_sliderCh1PositionOnScroll(wxScrollEvent& event)
{
    float time = pOsciloscope->window.horizontal.Capture;
    float capture = pOsciloscope->window.channel02.Capture;
    // steps
    sfSetYPositionB(getHw(), -m_sliderCh1Position->GetValue() + pOsciloscope->settings.getHardware()->getAnalogOffset(time, 1, capture));
    pOsciloscope->transferData();
    // voltage
    pOsciloscope->window.channel02.YPosition = double(-m_sliderCh1Position->GetValue()) * pOsciloscope->settings.getHardware()->getAnalogStep(time, 1, capture);
    m_textCtrlCh1Position->SetValue(pFormat->doubleToString(pOsciloscope->window.channel02.YPosition));
}

void OsciloskopOsciloskop::m_comboBoxFunctionTypeOnChoice(wxCommandEvent& event)
{
    pOsciloscope->window.function.Type = m_comboBoxFunction->GetSelection();
}

void OsciloskopOsciloskop::m_checkBoxXYVoltageGraphOnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.function.xyGraph = m_checkBoxXYVoltageGraph->IsChecked() ? 1 : 0;
}

void OsciloskopOsciloskop::m_checkBoxSignalFOnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.function.OscOnOff = m_checkBoxSignalF->IsChecked() ? 1 : 0;
}

void OsciloskopOsciloskop::m_checkBoxFFTFOnCheckBox(wxCommandEvent& event)
{
    pOsciloscope->window.function.FFTOnOff = m_checkBoxFFTF->IsChecked() ? 1 : 0;
}

void OsciloskopOsciloskop::m_comboBoxTriggerOnCombobox(wxCommandEvent& event)
{
    pOsciloscope->window.trigger.Mode = m_comboBoxTrigger->GetSelection();
    sfSetTriggerMode(getHw(), m_comboBoxTrigger->GetSelection());
    if(m_comboBoxTrigger->GetSelection() == 3)
    {
        pOsciloscope->window.trigger.Percent  = 0.f;
        sfSetTriggerPre(getHw(), pOsciloscope->window.trigger.Percent);
        m_textCtrlTriggerPre->SetValue(wxString::FromAscii(pFormat->floatToString(pOsciloscope->window.trigger.Percent)));
        m_sliderTriggerPre->SetValue(pOsciloscope->window.trigger.Percent);
        m_textCtrlTriggerPre->Disable();
        m_sliderTriggerPre->Disable();
    }
    else
    {
        m_textCtrlTriggerPre->Enable();
        m_sliderTriggerPre->Enable();
    }
    pOsciloscope->transferData();

    if (pOsciloscope->window.trigger.Mode == 2)
       m_buttonReArm->Enable();
    else
       m_buttonReArm->Disable();
}

void OsciloskopOsciloskop::m_comboBoxTriggerSourceOnCombobox(wxCommandEvent& event)
{
    double oldTriggerVoltagePerStep = pOsciloscope->getTriggerVoltagePerStep();
    pOsciloscope->window.trigger.Source = m_comboBoxTriggerSource->GetSelection();
    sfSetTriggerSource(getHw(), m_comboBoxTriggerSource->GetSelection());
    double newTriggerVoltagePerStep = pOsciloscope->getTriggerVoltagePerStep();
    RecalculateTriggerPosition(oldTriggerVoltagePerStep, newTriggerVoltagePerStep);
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_comboBoxTriggerSlopeOnCombobox(wxCommandEvent& event)
{
    pOsciloscope->window.trigger.Slope = m_comboBoxTriggerSlope->GetSelection();
    sfSetTriggerSlope(getHw(), m_comboBoxTriggerSlope->GetSelection());
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::m_buttonReArmOnButtonClick(wxCommandEvent& event)
{
    sfSetTriggerReArm(getHw(), true);
    pOsciloscope->transferData();
    sfSetTriggerReArm(getHw(), false);
}

void OsciloskopOsciloskop::m_textCtrlTriggerLevelOnTextEnter(wxCommandEvent& event)
{
    double  triggerLevel = pFormat->stringToDouble(m_textCtrlTriggerLevel->GetValue().ToAscii().data());
    int    numberOfSteps = int(triggerLevel / pOsciloscope->getTriggerVoltagePerStep());
    pOsciloscope->window.trigger.Level = numberOfSteps;
    sfSetTriggerLevel(getHw(), numberOfSteps);
    pOsciloscope->transferData();
    m_sliderTriggerLevel->SetValue(numberOfSteps);
}

void OsciloskopOsciloskop::m_spinBtnTrigLevelOnSpinDown(wxSpinEvent& event)
{
    double  triggerLevel = pFormat->stringToDouble(m_textCtrlTriggerLevel->GetValue().ToAscii().data());
    triggerLevel = triggerLevel - pOsciloscope->getTriggerVoltagePerStep();
    m_textCtrlTriggerLevel->SetValue(pFormat->doubleToString(triggerLevel));
    int    numberOfSteps = int(triggerLevel / pOsciloscope->getTriggerVoltagePerStep());
    pOsciloscope->window.trigger.Level = numberOfSteps;
    sfSetTriggerLevel(getHw(), numberOfSteps);
    pOsciloscope->transferData();
    m_sliderTriggerLevel->SetValue(numberOfSteps);
}

void OsciloskopOsciloskop::m_spinBtnTrigLevelOnSpinUp(wxSpinEvent& event)
{
    double  triggerLevel = pFormat->stringToDouble(m_textCtrlTriggerLevel->GetValue().ToAscii().data());
    triggerLevel = triggerLevel + pOsciloscope->getTriggerVoltagePerStep();
    m_textCtrlTriggerLevel->SetValue(pFormat->doubleToString(triggerLevel));
    int    numberOfSteps = int(triggerLevel / pOsciloscope->getTriggerVoltagePerStep());
    pOsciloscope->window.trigger.Level = numberOfSteps;
    sfSetTriggerLevel(getHw(), numberOfSteps);
    pOsciloscope->transferData();
    m_sliderTriggerLevel->SetValue(numberOfSteps);
}

void OsciloskopOsciloskop::m_sliderTriggerLevelOnScroll(wxScrollEvent& event)
{
    pOsciloscope->window.trigger.Level = m_sliderTriggerLevel->GetValue();
    sfSetTriggerLevel(getHw(), m_sliderTriggerLevel->GetValue());
    pOsciloscope->transferData();
    double triggerLevel = pOsciloscope->getTriggerVoltagePerStep() * double(pOsciloscope->window.trigger.Level);
    m_textCtrlTriggerLevel->SetValue(wxString::FromAscii(pFormat->doubleToString(triggerLevel)));
}

void OsciloskopOsciloskop::m_textCtrlTriggerHisteresisOnTextEnter(wxCommandEvent& event)
{
    double  hisLevel = pFormat->stringToDouble(m_textCtrlTriggerHisteresis->GetValue().ToAscii().data());
    int    numberOfSteps = int(hisLevel / pOsciloscope->getTriggerVoltagePerStep());
    pOsciloscope->window.trigger.His = numberOfSteps;
    sfSetTriggerHis(getHw(), numberOfSteps);
    pOsciloscope->transferData();
    //
    m_sliderTriggerHisteresis->SetValue(numberOfSteps);
}

void OsciloskopOsciloskop::m_spinBtnTrigHisOnSpinDown(wxSpinEvent& event)
{
    double  hisLevel = pFormat->stringToDouble(m_textCtrlTriggerHisteresis->GetValue().ToAscii().data());
    hisLevel = hisLevel - pOsciloscope->getTriggerVoltagePerStep();
    m_textCtrlTriggerHisteresis->SetValue(pFormat->doubleToString(hisLevel));
    int    numberOfSteps = int(hisLevel / pOsciloscope->getTriggerVoltagePerStep());
    pOsciloscope->window.trigger.His = numberOfSteps;
    sfSetTriggerHis(getHw(), numberOfSteps);
    pOsciloscope->transferData();
    //
    m_sliderTriggerHisteresis->SetValue(numberOfSteps);
}

void OsciloskopOsciloskop::m_spinBtnTrigHisOnSpinUp(wxSpinEvent& event)
{
    double  hisLevel = pFormat->stringToDouble(m_textCtrlTriggerHisteresis->GetValue().ToAscii().data());
    hisLevel = hisLevel + pOsciloscope->getTriggerVoltagePerStep();
    m_textCtrlTriggerHisteresis->SetValue(pFormat->doubleToString(hisLevel));
    int    numberOfSteps = int(hisLevel / pOsciloscope->getTriggerVoltagePerStep());
    pOsciloscope->window.trigger.His = numberOfSteps;
    sfSetTriggerHis(getHw(), numberOfSteps);
    pOsciloscope->transferData();
    //
    m_sliderTriggerHisteresis->SetValue(numberOfSteps);
}

void OsciloskopOsciloskop::m_sliderTriggerHisteresisOnScroll(wxScrollEvent& event)
{
    pOsciloscope->window.trigger.His = m_sliderTriggerHisteresis->GetValue();
    sfSetTriggerHis(getHw(), m_sliderTriggerHisteresis->GetValue());
    pOsciloscope->transferData();
    double hisLevel = pOsciloscope->getTriggerVoltagePerStep() * double(pOsciloscope->window.trigger.His);
    m_textCtrlTriggerHisteresis->SetValue(wxString::FromAscii(pFormat->doubleToString(hisLevel)));
}

void OsciloskopOsciloskop::m_textCtrlTriggerPreOnTextEnter(wxCommandEvent& event)
{
    double preTrigger = max(pFormat->stringToFloat(m_textCtrlTriggerPre->GetValue().ToAscii().data()), 0.f);
    preTrigger = min(preTrigger, 99.0);
    pOsciloscope->window.trigger.Percent = preTrigger;
    sfSetTriggerPre(getHw(), preTrigger);
    pOsciloscope->transferData();
    pOsciloscope->window.measure.data.pickX0.updateUI = true;
    pOsciloscope->window.measure.data.pickX1.updateUI = true;
    m_sliderTriggerPre->SetValue(pOsciloscope->window.trigger.Percent);
    m_textCtrlTriggerPre->SetValue(wxString::FromDouble(preTrigger));
}

void OsciloskopOsciloskop::m_spinBtnTrigPreOnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.trigger.Percent = max(pFormat->stringToFloat(m_textCtrlTriggerPre->GetValue().ToAscii().data()) - 1.0f, 0.f);
    pOsciloscope->window.trigger.Percent = min(pOsciloscope->window.trigger.Percent, 99.0);
    sfSetTriggerPre(getHw(), pFormat->stringToFloat(m_textCtrlTriggerPre->GetValue().ToAscii().data()));
    pOsciloscope->transferData();
    m_sliderTriggerPre->SetValue(pOsciloscope->window.trigger.Percent);
    m_textCtrlTriggerPre->SetValue(pFormat->floatToString(pOsciloscope->window.trigger.Percent));
}

void OsciloskopOsciloskop::m_spinBtnTrigPreOnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.trigger.Percent = max(pFormat->stringToFloat(m_textCtrlTriggerPre->GetValue().ToAscii().data()) + 1.0f, 0.f);
    pOsciloscope->window.trigger.Percent = min(pOsciloscope->window.trigger.Percent, 99.0);
    sfSetTriggerPre(getHw(), pFormat->stringToFloat(m_textCtrlTriggerPre->GetValue().ToAscii().data()));
    pOsciloscope->transferData();
    m_sliderTriggerPre->SetValue(pOsciloscope->window.trigger.Percent);
    m_textCtrlTriggerPre->SetValue(pFormat->floatToString(pOsciloscope->window.trigger.Percent));
}

void OsciloskopOsciloskop::m_sliderTriggerPreOnScroll(wxScrollEvent& event)
{
    pOsciloscope->window.trigger.Percent = max<int>(m_sliderTriggerPre->GetValue(), 0);
    sfSetTriggerPre(getHw(), m_sliderTriggerPre->GetValue());
    pOsciloscope->transferData();
    pOsciloscope->window.measure.data.pickX0.updateUI = true;
    pOsciloscope->window.measure.data.pickX1.updateUI = true;
    m_textCtrlTriggerPre->SetValue(wxString::FromAscii(pFormat->floatToString(pOsciloscope->window.trigger.Percent)));
}

void OsciloskopOsciloskop::m_textCtrlTriggerHoldoffOnTextEnter(wxCommandEvent& event)
{
    uint newHoldOff = max<int>(0, pFormat->stringToInteger(m_textCtrlTriggerHoldoff->GetValue().ToAscii().data()));
    sfSetHoldoff(getHw(), newHoldOff);
    newHoldOff = sfGetHoldoff(getHw());
    pOsciloscope->window.trigger.Holdoff = newHoldOff;
    pOsciloscope->transferData();
    m_sliderTriggerHoldoff->SetValue(newHoldOff);
    m_textCtrlTriggerHoldoff->SetValue(wxString::FromAscii(pFormat->integerToString(newHoldOff)));
}

void OsciloskopOsciloskop::m_spinBtnTrigHoldoffOnSpinDown(wxSpinEvent& event)
{
    uint newHoldOff = max<int>(0, sfGetHoldoff(getHw()) - 1);
    sfSetHoldoff(getHw(), newHoldOff);
    newHoldOff = sfGetHoldoff(getHw());
    pOsciloscope->window.trigger.Holdoff = newHoldOff;
    pOsciloscope->transferData();
    m_sliderTriggerHoldoff->SetValue(newHoldOff);
    m_textCtrlTriggerHoldoff->SetValue(pFormat->integerToString(newHoldOff));
}

void OsciloskopOsciloskop::m_spinBtnTrigHoldoffOnSpinUp(wxSpinEvent& event)
{
    uint newHoldOff = max<int>(0, sfGetHoldoff(getHw()) + 1);
    sfSetHoldoff(getHw(), newHoldOff);
    newHoldOff = sfGetHoldoff(getHw());
    pOsciloscope->window.trigger.Holdoff = newHoldOff;
    pOsciloscope->transferData();
    m_sliderTriggerHoldoff->SetValue(newHoldOff);
    m_textCtrlTriggerHoldoff->SetValue(pFormat->integerToString(newHoldOff));
}

void OsciloskopOsciloskop::m_sliderTriggerHoldoffOnScroll(wxScrollEvent& event)
{
    uint newHoldOff = max(0, m_sliderTriggerHoldoff->GetValue());
    sfSetHoldoff(getHw(), newHoldOff);
    newHoldOff = sfGetHoldoff(getHw());
    pOsciloscope->window.trigger.Holdoff = newHoldOff;
    pOsciloscope->transferData();
    m_sliderTriggerHoldoff->SetValue(newHoldOff);
    m_textCtrlTriggerHoldoff->SetValue(wxString::FromAscii(pFormat->integerToString(newHoldOff)));
}


