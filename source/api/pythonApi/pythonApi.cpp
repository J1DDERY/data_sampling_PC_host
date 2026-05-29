////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2021 - 2024 Dejan Priveršek
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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>   // to convert C++ STL containers to python list
#include <pybind11/numpy.h> // to wrap byte arrays to NumPy arrays

extern "C" {
#include <api/scopefunapi.h>
};

#include <version/sfversion.h>
#include <string>

namespace py = pybind11;
using namespace std;

template <typename Sequence>
py::array_t<typename Sequence::value_type> as_pyarray(Sequence&& seq) {
    // Move entire object to heap (Ensure is moveable!). Memory handled via Python capsule
    Sequence* seq_ptr = new Sequence(std::move(seq));
    auto capsule = py::capsule(seq_ptr, [](void* p) { delete reinterpret_cast<Sequence*>(p); });
    return py::array(seq_ptr->size(),  // shape of array
                     seq_ptr->data(),  // c-style contiguous strides for Sequence
                     capsule           // numpy array references this parent
    );
}

/*----------------------------------------
    pythonApi utility functions
----------------------------------------*/

std::vector<short> sfGetSamplesFromBytes(int ch, SHardware SHw, py::array_t<unsigned char> rawData) {
    auto rawDataB = rawData.mutable_unchecked(); // must do this to allow access to rawData array
    const auto rawDataSize = rawData.size();

    int requiredBytes = 0;
    uint nSamples = sfGetNumSamples(&SHw);
    std::vector<short> result(nSamples);

    int encodingFormat = sfGetDataEncodingFormat(&SHw);
    switch(encodingFormat)
    {
        case 0:
        case 15:
            requiredBytes = nSamples * 4;
            break;
        case 10:
        case 9:
        case 6:
        case 5:
            requiredBytes = nSamples * 2;
            break;
        default:
            requiredBytes = nSamples * 4;
            break;
    };

    // check if there is enough rawData provided for requested number of samples
    if (rawDataSize >= requiredBytes)
    {   // loop through data
                        uint nPos = 0;

                  // encodingFormat is 4 bit wide (B3.B2.B1.B0)
                  // individual bits are used to determine which channels are encoded in raw data
                  // temporary byte variables used by 2-byte encoding branches
                  ushort byte0, byte1, byte2, byte3;

        switch(encodingFormat)
        {
            case 0:
            case 15: //encodingFormat = "1111"
            {
                for (uint i = 0; i < nSamples; i++)
                {
                    nPos = 4*i;
                              uint packed = (uint)rawDataB[nPos]
                                                | ((uint)rawDataB[nPos + 1] << 8)
                                                | ((uint)rawDataB[nPos + 2] << 16)
                                                | ((uint)rawDataB[nPos + 3] << 24);

                              ishort analogCh1 = 0;
                              ishort analogCh2 = 0;
                              ushort digital = 0;
                              sfGetData(packed, &analogCh1, &analogCh2, &digital);

                    // Output CH1
                    if (ch == 1) {
                                    result[i] = analogCh1;
                    }

                    // Output CH2
                    if (ch == 2) {
                                    result[i] = analogCh2;
                    }

                    // Output Digital
                    if (ch == 3) {
                                    result[i] = digital;
                    }
                }
                break;
            }

            case 10: // encodingFormat = "1010"  — CH1 only (14-bit), no digital
            case  9: // encodingFormat = "1001"  — CH1 only (14-bit), no digital
            {
                for (uint i = 0; i < nSamples; i++)
                {
                    nPos = 2*i;

                    // 2-byte packed CH1: byte0=bits[7:0], byte1[5:0]=bits[13:8]
                    // byte1[7:6] unused
                    byte0 = rawDataB[nPos];
                    byte1 = rawDataB[nPos+1];

                    uint ch0 = (uint)byte0 | (((uint)byte1 & 0x3F) << 8);
                    if (ch0 & (1 << 13)) ch0 |= 0xFFFFC000; // sign-extend 14→32

                    if (ch == 1) {
                        result[i] = (ishort)(ch0 & 0xFFFF);
                    }
                    if (ch == 2) {
                        return {};
                    }
                    if (ch == 3) {
                        result[i] = 0; // digital removed
                    }
                }
                break;
            }

            case 6: // encodingFormat = "0110"  — CH2 only (14-bit), no digital
            case 5: // encodingFormat = "0101"  — CH2 only (14-bit), no digital
            {
                for (uint i = 0; i < nSamples; i++)
                {
                    nPos = 2*i;

                    // 2-byte packed CH2: byte0=bits[7:0], byte1[5:0]=bits[13:8]
                    // byte1[7:6] unused
                    byte0 = rawDataB[nPos];
                    byte1 = rawDataB[nPos+1];

                    uint ch1 = (uint)byte0 | (((uint)byte1 & 0x3F) << 8);
                    if (ch1 & (1 << 13)) ch1 |= 0xFFFFC000; // sign-extend 14→32

                    if (ch == 1) {
                        return {};
                    }
                    if (ch == 2) {
                        result[i] = (ishort)(ch1 & 0xFFFF);
                    }
                    if (ch == 3) {
                        result[i] = 0; // digital removed
                    }
                }
                break;
            }

            default:
                break;
        };

    }
    else return {};
    return result;
}

