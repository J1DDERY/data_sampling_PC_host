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
#include <scopefun/gui/OsciloskopTools.h>

////////////////////////////////////////////////////////////////////////////////
//
// Tools
//
////////////////////////////////////////////////////////////////////////////////

wxLocale* pLocalization = 0;

wxIMPLEMENT_ABSTRACT_CLASS(wxLanguageUserData, wxObject);

void OsciloskopOsciloskop::OnAutoDetectUsb()
{
    wxCommandEvent event;
    m_comboBoxCh0CaptureOnCombobox(event);
    m_comboBoxCh1CaptureOnCombobox(event);
}

int SDLCALL AutoDetectUsb(void* ptr)
{
    if (!ptr)
        return 1;

    while(true)
    {
        //TODO: AutoDetectUsb

        pOsciloscope->thread.function(EThreadApiFunction::afIsOpened);
        pOsciloscope->thread.wait();

        pOsciloscope->thread.function(EThreadApiFunction::afReadFpgaStatus);
        pOsciloscope->thread.wait();

        // usb
        if (!pOsciloscope->thread.isOpen())
        {
            pOsciloscope->thread.openUSB(pOsciloscope->settings.getHardware(), 0);
        }
        else // fpga
        {
            if (!pOsciloscope->thread.isFpga())
            {
                pOsciloscope->thread.uploadFpga(pOsciloscope->settings.getHardware());
                pOsciloscope->thread.function(EThreadApiFunction::afReadFpgaStatus);
                pOsciloscope->thread.wait();
            }
            else
            {
                if (!pOsciloscope->thread.isCalibrated())
                {
                    pOsciloscope->thread.useEepromCalibration(pOsciloscope->settings.getHardware());
                    pOsciloscope->thread.wait();
                }
                else
                {
                    pOsciloscope->UndoRedoOnOff(1);
                }
            }
        }
        SDL_Delay(100);
    }
    return 0;
}

int SDLCALL LuaStopScript(void* window)
{
   if (!window)
      return 1;

   OsciloskopDebug* debug = (OsciloskopDebug*)window;
   debug->ThreadStop();
   return 0;
}

int LuaMsg(const char* msg,void* window)
{
   if (!msg || !window)
      return 1;

   OsciloskopDebug* debug = (OsciloskopDebug*)window;
   debug->AppendText(msg);
   return 0;
}

// on Windows: if you install in Program Files folder and run as non-admin your write is restricted there
bool isFileWritable()
{
    FORMAT_BUFFER();
    FORMAT_PATH("data/startup/write.ok");
    const char* writeOk = "write ok";
    int ret = fileSaveString(formatBuffer, writeOk);
    return ret == 0 ? true : false;
}

int loadLanguageFromConfig()
{
    wxConfig config(wxApp::GetInstance()->GetAppName());
    long language = wxLANGUAGE_ENGLISH;
    if(!config.Read(wxT("wxTranslation_Language"), &language, wxLANGUAGE_UNKNOWN))
    {
        language = wxLANGUAGE_ENGLISH;
    }
    return language;
}

void saveLanguageToConfig(int id)
{
    wxConfig config(wxApp::GetInstance()->GetAppName());
    config.Write(wxT("wxTranslation_Language"), id);
    config.Flush();
}

void setLocalization(int id)
{
    if(pLocalization)
    {
        wxDELETE(pLocalization);
    }
    pLocalization = new wxLocale;
    pLocalization->Init(id, wxLOCALE_DONT_LOAD_DEFAULT);
    wxFileName fn = GetOscLangFolder();
    wxLocale::AddCatalogLookupPathPrefix(fn.GetPath());
    bool ret = pLocalization->AddCatalog(_T("oscilloscope"));
}

void setColors(wxWindow* pWindow)
{
    if(pOsciloscope->settings.getColors()->windowDefault)
    {
        return;
    }
    int frontRGBA = pOsciloscope->settings.getColors()->windowFront;
    int backRGBA = pOsciloscope->settings.getColors()->windowBack;
    wxColour frontColor;
    frontColor.SetRGBA(frontRGBA);
    wxColour backColor;
    backColor.SetRGBA(backRGBA);
    pWindow->SetForegroundColour(frontColor);
    pWindow->SetBackgroundColour(backColor);
    wxWindowList list = pWindow->GetChildren();
    for(int i = 0; i < list.GetCount(); i++)
    {
        wxWindowListNode* node = list.Item(i);
        node->GetData()->SetForegroundColour(frontColor);
        node->GetData()->SetBackgroundColour(backColor);
        setColors(node->GetData());
    }
}

