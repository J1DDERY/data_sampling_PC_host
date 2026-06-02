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
#include<scopefun/ScopeFun.h>
#include<api/scopefunapi.h>

#include "wx/wxprec.h"
#include "wx/app.h"
#include "wx/dynlib.h"
#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/button.h"
#include "wx/thread.h"
#include "wx/msgdlg.h"
#include "wx/filename.h"
#include <wx/stdpaths.h>
#include <wx/wxprec.h>
#include <wx/cmdline.h>

#if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    #include "wx/msw/wrapwin.h"
    #include <windows.h>
#endif

#include <scopefun/gui/OsciloskopTools.h>

extern void create();
extern void setup();
extern int  UpdateLicense();
extern int  runLuaScript(const char* script);


static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, "h",  "help",   "displays help",  wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, "t",  "test",   "run tests", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_SWITCH, "s",  "script", "run lua script", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_PARAM,  "",   "",       "",               wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "l",  "license", "license update", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_NONE },
};

class OscApp : public wxApp
{
    String m_luaScript;
public:
    void OnInitCmdLine(wxCmdLineParser& parser)
    {
        parser.SetDesc(g_cmdLineDesc);
        parser.SetSwitchChars(wxT("-"));
    }
    bool OnCmdLineParsed(wxCmdLineParser& parser)
    {
        bool updateLicense = parser.Found(wxT("l"));
        if(updateLicense)
        { UpdateLicense(); }
        bool luaS = parser.Found(wxT("s"));
        if(luaS)
        { m_luaScript = parser.GetParam(0).data().AsChar(); }
        // todo:
        // bool runTests = parser.Found(wxT("t"));
        // if (runTests)
        //    runTests();
        return true;
    }
    bool OnInit()
    {
        try
        {
            // 分配控制台窗口以便 printf/调试输出可见
            #if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
            if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole())
            {
                FILE* fpstdout = nullptr;
                freopen_s(&fpstdout, "CONOUT$", "w", stdout);
                FILE* fpstderr = nullptr;
                freopen_s(&fpstderr, "CONOUT$", "w", stderr);
                setvbuf(stdout, NULL, _IONBF, 0);
                setvbuf(stderr, NULL, _IONBF, 0);
                printf("[ScopeFun] Console attached. Debug output enabled.\n");
            }
            #endif

            // init
            if(!wxApp::OnInit())
            {
                return false;
            }
            #ifndef PLATFORM_LINUX
            void create();
            create();
            FORMAT_BUFFER();
            FORMAT("%s/", (const char*)wxGetCwd().char_str().data());
            pFormat->setCurrentWorkingPath(formatBuffer);
            pFormat->setCurrentWorkingExe(wxStandardPaths::Get().GetExecutablePath().char_str().data());
            void setup();
            setup();
            // script
            pOsciloscope->m_runScript = m_luaScript;
            pManager->start();
            #endif
	    // used by wxConfig
            SetAppName("Oscilloscope");
            // load localization
            //int language = loadLanguageFromConfig();
            //setLocalization(language);
            //saveLanguageToConfig(language);
            // gui
            void recreateGUI(int initial = 0);
            recreateGUI(1);
        }
        catch(...)
        {
            int debug = 1;
        }
        return true;
    }

    void OnIdle(wxIdleEvent& event)
    {
    }

    virtual int OnExit() override
    {
        // stop
        pManager->stop();
        return 0;
    }
};

#ifdef PLATFORM_LINUX
    wxIMPLEMENT_APP_NO_MAIN(OscApp);
#else
    wxIMPLEMENT_APP(OscApp);
#endif

#ifdef PLATFORM_LINUX

#include <unistd.h>

int main(int argc, char** argv)
{
    // create
    create();
    // working dir
    pFormat->setCurrentWorkingPath("/usr/lib/oscilloscope/");
    // setup
    setup();
    // start
    pManager->start();
    // main loop
    wxEntry(argc, argv);
    return 0;
}
#endif