/*----------------------------------------
    PYBIND11_MODULE
----------------------------------------*/

PYBIND11_MODULE(scopefun, m)
{

    std::string versionInfo = "";
    versionInfo += "ScopeFun API, v";
    versionInfo += EXE_VERSION_MAJOR;
    versionInfo += ".";
    versionInfo += EXE_VERSION_MINOR;
    versionInfo += ".";
    versionInfo += EXE_VERSION_MICRO;
    versionInfo += " ";
    versionInfo += EXE_VERSION_TIMESTAMP;
    m.doc() = versionInfo; // module docstring, __doc__

    /*----------------------------------------
        pythonApi utility functions
    ----------------------------------------*/

    m.def("sfGetSamplesFromBytes", [](int& ch, SHardware& SHw, py::array_t<unsigned char>& rawData) {
        auto result = sfGetSamplesFromBytes(ch, SHw, rawData);
        return as_pyarray(std::move(result));
    }, "Decodes samples from raw data.\nArg0 will select output channel: '1' = CH1, '2' = CH2, '3' = digital" );

    /*----------------------------------------
        Structures
    ----------------------------------------*/

    py::class_<SHardware>(m, "SHardware")
        .def(py::init<>())
        .def_readwrite("controlAddr",           &SHardware::controlAddr)
        .def_readwrite("controlData",           &SHardware::controlData)
        .def_readwrite("vgaina",                &SHardware::vgaina)
        .def_readwrite("vgainb",                &SHardware::vgainb)
        .def_readwrite("offseta",               &SHardware::offseta)
        .def_readwrite("offsetb",               &SHardware::offsetb)
        .def_readwrite("analogswitch",          &SHardware::analogswitch)
        .def_readwrite("triggerMode",           &SHardware::triggerMode)
        .def_readwrite("triggerSource",         &SHardware::triggerSource)
        .def_readwrite("triggerSlope",          &SHardware::triggerSlope)
        .def_readwrite("triggerLevel",          &SHardware::triggerLevel)
        .def_readwrite("triggerHis",            &SHardware::triggerHis)
        .def_readwrite("reserved1",             &SHardware::reserved1)
        .def_readwrite("xRange",                &SHardware::xRange)
        .def_readwrite("holdoffH",              &SHardware::holdoffH)
        .def_readwrite("holdoffL",              &SHardware::holdoffL)
        .def_readwrite("sampleSizeH",           &SHardware::sampleSizeH)
        .def_readwrite("sampleSizeL",           &SHardware::sampleSizeL)
        /* Generator and digital hardware fields removed from Python bindings */
        .def_readwrite("average",               &SHardware::average)
        .def_readwrite("preTriggerH",           &SHardware::preTriggerH)
        .def_readwrite("preTriggerL",           &SHardware::preTriggerL)
        /* generatorPhase fields removed */
        .def_readwrite("reserved3",             &SHardware::reserved3)
        .def_readwrite("reserved4",             &SHardware::reserved4)
        .def_readwrite("reserved5",             &SHardware::reserved5);
    //SHardware

    py::class_<SFrameHeader>(m, "SFrameHeader")
        .def(py::init<>())
        .def_readwrite("magic",                      &SFrameHeader::magic)
        .def_readwrite("deviceTemp",                 &SFrameHeader::deviceTemp)
        /* etsDelay and digital pattern counts removed from SFrameHeader */
        .def_readwrite("debug",                      &SFrameHeader::debug)
        .def_readwrite("paddBefore",                 &SFrameHeader::paddBefore)
        .def_readwrite("hardware",                   &SFrameHeader::hardware)
        .def_readwrite("paddAfter",                  &SFrameHeader::paddAfter)
        .def_readwrite("crc",                        &SFrameHeader::crc);
    //SFrameHeader;

    py::class_<SFrameData>(m, "SFrameData")
        .def(py::init<>())
        .def_readwrite("data", &SFrameData::data);
    //SFrameData;

    py::class_<SEeprom>(m, "SEeprom")
        .def(py::init<>())
        .def_readwrite("data", &SEeprom::data);
    //SEeprom;

    py::class_<SGUID>(m, "SGUID")
        .def(py::init<>())
        .def_readwrite("data1", &SGUID::data1)
        .def_readwrite("data2", &SGUID::data2)
        .def_readwrite("data3", &SGUID::data3)
        .def_property_readonly("data4", [](py::object& obj) {
            SGUID& o = obj.cast<SGUID&>();
                return py::array{8, o.data4, obj};
        }); //TODO: change data4 wrapping to allow writing
    //SGUID;

    py::class_<SFx3>(m, "SFx3")
        .def(py::init<>())
        .def_readwrite("size", &SFx3::size)
        .def_readwrite("data", &SFx3::data);
    //SFx3;

    py::class_<SFpga>(m, "SFpga")
        .def(py::init<>())
        .def_readwrite("size", &SFpga::size)
        .def_readwrite("data", &SFpga::data);
    //SFpga;

      /* Generator types removed from Python API */

    py::class_<SUsb>(m, "SUsb")
        .def(py::init<>())
        .def_readwrite("idVendor",   &SUsb::idVendor)
        .def_readwrite("idProduct",  &SUsb::idProduct)
        .def_readwrite("idSerial",   &SUsb::idSerial)
        .def_readwrite("timeoutEp2", &SUsb::timeoutEp2)
        .def_readwrite("timeoutEp4", &SUsb::timeoutEp4)
        .def_readwrite("timeoutEp6", &SUsb::timeoutEp6)
        .def_readwrite("xferSize",   &SUsb::xferSize)
        .def_readwrite("guid",       &SUsb::guid);
    //SUsb;

    py::class_<SDisplay>(m, "SDisplay")
        .def(py::init<>())
        .def_readwrite("analog0",  &SDisplay::analog0)
        .def_readwrite("analog1",  &SDisplay::analog1)
        .def_readwrite("digital",  &SDisplay::digital)
        .def_readwrite("fft0",     &SDisplay::fft0)
        .def_readwrite("fft1",     &SDisplay::fft1)
        .def_readwrite("samples",  &SDisplay::samples)
        .def_readwrite("captured", &SDisplay::captured)
        .def_readwrite("ppi",      &SDisplay::ppi)
      .def_readwrite("attr",     &SDisplay::attr);
    //SDisplay;

    /*----------------------------------------
    Python API - Context
    ----------------------------------------*/

    py::class_<SCtxFrame>(m, "SCtxFrame")
        .def(py::init<>())
        .def_readwrite("maxMemory", &SCtxFrame::maxMemory)
        .def_readwrite("data",      &SCtxFrame::data)
        .def_readwrite("received",  &SCtxFrame::received)
        .def_readwrite("frameSize", &SCtxFrame::frameSize);
    //SCtxFrame

    py::class_<SCtxDisplay>(m, "SCtxDisplay")
        .def(py::init<>())
        .def_readwrite("data", &SCtxDisplay::data);
    //SCtxDisplay

    py::class_<SCtxApi>(m, "SCtxApi")
        .def(py::init<>())
        .def_readwrite("version", &SCtxApi::version)
        .def_readwrite("major",   &SCtxApi::major)
        .def_readwrite("minor",   &SCtxApi::minor)
        .def_readwrite("active",  &SCtxApi::active)
        .def_readwrite("thread",  &SCtxApi::thread)
        .def_readwrite("lock",    &SCtxApi::lock)
        .def_readwrite("timeout", &SCtxApi::timeout);
    //SCtxApi

    py::class_<SFContext>(m, "SFContext")
        .def(py::init<>())
        .def_readwrite("api",            &SFContext::api)
        .def_readwrite("simulateOn",     &SFContext::simulateOn)
        .def_readwrite("simulateData",   &SFContext::simulateData)
        .def_readwrite("frame",          &SFContext::frame)
        .def_readwrite("functionType",   &SFContext::functionType)
        .def_readwrite("pCallback",      &SFContext::pCallback)
        .def_readwrite("pUserData",      &SFContext::pUserData)
        .def_readwrite("usb",            &SFContext::usb);
    //SFContext

    py::class_<SInt>(m, "SInt")
        .def(py::init<>())
        .def_readwrite("value", &SInt::value);
    //SInt

    py::class_<SFloat>(m, "SFloat")
        .def(py::init<>())
        .def_readwrite("value", &SFloat::value);
    //SFloat

    /*----------------------------------------
    Python API - Arrays
    ----------------------------------------*/

    // unsigned char array to Python NumPy array
    py::class_<SArrayMagic>(m, "SArrayMagic")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayMagic& o = obj.cast<SArrayMagic&>();
                return py::array{4, o.bytes, obj};
        });
    //SArrayMagic

    py::class_<SArrayDeviceTemp>(m, "SArrayDeviceTemp")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayDeviceTemp& o = obj.cast<SArrayDeviceTemp&>();
                return py::array{4, o.bytes, obj};
        });
    //SArrayDeviceTemp

    /* SArrayETS and SArrayDigPatternCompleteCnt removed from Python bindings */

    py::class_<SArrayDebug>(m, "SArrayDebug")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayDebug& o = obj.cast<SArrayDebug&>();
                return py::array{4, o.bytes, obj};
        });
    //SArrayDebug

    py::class_<SArrayPaddingBefore>(m, "SArrayPaddingBefore")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayPaddingBefore& o = obj.cast<SArrayPaddingBefore&>();
                return py::array{236, o.bytes, obj};
        });
    //SArrayPaddingBefore

    py::class_<SArrayHardware>(m, "SArrayHardware")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayHardware& o = obj.cast<SArrayHardware&>();
                return py::array{128, o.bytes, obj};
        });
    //SArrayHardware

    py::class_<SArrayPaddingAfter>(m, "SArrayPaddingAfter")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayPaddingAfter& o = obj.cast<SArrayPaddingAfter&>();
                return py::array{639, o.bytes, obj};
        });
    //SArrayPaddingAfter

    py::class_<SArrayCrc>(m, "SArrayCrc")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayCrc& o = obj.cast<SArrayCrc&>();
                return py::array{1, o.bytes, obj};
        });
    //SArrayCrc

    py::class_<SArrayString256>(m, "SArrayString256")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayString256& o = obj.cast<SArrayString256&>();
                return py::array{256, o.bytes, obj};
        });
    //SArrayString256

    py::class_<SArrayFrameData>(m, "SArrayFrameData")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayFrameData& o = obj.cast<SArrayFrameData&>();
                return py::array{SCOPEFUN_FRAME_MEMORY, o.bytes, obj};
        });
    //SArrayFrameData

    py::class_<SArrayEEPROM>(m, "SArrayEEPROM")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayEEPROM& o = obj.cast<SArrayEEPROM&>();
                return py::array{SCOPEFUN_EEPROM_BYTES, o.bytes, obj};
        });
    //SArrayEEPROM

    py::class_<SArrayFx3>(m, "SArrayFx3")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayFx3& o = obj.cast<SArrayFx3&>();
                return py::array{SCOPEFUN_FIRMWARE_FX3, o.bytes, obj};
        });
    //SArrayFx3

    py::class_<SArrayFpga>(m, "SArrayFpga")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayFpga& o = obj.cast<SArrayFpga&>();
                return py::array{SCOPEFUN_FIRMWARE_FPGA, o.bytes, obj};
        });
    //SArrayFpga

      /* SArrayGeneratorData removed */

    m.def("sfCreateSFloat",           &sfCreateSFloat);
    m.def("sfCreateSInt",             &sfCreateSInt);
    m.def("sfCreateSFContext",        &sfCreateSFContext);
    m.def("sfCreateSDisplay",         &sfCreateSDisplay);
    m.def("sfCreateSUsb",             &sfCreateSUsb);
    m.def("sfCreateSHardware",        &sfCreateSHardware);
    m.def("sfCreateSFx3",             &sfCreateSFx3);
    m.def("sfCreateSFpga",            &sfCreateSFpga);
      /* sfCreate SGenerator removed */
    m.def("sfCreateSEeprom",          &sfCreateSEeprom);
    m.def("sfCreateSFrameData",       &sfCreateSFrameData);
    m.def("sfCreateSFrameHeader",     &sfCreateSFrameHeader);

    m.def("sfDeleteSFloat",       &sfDeleteSFloat,
          py::arg("SFloat"));
    m.def("sfDeleteSInt",         &sfDeleteSInt,
          py::arg("SInt"));
    m.def("sfDeleteSFContext",    &sfDeleteSFContext,
          py::arg("SFContext"));
    m.def("sfDeleteSDisplay",     &sfDeleteSDisplay,
          py::arg("SDisplay"));
    m.def("sfDeleteSUsb",         &sfDeleteSUsb,
          py::arg("SUsb"));
    m.def("sfDeleteSHardware",    &sfDeleteSHardware,
          py::arg("SHardware"));
    m.def("sfDeleteSFx3",         &sfDeleteSFx3,
          py::arg("SFx3"));
    m.def("sfDeleteSFpga",        &sfDeleteSFpga,
          py::arg("SFpga"));
    /* sfDelete SGenerator removed */
    m.def("sfDeleteSEeprom",      &sfDeleteSEeprom,
          py::arg("SEeprom"));
    m.def("sfDeleteSFrameData",   &sfDeleteSFrameData,
          py::arg("SFrameData"));
    m.def("sfDeleteSFrameHeader", &sfDeleteSFrameHeader,
          py::arg("SFrameHeader"));

    /*----------------------------------------
       Initialization
    ----------------------------------------*/

    m.def("sfApiInit",          &sfApiInit,          "API Init (USB)");
    m.def("sfApiCreateContext", &sfApiCreateContext, "Create API context",
          py::arg("ctx"),
          py::arg("memory"));
    m.def("sfApiDeleteContext", &sfApiDeleteContext, "Delete API context",
          py::arg("ctx"));
    m.def("sfApiVersion",       &sfApiVersion,       "Check API version",
          py::arg("ctx"),
          py::arg("version"),
          py::arg("major"),
          py::arg("minor"));
    m.def("sfApiExit", &sfApiExit,           "API Exit (USB)");
    m.def("sfIsActive", &sfIsActive,         "Check if API is active",
          py::arg("ctx"));
    m.def("sfIsThreadSafe", &sfIsThreadSafe, "(SFContext* ctx)",
          py::arg("ctx"));
    m.def("sfSetThreadSafe", &sfSetThreadSafe, "(SFContext* ctx, int threadSafe)",
          py::arg("ctx"),
          py::arg("threadSafe"));
    m.def("sfSetActive", &sfSetActive, "(SFContext* ctx, int active)",
          py::arg("ctx"),
          py::arg("active"));
    m.def("sfSetTimeOut", &sfSetTimeOut, "(SFContext* ctx, int timeout)",
          py::arg("ctx"),
          py::arg("timeout"));
    m.def("sfGetTimeOut", &sfGetTimeOut, "(SFContext* ctx, SInt* timeout)",
          py::arg("ctx"),
          py::arg("timeout"));

    /*----------------------------------------
        hardware
    ----------------------------------------*/

    m.def("sfHardwareOpen", &sfHardwareOpen, "Open USB connection",
          py::arg("ctx"),
          py::arg("cfg"),
          py::arg("version"));

    m.def("sfHardwareReset", &sfHardwareReset, "Reset hardware",
          py::arg("ctx"));

    m.def("sfHardwareIsOpened", &sfHardwareIsOpened, "Check if USB connection is established",
          py::arg("ctx"),
          py::arg("open"));

    m.def("sfHardwareConfig", &sfHardwareConfig, "Apply configuration to hardware",
          py::arg("ctx"),
          py::arg("hw"));

    m.def("sfHardwareCapture", &sfHardwareCapture, "Read raw data from hardware",
          py::arg("ctx"),
          py::arg("buffer"),
          py::arg("len"),
          py::arg("offset"),
          py::arg("received"));

    m.def("sfHardwareUploadFx3", &sfHardwareUploadFx3, "Load firmware image to FX3 RAM",
          py::arg("ctx"),
          py::arg("fx3"));

    m.def("sfHardwareUploadFpga", &sfHardwareUploadFpga, "Load firmware image to FPGA",
          py::arg("ctx"),
          py::arg("fpga"));

      /* sfHardwareUploadGenerator removed */

    m.def("sfHardwareEepromRead", &sfHardwareEepromRead, "Read data bytes from EEPROM",
          py::arg("ctx"),
          py::arg("eeprom"),
          py::arg("size"),
          py::arg("adress"));

    m.def("sfHardwareEepromReadFirmwareID", &sfHardwareEepromReadFirmwareID, "Read FX3 firmware ID (16 byte char)",
          py::arg("ctx"),
          py::arg("eeprom"),
          py::arg("adress"));

    m.def("sfHardwareReadFpgaStatus", &sfHardwareReadFpgaStatus, "Check FPGA firmware status",
          py::arg("ctx"),
          py::arg("fpga"));

    m.def("sfHardwareEepromWrite", &sfHardwareEepromWrite, "Write to EEPROM",
          py::arg("ctx"),
          py::arg("eeprom"),
          py::arg("size"),
          py::arg("adress"));

    m.def("sfHardwareEepromErase", &sfHardwareEepromErase, "Erase EEPROM",
          py::arg("ctx"));

    m.def("sfHardwareClose", &sfHardwareClose, "Close USB connection",
          py::arg("ctx"));

    /*----------------------------------------
        Frame
    ----------------------------------------*/

    m.def("sfFrameCapture", &sfFrameCapture, "Read frame data from hardware",
          py::arg("ctx"),
          py::arg("received"),
          py::arg("frameSize"));

    m.def("sfFrameOutput", &sfFrameOutput, "Copy frame data to frame buffer",
          py::arg("ctx"),
          py::arg("buffer"),
          py::arg("len"));

    m.def("sfFrameDisplayFunction", &sfFrameDisplayFunction, "Select output for the function channel",
          py::arg("ctx"),
          py::arg("fun"));

    m.def("sfFrameDisplay", &sfFrameDisplay, "Generate data for rendering",
          py::arg("ctx"),
          py::arg("buffer"),
          py::arg("len"),
          py::arg("display"),
          py::arg("displayPos"),
          py::arg("displayZoom"));

    /*----------------------------------------
        Header
    ----------------------------------------*/

    m.def("sfGetHeader", &sfGetHeader, "Get raw header data from frame buffer",
          py::arg("ctx"),
          py::arg("frame"),
          py::arg("header"));

    m.def("sfGetHeaderHardware", &sfGetHeaderHardware, "Read hardware registers data from header buffer",
          py::arg("header"),
          py::arg("hw"));

      /* sfGetHeaderEts removed */

    m.def("sfGetHeaderTemperature", &sfGetHeaderTemperature, "Read FPGA temperature from header buffer",
          py::arg("header"),
          py::arg("temperature"));

    /*----------------------------------------
        Hardware registers ID
    ----------------------------------------*/

    m.def("sfHardwareWordCnt", &sfHardwareWordCnt, "Get number of hardware registers",
          py::arg("cnt"));

    m.def("sfHardwareWordId", &sfHardwareWordId, "Get hardware register ID",
          py::arg("index"),
          py::arg("string"));

    /*----------------------------------------
        Data
    ----------------------------------------*/

    m.def("sfGetData", &sfGetData, "Get samples from raw 32-bit data",
          py::arg("data"),
          py::arg("analogCh1"),
          py::arg("analogCh2"),
          py::arg("digital"));

    /*----------------------------------------
        Set hardware registers
    ----------------------------------------*/

    m.def("sfSetDefault", &sfSetDefault, "Set hardware registers to default values in SHardware struct",
          py::arg("hw"));

    m.def("sfSetFrameSize", &sfSetFrameSize, "Set frame size (in Bytes) in SHardware struct",
          py::arg("hw"),
          py::arg("frameSize"));

    m.def("sfSetNumSamples", &sfSetNumSamples, "Set number of samples in SHardware struct",
          py::arg("hw"),
          py::arg("numSamples"));

    m.def("sfSetAnalogSwitchBit", &sfSetAnalogSwitchBit, "Set analog switch bits in SHardware struct",
          py::arg("hw"),
          py::arg("bit"),
          py::arg("value"));

      /* sfSetEts removed */

    m.def("sfSetYRangeScaleA", &sfSetYRangeScaleA, "Set channel 1 gain in SHardware struct",
          py::arg("hw"),
          py::arg("attr"),
          py::arg("gain"));

    m.def("sfSetYPositionA", &sfSetYPositionA, "Set channel 1 offset in SHardware struct",
          py::arg("hw"),
          py::arg("pos"));

    m.def("sfSetYRangeScaleB", &sfSetYRangeScaleB, "Set channel 2 gain in SHardware struct",
          py::arg("hw"),
          py::arg("attr"),
          py::arg("gain"));

    m.def("sfSetYPositionB", &sfSetYPositionB, "Set channel 2 offset in SHardware struct",
          py::arg("hw"),
          py::arg("pos"));

    m.def("sfSetXRange", &sfSetXRange, "Set sampling interval in SHardware struct",
          py::arg("hw"),
          py::arg("range"));

    /*
    m.def("sfSetControl", &sfSetControl, "Set control register in SHardware struct",
          py::arg("hw"),
          py::arg("selected"));
    */ // TODO: use ADC register as hardware control register

    m.def("sfSetSampleSize", &sfSetSampleSize, "Set number of samples in SHardware struct",
          py::arg("hw"),
          py::arg("value"));
    // TODO: check if sfSetNumSamples already does the same

    m.def("sfSetTriggerSource", &sfSetTriggerSource, "Set trigger source in SHardware struct",
          py::arg("hw"),
          py::arg("value"));

    m.def("sfSetTriggerReArm", &sfSetTriggerReArm, "Set trigger re-arm bit in SHardware struct",
          py::arg("hw"),
          py::arg("on"));

    m.def("sfSetTriggerMode", &sfSetTriggerMode, "Set trigger mode in SHardware struct",
          py::arg("hw"),
          py::arg("value"));

    m.def("sfSetTriggerSlope", &sfSetTriggerSlope, "Set trigger slope in SHardware struct",
          py::arg("hw"),
          py::arg("value"));

    m.def("sfSetTriggerPre", &sfSetTriggerPre, "Set pre-trigger value (%) in SHardware struct",
          py::arg("hw"),
          py::arg("perc"));

    m.def("sfSetTriggerHis", &sfSetTriggerHis, "Set trigger hysteresis value in SHardware struct",
          py::arg("hw"),
          py::arg("perc"));

    m.def("sfSetTriggerLevel", &sfSetTriggerLevel, "Set trigger level in SHardware struct",
          py::arg("hw"),
          py::arg("perc"));

    m.def("sfSetHoldoff", &sfSetHoldoff, "Set holdoff value in SHardware struct",
          py::arg("hw"),
          py::arg("holdoff"));

    m.def("sfSetAverage", &sfSetAverage, "Set averaging bit in SHardware struct",
          py::arg("hw"),
          py::arg("enable"));

    /*----------------------------------------
        Generator AWG 1
    ----------------------------------------*/

    /* Generator AWG1 bindings removed */

    /*----------------------------------------
        Generator AWG 2
    ----------------------------------------*/

    /* Generator AWG2 bindings removed */

    /*----------------------------------------
        Digital channels
    ----------------------------------------*/

    /* Digital GPIO bindings removed */

    /*----------------------------------------
        Digital trigger
    ----------------------------------------*/

    /* Digital trigger bindings removed */

    /*----------------------------------------
        Frame Data Encoding
    ----------------------------------------*/

    m.def("sfSetDataEncodingFormat", &sfSetDataEncodingFormat, "Set samples encoding format in SHardware struct",
          py::arg("hw"),
		  py::arg("encoding"));

    /*----------------------------------------
        Read hardware registers
    ----------------------------------------*/

    m.def("sfGetFrameSize", &sfGetFrameSize, "Read frame size in Bytes from SHardware struct",
          py::arg("hw"));

    m.def("sfGetNumSamples", &sfGetNumSamples, "Read number of samples from SHardware struct",
          py::arg("hw"));

    m.def("sfGetAnalogSwitch", &sfGetAnalogSwitch, "Read analog switch bits from SHardware struct",
          py::arg("hw"));

      /* sfGetEts removed */

    m.def("sfGetControl", &sfGetControl, "Read hardware control value from SHardware struct",
          py::arg("hw"));

    m.def("sfGetYGainA", &sfGetYGainA, "Read analog CH1 gain value from SHardware struct",
          py::arg("hw"));

	/*
    m.def("sfGetYScaleA", &sfGetYScaleA, "Not implemented",
          py::arg("hw"));
	*/

    m.def("sfGetYPositionA", &sfGetYPositionA, "Read analog CH1 offset value from SHardware struct",
          py::arg("hw"));

    m.def("sfGetYGainB", &sfGetYGainB, "Read analog CH2 gain value from SHardware struct",
          py::arg("hw"));

	/*
    m.def("sfGetYScaleB", &sfGetYScaleB, "Not implemented",
          py::arg("hw"));
	*/

    m.def("sfGetYPositionB", &sfGetYPositionB, "Read analog CH2 offset from SHardware struct",
          py::arg("hw"));

    m.def("sfGetTriggerSource", &sfGetTriggerSource, "Read analog trigger source from SHardware struct",
          py::arg("hw"));

    m.def("sfGetTriggerMode", &sfGetTriggerMode, "Read analog trigger mode from SHardware struct",
          py::arg("hw"));

    m.def("sfGetTriggerSlope", &sfGetTriggerSlope, "Read analog trigger slope from SHardware struct",
          py::arg("hw"));

    m.def("sfGetTriggerPre", &sfGetTriggerPre, "Read pre-trigger value (%) from SHardware struct",
          py::arg("hw"));

    m.def("sfGetTriggerHis", &sfGetTriggerHis, "Read analog trigger hysteresis from SHardware struct",
          py::arg("hw"));

    m.def("sfGetTriggerLevel", &sfGetTriggerLevel, "Read analog trigger level from SHardware struct",
          py::arg("hw"));

    m.def("sfGetXRange", &sfGetXRange, "Read sampling interval from SHardware struct",
          py::arg("hw"));

    m.def("sfGetSampleSize", &sfGetSampleSize, "Read samplesize from SHardware struct",
          py::arg("hw"));

    m.def("sfGetHoldoff", &sfGetHoldoff, "Read hodoff from SHardware struct",
          py::arg("hw"));

    /* Digital getters removed */

    /*----------------------------------------
        Get Generator AWG 1
    ----------------------------------------*/

    /* Generator AWG1 getters removed */

    /*----------------------------------------
        Get Generator AWG 2
    ----------------------------------------*/

    /* Generator AWG2 getters removed */

    /*----------------------------------------
        Get Digital setup
    ----------------------------------------*/

    /* Digital getters removed */

    m.def("sfGetAverage", &sfGetAverage, "Read averaging bit status SHardware struct",
          py::arg("hw"));

    /*----------------------------------------
        Frame Data Encoding
    ----------------------------------------*/

    m.def("sfGetDataEncodingFormat", &sfGetDataEncodingFormat, "Get samples encoding format from SHardware struct",
          py::arg("hw"));

}