void recreateGUI(int initial)
{
    // destroy current top level window
    wxApp* pApp = (wxApp*)wxApp::GetInstance();
    wxWindow* topwindow = pApp->GetTopWindow();
    /*if (topwindow)
    {
       wxArrayString list = wxTranslations::Get()->GetAvailableTranslations(_T("oscilloscope"));
       for (uint i = 0; i < (uint)list.Count(); i++)
       {
          wxLanguageInfo* info = (wxLanguageInfo*)wxLocale::FindLanguageInfo(list.Item(i));
          if (!info)
          {
             continue;
          }
          wxMenuItem* item = ((OsciloskopOsciloskop*)topwindow)->m_menu6->FindItemByPosition(i);
          ((OsciloskopOsciloskop*)topwindow)->Disconnect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OsciloskopOsciloskop::MenuLanguageSelection), 0, topwindow);
       }
       pApp->SetTopWindow(NULL);
       topwindow->Destroy();
    }*/
    // create new top level frame
    OsciloskopOsciloskop* frame = new OsciloskopOsciloskop(0);
    pApp->SetTopWindow(frame);
    frame->OnInit();
    // localization
    /*frame->m_menu6->GetMenuItems().empty();
    wxArrayString list = wxTranslations::Get()->GetAvailableTranslations(_T("oscilloscope"));
    for (uint i = 0; i < (uint)list.Count(); i++)
    {
       wxLanguageInfo* info = (wxLanguageInfo*)wxLocale::FindLanguageInfo(list.Item(i));
       if (!info)
       {
          continue;
       }
       wxMenuItem* menuItem = new wxMenuItem(frame->m_menu6, wxID_ANY, info->Description, wxEmptyString, wxITEM_NORMAL);
       frame->m_menu6->Append(menuItem);
       wxLanguageUserData* userData = new wxLanguageUserData();
       userData->data = (wxLanguage)info->Language;
       frame->Connect(menuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OsciloskopOsciloskop::MenuLanguageSelection), (wxObject*)userData, frame);
    }*/
    // set clinet size for proper border calculation
    int sx = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    int sy = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    frame->SetClientSize(45 * sx / 80, 7 * sy / 8);
    // border
    wxRect rect3 = frame->GetClientRect();
    wxRect rect4 = frame->GetScreenRect();
    int  borderX = (rect4.width - rect3.width) / 2;
    int  borderY = (rect4.height - rect3.height) / 2;
    wxPoint pt(35 * sx / 80, sy / 16);
    if(initial)
    {
        pt.x -= borderX;
        #ifdef MAC
        pt.y -= 2 * borderY;
        #endif
        #ifdef _WINDOWS
        pt.y -= borderY;
        #endif
    }
    // proper client size
    #ifdef _WINDOWS
    frame->SetClientSize(45 * sx / 80, 7 * sy / 8 - borderY + borderX);
    #endif
    // position
    frame->SetPosition(pt);
    // show window
    frame->Show(true);
    frame->Layout();
}

void LoadSlot(int slot, const char* file)
{
    slot = clamp(slot, 0, 3);
    SDL_RWops* ctx = SDL_RWFromFile(file, "rb");
    if(ctx)
    {
        SDL_RWread(ctx, &pOsciloscope->windowState[slot], 1, sizeof(WndMain));
        SDL_RWclose(ctx);
    }
}

void SaveSlot(int slot, const char* file)
{
    slot = clamp(slot, 0, 3);
    SDL_RWops* ctx = SDL_RWFromFile(file, "wb+");
    if(ctx)
    {
        SDL_RWwrite(ctx, &pOsciloscope->windowState[slot], 1, sizeof(WndMain));
        SDL_RWclose(ctx);
    }
}

wxFileName GetOscDataFolder()
{
    wxFileName fn;
    #if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    fn = wxStandardPaths::Get().GetExecutablePath();
    #endif
    #ifdef PLATFORM_MAC
    fn = wxStandardPaths::Get().GetResourcesDir();
    #endif
    #ifdef PLATFORM_LINUX
    fn = "/usr/lib/oscilloscope";
    #endif
    return fn;
}

