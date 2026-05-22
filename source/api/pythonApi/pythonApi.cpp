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
        .def_readwrite("generatorType0",        &SHardware::generatorType0)
        .def_readwrite("generatorVoltage0",     &SHardware::generatorVoltage0)
        .def_readwrite("generatorOffset0",      &SHardware::generatorOffset0)
        .def_readwrite("generatorDeltaH0",      &SHardware::generatorDeltaH0)
        .def_readwrite("generatorDeltaL0",      &SHardware::generatorDeltaL0)
        .def_readwrite("generatorSquareDuty0",  &SHardware::generatorSquareDuty0)
        .def_readwrite("generatorType1",        &SHardware::generatorType1)
        .def_readwrite("generatorVoltage1",     &SHardware::generatorVoltage1)
        .def_readwrite("generatorOffset1",      &SHardware::generatorOffset1)
        .def_readwrite("generatorDeltaH1",      &SHardware::generatorDeltaH1)
        .def_readwrite("generatorDeltaL1",      &SHardware::generatorDeltaL1)
        .def_readwrite("generatorSquareDuty1",  &SHardware::generatorSquareDuty1)
        .def_readwrite("digitalPattern1a",      &SHardware::digitalPattern1a)
        .def_readwrite("digitalPattern1b",      &SHardware::digitalPattern1b)
        .def_readwrite("digitalMask1",          &SHardware::digitalMask1)
        .def_readwrite("digitalPattern2a",      &SHardware::digitalPattern2a)
        .def_readwrite("digitalPattern2b",      &SHardware::digitalPattern2b)
        .def_readwrite("digitalMask2",          &SHardware::digitalMask2)
        .def_readwrite("digitalPattern3a",      &SHardware::digitalPattern3a)
        .def_readwrite("digitalPattern3b",      &SHardware::digitalPattern3b)
        .def_readwrite("digitalMask3",          &SHardware::digitalMask3)
        .def_readwrite("digitalPattern4a",      &SHardware::digitalPattern4a)
        .def_readwrite("digitalPattern4b",      &SHardware::digitalPattern4b)
        .def_readwrite("digitalMask4",          &SHardware::digitalMask4)
        .def_readwrite("dt_delayMaxcnt1",       &SHardware::dt_delayMaxcnt1)
        .def_readwrite("dt_delayMaxcnt2",       &SHardware::dt_delayMaxcnt2)
        .def_readwrite("dt_delayMaxcnt3",       &SHardware::dt_delayMaxcnt3)
        .def_readwrite("dt_delayMaxcnt4",       &SHardware::dt_delayMaxcnt4)
        .def_readwrite("dt_control",            &SHardware::dt_control)
        .def_readwrite("digitalVoltage",        &SHardware::digitalVoltage)
        .def_readwrite("digitalInputOutput",    &SHardware::digitalInputOutput)
        .def_readwrite("digitalOutputBit",      &SHardware::digitalOutputBit)
        .def_readwrite("digitalOutputMask",     &SHardware::digitalOutputMask)
        .def_readwrite("digitalClkDivideH",     &SHardware::digitalClkDivideH)
        .def_readwrite("digitalClkDivideL",     &SHardware::digitalClkDivideL)
        .def_readwrite("average",               &SHardware::average)
        .def_readwrite("preTriggerH",           &SHardware::preTriggerH)
        .def_readwrite("preTriggerL",           &SHardware::preTriggerL)
        .def_readwrite("generatorPhase0",       &SHardware::generatorPhase0)
        .def_readwrite("generatorPhase1",       &SHardware::generatorPhase1)
        .def_readwrite("reserved3",             &SHardware::reserved3)
        .def_readwrite("reserved4",             &SHardware::reserved4)
        .def_readwrite("reserved5",             &SHardware::reserved5);
    //SHardware

    py::class_<SFrameHeader>(m, "SFrameHeader")
        .def(py::init<>())
        .def_readwrite("magic",                      &SFrameHeader::magic)
        .def_readwrite("deviceTemp",                 &SFrameHeader::deviceTemp)
        .def_readwrite("etsDelay",                   &SFrameHeader::etsDelay)
        .def_readwrite("digitalPatternCompleteCnt",  &SFrameHeader::digitalPatternCompleteCnt)
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

    py::class_<SGeneratorConfig>(m, "SGeneratorConfig")
        .def(py::init<>())
        .def_readwrite("GeneratorCustomDataMap",  &SGeneratorConfig::GeneratorCustomDataMap)
        .def_readwrite("GeneratorReserved02",     &SGeneratorConfig::GeneratorReserved02)
        .def_readwrite("GeneratorReserved03",     &SGeneratorConfig::GeneratorReserved03)
        .def_readwrite("GeneratorReserved04",     &SGeneratorConfig::GeneratorReserved04)
        .def_readwrite("GeneratorReserved05",     &SGeneratorConfig::GeneratorReserved05)
        .def_readwrite("GeneratorReserved06",     &SGeneratorConfig::GeneratorReserved06)
        .def_readwrite("GeneratorReserved07",     &SGeneratorConfig::GeneratorReserved07)
        .def_readwrite("GeneratorReserved08",     &SGeneratorConfig::GeneratorReserved08)
        .def_readwrite("GeneratorReserved09",     &SGeneratorConfig::GeneratorReserved09)
        .def_readwrite("GeneratorReserved10",     &SGeneratorConfig::GeneratorReserved10)
        .def_readwrite("GeneratorReserved11",     &SGeneratorConfig::GeneratorReserved11)
        .def_readwrite("GeneratorReserved12",     &SGeneratorConfig::GeneratorReserved12)
        .def_readwrite("GeneratorReserved13",     &SGeneratorConfig::GeneratorReserved13)
        .def_readwrite("GeneratorReserved14",     &SGeneratorConfig::GeneratorReserved14)
        .def_readwrite("GeneratorReserved15",     &SGeneratorConfig::GeneratorReserved15)
        .def_readwrite("GeneratorReserved16",     &SGeneratorConfig::GeneratorReserved16)
        .def_readwrite("GeneratorReserved17",     &SGeneratorConfig::GeneratorReserved17)
        .def_readwrite("GeneratorReserved18",     &SGeneratorConfig::GeneratorReserved18)
        .def_readwrite("GeneratorReserved19",     &SGeneratorConfig::GeneratorReserved19)
        .def_readwrite("GeneratorReserved20",     &SGeneratorConfig::GeneratorReserved20)
        .def_readwrite("GeneratorReserved21",     &SGeneratorConfig::GeneratorReserved21)
        .def_readwrite("GeneratorReserved22",     &SGeneratorConfig::GeneratorReserved22)
        .def_readwrite("GeneratorReserved23",     &SGeneratorConfig::GeneratorReserved23)
        .def_readwrite("GeneratorReserved24",     &SGeneratorConfig::GeneratorReserved24)
        .def_readwrite("GeneratorReserved25",     &SGeneratorConfig::GeneratorReserved25)
        .def_readwrite("GeneratorReserved26",     &SGeneratorConfig::GeneratorReserved26)
        .def_readwrite("GeneratorReserved27",     &SGeneratorConfig::GeneratorReserved27)
        .def_readwrite("GeneratorReserved28",     &SGeneratorConfig::GeneratorReserved28)
        .def_readwrite("GeneratorReserved29",     &SGeneratorConfig::GeneratorReserved29)
        .def_readwrite("GeneratorReserved30",     &SGeneratorConfig::GeneratorReserved30)
        .def_readwrite("GeneratorReserved31",     &SGeneratorConfig::GeneratorReserved31)
        .def_readwrite("GeneratorReserved32",     &SGeneratorConfig::GeneratorReserved32);
    //

    py::class_<SGeneratorData>(m, "SGeneratorData")
        .def(py::init<>())
        .def_readwrite("type",   &SGeneratorData::type)      //AWG1, AWG2, Digital
        .def_readwrite("length", &SGeneratorData::length)    //Number of custom samples
        .def_readwrite("data",   &SGeneratorData::data);     //samples values
    //SGeneratorData; */

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
        .def_readwrite("ets",      &SDisplay::ets)
        .def_readwrite("attr",     &SDisplay::attr)
        .def_readwrite("digitalPatternCompleteCnt",  &SDisplay::digitalPatternCompleteCnt);
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
        .def_readwrite("generatorCount", &SFContext::generatorCount)
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

    py::class_<SArrayETS>(m, "SArrayETS")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayETS& o = obj.cast<SArrayETS&>();
                return py::array{4, o.bytes, obj};
        });
    //SArrayETS

    py::class_<SArrayDigPatternCompleteCnt>(m, "SArrayDigPatternCompleteCnt")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayDigPatternCompleteCnt& o = obj.cast<SArrayDigPatternCompleteCnt&>();
                return py::array{4, o.bytes, obj};
        });
    //SArrayDigPatternCompleteCnt

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

    py::class_<SArrayGeneratorData>(m, "SArrayGeneratorData")
        .def(py::init())
        .def_property_readonly("bytes", [](py::object& obj) {
            SArrayGeneratorData& o = obj.cast<SArrayGeneratorData&>();
                return py::array{SCOPEFUN_GENERATOR, o.bytes, obj};
        });
    //SArrayGeneratorData

    m.def("sfCreateSFloat",           &sfCreateSFloat);
    m.def("sfCreateSInt",             &sfCreateSInt);
    m.def("sfCreateSFContext",        &sfCreateSFContext);
    m.def("sfCreateSDisplay",         &sfCreateSDisplay);
    m.def("sfCreateSUsb",             &sfCreateSUsb);
    m.def("sfCreateSHardware",        &sfCreateSHardware);
    m.def("sfCreateSFx3",             &sfCreateSFx3);
    m.def("sfCreateSFpga",            &sfCreateSFpga);
    m.def("sfCreateSGeneratorConfig", &sfCreateSGeneratorConfig);
    m.def("sfCreateSGeneratorData",   &sfCreateSGeneratorData);
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
    m.def("sfDeleteSGeneratorConfig",  &sfDeleteSGeneratorConfig,
          py::arg("SGeneratorConfig"));
    m.def("sfDeleteSGeneratorData",    &sfDeleteSGeneratorData,
          py::arg("SGeneratorData"));
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

    m.def("sfHardwareUploadGenerator", &sfHardwareUploadGenerator, "Load generator configuration and custom data samples",
          py::arg("ctx"),
          py::arg("genConfig"),
          py::arg("genData"));

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

    m.def("sfGetHeaderEts", &sfGetHeaderEts, "Read ETS delay from header buffer",
          py::arg("header"),
          py::arg("ets"));

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

    m.def("sfSetEts", &sfSetEts, "Set ETS enabled bit in SHardware struct",
          py::arg("hw"),
          py::arg("enable"));

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

    m.def("sfSetGeneratorType0", &sfSetGeneratorType0, "Set AWG1 type in SHardware struct",
          py::arg("hw"),
          py::arg("type"));

    m.def("sfSetGeneratorOn0", &sfSetGeneratorOn0, "Set AWG1 on/off bit in SHardware struct",
          py::arg("hw"),
          py::arg("onoff"));

    m.def("sfSetGeneratorVoltage0", &sfSetGeneratorVoltage0, "Set AWG1 amplitude in SHardware struct",
          py::arg("hw"),
          py::arg("volt"));

    m.def("sfSetGeneratorOffset0", &sfSetGeneratorOffset0, "Set AWG1 offset in SHardware struct",
          py::arg("hw"),
          py::arg("offset"));

    m.def("sfSetGeneratorFrequency0", &sfSetGeneratorFrequency0, "Set AWG1 frequency in SHardware struct",
          py::arg("hw"),
          py::arg("freq"),
          py::arg("fs"));

    m.def("sfSetGeneratorSquareDuty0", &sfSetGeneratorSquareDuty0, "Set AWG1 square wave duty value in SHardware struct",
          py::arg("hw"),
          py::arg("duty"));

    m.def("sfSetGeneratorPhase0", &sfSetGeneratorPhase0, "Set AWG1 phase",
          py::arg("hw"),
          py::arg("phase"));

    m.def("sfSetGeneratorDelta0", &sfSetGeneratorDelta0, "Set AWG1 delta",
          py::arg("hw"),
          py::arg("delta"));

    /*----------------------------------------
        Generator AWG 2
    ----------------------------------------*/

    m.def("sfSetGeneratorType1", &sfSetGeneratorType1, "Set AWG2 type in SHardware struct",
          py::arg("hw"),
          py::arg("type"));

    m.def("sfSetGeneratorOn1", &sfSetGeneratorOn1, "Set AWG2 on/off bit in SHardware struct",
          py::arg("hw"),
          py::arg("onoff"));

    m.def("sfSetGeneratorVoltage1", &sfSetGeneratorVoltage1, "Set AWG2 amplitude in SHardware struct",
          py::arg("hw"),
          py::arg("volt"));

    m.def("sfSetGeneratorOffset1", &sfSetGeneratorOffset1, "Set AWG2 offset in SHardware struct",
          py::arg("hw"),
          py::arg("perc"));

    m.def("sfSetGeneratorFrequency1", &sfSetGeneratorFrequency1, "Set AWG2 frequency in SHardware struct",
          py::arg("hw"),
          py::arg("freq"),
          py::arg("fs"));

    m.def("sfSetGeneratorSquareDuty1", &sfSetGeneratorSquareDuty1, "Set AWG2 square wave duty value in SHardware struct",
          py::arg("hw"),
          py::arg("duty"));

    m.def("sfSetGeneratorPhase1", &sfSetGeneratorPhase1, "Set AWG2 phase",
          py::arg("hw"),
          py::arg("phase"));

    m.def("sfSetGeneratorDelta1", &sfSetGeneratorDelta1, "Set AWG2 delta",
          py::arg("hw"),
          py::arg("delta"));

    /*----------------------------------------
        Digital channels
    ----------------------------------------*/

    m.def("sfSetDigitalVoltage", &sfSetDigitalVoltage, "Set digital channels voltage level in SHardware struct",
          py::arg("hw"),
          py::arg("volt"),
          py::arg("kDigital"));

    m.def("sfSetDigitalInputOutput", &sfSetDigitalInputOutput, "Set digital channels direction bits in SHardware struct",
          py::arg("hw"),
          py::arg("inout15"),
          py::arg("inout7"));

    m.def("sfSetDigitalOutputBit", &sfSetDigitalOutputBit, "Set digital channels output bits in SHardware struct",
          py::arg("hw"),
          py::arg("bit"),
          py::arg("onoff"));

    m.def("sfSetDigitalClockDivide", &sfSetDigitalClockDivide, "Set digital channels clock divider in SHardware struct",
          py::arg("hw"),
          py::arg("divider"));

    m.def("sfSetDigitalPatternOutputEn", &sfSetDigitalPatternOutputEn, "Set digital pattern output enable bit in SHardware struct",
          py::arg("hw"),
          py::arg("outputEnable"));

    m.def("sfSetDigitalPatternOutputMode", &sfSetDigitalPatternOutputMode, "Set digital pattern output mode (0-loop, 1-single burst)",
          py::arg("hw"),
          py::arg("outputEnable"));

    m.def("sfSetDigitalPatternOutputRestart", &sfSetDigitalPatternOutputRestart, "Set digital pattern output restart bit in SHardware struct",
          py::arg("hw"),
          py::arg("outputRestart"));

    m.def("sfSetDigitalPatternOutputRestartOnUpload", &sfSetDigitalPatternOutputRestartOnUpload, "Set digital pattern output restartOnUpload bit in SHardware struct",
          py::arg("hw"),
          py::arg("outputRestartOnUpload"));

    /*----------------------------------------
        Digital trigger
    ----------------------------------------*/

    m.def("sfSetDigitalStart", &sfSetDigitalStart, "Set digital trigger channels start stage in SHardware struct",
          py::arg("hw"),
          py::arg("start"));

    m.def("sfSetDigitalMode", &sfSetDigitalMode, "Set digital channels trigger mode in SHardware struct (serial / parallel)",
          py::arg("hw"),
          py::arg("mode"));

    m.def("sfSetDigitalChannel", &sfSetDigitalChannel, "Select digital channels trigger source in SHardware struct",
          py::arg("hw"),
          py::arg("channel"));

    m.def("sfSetDigitalDelay", &sfSetDigitalDelay, "Select digital channels trigger delay in SHardware struct",
          py::arg("hw"),
          py::arg("stage"),
		  py::arg("delay"));

    m.def("sfSetDigitalMask", &sfSetDigitalMask, "Select digital channels trigger mask in SHardware struct",
          py::arg("hw"),
          py::arg("stage"),
          py::arg("bit"),
		  py::arg("value"));

    m.def("sfSetDigitalPattern", &sfSetDigitalPattern, "Select digital channels trigger pattern in SHardware struct",
          py::arg("hw"),
          py::arg("stage"),
          py::arg("bit"),
		  py::arg("pattern"));

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

    m.def("sfGetEts", &sfGetEts, "Read ETS bit from SHardware struct",
          py::arg("hw"));

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

    m.def("sfGetDigitalStart", &sfGetDigitalStart, "Read digital channels trigger start stage from SHardware struct",
          py::arg("hw"));

    m.def("sfGetDigitalMode", &sfGetDigitalMode, "Read digital channels trigger mode from SHardware struct",
          py::arg("hw"));

    m.def("sfGetDigitalChannel", &sfGetDigitalChannel, "Read digital channels trigger source from SHardware struct",
          py::arg("hw"));

    m.def("sfGetDigitalDelay", &sfGetDigitalDelay, "Read digital channels trigger delay from SHardware struct",
          py::arg("hw"),
          py::arg("stage"));

    m.def("sfGetDigitalMask", &sfGetDigitalMask, "Read digital channels trigger mask from SHardware struct",
          py::arg("hw"),
          py::arg("stage"),
          py::arg("bit"));

    /*----------------------------------------
        Get Generator AWG 1
    ----------------------------------------*/

    m.def("sfGetGeneratorType0", &sfGetGeneratorType0, "Read AWG1 type from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorOn0", &sfGetGeneratorOn0, "Read AWG1 enable bit from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorVoltage0", &sfGetGeneratorVoltage0, "Read AWG1 amplitude from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorOffset0", &sfGetGeneratorOffset0, "Read AWG1 offset from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorFrequency0", &sfGetGeneratorFrequency0, "Read AWG1 frequency from SHardware struct",
          py::arg("hw"),
		  py::arg("fs"));

    m.def("sfGetGeneratorSquareDuty0", &sfGetGeneratorSquareDuty0, "Read AWG1 square wave duty from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorPhase0", &sfGetGeneratorPhase0, "Read AWG1 phase value from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorDelta0", &sfGetGeneratorDelta0, "Read AWG1 delta value from SHardware struct",
          py::arg("hw"));

    /*----------------------------------------
        Get Generator AWG 2
    ----------------------------------------*/

    m.def("sfGetGeneratorType1", &sfGetGeneratorType1, "Read AWG2 type from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorOn1", &sfGetGeneratorOn1, "Read AWG2 enable bit from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorVoltage1", &sfGetGeneratorVoltage1, "Read AWG2 amplitude from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorOffset1", &sfGetGeneratorOffset1, "Read AWG2 offset from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorFrequency1", &sfGetGeneratorFrequency1, "Read AWG2 frequency from SHardware struct",
          py::arg("hw"),
		  py::arg("fs"));

    m.def("sfGetGeneratorSquareDuty1", &sfGetGeneratorSquareDuty1, "Read AWG2 square wave duty from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorPhase1", &sfGetGeneratorPhase1, "Read AWG2 phase value from SHardware struct",
          py::arg("hw"));

    m.def("sfGetGeneratorDelta1", &sfGetGeneratorDelta1, "Read AWG2 delta value from SHardware struct",
          py::arg("hw"));

    /*----------------------------------------
        Get Digital setup
    ----------------------------------------*/

    m.def("sfGetDigitalVoltage", &sfGetDigitalVoltage, "Read digital channels voltage from SHardware struct",
          py::arg("hw"),
		  py::arg("kDigital"));

    m.def("sfGetDigitalInputOutput11to6", &sfGetDigitalInputOutput11to6, "Read digital direction (channels 11 .. 6) from SHardware struct",
          py::arg("hw"));

    m.def("sfGetDigitalInputOutput5to0", &sfGetDigitalInputOutput5to0, "Read digital direction (channels 5 .. 0) from SHardware struct",
          py::arg("hw"));

    m.def("sfGetDigitalOutputBit", &sfGetDigitalOutputBit, "Read digital channels output level from SHardware struct",
          py::arg("hw"),
          py::arg("bit"));

    m.def("sfGetDigitalClockDivide", &sfGetDigitalClockDivide, "Read digital channels clock divider from SHardware struct",
          py::arg("hw"));

    m.def("sfGetDigitalPattern", &sfGetDigitalPattern, "Read digital channels trigger pattern from SHardware struct",
          py::arg("hw"),
          py::arg("stage"),
          py::arg("bit"));

    m.def("sfGetDigitalPatternOutputEn", &sfGetDigitalPatternOutputEn, "Read digital pattern output enable bit from SHardware struct",
          py::arg("hw"));

    m.def("sfGetDigitalPatternOutputMode", &sfGetDigitalPatternOutputMode, "Read digital pattern output mode (0-loop, 1-single burst)",
          py::arg("hw"));

    m.def("sfGetDigitalPatternOutputRestart", &sfGetDigitalPatternOutputRestart, "Read digital pattern output restart bit from SHardware struct",
          py::arg("hw"));

    m.def("sfGetDigitalPatternOutputRestartOnUpload", &sfGetDigitalPatternOutputRestartOnUpload, "Read digital pattern output restartOnUpload bit from SHardware struct",
          py::arg("hw"));

    m.def("sfGetAverage", &sfGetAverage, "Read averaging bit status SHardware struct",
          py::arg("hw"));

    /*----------------------------------------
        Frame Data Encoding
    ----------------------------------------*/

    m.def("sfGetDataEncodingFormat", &sfGetDataEncodingFormat, "Get samples encoding format from SHardware struct",
          py::arg("hw"));

}

