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
//==============================================================================
// app.cpp - 应用程序入口与主窗口初始化
// 功能：定义 wxWidgets 应用程序类 OscApp，处理命令行参数解析、
//       控制台分配、模块初始化、GUI创建以及程序生命周期管理。
//==============================================================================
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

// 前向声明：在 managers.cpp 中定义的初始化函数
extern void create();
extern void setup();
extern int  UpdateLicense();
extern int  runLuaScript(const char* script);


//==============================================================================
// 命令行参数描述表
// -h : 显示帮助信息
// -t : 运行测试（当前已注释）
// -s : 运行Lua脚本
// -l : 更新许可证
//==============================================================================
static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, "h",  "help",   "displays help",  wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, "t",  "test",   "run tests", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_SWITCH, "s",  "script", "run lua script", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_PARAM,  "",   "",       "",               wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "l",  "license", "license update", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_NONE },
};

//==============================================================================
// OscApp - 示波器应用程序主类
// 继承自 wxApp，管理应用程序的完整生命周期：
//   - 解析命令行参数
//   - 初始化引擎模块（create/setup）
//   - 启动管理器线程
//   - 创建GUI界面
//   - 退出时清理资源
//==============================================================================
class OscApp : public wxApp
{
    String m_luaScript;                       // 通过 -s 参数指定的Lua脚本路径
public:
    //--------------------------------------------------------------------------
    // OnInitCmdLine - 设置命令行解析器的参数描述表
    //--------------------------------------------------------------------------
    void OnInitCmdLine(wxCmdLineParser& parser)
    {
        parser.SetDesc(g_cmdLineDesc);
        parser.SetSwitchChars(wxT("-"));
    }
    //--------------------------------------------------------------------------
    // OnCmdLineParsed - 解析命令行参数后的回调
    // 处理 -l（更新许可证）和 -s（运行脚本）参数
    //--------------------------------------------------------------------------
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
    //--------------------------------------------------------------------------
    // OnInit - 应用程序初始化入口
    // 1. 分配控制台窗口（Windows），用于 printf/调试输出
    // 2. 调用 wxApp::OnInit() 完成 wxWidgets 初始化
    // 3. 调用 create() 创建所有管理器单例
    // 4. 设置当前工作目录和可执行文件路径
    // 5. 调用 setup() 配置管理器启动/更新/停止顺序
    // 6. 启动管理器线程循环
    // 7. 创建并显示GUI主窗口
    //--------------------------------------------------------------------------
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
            create();                                                           // 创建所有管理器实例
            FORMAT_BUFFER();
            FORMAT("%s/", (const char*)wxGetCwd().char_str().data());
            pFormat->setCurrentWorkingPath(formatBuffer);                       // 设置当前工作路径
            pFormat->setCurrentWorkingExe(wxStandardPaths::Get().GetExecutablePath().char_str().data());  // 设置可执行文件路径
            void setup();
            setup();                                                            // 配置管理器启动/更新/停止顺序
            // script
            pOsciloscope->m_runScript = m_luaScript;                           // 设置要运行的Lua脚本
            pManager->start();                                                  // 启动管理器主循环
            #endif
	    // used by wxConfig
            SetAppName("Oscilloscope");
            // load localization
            //int language = loadLanguageFromConfig();
            //setLocalization(language);
            //saveLanguageToConfig(language);
            // gui
            void recreateGUI(int initial = 0);
            recreateGUI(1);                                                     // 创建GUI界面（首次创建）
        }
        catch(...)
        {
            int debug = 1;
        }
        return true;
    }

    //--------------------------------------------------------------------------
    // OnIdle - 空闲事件处理（当前为空，可用于后台任务）
    //--------------------------------------------------------------------------
    void OnIdle(wxIdleEvent& event)
    {
    }

    //--------------------------------------------------------------------------
    // OnExit - 应用程序退出时的清理工作
    // 停止所有管理器线程
    //--------------------------------------------------------------------------
    virtual int OnExit() override
    {
        // stop
        pManager->stop();
        return 0;
    }
};

//==============================================================================
// 平台相关的应用程序入口宏
// Linux: 使用 wxIMPLEMENT_APP_NO_MAIN（自定义main）
// 其他:  使用 wxIMPLEMENT_APP（标准入口）
//==============================================================================
#ifdef PLATFORM_LINUX
    wxIMPLEMENT_APP_NO_MAIN(OscApp);
#else
    wxIMPLEMENT_APP(OscApp);
#endif

//==============================================================================
// Linux 平台自定义 main 函数
// 1. create()  - 创建所有管理器实例
// 2. 设置工作目录为 /usr/lib/oscilloscope/
// 3. setup()   - 配置管理器顺序
// 4. start()   - 启动管理器循环
// 5. wxEntry() - 进入 wxWidgets 事件循环
//==============================================================================
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