wxFileName GetOscLangFolder()
{
    wxFileName fn;
    #if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    fn = wxStandardPaths::Get().GetExecutablePath();
    fn.AppendDir("locale");
    #endif
    #ifdef PLATFORM_MAC
    fn = wxStandardPaths::Get().GetResourcesDir();
    fn.AppendDir("Resources");
    fn.AppendDir("locale");
    #endif
    #ifdef PLATFORM_LINUX
    fn = "/usr/share/locale/";
    #endif
    return fn;
}


//class sfRenderTimer : public wxTimer
//{
//public:
//   void Notify()
//   {
//      wxWakeUpIdle();
//   }
//};
//
//wxBEGIN_EVENT_TABLE(OsciloskopOsciloskop, wxFrame)
//EVT_TIMER(TIMER_ID, OsciloskopOsciloskop::OnTimer)
//wxEND_EVENT_TABLE()


void _setYDisplay(float& volt, uint& unit, VoltageCapture selected)
{
    switch(selected)
    {
        case  vc2Volt:
            volt = 2.f;
            unit = (uint)vVolt;
            break;
        case  vc1Volt:
            volt = 1.f;
            unit = (uint)vVolt;
            break;
        case  vc500Mili:
            volt = 500.f;
            unit = (uint)vMili;
            break;
        case  vc200Mili:
            volt = 200.f;
            unit = (uint)vMili;
            break;
        case  vc100Mili:
            volt = 100.f;
            unit = (uint)vMili;
            break;
        case  vc50Mili:
            volt = 50.f;
            unit = (uint)vMili;
            break;
        case  vc20Mili:
            volt = 20.f;
            unit = (uint)vMili;
            break;
        case  vc10Mili:
            volt = 10.f;
            unit = (uint)vMili;
            break;
        default:
            CORE_ERROR("invalid volt value", 0);
    };
}

void setMask(int idx, bool value)
{
    int stage = pOsciloscope->window.trigger.stage;
    pOsciloscope->window.trigger.mask[stage][idx] = value;
    sfSetDigitalMask(getHw(), (DigitalStage)stage, (DigitalBit)idx, (DigitalPattern)pOsciloscope->window.trigger.mask[stage][idx]);
    pOsciloscope->transferData();
}

void setMaskNoTransfer(int idx, bool value)
{
    int stage = pOsciloscope->window.trigger.stage;
    pOsciloscope->window.trigger.mask[stage][idx] = value;
    sfSetDigitalMask(getHw(), (DigitalStage)stage, (DigitalBit)idx, (DigitalPattern)pOsciloscope->window.trigger.mask[stage][idx]);
}

void setPattern(int idx, wxChoice* comboBox)
{
    if(comboBox->GetSelection() == 4)
    {
        setMask(idx, false);
    }
    else
    {
        setMaskNoTransfer(idx, true);
        int stage = pOsciloscope->window.trigger.stage;
        pOsciloscope->window.trigger.pattern[stage][idx] = comboBox->GetSelection();
        sfSetDigitalPattern(getHw(), (DigitalStage)stage, (DigitalBit)idx, (DigitalPattern)pOsciloscope->window.trigger.pattern[stage][idx]);
        pOsciloscope->transferData();
    }
}

void setPatternNoTransfer(int idx, wxChoice* comboBox)
{
    if(comboBox->GetSelection() == 4)
    {
        setMaskNoTransfer(idx, false);
    }
    else
    {
        setMaskNoTransfer(idx, true);
        int stage = pOsciloscope->window.trigger.stage;
        pOsciloscope->window.trigger.pattern[stage][idx] = comboBox->GetSelection();
        sfSetDigitalPattern(getHw(), (DigitalStage)stage, (DigitalBit)idx, (DigitalPattern)pOsciloscope->window.trigger.pattern[stage][idx]);
    }
}

void maximizeOscWindow()
{
    // get top level window
    wxApp* pApp = (wxApp*)wxApp::GetInstance();
    wxFrame* topwindow = (wxFrame*)pApp->GetTopWindow();
    // window width & height
    int displayWidth = 0;
    int displayHeight = 0;
    wxDisplaySize(&displayWidth, &displayHeight);
    float osc = float(displayWidth) / (1.5f);
    float ogl = osc * 0.5f;
    // border
    wxRect rect3 = topwindow->GetClientRect();
    wxRect rect4 = topwindow->GetScreenRect();
    int  borderX = (rect4.width - rect3.width) / 2;
    int  borderY = (rect4.height - rect3.height) / 2;
    // size & pos
    topwindow->SetSize(wxSize(osc - borderX, displayHeight));
    topwindow->SetPosition(wxPoint(ogl + borderX, 0));
}

