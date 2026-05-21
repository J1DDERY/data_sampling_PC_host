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
#ifndef __GUI_HEADER__
#define __GUI_HEADER__

// forward
class OsciloskopStorage;
class OsciloskopConnection;
class OsciloskopMeasure;
class OsciloskopOscoloskop;
class OsciloskopDebug;
class OsciloskopInfo;
class OsciloskopDisplay;
class OsciloskopThermal;
class wxStreamToCppTextRedirector;

#include <scopefun/ScopeFun.h>
extern "C" {
#include <api/scopefunapi.h>
}

#include <wx/filedlg.h>
#include <wx/app.h>
#include <wx/aui/auibook.h>
#include <wx/dcbuffer.h>
#include <wx/stdpaths.h>
#include <wx/app.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/clipbrd.h>
#include <wx/config.h>
#include <wx/clipbrd.h>
#include <wx/eventfilter.h>
#include <wx/app.h>

#include <version/sfversion.h>

#include <sstream>

#endif
//////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//////////////////////////////////////////////////////////////////////////////////////