void maximizeOglWindow(int& outW, int& outH)
{
    // get top level window
    wxApp* pApp = (wxApp*)wxApp::GetInstance();
    wxFrame* topwindow = (wxFrame*)pApp->GetTopWindow();
    // size
    int displayWidth = 0;
    int displayHeight = 0;
    wxDisplaySize(&displayWidth, &displayHeight);
    float osc = float(displayWidth) / (1.5f);
    float ogl = osc * 0.5f;
    // border
    wxRect rect3 = topwindow->GetClientRect();
    wxRect rect4 = topwindow->GetScreenRect();
    int  borderX = (rect4.width - rect3.width) / 2;
    int  borderY = (rect4.height - rect3.height) / 2;
    // ogl
    outW = ogl - borderX;
    outH = displayHeight - borderY - borderX;
    SDL_SetWindowSize(pOsciloscope->sdlWindow, ogl - borderX, displayHeight - borderY - borderX);
    SDL_SetWindowPosition(pOsciloscope->sdlWindow, borderX, borderY);
}

/*
int SDLCALL PopulateFunction(void* data)
{
    ((OsciloskopMeasure*)data)->PopulateWithData();
    return 0;
}
*/

void SetupUI(WndMain& window)
{
    wxApp*         pApp = (wxApp*)wxApp::GetInstance();
    wxWindow* topwindow = pApp->GetTopWindow();
    OsciloskopOsciloskop* pOsc = (OsciloskopOsciloskop*)topwindow;
    pOsc->setupUI(window);
}

////////////////////////////////////////////////////////////////////////////////
//
// OsciloskopOsciloskop
//
////////////////////////////////////////////////////////////////////////////////

OsciloskopOsciloskop::~OsciloskopOsciloskop()
{
    m_dynamicEvents->clear();
}

void OsciloskopOsciloskop::SetDigital13To16(bool enable)
{
}

void OsciloskopOsciloskop::SaveOldSlotLoadNewSlot(int newSlot)
{
    int oldSlot = pOsciloscope->windowSlot;
    pOsciloscope->windowState[oldSlot] = pOsciloscope->window;
    pOsciloscope->window = pOsciloscope->windowState[newSlot];
}


void OsciloskopOsciloskop::loadWindow(int slot)
{
    slot = clamp(slot, 0, 3);
    pOsciloscope->window = pOsciloscope->windowState[slot];
    pOsciloscope->windowSlot = slot;
    String slotName = pOsciloscope->windowName[slot];
    if(slotName.getLength() > 32)
    {
        slotName.remove(0, slotName.getLength() - 32);
        slotName.insert(0, "...");
    }
    GetMenuBar()->GetMenu(6)->FindItemByPosition(slot)->SetItemLabel(slotName.asChar());
    loadSlot(pOsciloscope->window);
}

void OsciloskopOsciloskop::loadSlot(WndMain& slot)
{
    pOsciloscope->oscCameraSetup(pOsciloscope->window.fftDigital.is(VIEW_SELECT_OSC_3D));
    pOsciloscope->fftCameraSetup(pOsciloscope->window.fftDigital.is(VIEW_SELECT_FFT_3D));
    setupUI(slot);
    pOsciloscope->setupControl(slot);
    pOsciloscope->transferData();
}

void OsciloskopOsciloskop::setupUI(WndMain window)
{
    //TODO: SetupUI at program start
    ////////////////////////////////////////////////////////////////////////////////////////
    // horizontal
    ////////////////////////////////////////////////////////////////////////////////////////
    // capture
    m_comboBoxTimeCapture->SetSelection(captureTimeFromValue(window.horizontal.Capture));
    // display
    uint  multiEnum = multiplyerFromValue(window.horizontal.Display);
    float multiFloat = multiplyerFromEnum(multiEnum);
    // position
    m_textCtrlTimePosition->SetValue(wxString::FromAscii(pFormat->floatToString(window.horizontal.Position)));
    m_sliderTimePosition->SetValue(window.horizontal.Position);
    // control
    m_comboBoxTimeControl->SetSelection(window.horizontal.Control);
    //  FrameSize
    const char* str = pFormat->integerToString((int)window.horizontal.FrameSize);
    m_textCtrlTimeFrameSize->SetValue(wxString::FromAscii(str));
    // Frame
    int frameIndex = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameIndex);
    int frameCount = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameCount);
    m_textCtrlTimeFrame->SetValue(wxString::FromAscii(pFormat->integerToString(frameIndex%frameCount)));
    m_sliderTimeFrame->SetValue(frameIndex%frameCount);
    m_sliderTimeFrame->SetMax(frameCount);
    // FFTSize
    m_textCtrlTimeFFTSize->SetValue(wxString::FromAscii(pFormat->integerToString(window.horizontal.FFTSize)));
    // ETS
    m_checkBoxETS->SetValue(window.horizontal.ETS);
    ////////////////////////////////////////////////////////////////////////////////////////
    // channel 0
    ////////////////////////////////////////////////////////////////////////////////////////
    // Capture
    m_comboBoxCh0Capture->SetSelection(captureVoltFromValue(window.channel01.Capture));
    // Scale
    m_textCtrlCh0Scale->SetValue(wxString::FromAscii(pFormat->floatToString(window.channel01.Scale)));
    // YPosition
    m_sliderCh0Position->SetValue(window.channel01.YPosition);
    m_textCtrlCh0Position->SetValue(wxString::FromAscii(pFormat->floatToString(window.channel01.YPosition)));
    // OscOnOff
    m_checkBoxSignal1->SetValue(window.channel01.OscOnOff);
    // FFTOnOff
    m_checkBoxFFT1->SetValue(window.channel01.FFTOnOff);
    // Invert
    m_checkBoxCh0Invert->SetValue(window.channel01.Invert);
    // Ground
    m_checkBoxCh0Ground->SetValue(window.channel01.Ground);
    // AcDc
    m_choiceCh0ACDC->SetSelection(window.channel01.AcDc);
    ////////////////////////////////////////////////////////////////////////////////////////
    // channel 1
    ////////////////////////////////////////////////////////////////////////////////////////
    // Capture
    m_comboBoxCh1Capture->SetSelection(captureVoltFromValue(window.channel02.Capture));
    // Scale
    m_textCtrlCh1Scale->SetValue(wxString::FromAscii(pFormat->floatToString(window.channel02.Scale)));
    // YPosition
    m_sliderCh1Position->SetValue(window.channel02.YPosition);
    m_textCtrlCh1Position->SetValue(wxString::FromAscii(pFormat->floatToString(window.channel02.YPosition)));
    // OscOnOff
    m_checkBoxSignal2->SetValue(window.channel02.OscOnOff);
    // FFTOnOff;
    m_checkBoxFFT2->SetValue(window.channel02.FFTOnOff);
    // Invert
    m_checkBoxCh1Invert->SetValue(window.channel02.Invert);
    // Ground
    m_checkBoxCh1Ground->SetValue(window.channel02.Ground);
    // AcDc
    m_choiceCh1ACDC->SetSelection(window.channel02.AcDc);
    ////////////////////////////////////////////////////////////////////////////////////////
    // digital
    ////////////////////////////////////////////////////////////////////////////////////////
    /*
    // on/off
    m_checkBox1->SetValue(window.digital.digital[0]);
    m_checkBox2->SetValue(window.digital.digital[1]);
    m_checkBox3->SetValue(window.digital.digital[2]);
    m_checkBox4->SetValue(window.digital.digital[3]);
    m_checkBox5->SetValue(window.digital.digital[4]);
    m_checkBox6->SetValue(window.digital.digital[5]);
    m_checkBox7->SetValue(window.digital.digital[6]);
    m_checkBox8->SetValue(window.digital.digital[7]);
    m_checkBox9->SetValue(window.digital.digital[8]);
    m_checkBox10->SetValue(window.digital.digital[9]);
    m_checkBox11->SetValue(window.digital.digital[10]);
    m_checkBox12->SetValue(window.digital.digital[11]);
    // output
    m_choiceBit0->SetSelection(window.digital.output[0]);
    m_choiceBit1->SetSelection(window.digital.output[1]);
    m_choiceBit2->SetSelection(window.digital.output[2]);
    m_choiceBit3->SetSelection(window.digital.output[3]);
    m_choiceBit4->SetSelection(window.digital.output[4]);
    m_choiceBit5->SetSelection(window.digital.output[5]);
    m_choiceBit6->SetSelection(window.digital.output[6]);
    m_choiceBit7->SetSelection(window.digital.output[7]);
    m_choiceBit8->SetSelection(window.digital.output[8]);
    m_choiceBit9->SetSelection(window.digital.output[9]);
    m_choiceBit10->SetSelection(window.digital.output[10]);
    m_choiceBit11->SetSelection(window.digital.output[11]);
    */
    ////////////////////////////////////////////////////////////////////////////////////////
    // digital setup
    ////////////////////////////////////////////////////////////////////////////////////////
    // voltage
    m_textCtrlDigitalVoltage->SetValue(wxString::FromAscii(pFormat->floatToString(window.digitalSetup.voltage)));
    // divider
    m_textCtrlFreqDivider->SetValue(wxString::FromAscii(pFormat->integerToString(window.digitalSetup.divider)));
    double freq = 250000000.0 / (double(window.digitalSetup.divider) + 1.0);
    m_staticTextMhz->SetLabel(wxString::FromAscii(pFormat->doubleToString(freq)).append(wxT(" Hz")));
    //digital pattern output enable
    if (pOsciloscope->window.digitalSetup.digitalPatternOutEn) {
        m_toggleBtnDigitalPatternOutputEnable->SetValue(1);
    }
    else {
        m_toggleBtnDigitalPatternOutputEnable->SetValue(0);
    }
    // inputOutput11
    m_choiceInputOutput->SetSelection(window.digitalSetup.inputOutput11to6);
    if (m_choiceInputOutput->GetSelection() == 1)
    {
        m_choiceDS815->Disable();
        m_choiceBit6->Disable();
        m_choiceBit7->Disable();
        m_choiceBit8->Disable();
        m_choiceBit9->Disable();
        m_choiceBit10->Disable();
        m_choiceBit11->Disable();
    }
    else
    {
        m_choiceDS815->Enable();
        m_choiceBit6->Enable();
        m_choiceBit7->Enable();
        m_choiceBit8->Enable();
        m_choiceBit9->Enable();
        m_choiceBit10->Enable();
        m_choiceBit11->Enable();
    }
    // inputOutput5to0
    m_choiceInputOutput1->SetSelection(window.digitalSetup.inputOutput5to0);
    if (m_choiceInputOutput1->GetSelection() == 1)
    {
        m_choiceDS70->Disable();
        m_choiceBit0->Disable();
        m_choiceBit1->Disable();
        m_choiceBit2->Disable();
        m_choiceBit3->Disable();
        m_choiceBit4->Disable();
        m_choiceBit5->Disable();
    }
    else
    {
        m_choiceDS70->Enable();
        m_choiceBit0->Enable();
        m_choiceBit1->Enable();
        m_choiceBit2->Enable();
        m_choiceBit3->Enable();
        m_choiceBit4->Enable();
        m_choiceBit5->Enable();
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    // function
    ////////////////////////////////////////////////////////////////////////////////////////
    // xyGraph
    m_checkBoxXYVoltageGraph->SetValue(window.function.xyGraph);
    // Type
    m_comboBoxFunction->SetSelection(window.function.Type);
    // OscOnOff
    m_checkBoxSignalF->SetValue(window.function.OscOnOff);
    // FFTOnOff;
    m_checkBoxFFTF->SetValue(window.function.FFTOnOff);
    ////////////////////////////////////////////////////////////////////////////////////////
    // trigger
    ////////////////////////////////////////////////////////////////////////////////////////
    // Source
    m_comboBoxTriggerSource->SetSelection(window.trigger.Source);
    // Slope
    m_comboBoxTriggerSlope->SetSelection(window.trigger.Slope);
    // Mode
    m_comboBoxTrigger->SetSelection(window.trigger.Mode);
    // Level
    m_textCtrlTriggerLevel->SetValue(wxString::FromAscii(pFormat->floatToString(window.trigger.Level)));
    m_sliderTriggerLevel->SetValue(window.trigger.Level);
    // His
    m_textCtrlTriggerHisteresis->SetValue(wxString::FromAscii(pFormat->floatToString(window.trigger.His)));
    m_sliderTriggerHisteresis->SetValue(window.trigger.His);
    // Percent
    m_textCtrlTriggerPre->SetValue(wxString::FromAscii(pFormat->floatToString(window.trigger.Percent)));
    m_sliderTriggerPre->SetValue(window.trigger.Percent);
    // Holdoff
    m_textCtrlTriggerHoldoff->SetValue(wxString::FromAscii(pFormat->floatToString(window.trigger.Holdoff)));
    m_sliderTriggerHoldoff->SetValue(window.trigger.Holdoff);
    // stage
    m_comboBoxDigitalStage->SetSelection(window.trigger.stage);
    // stageStart
    m_comboBoxDigitalStageStart->SetSelection(window.trigger.stageStart);
    // stageMode
    m_comboBoxDigitalMode->SetSelection(window.trigger.stageMode);
    if (pOsciloscope->window.trigger.stageMode == 0) {
        m_staticText68->Disable();
        m_comboBoxDigitalSerialChannel->Disable();
    }
    else {
        m_staticText68->Enable();
        m_comboBoxDigitalSerialChannel->Enable();
    }
    // stageChannel
    m_comboBoxDigitalSerialChannel->SetSelection(window.trigger.stageChannel);
    // stage
    int istage = window.trigger.stage;
    // delay[4]
    ushort delay = window.trigger.delay[istage];
    m_textCtrDigitallDelay->SetValue(wxString::FromAscii(pFormat->integerToString(delay)));
    // pattern[4]
    m_comboBoxBit0->SetSelection(window.trigger.pattern[istage][0]);
    m_comboBoxBit1->SetSelection(window.trigger.pattern[istage][1]);
    m_comboBoxBit2->SetSelection(window.trigger.pattern[istage][2]);
    m_comboBoxBit3->SetSelection(window.trigger.pattern[istage][3]);
    m_comboBoxBit4->SetSelection(window.trigger.pattern[istage][4]);
    m_comboBoxBit5->SetSelection(window.trigger.pattern[istage][5]);
    m_comboBoxBit6->SetSelection(window.trigger.pattern[istage][6]);
    m_comboBoxBit7->SetSelection(window.trigger.pattern[istage][7]);
    m_comboBoxBit8->SetSelection(window.trigger.pattern[istage][8]);
    m_comboBoxBit9->SetSelection(window.trigger.pattern[istage][9]);
    m_comboBoxBit10->SetSelection(window.trigger.pattern[istage][10]);
    m_comboBoxBit11->SetSelection(window.trigger.pattern[istage][11]);
    // mask[4]
    if (!window.trigger.mask[istage][0])
    {
    m_comboBoxBit0->SetSelection(4);
    }
    if (!window.trigger.mask[istage][1])
    {
    m_comboBoxBit1->SetSelection(4);
    }
    if (!window.trigger.mask[istage][2])
    {
    m_comboBoxBit2->SetSelection(4);
    }
    if (!window.trigger.mask[istage][3])
    {
    m_comboBoxBit3->SetSelection(4);
    }
    if (!window.trigger.mask[istage][4])
    {
    m_comboBoxBit4->SetSelection(4);
    }
    if (!window.trigger.mask[istage][5])
    {
    m_comboBoxBit5->SetSelection(4);
    }
    if (!window.trigger.mask[istage][6])
    {
    m_comboBoxBit6->SetSelection(4);
    }
    if (!window.trigger.mask[istage][7])
    {
    m_comboBoxBit7->SetSelection(4);
    }
    if (!window.trigger.mask[istage][8])
    {
    m_comboBoxBit8->SetSelection(4);
    }
    if (!window.trigger.mask[istage][9])
    {
    m_comboBoxBit9->SetSelection(4);
    }
    if (!window.trigger.mask[istage][10])
    {
    m_comboBoxBit10->SetSelection(4);
    }
    if (!window.trigger.mask[istage][11])
    {
    m_comboBoxBit11->SetSelection(4);
    }

    if (pHardwareGenerator)
    {
    pHardwareGenerator->SetupUI();
    }

    DigitalMiddlePinMaxVoltText(pOsciloscope->window.digitalSetup.voltage);
}

void OsciloskopHardwareGenerator::SetupUI()
{
    wxInitDialogEvent evt;
    HardwareGeneratorOnInitDialog(evt);
}

int OsciloskopOsciloskop::getCurrentSlot()
{
    int index = 0;
    if(GetMenuBar()->GetMenu(6)->FindItemByPosition(0)->IsChecked())
    {
        index = 0;
    }
    if(GetMenuBar()->GetMenu(6)->FindItemByPosition(1)->IsChecked())
    {
        index = 1;
    }
    if(GetMenuBar()->GetMenu(6)->FindItemByPosition(2)->IsChecked())
    {
        index = 2;
    }
    if(GetMenuBar()->GetMenu(6)->FindItemByPosition(3)->IsChecked())
    {
        index = 3;
    }
    return index;
}


void OsciloskopOsciloskop::RecalculateTriggerPosition(double oldTriggerVoltagePerStep, double newTriggerVoltagePerStep)
{
    // trigger level
    int    newTriggerSteps = pOsciloscope->window.trigger.Level;
    double newTriggerLevel = double(pOsciloscope->window.trigger.Level) * newTriggerVoltagePerStep;
    m_sliderTriggerLevel->SetValue(newTriggerSteps);
    m_textCtrlTriggerLevel->SetValue(pFormat->doubleToString(newTriggerLevel));
    pOsciloscope->window.trigger.Level = newTriggerSteps;
    sfSetTriggerLevel(getHw(), newTriggerSteps);
    // hysteresis level
    int     hisSteps = pOsciloscope->window.trigger.His;
    double  hisLevel = double(pOsciloscope->window.trigger.His) * newTriggerVoltagePerStep;
    m_sliderTriggerHisteresis->SetValue(hisSteps);
    m_textCtrlTriggerHisteresis->SetValue(pFormat->doubleToString(hisLevel));
    pOsciloscope->window.trigger.His = hisSteps;
    sfSetTriggerHis(getHw(), hisSteps);
}

void OsciloskopOsciloskop::setTimeMode(int value)
{
    // mode
    wxCommandEvent evt;
    switch(value)
    {
        case SIGNAL_MODE_PLAY:
            m_buttonPlayOnButtonClick(evt);
            break;
        case SIGNAL_MODE_PAUSE:
            m_buttonPauseOnButtonClick(evt);
            break;
        case SIGNAL_MODE_CAPTURE:
            m_buttonCaptureOnButtonClick(evt);
            break;
        case SIGNAL_MODE_SIMULATE:
            m_buttonSimulateOnButtonClick(evt);
            break;
        case SIGNAL_MODE_CLEAR:
            m_buttonClearOnButtonClick(evt);
            break;
    };
}

void OsciloskopOsciloskop::SetButtonColors()
{
    m_buttonPlay->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
    m_buttonPlay->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
    m_buttonPause->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
    m_buttonPause->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
    m_buttonCapture->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
    m_buttonCapture->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
    m_buttonSimulate->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
    m_buttonSimulate->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
    m_buttonClear->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
    m_buttonClear->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
}


void OsciloskopOsciloskop::OnInit()
{
    userinterfaceupdate = 1;
    //    m_timer.Start(1); // 1 milisecond interval
    pStorage = new OsciloskopStorage(this);
    pConnection = new OsciloskopConnection(this);
    pMeasure = new OsciloskopMeasure(this);
    pDebug = new OsciloskopDebug(this);
    pInfo = new OsciloskopInfo(this);
    pDisplay = new OsciloskopDisplay(this);
    pThermal = new OsciloskopThermal(this);
    pSoftwareGenerator = new OsciloskopSoftwareGenerator(this);
    pHardwareGenerator = new OsciloskopHardwareGenerator(this);
    once = 1;
    pulse = 0;
    timer = 0.0;
}

void OsciloskopOsciloskop::MenuScriptSelection(wxCommandEvent& event)
{
    /* TODO: erase Lua
    OsciloscopeScript* script = (OsciloscopeScript*)(event.m_callbackUserData);
    if(script->GetUserData() == 0)
    {
        OsciloskopDebug*   pDebug = new OsciloskopDebug(this);
        script->SetUserData(pDebug);
        pDebug->AssignScript(script);
        pDebug->Show();
    }
    else
    {
        ((OsciloskopDebug*)script->GetUserData())->Show();
    }
    */
}

void OsciloskopOsciloskop::DigitalMiddlePinMaxVoltText(float voltageMax)
{
   FORMAT_BUFFER();
   FORMAT("[+%.2f]", voltageMax);
   //m_staticText109->SetLabelText(formatBuffer);
   //m_staticText126->SetLabelText(formatBuffer);
   m_staticText123->SetLabelText(formatBuffer);
}

//
//void OsciloskopOsciloskop::m_menuItemAutoCalibrateOnMenuSelection(wxCommandEvent& event)
//{
//   wxFileName fn = GetOscDataFolder();
//   wxString fileName = fn.GetPath().append(_("/data/calibrate/calibrate.slot"));
//   wxString saveName = fn.GetPath().append(_("/data/calibrate/start.slot"));
//   pOsciloscope->windowState[getCurrentSlot()] = pOsciloscope->window;
//   SaveSlot(getCurrentSlot(), saveName.ToAscii().data());
//   LoadSlot(getCurrentSlot(), fileName.ToAscii().data());
//   loadWindow(getCurrentSlot());
//   pOsciloscope->AutoCalibrate();
//}

