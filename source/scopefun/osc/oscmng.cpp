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
#include<scopefun/ScopeFun.h>


////////////////////////////////////////////////////////////////////////////////
// defines
////////////////////////////////////////////////////////////////////////////////
// #define MOUSE_DEBUG
#define MAXIMUM_HISTORY_SIZE  (4*GIGABYTE - 1)
#define MINIMUM_HISTORY_COUNT 8

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER(Osciloscope);

int SDLCALL RenderThreadFunction(void* data);
void SetupUI(WndMain& window);

////////////////////////////////////////////////////////////////////////////////
// variables
////////////////////////////////////////////////////////////////////////////////

OscFileThread::OscFileThread()
{
    SDL_AtomicSet(&atomic, 0);
    thread = 0;
}

int OscFileThread::isRunning()
{
    return SDL_AtomicGet(&atomic);
}

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeGrid
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeGrid::set(float x, float y)
{
    xCount = x;
    xMax   =  xCount / 2.f;
    xMin   = -xCount / 2.f;
    yCount = y;
    yMax   =  yCount / 2.f;
    yMin   = -yCount / 2.f;
}

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeCamera
//
////////////////////////////////////////////////////////////////////////////////
OsciloscopeCamera::OsciloscopeCamera()
{
    zoom  = 1.f;
    theta = 135.f;
    phi   = 45.f;
}

////////////////////////////////////////////////////////////////////////////////
//
// OscContext
//
////////////////////////////////////////////////////////////////////////////////
OscContext::OscContext()
{
    lock = 0;
}

void OscContext::setDisplay(SDisplay* dis)
{
    SDL_AtomicLock(&lock);
    display = *dis;
    SDL_AtomicUnlock(&lock);
}
void OscContext::getDisplay(SDisplay* dis)
{
    SDL_AtomicLock(&lock);
    *dis = display;
    SDL_AtomicUnlock(&lock);
}

////////////////////////////////////////////////////////////////////////////////
//
// ThreadApi
//
////////////////////////////////////////////////////////////////////////////////
ThreadApi::ThreadApi()
{
    timeout = 1000;
    lock = 0;
    SDL_AtomicSet(&sync, 0);
    wait();
    SDL_AtomicSet(&open, 0);
    wait();
    SDL_AtomicSet(&fpga, 0);
    wait();
    SDL_AtomicSet(&calibrated, 0);
    wait();
    SDL_AtomicSet(&vid, 0);
    wait();
    SDL_AtomicSet(&pid, 0);
    wait();
    SDL_AtomicSet(&sid, 0);
    wait();
    SDL_AtomicSet(&version, HARDWARE_VERSION);
    wait();
    SDL_AtomicSet(&header, 0);
    wait();
    SDL_AtomicSet(&data, 0);
    wait();
    SDL_AtomicSet(&packet, 0);
    wait();
    SDL_memset(&usbData, 0, sizeof(SUsb));
    usbSize = 0;
    SDL_memset(&fx3Data, 0, sizeof(SFx3));
    fx3Size = 0;
    SDL_memset(&fpgaData, 0, sizeof(SFpga));
    fpgaSize = 0;
    SDL_memset(&eepromData, 0, sizeof(SEeprom));
    eepromSize   = 0;
    eepromOffset = 0;
    wait();
    SDL_memset(&displayData, 0, sizeof(SDisplay));
    SDL_memset(&config, 0, sizeof(SHardware));
}

////////////////////////////////////////////////////////////////////////////////
//
// thread
//
////////////////////////////////////////////////////////////////////////////////
void ThreadApi::function(EThreadApiFunction f)
{
    SDL_AtomicLock(&lock);
    SDL_AtomicAdd(&sync, 1);
    SDL_AtomicSet(&ret[func], 0);
    func.pushBack(f);
    SDL_AtomicUnlock(&lock);
}

void ThreadApi::wait()
{
    while(SDL_AtomicCAS(&sync, 0, 0) == SDL_FALSE)
    {
        SDL_Delay(1);
    }
}

int ThreadApi::result(EThreadApiFunction func)
{
    return SDL_AtomicGet(&ret[func]);
}

void ThreadApi::resultClear(EThreadApiFunction func)
{
    SDL_AtomicSet(&ret[func], 0);
}

void ThreadApi::resultClearAll()
{
    for(int i = 0; i < afLast; i++)
    { resultClear((EThreadApiFunction)i); }
}

void ThreadApi::update()
{
    // functions
    int decr = 0;
    Array<EThreadApiFunction, 22>  execute;
    SDL_AtomicLock(&lock);
    decr = -1 * func.getCount();
    for(int i = 0; i < func.getCount(); i++)
    {
        execute.pushBack(func[i]);
    }
    func.clear();
    SDL_AtomicUnlock(&lock);
    // default
    SInt iopened = {0};
    int iversion = SDL_AtomicGet(&version);
    /*
    sfHardwareIsOpened(getCtx(), &iopened);
    SDL_AtomicSet(&open, iopened.value);
    */
    while(true)
    {
        uint count = execute.getCount();
        if(count == 0) { break; }
        EThreadApiFunction f = execute.first();
        execute.popFront();
        int iret = 0;
        switch(f)
        {
            //TODO: FX3 USB functions
            case afInit:
                SDL_AtomicLock(&lock);
                sfApiInit();
                sfSetThreadSafe(getCtx(), threadSafe);
                sfSetActive(getCtx(), active);
                sfSetTimeOut(getCtx(), timeout);
                SDL_AtomicUnlock(&lock);
                break;
            case afIsOpened:
                iret = sfHardwareIsOpened(getCtx(), &iopened);
                SDL_AtomicSet(&open, iopened.value);
                break;
            case afOpenUsb:
                SDL_AtomicLock(&lock);
                iret += sfHardwareOpen(getCtx(), &usbData, iversion);
                iret += sfHardwareIsOpened(getCtx(), &iopened);
                SDL_AtomicSet(&open, iopened.value);
                SDL_AtomicUnlock(&lock);
                break;
            case afUploadFpga:
                SDL_AtomicLock(&lock);
                iret += sfHardwareUploadFpga(getCtx(), &fpgaData);
                SDL_AtomicSet(&fpga, iopened.value);
                SDL_AtomicUnlock(&lock);
                break;
            case afUploadFxx:
                SDL_AtomicLock(&lock);
                iret += sfHardwareUploadFx3(getCtx(), &fx3Data);
                SDL_AtomicUnlock(&lock);
                break;
            case afResetUsb:
                iret += sfHardwareReset(getCtx());
                break;
            case afCloseUsb:
                iret += sfHardwareClose(getCtx());
                break;
            case afEEPROMRead:
                SDL_AtomicLock(&lock);
                iret += sfHardwareEepromRead(getCtx(), &eepromData, eepromSize, eepromOffset);
                SDL_AtomicUnlock(&lock);
                break;
            case afEEPROMReadFirmwareID:
                SDL_AtomicLock(&lock);
                iret += sfHardwareEepromReadFirmwareID(getCtx(), &eepromData, eepromOffset);
                SDL_AtomicUnlock(&lock);
                break;
            case afEEPROMWrite:
                SDL_AtomicLock(&lock);
                iret += sfHardwareEepromWrite(getCtx(), &eepromData, eepromSize, eepromOffset);
                SDL_AtomicUnlock(&lock);
                break;
            case afEEPROMErase:
                iret += sfHardwareEepromErase(getCtx());
                break;
            case afHardwareConfig:
                {
                    SHardware hw = { 0 };
                    getConfig(&hw);
                    iret += sfHardwareConfig(getCtx(), &hw);
                    //SDL_AtomicSet(&fpga, iopened.value);
                }
                break;
            case afReadFpgaStatus:
               {
                  SDL_AtomicLock(&lock);
                  SInt status = { 0 };
                  iret += sfHardwareReadFpgaStatus(getCtx(), &status);
                  SDL_AtomicSet(&fpga,status.value);
                  SDL_AtomicUnlock(&lock);
               }
               break;
        };
        SDL_AtomicSet(&ret[f], iret);
    }
    SDL_AtomicAdd(&sync, decr);
}

////////////////////////////////////////////////////////////////////////////////
//
// api
//
////////////////////////////////////////////////////////////////////////////////
int ThreadApi::getVersion()
{
    return SDL_AtomicGet(&version);
}
int ThreadApi::isOpen()
{
    return SDL_AtomicGet(&open);
}
int ThreadApi::isFpga()
{
    return SDL_AtomicGet(&fpga);
}

int  ThreadApi::isCalibrated()
{
   return SDL_AtomicGet(&calibrated);
}

void ThreadApi::setInit(int mem, int ithread, int iactive, int tt)
{
    SDL_AtomicLock(&lock);
    memory = mem;
    threadSafe = ithread;
    active = iactive;
    timeout = tt;
    sfApiCreateContext(getCtx(), memory);
    SDL_AtomicUnlock(&lock);
}
void ThreadApi::setFrame(int  v, int  h, int  d, int  p)
{
    SDL_AtomicSet(&version, v);
    SDL_AtomicSet(&header, h);
    SDL_AtomicSet(&data, d);
    SDL_AtomicSet(&packet, p);
}
void ThreadApi::getFrame(int* v, int* h, int* d, int* p)
{
    *v = SDL_AtomicGet(&version);
    *h = SDL_AtomicGet(&header);
    *d = SDL_AtomicGet(&data);
    *p = SDL_AtomicGet(&packet);
}
void ThreadApi::setUSB(SUsb* usb)
{
    SDL_AtomicLock(&lock);
    usbData = *usb;
    usbSize = sizeof(SUsb);
    SDL_AtomicUnlock(&lock);
}
void ThreadApi::getUSB(SUsb* usb)
{
    SDL_AtomicLock(&lock);
    *usb = usbData;
    SDL_AtomicUnlock(&lock);
}
void ThreadApi::setConfig(SHardware* c1)
{
    SDL_AtomicLock(&lock);
    config = *c1;
    SDL_AtomicUnlock(&lock);
}
void ThreadApi::getConfig(SHardware* c1)
{
    SDL_AtomicLock(&lock);
    *c1 = config;
    SDL_AtomicUnlock(&lock);
}

void ThreadApi::setEEPROM(SEeprom* data, int  size, int  offset)
{
    SDL_AtomicLock(&lock);
    eepromData   = *data;
    eepromSize   = size;
    eepromOffset = offset;
    SDL_AtomicUnlock(&lock);
}
void ThreadApi::getEEPROM(SEeprom* data, int* size, int* offset)
{
    SDL_AtomicLock(&lock);
    *data   = eepromData;
    *size   = eepromSize;
    *offset = eepromOffset;
    SDL_AtomicUnlock(&lock);
}
void ThreadApi::setDisplay(SDisplay* dis)
{
    SDL_AtomicLock(&lock);
    displayData = *dis;
    SDL_AtomicUnlock(&lock);
}

////////////////////////////////////////////////////////////////////////////////
//
// procedure
//
////////////////////////////////////////////////////////////////////////////////

int ThreadApi::uploadFpga(OscHardware* oscHw)
{
   SDL_AtomicLock(&lock);
      // fpga
      char*  buffer = 0;
      ilarge bufferSize = 0;
      char fpgaPath[1024] = { 0 };
      int  ret = pFormat->formatPath(fpgaPath, 1024, oscHw->fpgaFirmware.asChar());
      int fret = fileLoad(fpgaPath, &buffer, &bufferSize);
      if(fret==0)
      {
         fpgaData.size = fpgaSize = bufferSize;
         SDL_memcpy(fpgaData.data.bytes, buffer, fpgaSize);
         pMemory->free(buffer);
      }
   SDL_AtomicUnlock(&lock);
   function(afUploadFpga);
   wait();
   SDL_Delay(200);
   return 0;
}

int ThreadApi::openUSB(OscHardware* oscHw, bool factory)
{
    if(SDL_AtomicGet(&open) == 0)
    {
        SDL_AtomicSet(&calibrated,0);
        SUsb usb = oscHw->getUSB(factory);
        setUSB(&usb);
        function(afOpenUsb);
        wait();
        /*
        if(SDL_AtomicGet(&open) == 0)
        {
            SDL_Delay(100);
            /*
            usb.idVendor  = CYPRESS_VID;
            usb.idProduct = CYPRESS_PID;
            setUSB(&usb);
            function(afOpenUsb);
            wait();
        }
        */
    }
    return 0;
}

int ThreadApi::useEepromCalibration(OscHardware* oscHw)
{
    // calibration
    if(SDL_AtomicGet(&open) == 1)
    {
        SDL_AtomicLock(&lock);
        eepromSize = sizeof(OscHardware);
        eepromOffset = 256000;
        SDL_AtomicUnlock(&lock);
        function(afEEPROMRead);
        wait();
        // use calibration from eeprom
        if(result(afEEPROMRead) == 0)
        {
            // apply OscHardware struct from EEPROM (all data)
            SDL_AtomicLock(&lock);
            SDL_memcpy(oscHw, &eepromData, eepromSize);
            // revert non-calibration related data (use data from file hardware.json instead)
            oscHw->loadHardwareFromFile();
            SDL_AtomicUnlock(&lock);
            SDL_AtomicSet(&calibrated, 1);
        }
    }
    return 0;
}

int ThreadApi::writeUsbToEEPROM(OscHardware* oscHw, bool factory)
{
    // TODO: Write FX3 firmware to RAM and EEPROM
    int delay_between_retries = 500;
    int nRetries = 8;
    for (int i = 0; i < nRetries; i++)
    {
        // open USB device
        if (factory == 1) {
            openUSB(oscHw, factory);
        }
        if(SDL_AtomicGet(&open) > 0)
        {
            printf("(%i); Starting FX3 firmware write to EEPROM.\n", i);
            SDL_AtomicLock(&lock);
            // setup data for loading FX3.img to FX3 RAM
            char fx3Path[1024] = { 0 };
            int         ret = pFormat->formatPath(fx3Path, 1024, oscHw->usbFirmware.asChar());
            fx3Size = (uint)SDL_strlen(fx3Path);
            fx3Data.size = fx3Size;
            SDL_memcpy(fx3Data.data.bytes, fx3Path, fx3Size);
            fx3Data.data.bytes[fx3Size] = 0;
            int iversion = SDL_AtomicGet(&version);
            SDL_AtomicUnlock(&lock);
            SUsb usb = oscHw->getUSB(factory);
            setUSB(&usb);
            function(afUploadFxx); // load fx3.img to FX3 RAM
            wait();
            SUsb sfusb = oscHw->getUSB(0); // select scopefun vid/pid
            setUSB(&sfusb);
            //function(afOpenUsb); //not required as scopefun device is already being opened in another thread
            // wait for scopefun device
            for (int j = 0; j < nRetries; j++) {
                if (pOsciloscope->thread.isOpen() and (((UsbContext*)getCtx()->usb)->vendorId != CYPRESS_VID)) {
                    printf("(%i); ScopeFun device s/n: %s.\n", j, ((UsbContext*)getCtx()->usb)->serialBuffer);
                    break;
                }
                else {
                    SDL_Delay(delay_between_retries);
                }
            }
            if (result(afOpenUsb) ==  SCOPEFUN_SUCCESS) {
                // setup data for loading FX3.img to EEPROM
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Writing FX3 firmware will start.", "Please wait and do not disconnect USB.", 0);
                SDL_AtomicLock(&lock);
                eepromSize = sizeof(SEeprom); //fx3 firmware image size
                char path[1024] = { 0 };
                ret = pFormat->formatPath(path, 1024, oscHw->usbFirmware.asChar());
                eepromOffset = 0;
                fileLoadPtr(path, (char*)&eepromData.data.bytes[0], &eepromSize);
                SDL_AtomicUnlock(&lock);
                // write eeprom
                function(afEEPROMWrite);
                wait();
            }
            else
            #if (defined(PLATFORM_WIN) || defined(PLATFORM_MINGW))
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Not found.", "USB device not found.\nPlease install WinUSB driver.", 0);
            #else
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Not found.", "USB device not found.", 0);
            #endif // defined
            break;
        }
        else {
            // wait for USB device
            SDL_Delay(delay_between_retries);
        }
    }

    //collect results
    int iret = result(afOpenUsb) + result(afUploadFxx) + result(afEEPROMWrite);
    if(iret == SCOPEFUN_SUCCESS)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Write USB data to EEPROM was successful.", 0);
    }
    else
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Write USB data to EEPROM failed.", 0);
    }
    return iret;
}

int ThreadApi::writeUsbToRam(OscHardware* oscHw, bool factory)
{
    // TODO: Write FX3 firmware to RAM
    int delay_between_retries = 500;
    int nRetries = 8;
    for (int i = 0; i < nRetries; i++)
    {
        // open USB device
        if (factory == 1) {
            openUSB(oscHw, factory);
        }
        if(SDL_AtomicGet(&open) > 0)
        {
            printf("(%i); Starting FX3 firmware load to RAM.\n", i);
            SDL_AtomicLock(&lock);
            // setup data for loading FX3.img to FX3 RAM
            char fx3Path[1024] = { 0 };
            int         ret = pFormat->formatPath(fx3Path, 1024, oscHw->usbFirmware.asChar());
            fx3Size = (uint)SDL_strlen(fx3Path);
            fx3Data.size = fx3Size;
            SDL_memcpy(fx3Data.data.bytes, fx3Path, fx3Size);
            fx3Data.data.bytes[fx3Size] = 0;
            int iversion = SDL_AtomicGet(&version);
            SDL_AtomicUnlock(&lock);
            SUsb usb = oscHw->getUSB(factory);
            setUSB(&usb);
            function(afUploadFxx); // load fx3.img to FX3 RAM
            wait();
            SUsb sfusb = oscHw->getUSB(0); // select scopefun vid/pid
            setUSB(&sfusb);
            // wait for scopefun device
            for (int j = 0; j < nRetries; j++) {
                if (pOsciloscope->thread.isOpen() and (((UsbContext*)getCtx()->usb)->vendorId != CYPRESS_VID)) {
                    printf("(%i); ScopeFun device s/n: %s.\n", j, ((UsbContext*)getCtx()->usb)->serialBuffer);
                    break;
                }
                else {
                    SDL_Delay(delay_between_retries);
                }
            }
            break;
        }
        else {
            // wait for USB device
            SDL_Delay(delay_between_retries);
        }
    }

    int iret = result(afOpenUsb) + result(afUploadFxx);
    /*
    if(iret == SCOPEFUN_SUCCESS)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Write USB data to RAM was successful.", 0);
    }
    else
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Write USB data to RAM failed.", 0);
    }
    */
    return iret;
}

int ThreadApi::readFirmwareIDFromEEPROM(OscHardware* hw)
{
    int iversion = SDL_AtomicGet(&version);
    openUSB(hw, 0);
    if(SDL_AtomicGet(&open) > 0)
    {
        SDL_AtomicLock(&lock);
        int iversion = SDL_AtomicGet(&version);
        eepromSize   = SCOPEFUN_EEPROM_FIRMWARE_NAME_BYTES;
        eepromOffset = 0;
        SDL_AtomicUnlock(&lock);
        function(afEEPROMReadFirmwareID);
        wait();
    }
    int iret = result(afEEPROMReadFirmwareID);
    if(iret == SCOPEFUN_SUCCESS)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Read firmware ID data from EEPROM was successful.", 0);
    }
    else
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Read firmware ID data from EEPROM failed.", 0);
    }
    return iret;
}

int ThreadApi::readUsbFromEEPROM(OscHardware* hw, int readsize)
{
    int iversion = SDL_AtomicGet(&version);
    openUSB(hw, 0);
    if(SDL_AtomicGet(&open) > 0)
    {
        SDL_AtomicLock(&lock);
        int iversion = SDL_AtomicGet(&version);
        ilarge size = readsize;
        if(size == 0)
        {
            char path[1024] = { 0 };
            int         ret = pFormat->formatPath(path, 1024, hw->usbFirmware.asChar());
            fileSize(path, &size);
        }
        eepromSize   = size;
        eepromOffset = 0;
        SDL_AtomicUnlock(&lock);
        function(afEEPROMRead);
        wait();
    }
    int iret = result(afEEPROMRead);
    if(iret == SCOPEFUN_SUCCESS)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Read USB data from EEPROM was successful.", 0);
    }
    else
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Read USB data from EEPROM failed.", 0);
    }
    return iret;
}

int ThreadApi::writeCalibrateSettingsToEEPROM(OscHardware* hw)
{
    int iversion = SDL_AtomicGet(&version);
    openUSB(hw, 0);
    if(SDL_AtomicGet(&open) > 0)
    {
        // do not change this code
        // clear
        SDL_AtomicLock(&lock);
        eepromSize   = sizeof(OscHardware);
        eepromOffset = 256000;
        SDL_memset((void*)&eepromData, 0, (size_t)eepromSize);
        SDL_AtomicUnlock(&lock);
        function(afEEPROMWrite);
        wait();
        // write
        SDL_AtomicLock(&lock);
        eepromSize   = sizeof(OscHardware);
        eepromOffset = 256000;
        SDL_memcpy((void*)&eepromData, (void*)hw, (size_t)eepromSize);
        SDL_AtomicUnlock(&lock);
        function(afEEPROMWrite);
        wait();
    }
    int iret = result(afEEPROMWrite);
    if(iret == SCOPEFUN_SUCCESS)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Write calibration data to EEPROM was successful.", 0);
    }
    else
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Write calibration data to EEPROM failed.", 0);
    }
    return iret;
}

int ThreadApi::eraseCalibrationEEPROM(OscHardware* hw)
{
    int iversion = SDL_AtomicGet(&version);
    openUSB(hw, 0);
    if(SDL_AtomicGet(&open) > 0)
    {
        // do not change this code
        // clear
        SDL_AtomicLock(&lock);
        eepromSize   = sizeof(OscHardware);
        eepromOffset = 256000;
        SDL_memset((void*)&eepromData, 0, (size_t)eepromSize);
        SDL_AtomicUnlock(&lock);
        function(afEEPROMWrite);
        wait();
    }
    int iret = result(afEEPROMWrite);
    if(iret == SCOPEFUN_SUCCESS)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Erase calibration from EEPROM was successful.", 0);
    }
    else
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Erase calibration from EEPROM failed.", 0);
    }
    return iret;
}

int ThreadApi::readCalibrateSettingsFromEEPROM(OscHardware* hw)
{
    int iversion = SDL_AtomicGet(&version);
    openUSB(hw, 0);
    if(SDL_AtomicGet(&open) > 0)
    {
        SDL_AtomicLock(&lock);
        eepromSize = sizeof(OscHardware);
        eepromOffset = 256000;
        SDL_AtomicUnlock(&lock);
        function(afEEPROMRead);
        wait();
    }
    int iret = result(afEEPROMRead);
    if(iret == SCOPEFUN_SUCCESS)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Read calibration data from EEPROM was successful.", 0);
    }
    else
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Read calibration data from EEPROM failed.", 0);
    }
    return iret;
}


int ThreadApi::eraseEEPROM(OscHardware* hw)
{
    openUSB(hw, 0);
    // upload
    function(afUploadFxx);
    // old hardware ?
    int ver = SDL_AtomicGet(&version);
    function(afEEPROMErase);
    wait();
    int iret = result(afEEPROMErase);
    if(iret == SCOPEFUN_SUCCESS)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Erase EEPROM was successful.", 0);
    }
    else
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Erase EEPROM failed.", 0);
    }
    return iret;
}

int ThreadApi::captureFrameData(SFrameData* buffer, int toReceive, int* transfered, int type)
{
    // return sfHardwareCapture(getCtx(), buffer, toReceive, transfered, type);
    return 0;
}

int ThreadApi::hardwareControlFunction(SHardware* hw)
{
    setConfig(hw);
    int ver = SDL_AtomicGet(&version);
    function(afHardwareConfig);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Oscilloscope
//
////////////////////////////////////////////////////////////////////////////////
OsciloscopeManager::OsciloscopeManager()
{
    SDL_AtomicSet(&captureWatchdogTimer, 0);
    SDL_AtomicSet(&captureWatchdogHealthy, 1);
    SDL_AtomicSet(&debugFrameCount, 0);
    SDL_AtomicSet(&debugStuckCount, 0);
    SDL_AtomicSet(&m_undoRedoEnabled,0);
    dtUpdate = 0.0;
    dtRender = 0.0;
    ctx = new OscContext();
    serverThread = 0;
    serverActive = false;
    scrollThread = false;
    frameTime = 0.f;
    SDL_AtomicSet(&clearRenderTarget, 1);
    SDL_AtomicSet(&clearThermal, 0);
    analogWindowSize = 1.f;
    sliderMode  = 0;
    openglFocus = 1;
    SDL_AtomicSet(&signalMode, SIGNAL_MODE_PAUSE);
    windowSlot = 0;
    SDL_AtomicSet(&oscExit, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Watchdog
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeManager::watchdogPing()
{
    SDL_AtomicSet(&captureWatchdogTimer, (int)SDL_GetTicks());
    SDL_AtomicSet(&captureWatchdogHealthy, 1);
}

int OsciloscopeManager::watchdogCheck()
{
    int now    = (int)SDL_GetTicks();
    int last   = SDL_AtomicGet(&captureWatchdogTimer);
    int elapsed = now - last;
    int stuckCount = SDL_AtomicGet(&debugStuckCount);

    // If > 6 seconds since last ping, capture thread is stuck
    if (elapsed > 6000 && last > 0)
    {
        SDL_AtomicSet(&captureWatchdogHealthy, 0);
        SDL_AtomicAdd(&debugStuckCount, 1);
        CORE_MESSAGE("Watchdog: capture thread stuck for %d ms (count=%d)",
                     elapsed, SDL_AtomicGet(&debugStuckCount));

        // Recover: pause capture
        SDL_AtomicSet(&signalMode, SIGNAL_MODE_PAUSE);

        // Reset partial frame state so next capture starts fresh
        SFContext* sfCtx = getCtx();
        if (sfCtx)
        {
            sfCtx->frame.received = 0;
            CORE_MESSAGE("%s", "Watchdog: frame state reset");
        }

        // 禁止USB复位：FX3固件在RAM中，USB复位会丢失固件
        // 导致设备回退到bootloader模式，无法再次连接
        // 改为仅仅重置frame状态，等待下一次捕获
        if (stuckCount > 2)
        {
            CORE_MESSAGE("%s", "Watchdog: USB reset disabled (would lose FX3 firmware). Resetting frame state only.");
            SDL_AtomicSet(&debugStuckCount, 0);
            //pOsciloscope->thread.function(EThreadApiFunction::afResetUsb);
        }
        return 1; // stuck, recovered
    }
    return 0; // healthy
}

void* malloc_fn2(size_t sz)
{
    return pMemory->allocate(sz);
}

void free_fn2(void* ptr)
{
    return pMemory->free(ptr);
}

cJSON_Hooks hooks;

int usbHotPlugCallback(int flags)
{
    return 0;
}

int SDLCALL CaptureDataThreadFunction(void* data);
int SDLCALL GenerateFrameThreadFunction(void* data);
int SDLCALL ControlHardwareThreadFunction(void* data);
int SDLCALL AutoDetectUsb(void* ptr);
int LuaMsg(const char* msg, void* windows);

int OsciloscopeManager::start()
{
    ////////////////////////////////////////////////
    // load settings
    ////////////////////////////////////////////////
    hooks.malloc_fn = malloc_fn2;
    hooks.free_fn = free_fn2;
    cJSON_InitHooks(&hooks);
    settings.load();
    wheel = 0;
    signalPosition = 0.5f;
    signalZoom     = 1.f;
    sliderPosition = 0.5f;
    ////////////////////////////////////////////////
    // api
    ////////////////////////////////////////////////
    pOsciloscope->thread.setInit(settings.getSettings()->memoryFrame * MEGABYTE, 0, 1, 1000);
    pOsciloscope->thread.function(afInit);
    ////////////////////////////////////////////////
    // script
    ////////////////////////////////////////////////
    /* TODO: erase Lua
    if(m_runScript.getLength() > 0)
    {
        m_callback.Add(m_runScript,LuaMsg);
        m_callback.Get(0)->Run();
    }

    sfFrameDisplayCallback(getCtx(), m_callback.Ptr(), this);
    */
    ////////////////////////////////////////////////
    // apply canvas allocation settings
    ////////////////////////////////////////////////
    pCanvas3d->setVertexBufferSize(settings.getSettings()->renderVertexBufferSizeMegaByte * 1024 * 1024);
    pCanvas3d->setThreadBuffer(MAX_THREAD);
    pCanvas2d->setThreadBuffer(MAX_THREAD);
    ////////////////////////////////////////////////
    // apply default settings
    ////////////////////////////////////////////////
    sfSetDefault(getHw());
    sfSetYRangeScaleA(getHw(), getAttr(vc2Volt), getGain(0, vc2Volt));
    sfSetYRangeScaleB(getHw(), getAttr(vc2Volt), getGain(1, vc2Volt));
    //////////////////////////////////////////////////////////
    // thread count
    //////////////////////////////////////////////////////////
    uint cpuCount = SDL_GetCPUCount();
    // pRender->setThreadCount(min(settings.getSettings()->renderThreadCount, MAX_THREAD));
    pRender->setThreadCount(MAX_THREAD);

    //////////////////////////////////////////////////////////
    // allocate memory for signal
    //////////////////////////////////////////////////////////
    allocate();
    //////////////////////////////////////////////////////////
    // setup thread data
    //////////////////////////////////////////////////////////
    SDL_AtomicSet(&captureDataThreadActive, 1);
    SDL_AtomicSet(&generateFrameThreadActive, 1);
    SDL_AtomicSet(&controlHardwareThreadActive, 1);
    SDL_AtomicSet(&updateThreadActive, 1);
    threadLoop.capture.setCount(min(settings.getSettings()->renderThreadCount, MAX_THREAD));
    threadLoop.update.setCount(min(settings.getSettings()->renderThreadCount, MAX_THREAD));
    //////////////////////////////////////////////////////////
    // camera setup
    //////////////////////////////////////////////////////////
    cameraOsc.ortho.View.Pos() = Vector4(0.5f, 0.f, -1.0f, 1.f);
    cameraOsc.zoom = 1.f;
    cameraFFT.ortho.View.Pos() = Vector4(0.5f, 0.f, -1.0f, 1.f);
    cameraFFT.zoom = 1.f;
    grid.set(10.f, 10.f);
    ////////////////////////////////////////////////
    // SDL
    ////////////////////////////////////////////////
    //std::cout << "Initializing SDL2. " << std::endl;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        std::cerr << "Failed to initialize SDL2: " << SDL_GetError() << std::endl;
        return 1;
    };
    pRender->height = pOsciloscope->settings.getSettings()->windowDisplayWidth;
    pRender->width  = pOsciloscope->settings.getSettings()->windowDisplayHeight;
    int renderWidth = pOsciloscope->settings.getSettings()->windowDisplayWidth;
    int renderHeight = pOsciloscope->settings.getSettings()->windowDisplayHeight;
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    int cx = 0;
    int cy = mode.h / 16;
    int cw = 35 * mode.w / 80;
    int ch = 7 * mode.h / 8;
    pOsciloscope->sdlX = cx - (pOsciloscope->settings.getSettings()->windowControlWidth + pOsciloscope->settings.getSettings()->windowDisplayWidth) / 2;
    pOsciloscope->sdlY = cy - pOsciloscope->settings.getSettings()->windowDisplayHeight / 2;
    pOsciloscope->sdlW = pOsciloscope->settings.getSettings()->windowDisplayWidth;
    pOsciloscope->sdlH = pOsciloscope->settings.getSettings()->windowDisplayHeight;
    pOsciloscope->sdlX = cx;
    pOsciloscope->sdlY = cy;
    pOsciloscope->sdlW = cw;
    pOsciloscope->sdlH = ch;
    pRender->height = cw;
    pRender->width  = ch;
    renderWidth  = cw;
    renderHeight = ch;
    pOsciloscope->sdlContext = 0;
    pOsciloscope->sdlWindow = 0;
    if(pOsciloscope->settings.getSettings()->renderShaders21 == 0)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        pOsciloscope->sdlWindow = SDL_CreateWindow("ScopeFun", pOsciloscope->sdlX, pOsciloscope->sdlY, renderWidth, renderHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
        pOsciloscope->sdlContext = SDL_GL_CreateContext(pOsciloscope->sdlWindow);
    }
    if(!pOsciloscope->sdlContext)
    {
        if(pOsciloscope->sdlWindow)
        { SDL_DestroyWindow(pOsciloscope->sdlWindow); }
        SDL_GL_ResetAttributes();
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        pOsciloscope->sdlWindow = SDL_CreateWindow("ScopeFun", pOsciloscope->sdlX, pOsciloscope->sdlY, renderWidth, renderHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
        pOsciloscope->sdlContext = SDL_GL_CreateContext(pOsciloscope->sdlWindow);
        if(!pOsciloscope->sdlContext)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "At least OpenGL 2.1(shaders) is required in order to run this program.", 0);
            return 1;
        }
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", "Thermal rendering will be disabled.\nYou need OpenGL 3.2 graphics in order to use this feature.", 0);
        grSetMode(OPENGL_MODE_21);
    }
    else
    {
        grSetMode(OPENGL_MODE_32);
    }
    pRender->sdlWindow = pOsciloscope->sdlWindow;

    #if !defined(LINUX) && !defined(MAC)
    SDL_GetWindowSize(pOsciloscope->sdlWindow, &pOsciloscope->sdlW, &pOsciloscope->sdlH);
    #endif
    //////////////////////////////////////////////////
    //// glew
    //////////////////////////////////////////////////
    grCreateDevice();
    ////////////////////////////////////////////////
    // icon
    ////////////////////////////////////////////////
    #ifndef MAC
    FORMAT_BUFFER();
    FORMAT_PATH("data/icon/icon64.bmp")
    SDL_Surface* icon = SDL_LoadBMP(formatBuffer);
    SDL_SetWindowIcon(pOsciloscope->sdlWindow, icon);
    SDL_FreeSurface(icon);
    #endif
    ////////////////////////////////////////////////////////////
    //// shaders
    ////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        pOsciloscope->shadowLine3dShader = grCreateShader();
        pOsciloscope->shadowColorShader = grCreateShader();
        pOsciloscope->shadowCoolingShader = grCreateShader();
        pOsciloscope->shadowLine3dShader->compile("shadow/line3d.fx", pCanvas3d->declPos3D, GrShaderDefine(nullptr));
        pOsciloscope->shadowColorShader->compile("shadow/color.fx", pCanvas2d->declPosTex2D, GrShaderDefine(nullptr));
        pOsciloscope->shadowCoolingShader->compile("shadow/cooling.fx", pCanvas2d->declPosTex2D, GrShaderDefine(nullptr));
    }
    ////////////////////////////////////////////////////////////
    //// Canvas / Fonts
    ////////////////////////////////////////////////////////////
    pRender->start();
    pCanvas2d->start();
    pCanvas3d->start();
    pFont->start();
    //////////////////////////////////////////////////
    //// resize
    //////////////////////////////////////////////////
    pRender->resize(pOsciloscope->sdlW, pOsciloscope->sdlH);
    //////////////////////////////////////////////////
    //// onApplicationInit
    //////////////////////////////////////////////////
    onApplicationInit();
    //////////////////////////////////////////////////////////
    // launch capture and update thread
    //////////////////////////////////////////////////////////
    startThreads();
    ////////////////////////////////////////////////
    // callbacks
    ////////////////////////////////////////////////
    pRender->registerCallback(this, 10);
    pCamera->registerCamera(&cameraOsc.ortho);
    pCamera->registerCamera(&cameraFFT.ortho);
    return 0;
}

void OsciloscopeManager::startThreads()
{
    FORMAT_BUFFER();
    ///////////////////////////////////////////////
    // launch render thread
    ///////////////////////////////////////////////
    usbTransfer = 0;
    renderLock = 0;
    uploadFirmwareLock = 0;
    displayLock = 0;
    captureLock = 0;
    SDL_AtomicSet(&contextCreated, 0);
    //////////////////////////////////////////////////////////
    // setup canvas
    //////////////////////////////////////////////////////////
    for(uint i = 0; i < pRender->getThreadCount(); i++)
    {
        pCanvas3d->threadBegin(i);
        pCanvas2d->threadBegin(i);
    }
    ///////////////////////////////////////////////
    // launch capture thread
    ///////////////////////////////////////////////
    SDL_zero(renderWindow);
    SDL_zero(renderData);
    pControlHardware  = SDL_CreateThreadWithStackSize(ControlHardwareThreadFunction,  "ControlHardware", 1024 * 1024, this);
    pCaptureData      = SDL_CreateThreadWithStackSize(CaptureDataThreadFunction,      "CaptureData",     1024 * 1024, this);
    pGenerateFrame    = SDL_CreateThreadWithStackSize(GenerateFrameThreadFunction,    "GenerateFrame",   1024 * 1024, this);
    pThreadUsbConnect = SDL_CreateThread(AutoDetectUsb, "AutoDetectUsb", this);
}

void OsciloscopeManager::exitThreads()
{
    //int  threadReturnValue;
    //SDL_WaitThread(pThreadUsbConnect, &threadReturnValue);
}

void OsciloscopeManager::oscCameraSetup(int enable)
{
    if(enable)
    {
        Vector4 pos = Vector4(1.f,  1.f, -1.f, 1.f);
        Vector4 at  = Vector4(0.5f, 0.0f, 0.5f, 1.f);
        Vector4 up  = Vector4(0.f,  1.f, 0.f, 1.f);
        cameraOsc.zoom = 1.f;
        cameraOsc.ortho.lookAt(pos, at, up);
    }
    else
    {
        Vector4 pos = Vector4(0.5f, 0.f, -1.f, 1);
        Vector4  at = Vector4(0.5f, 0.f,  0.f, 1.f);
        Vector4  up = Vector4(0.f,  1.f,  0.f, 1.f);
        cameraOsc.zoom = 1.f;
        cameraOsc.ortho.lookAt(pos, at, up);
    }
    cameraOsc.theta = 0;
    cameraOsc.phi = 0;
}

uint OsciloscopeManager::max3dTesselation(uint value, WndMain& wnd)
{
    int onOff = 0;
    if(wnd.channel01.OscOnOff)
    {
        onOff++;
    }
    if(wnd.channel02.OscOnOff)
    {
        onOff++;
    }
    if(wnd.function.OscOnOff)
    {
        onOff++;
    }
    ularge triangleBytes = 32 * 3; // pos, normal
    ularge safetyBytes   = 2 * MEGABYTE; // pos, normal
    while(1)
    {
        ularge neededBytes = (ularge(onOff * settings.getSettings()->historyFrameDisplay) * ularge(NUM_SAMPLES * 2) * triangleBytes) / value;
        ularge actualBytes = (ularge(settings.getSettings()->renderVertexBufferSizeMegaByte) * MEGABYTE - safetyBytes);
        if(neededBytes < actualBytes || value > 1024)
        {
            return value;
        }
        value++;
    }
    return value;
}

void OsciloscopeManager::fftCameraSetup(int enable)
{
    if(enable)
    {
        Vector4 pos = Vector4(1.f, 1.f, -1.f,  1.f);
        Vector4  at = Vector4(0.5f, 0.f,  0.5f, 1.f);
        Vector4  up = Vector4(0.f, 1.f,  0.f,  1.f);
        cameraFFT.zoom = 1.f;
        cameraFFT.ortho.lookAt(pos, at, up);
    }
    else
    {
        Vector4 pos = Vector4(0.5f, 0.f, -1.f, 1);
        Vector4  at = Vector4(0.5f, 0.f, 0.f, 1.f);
        Vector4  up = Vector4(0.f,  1.f, 0.f, 1.f);
        cameraFFT.zoom = 1.f;
        cameraFFT.ortho.lookAt(pos, at, up);
    }
    cameraFFT.theta = 0;
    cameraFFT.phi = 0;
}

int OsciloscopeManager::update(float dt)
{
    if(fileThread.isRunning())
    {
        return 0;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // setup
    ////////////////////////////////////////////////////////////////////////////////
    // clearRenderTarget = !window.horizontal.ETS;
    ////////////////////////////////////////////////////////////////////////////////
    // mouse
    ////////////////////////////////////////////////////////////////////////////////
    int mXpoint = pInput->getMouseX();
    int mYpoint = pInput->getMouseY();
    int mRelX = pInput->getMouseRelX();
    int mRelY = pInput->getMouseRelY();
    int mWheel = pInput->getMouseWheel();
    ////////////////////////////////////////////////////////////////////////////////
    // undo
    ////////////////////////////////////////////////////////////////////////////////
    if(pInput->isKey(SDLK_u))
    { transferUndo(); }
    ////////////////////////////////////////////////////////////////////////////////
    // redo
    ////////////////////////////////////////////////////////////////////////////////
    if(pInput->isKey(SDLK_r))
    { transferRedo(); }
    ////////////////////////////////////////////////////////////////////////////////
    // window dimensions
    ////////////////////////////////////////////////////////////////////////////////
    double wWidth  = (double)pRender->width;
    double wHeight = (double)pRender->height;
    double wAspect = (double)pRender->aspect;
    ////////////////////////////////////////////////////////////////////////////////
    // mouse relative move in focus window (drag direction)
    ////////////////////////////////////////////////////////////////////////////////
    double mouseRelX = double(mRelX) / wWidth;
    double mouseRelY = double(mRelY) / wHeight;
    ////////////////////////////////////////////////////////////////////////////////
    // speed
    ////////////////////////////////////////////////////////////////////////////////
    float moveSpeed    = 22.f;
    ////////////////////////////////////////////////////////////////////////////////
    // move speed
    ////////////////////////////////////////////////////////////////////////////////
    Vector4 CamMoveSpeed = Vector4(moveSpeed * dt, moveSpeed * dt, 0.f, 0.f);
    ////////////////////////////////////////////////////////////////////////////////
    // View
    ////////////////////////////////////////////////////////////////////////////////
    if(!window.fftDigital.is(VIEW_SELECT_OSC_3D) && !window.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        if((mXpoint > 0 && mXpoint < wWidth && mYpoint > 0.f && mYpoint < wHeight))
        {
            ////////////////////////////////////////////////////////////////////////////////
            // zoom
            ////////////////////////////////////////////////////////////////////////////////
            if(mWheel)
            {
                SDL_AtomicSet(&clearThermal, 1);
                SDL_AtomicSet(&clearRenderTarget, 1);
                double capureZoom   = window.horizontal.Capture * signalZoom;
                const int zoomCount = 64;
                double bucket[3]    = { 2, 4, 8 };
                double multiply[zoomCount] = { 0.0 };
                for(int i = 0; i < zoomCount; i++)
                {
                    multiply[i] = bucket[i % 3] * DOUBLE_FEMTO * SDL_pow(10, i / 3);
                }
                for(int i = 0; i < zoomCount; i++)
                {
                    double precision = DOUBLE_FEMTO * SDL_pow(10, i / 3);;
                    if(capureZoom + precision > multiply[i] && capureZoom - precision < multiply[i])
                    {
                        if(mWheel > 0) { signalZoom = multiply[clamp<int>(i - 1, 0, zoomCount)] / window.horizontal.Capture; }
                        else           { signalZoom = multiply[clamp<int>(i + 1, 0, zoomCount)] / window.horizontal.Capture; }
                    }
                }
                signalZoom     = clamp<double>(signalZoom, DOUBLE_FEMTO, 1.0);
                //cameraFFT.zoom = signalZoom;  // TODO: implement FFT zoom
                cameraOsc.zoom = signalZoom;
            }
            ////////////////////////////////////////////////////////////////////////////////
            // measure
            ////////////////////////////////////////////////////////////////////////////////
            double mouseNormalizedX = double(mXpoint) / wWidth - 0.5;
            mouseNormalizedX *= 1.2f;
            float mouseNormalizedY = float(mYpoint) / (float(pRender->width) * analogWindowSize);
            mouseNormalizedY  = mouseNormalizedY * 1.2f;
            mouseNormalizedY -= (0.5f * 1.2f);
            double mouseOSC  =  mouseNormalizedX  + cameraOsc.ortho.View.Pos().x;
            double mouseFFT  =  mouseNormalizedX * cameraFFT.zoom + (cameraFFT.ortho.View.Pos().x);
            double mouseY = -mouseNormalizedY;
            ////////////////////////////////////////////////////////////////////////////////
            // move
            ////////////////////////////////////////////////////////////////////////////////
            if(pInput->isClick(SDL_BUTTON_LEFT))
            {
                window.measure.data.pickX0.onClick();
                window.measure.data.pickX1.onClick();
                window.measure.data.pickY0.onClick();
                window.measure.data.pickY1.onClick();
                window.measure.data.pickFFT0.onClick();
                window.measure.data.pickFFT1.onClick();
            }
            window.measure.data.pickX0.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
            window.measure.data.pickX1.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
            window.measure.data.pickY0.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
            window.measure.data.pickY1.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
            window.measure.data.pickFFT0.onUpdate(mouseFFT, mouseY, 0.f, window.horizontal.Frame);
            window.measure.data.pickFFT1.onUpdate(mouseFFT, mouseY, 0.f, window.horizontal.Frame);
            if(pInput->isMouse(SDL_BUTTON_LEFT))
            {
                SDL_AtomicSet(&clearRenderTarget, 1);
                //clearEts(1);
                //float zoomOffset = (1.f / signalZoom) / 2.f;
                OsciloscopeSlider slider;
                slider.Rectangle(sliderRectW, sliderRectH, sliderRectX, sliderRectY, wWidth, wHeight, analogWindowSize);
                slider.MinMax(sliderMin, sliderMax, wWidth, wHeight, sliderPosition, analogWindowSize, signalZoom /*cameraFFT.zoom */);
                insideSliderBox  = (mXpoint > sliderMin && mXpoint < sliderMax && mYpoint >= sliderRectY && mYpoint <= (sliderRectY + sliderRectH));
                insideSliderArea = (mYpoint >= sliderRectY && mYpoint <= sliderRectY + sliderRectH);
                // slider
                if((sliderMode == 1 || insideSliderBox))
                {
                    SDL_AtomicSet(&clearThermal, 1);
                    sliderMode = 1;
                    double box = 0.5 * (sliderMax - sliderMin) / wWidth;
                    sliderPosition += mouseRelX;
                    sliderPosition  = clamp<double>(sliderPosition, 0.0 + box, 1.0 - box);
                    // min, max
                    double signalPosMin = 0.0 + 0.5 * signalZoom;
                    double signalPosMax = 1.0 - 0.5 * signalZoom;
                    signalPosition = clamp<double>(sliderPosition, signalPosMin, signalPosMax);
                }
                // up / down
                else if(insideSliderArea)
                {
                    sliderMode = 2;
                    analogWindowSize += (float(mRelY)) / (float(pRender->width));
                }
                else if(sliderMode == 2)
                {
                    analogWindowSize += (float(mRelY)) / (float(pRender->width));
                }
                else
                {
                    if(mouseRelX != 0.0f)
                    {
                        SDL_AtomicSet(&clearThermal, 1);
                        double moveFactor = (cameraFFT.ortho.width / cameraOsc.ortho.width);
                        Vector4 Move = Vector4(-mouseRelX, 0, 0, 1);
                        Vector4 MoveFFT = Move * CamMoveSpeed * Vector4(cameraFFT.zoom * moveFactor);
                        Vector4 MoveOsc = Move * CamMoveSpeed * Vector4(cameraOsc.zoom);
                        double move = -mouseRelX * signalZoom;
                        signalPosition += move;
                        double sMin = 0 + 0.5 * signalZoom;
                        double sMax = 1 - 0.5 * signalZoom;
                        signalPosition = clamp<double>(signalPosition, sMin, sMax);
                        sliderPosition = signalPosition;
                    }
                }
            }
            else
            {
                sliderMode = 0;
            }
            ////////////////////////////////////////////////////////////////////////////////
            // reset
            ////////////////////////////////////////////////////////////////////////////////
            if(pInput->isMouse(SDL_BUTTON_RIGHT))
            {
                SDL_AtomicSet(&clearThermal, 1);
                SDL_AtomicSet(&clearRenderTarget, 1);
                clearEts(1);
                cameraFFT.ortho.View.Pos() = Vector4(0.5, 0, -1.f, 1.f);
                cameraFFT.zoom = 1.f;
                cameraOsc.ortho.View.Pos() = Vector4(0.5, 0, -1.f, 1.f);
                cameraOsc.zoom = 1.f;
                signalPosition = 0.5f;
                signalZoom     = 1.f;
                sliderPosition = 0.5f;
            }
        }
    }
    analogWindowSize = clamp(analogWindowSize, 0.f, 1.f / (float)wAspect);
    float oscW = wWidth;
    float oscH = wWidth * analogWindowSize;
    float oscA = oscW / oscH;
    float fftW = wWidth;
    float fftH = wHeight - wWidth * analogWindowSize;
    float fftA = fftW / fftH;
    fftAspectX = wWidth / (wHeight - wWidth * analogWindowSize);
    fftAspectY = (wWidth - wHeight * analogWindowSize) / wHeight;
    oscAspectX = wWidth / (wWidth * analogWindowSize);
    oscAspectY = (wHeight * analogWindowSize) / wHeight;
    float scale = (0.5f / wAspect);
    ////////////////////////////////////////////////////////////////////////////////
    // FFT Camera
    ////////////////////////////////////////////////////////////////////////////////
    float fftWidth  = 1.2f;
    float fftHeight = 1.2f;
    fftScaleX = 1.f * scale;
    fftScaleY = fftA * scale;
    cameraFFT.ortho.setOrthographic(fftWidth, fftHeight, 0.0001f, 1.2f);
    ////////////////////////////////////////////////////////////////////////////////
    // orthographics
    ////////////////////////////////////////////////////////////////////////////////
    float oscWidth  = 1.2f;
    float oscHeight = 1.2f;
    oscScaleX = 1.f * scale;
    oscScaleY = oscA * scale;
    cameraOsc.ortho.setOrthographic(oscWidth, oscHeight, 0.0001f, 1.2f);
    ////////////////////////////////////////////////////////////////////////////////
    // 3d fft
    ////////////////////////////////////////////////////////////////////////////////
    if(window.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        // persepctive
        int sizeX       = 0;
        int sizeY       = 0;
        int sizeYOsc    = 0;
        int mouseInView = 0;
        {
            sizeX       = wWidth;
            sizeY       = wHeight - wWidth * analogWindowSize;
            sizeYOsc    = wWidth * analogWindowSize;
            mouseInView = mXpoint > 0 && mXpoint < sizeX && mYpoint > wWidth * analogWindowSize && mYpoint < wHeight;
        }
        float aspect = float(sizeX) / float(sizeY);
        cameraFFT.ortho.setPerspective(45.f, aspect, 0.1f, 7.f);
        // mouse
        if(mouseInView)
        {
            if(pInput->isClick(SDL_BUTTON_LEFT))
            {
                window.measure.data.pickFFT0.onClick();
                window.measure.data.pickFFT1.onClick();
            }
            Matrix4x4 proj = matMultiply(matInverse(cameraFFT.ortho.Perspective), cameraFFT.ortho.View);
            Vector4     l0 = matUnprojectScreenSpace(mXpoint, sizeY - (mYpoint - sizeYOsc), sizeX, sizeY, proj, 0.25f);
            Vector4     l1 = matUnprojectScreenSpace(mXpoint, sizeY - (mYpoint - sizeYOsc), sizeX, sizeY, proj, 0.75f);
            Vector4      l = vecNormalize3d(l1 - l0);
            // pick X
            {
                Vector4      n = Vector4(0, 0, 1, 0);
                float dotUpper = -1.f * vecDot3d(l0, n);
                float dotlower = vecDot3d(l, n);
                if(dotlower != 0.f)
                {
                    float d = dotUpper / dotlower;
                    Vector4 pos = Vector4(d) * l + l0;
                    window.measure.data.pickFFT0.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                    window.measure.data.pickFFT1.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                }
            }
            if(pInput->isMouse(SDL_BUTTON_LEFT))
            {
                cameraFFT.theta =  TO_RAD * float(mRelX) / float(sizeX) * dt * 10000.f;
                cameraFFT.phi   = -TO_RAD * float(mRelY) / float(sizeY) * dt * 10000.f;
                SDL_AtomicSet(&clearThermal, 1);
            }
            else
            {
                cameraFFT.theta = 0;
                cameraFFT.phi = 0;
            }
        }
        // rotate
        cameraFFT.ortho.View = matMultiply(cameraFFT.ortho.View, matRotationY(cameraFFT.theta));
        cameraFFT.ortho.View = matMultiply(cameraFFT.ortho.View, matRotationAxisAngle(cameraFFT.ortho.View.RowX(), cameraFFT.phi));
        // scale
        fftScaleX = 1.f;
        fftScaleY = 1.f;
        // wheel
        if(mWheel && mouseInView)
        {
            SDL_AtomicSet(&clearThermal, 1);
            SDL_AtomicSet(&clearRenderTarget, 1);
            clearEts(1);
            cameraFFT.zoom -= 0.01f * dt * mWheel;
            cameraFFT.zoom = clamp<float>(cameraFFT.zoom, 0.05f, 2.f);
        }
        float  cameraR = cameraFFT.zoom * 2.0f;
        cameraFFT.ortho.View.Pos() = Vector4(0.5, 0, 0.5f, 1) + cameraFFT.ortho.View.RowZ() * Vector4(-cameraR, -cameraR, -cameraR, 1);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // 3d osc
    ////////////////////////////////////////////////////////////////////////////////
    if(window.fftDigital.is(VIEW_SELECT_OSC_3D))
    {
        // persepctive
        int sizeX       = 0;
        int sizeY       = 0;
        int mouseInView = 0;
        {
            sizeX = wWidth;
            sizeY = wWidth * analogWindowSize;
            mouseInView = mXpoint > 0 && mXpoint < sizeX && mYpoint > 0 && mYpoint < sizeY;
        }
        float aspect = float(sizeX) / float(sizeY);
        cameraOsc.ortho.setPerspective(45.f, aspect, 0.1f, 7.f);
        // mouse
        if(mouseInView)
        {
            ////////////////////////////////////////////////////////////////////////////////
            // move
            ////////////////////////////////////////////////////////////////////////////////
            if(pInput->isClick(SDL_BUTTON_LEFT))
            {
                window.measure.data.pickX0.onClick();
                window.measure.data.pickX1.onClick();
                window.measure.data.pickY0.onClick();
                window.measure.data.pickY1.onClick();
            }
            Matrix4x4 proj = matMultiply(matInverse(cameraOsc.ortho.Perspective), cameraOsc.ortho.View);
            Vector4     l0 = matUnprojectScreenSpace(mXpoint, sizeY - mYpoint, sizeX, sizeY, proj, 0.25f);
            Vector4     l1 = matUnprojectScreenSpace(mXpoint, sizeY - mYpoint, sizeX, sizeY, proj, 0.75f);
            Vector4      l = vecNormalize3d(l1 - l0);
            // pick X
            {
                Vector4      n = Vector4(0, 0, 1, 0);
                float dotUpper = -1.f * vecDot3d(l0, n);
                float dotlower = vecDot3d(l, n);
                if(dotlower != 0.f)
                {
                    float d = dotUpper / dotlower;
                    Vector4 pos = Vector4(d) * l +  l0;
                    window.measure.data.pickX0.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                    window.measure.data.pickX1.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                }
            }
            // pick Y
            {
                Vector4     n = Vector4(0, 0, 1, 0);
                float dotUpper = -1.f * vecDot3d(l0, n);
                float dotlower = vecDot3d(l, n);
                if(dotlower != 0.f)
                {
                    float d = dotUpper / dotlower;
                    Vector4 pos = Vector4(d) * l + l0;
                    window.measure.data.pickY0.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                    window.measure.data.pickY1.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                }
            }
            if(pInput->isMouse(SDL_BUTTON_LEFT))
            {
                cameraOsc.theta =  TO_RAD * float(mRelX) / float(sizeX) * dt * 10000.f;
                cameraOsc.phi   = -TO_RAD * float(mRelY) / float(sizeY) * dt * 10000.f;
                SDL_AtomicSet(&clearThermal, 1);
            }
            else
            {
                cameraOsc.theta = 0;
                cameraOsc.phi = 0;
            }
        }
        // rotate
        cameraOsc.ortho.View = matMultiply(cameraOsc.ortho.View, matRotationY(cameraOsc.theta));
        cameraOsc.ortho.View = matMultiply(cameraOsc.ortho.View, matRotationAxisAngle(cameraOsc.ortho.View.RowX(), cameraOsc.phi));
        // scale
        oscScaleX = 1.f;
        oscScaleY = 1.f;
        // wheel
        float zoomZoom = 0.f;
        if(mWheel && mouseInView)
        {
            zoomZoom = 0.1f * dt * float(mWheel);
            SDL_AtomicSet(&clearThermal, 1);
            cameraOsc.zoom -= 0.01f * dt * mWheel;
            cameraOsc.zoom = clamp<float>(cameraOsc.zoom, 0.05f, 2.f);
        }
        // position
        float  cameraR = cameraOsc.zoom * 2.0f;
        cameraOsc.ortho.View.Pos() = Vector4(0.5, 0, 0.5f, 1) + cameraOsc.ortho.View.RowZ() * Vector4(-cameraR, -cameraR, -cameraR, 1);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // mouuse debug
    ////////////////////////////////////////////////////////////////////////////////
    #ifdef MOUSE_DEBUG
    int lheight = 25;
    pFont->setSize(0.5f);
    FORMAT_BUFFER();
    FORMAT("Mouse  : %d,%d", mXpoint, mYpoint);
    pFont->writeText(100, 100,  formatBuffer);
    FORMAT("Mouse Rel : %d,%d", mRelX, mRelY);
    pFont->writeText(100, 100 + lheight, formatBuffer);
    FORMAT("Left   : %d", pInput->isMouse(SDL_BUTTON_LEFT));
    pFont->writeText(100, 100 + 2 * lheight,  formatBuffer);
    /*
    FORMAT("Middle : %d",pInput->isMouse(SDL_BUTTON_MIDDLE));
    pFont->writeText(100,100+lheight*3,formatBuffer );

    FORMAT("Right  : %d",pInput->isMouse(SDL_BUTTON_RIGHT));
    pFont->writeText(100,100+lheight*4,formatBuffer  );

    FORMAT("Wheel  : %d",mWheel);
    pFont->writeText(100,100+lheight*5, formatBuffer );*/
    #endif
    return 0;
}

void setAnalogViewport(int width, int height, float size)
{
    size = max(size, 0.000000001f);
    grViewport(0, float(height - width * size), (float)width, (float)width * size, 0.f, 1.f);
}

void setFFTViewport(int width, int height, float size)
{
    size = max(size, 0.000000001f);
    grViewport(0, 0, (float)width, (float)(height - width * size), 0.f, 1.f);
}



void OsciloscopeManager::renderThread(uint threadId, OsciloscopeThreadData& threadData, OsciloscopeThreadRenderer& renderer, OsciloscopeFFT& fft)
{
    WndMain&                 wndMain = threadData.m_window;
    OsciloscopeRenderData&    render = threadData.m_render;
    SDisplay&                  frame = threadData.m_frame;
    MeasureData&             measure = measureData[threadId];
    ////////////////////////////////////////////////////////////////////////////////
    // begin
    ////////////////////////////////////////////////////////////////////////////////
    pRender->GetFlag(threadId).set(render.flags.get());
    pCanvas3d->threadBegin(threadId);
    pCanvas2d->threadBegin(threadId);
    pFont->threadStart(threadId);
    ////////////////////////////////////////////////////////////////////////////////
    // measure signal
    ////////////////////////////////////////////////////////////////////////////////
    renderer.measureSignal(threadId, threadData, measure, fft);
    ////////////////////////////////////////////////////////////////////////////////
    // Mode
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_THERMAL);
    pCanvas2d->setMode(threadId, RENDER_MODE_THERMAL);
    ////////////////////////////////////////////////////////////////////////////////
    // preOscRender
    ////////////////////////////////////////////////////////////////////////////////
    renderer.preOscRender(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // width/height
    ////////////////////////////////////////////////////////////////////////////////
    float width  = (float)pRender->width;
    float height = (float)pRender->height;
    ////////////////////////////////////////////////////////////////////////////////
    // heating
    ////////////////////////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        if(wndMain.horizontal.Mode != SIGNAL_MODE_PAUSE && wndMain.thermal.enabled)
        {
            ////////////////////////////////////////////////////////////////////////////////
            // Shadow Channel01
            ////////////////////////////////////////////////////////////////////////////////
            if(wndMain.channel01.OscOnOff)
            {
                renderer.renderAnalog(threadId, threadData, 0.f, 0, 1, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture, render.colorChannel0, wndMain.channel01.Invert);
            }
            ////////////////////////////////////////////////////////////////////////////////
            // Shadow Channel02
            ////////////////////////////////////////////////////////////////////////////////
            if(wndMain.channel02.OscOnOff)
            {
                renderer.renderAnalog(threadId, threadData, 0.f, 1, 1, frame, wndMain.horizontal.Capture, wndMain.channel02.Capture, render.colorChannel1, wndMain.channel02.Invert);
            }
        }
    }
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_COOLING);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_COOLING);
    ////////////////////////////////////////////////////////////////////////////////
    // cooling
    ////////////////////////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        if(wndMain.horizontal.Mode != SIGNAL_MODE_PAUSE && wndMain.thermal.enabled)
        {
            pCanvas2d->beginBatch(threadId, CANVAS2D_BATCH_RECTANGLEUV, 1);
            pCanvas2d->bRectangleUV(threadId, Vector4(0.f, 0.f, 0.f, 1.f), Vector4((float)render.width, (float)render.height, 1.f, 0.f));
            Vector4 vector;
            vector.x = window.thermal.cooling;
            vector.y = 0.f;
            vector.z = 0.f;
            vector.w = 0.f;
            pCanvas2d->addConstant(threadId, vector);
            pCanvas2d->endBatch(threadId, -1, render.shadowTexture, BLEND_MODE_COPY, SAMPLE_STATE_CANVAS2D, render.shadowCoolingShader);
        }
    }
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_BLEND_IN_HEATING);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_BLEND_IN_HEATING);
    ////////////////////////////////////////////////////////////////////////////////
    // blend
    ////////////////////////////////////////////////////////////////////////////////
    if(render.shadowTexture != 0)
    {
        int debug = 1;
    }
    pCanvas2d->beginBatch(threadId, CANVAS2D_BATCH_RECTANGLEUV, 1);
    pCanvas2d->bRectangleUV(threadId, Vector4(0.f, 0.f, 0.f, 1.f), Vector4((float)render.width, (float)render.height, 1.f, 0.f));
    pCanvas2d->endBatch(threadId, -1, render.shadowTexture, BLEND_MODE_ALPHA, SAMPLE_STATE_CANVAS2D, render.shadowColorShader);
    ////////////////////////////////////////////////////////////////////////////////
    // Mode
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_3D);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_3D);
    ////////////////////////////////////////////////////////////////////////////////
    // fps
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderFps(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // Signal 3d
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
    {
        if(threadData.m_historyCount > 0)
        {
            if(wndMain.channel01.OscOnOff || wndMain.channel02.OscOnOff || wndMain.function.OscOnOff)
            {
                if(threadData.m_historyCount > 0)
                {
                    float    zDelta = 1.f / (threadData.m_historyCount - 1);
                    float         z = 1.f;
                    for(uint i = 0; i < threadData.m_historyCount; i++)
                    {
                        if(wndMain.channel01.OscOnOff)
                        {
                            uint r = COLOR_R(render.colorChannel0);
                            uint g = COLOR_G(render.colorChannel0);
                            uint b = COLOR_B(render.colorChannel0);
                            renderer.renderAnalog3d(threadId, threadData, i, z, 0, threadData.m_history[i], wndMain.horizontal.Capture, wndMain.channel01.Capture, COLOR_ARGB(wndMain.display.alpha3dCh0, r, g, b), wndMain.channel01.Invert);
                        }
                        if(wndMain.channel02.OscOnOff)
                        {
                            uint r = COLOR_R(render.colorChannel1);
                            uint g = COLOR_G(render.colorChannel1);
                            uint b = COLOR_B(render.colorChannel1);
                            renderer.renderAnalog3d(threadId, threadData, i, z, 1, threadData.m_history[i], wndMain.horizontal.Capture, wndMain.channel01.Capture, COLOR_ARGB(wndMain.display.alpha3dCh1, r, g, b), wndMain.channel02.Invert);
                        }
                        if(wndMain.function.OscOnOff)
                        {
                            uint r = COLOR_R(render.colorFunction);
                            uint g = COLOR_G(render.colorFunction);
                            uint b = COLOR_B(render.colorFunction);
                            renderer.renderAnalogFunction3d(threadId, threadData, threadData.m_history[i], i, z, COLOR_ARGB(wndMain.display.alpha3dFun, r, g, b));
                        }
                        z -= zDelta;
                    }
                    if(wndMain.channel01.OscOnOff)
                    {
                        uint r = COLOR_R(render.colorChannel0);
                        uint g = COLOR_G(render.colorChannel0);
                        uint b = COLOR_B(render.colorChannel0);
                        renderer.renderSurface3d(threadId, threadData, 0, COLOR_ARGB(wndMain.display.alpha3dCh0, r, g, b));
                    }
                    if(wndMain.channel02.OscOnOff)
                    {
                        uint r = COLOR_R(render.colorChannel1);
                        uint g = COLOR_G(render.colorChannel1);
                        uint b = COLOR_B(render.colorChannel1);
                        renderer.renderSurface3d(threadId, threadData, 1, COLOR_ARGB(wndMain.display.alpha3dCh1, r, g, b));
                    }
                    if(wndMain.function.OscOnOff)
                    {
                        uint r = COLOR_R(render.colorFunction);
                        uint g = COLOR_G(render.colorFunction);
                        uint b = COLOR_B(render.colorFunction);
                        renderer.renderSurface3d(threadId, threadData, -1, COLOR_ARGB(wndMain.display.alpha3dFun, r, g, b));
                    }
                }
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Mode
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_2D);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_2D);
    ////////////////////////////////////////////////////////////////////////////////
    // Grid
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderAnalogGrid(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // Axis
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderAnalogAxis(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // Units
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderAnalogUnits(threadId, threadData);
    //////////////////////////////////////////////////////////////////////////////////
    //// ETS clear
    //////////////////////////////////////////////////////////////////////////////////
    //if(wndMain.horizontal.ETS)
    //{
    //    if(wndMain.channel01.OscOnOff)
    //    {
    //        if(frame.attr & ETS_CLEAR)
    //        {
    //            renderer.renderAnalog(threadId, threadData, 0.f, 0, 0, threadData.m_etsClear, wndMain.horizontal.Capture, wndMain.channel01.Capture, 0xff303030, wndMain.channel01.Invert);
    //        }
    //    }
    //    if(wndMain.channel02.OscOnOff)
    //    {
    //        if(frame.attr & ETS_CLEAR)
    //        {
    //            renderer.renderAnalog(threadId, threadData, 0.f, 1, 0, threadData.m_etsClear, wndMain.horizontal.Capture, wndMain.channel02.Capture, 0xff303030, wndMain.channel02.Invert);
    //        }
    //    }
    //}

    ////////////////////////////////////////////////////////////////////////////////
    // Signal
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.horizontal.ETS)
    {
       ////////////////////////////////////////////////////////////////////////////////
       // ETS
       ////////////////////////////////////////////////////////////////////////////////
       for (uint i = 0; i < threadData.m_historyCount; i++)
       {
          if (wndMain.channel01.OscOnOff)
          {
             renderer.renderAnalog(threadId, threadData, 0.f, 0, 0, threadData.m_history[i], wndMain.horizontal.Capture, wndMain.channel01.Capture, render.colorChannel0, wndMain.channel01.Invert);
          }
          if (wndMain.channel02.OscOnOff)
          {
             renderer.renderAnalog(threadId, threadData, 0.f, 1, 0, threadData.m_history[i], wndMain.horizontal.Capture, wndMain.channel02.Capture, render.colorChannel1, wndMain.channel02.Invert);
          }
          if (wndMain.function.OscOnOff)
          {
             renderer.renderAnalogFunction(threadId, threadData, 0.f, wndMain.function.Type, threadData.m_history[i], wndMain.horizontal.Capture, wndMain.channel01.Capture, wndMain.channel02.Capture, render.colorFunction, wndMain.channel01.Invert, wndMain.channel02.Invert);
          }
       }
    }
    else
    {
       ////////////////////////////////////////////////////////////////////////////////
       // Signal Channel01
       ////////////////////////////////////////////////////////////////////////////////
       if (wndMain.channel01.OscOnOff)
       {
          renderer.renderAnalog(threadId, threadData, 0.f, 0, 0, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture, render.colorChannel0, wndMain.channel01.Invert);
       }
       ////////////////////////////////////////////////////////////////////////////////
       // Signal Channel02
       ////////////////////////////////////////////////////////////////////////////////
       if (wndMain.channel02.OscOnOff)
       {
          renderer.renderAnalog(threadId, threadData, 0.f, 1, 0, frame, wndMain.horizontal.Capture, wndMain.channel02.Capture, render.colorChannel1, wndMain.channel02.Invert);
       }
       ////////////////////////////////////////////////////////////////////////////////
       // Function
       ////////////////////////////////////////////////////////////////////////////////
       if (wndMain.function.OscOnOff)
       {
          renderer.renderAnalogFunction(threadId, threadData, 0.f, wndMain.function.Type, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture, wndMain.channel02.Capture, render.colorFunction, wndMain.channel01.Invert, wndMain.channel02.Invert);
       }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // FunctionXY
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.function.xyGraph)
    {
        renderer.renderAnalogFunctionXY(threadId, threadData, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture, wndMain.channel02.Capture, render.colorXyGraph);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Mode
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_MEASURE);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_MEASURE);
    ////////////////////////////////////////////////////////////////////////////////
    // measure
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderMeasure(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // preFftRender
    ////////////////////////////////////////////////////////////////////////////////
    renderer.preFftRender(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // Mode
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_FFT3D);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_FFT3D);
    renderer.renderMeasureFFT(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // FFT 3d
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        if(threadData.m_historyCount > 0)
        {
            int framesCount = threadData.m_historyCount;
            float    zDelta = 1.f / framesCount;
            float         z = 1.f;
            for(uint i = 0; i < (uint)framesCount; i++)
            {
                if(wndMain.channel01.FFTOnOff)
                {
                    uint r = COLOR_R(render.colorChannel0);
                    uint g = COLOR_G(render.colorChannel0);
                    uint b = COLOR_B(render.colorChannel0);
                    renderer.renderFFT(threadId, threadData, measure, fft, threadData.m_history[i], false, z, 0, COLOR_ARGB(25, r, g, b));
                }
                if(wndMain.channel02.FFTOnOff)
                {
                    uint r = COLOR_R(render.colorChannel1);
                    uint g = COLOR_G(render.colorChannel1);
                    uint b = COLOR_B(render.colorChannel1);
                    renderer.renderFFT(threadId, threadData, measure, fft, threadData.m_history[i], false, z, 1, COLOR_ARGB(25, r, g, b));
                }
                if(wndMain.function.FFTOnOff)
                {
                    uint r = COLOR_R(render.colorFunction);
                    uint g = COLOR_G(render.colorFunction);
                    uint b = COLOR_B(render.colorFunction);
                    renderer.renderFFT(threadId, threadData, measure, fft, threadData.m_history[i], true, z, 0, COLOR_ARGB(25, r, g, b));
                }
                z -= zDelta;
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Mode
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_FFT2D);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_FFT2D);
    ////////////////////////////////////////////////////////////////////////////////
    // FFT Grid
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderFFTGrid(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // FFT Units
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderFFTUnits(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // FFT Axis
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderFFTAxis(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // FFT
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.fftDigital.is(VIEW_SELECT_FFT_2D))
    {
        if(wndMain.channel01.FFTOnOff)
        {
            renderer.renderFFT(threadId, threadData, measure, fft, threadData.m_frame, false, 0.f, 0, render.colorChannel0);
        }
        if(wndMain.channel02.FFTOnOff)
        {
            renderer.renderFFT(threadId, threadData, measure, fft, threadData.m_frame, false, 0.f, 1, render.colorChannel1);
        }
        if(wndMain.function.FFTOnOff)
        {
            renderer.renderFFT(threadId, threadData, measure, fft, threadData.m_frame, true, 0.f, 1, render.colorFunction);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // slider
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_SLIDER);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_SLIDER);
    renderer.renderSlider(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // end
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->threadEnd(threadId);
    pCanvas2d->threadEnd(threadId);
}

void OsciloscopeManager::renderMain(uint threadId)
{
    ////////////////////////////////////////////////////////////////////////////////
    // setup
    ////////////////////////////////////////////////////////////////////////////////
    grSetRenderTarget(depth, color);
    grClearDepthStencil(depth, 0, 1.f, 0);
    ////////////////////////////////////////////////////////////////////////////////
    // clear
    ////////////////////////////////////////////////////////////////////////////////
    Flag64 flags = pRender->GetFlag(threadId);
    if(flags.is(rfClearRenderTarget))
    {
        grClearRenderTarget(0, renderData.colorBackground);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // begin
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->mainBegin(threadId);
    pCanvas2d->mainBegin(threadId);
    ////////////////////////////////////////////////////////////////////////////////
    // depth stencil
    ////////////////////////////////////////////////////////////////////////////////
    grStateDepthStencil(DEPTH_STENCIL_OFF);
    grStateRasterizer(RASTERIZER_CULLNONE);
    ////////////////////////////////////////////////////////////////////////////////
    // frame [0-1]
    ////////////////////////////////////////////////////////////////////////////////
    int shadowFrame0 =  pTimer->getFrame(0) % 2;
    int shadowFrame1 = (pTimer->getFrame(0) + 1) % 2;
    if(window.horizontal.Mode == SIGNAL_MODE_PAUSE)
    {
        shadowFrame0 = 0;
        shadowFrame1 = 1;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // clear
    ////////////////////////////////////////////////////////////////////////////////
    if((!window.thermal.enabled || SDL_AtomicGet(&clearThermal)) && grGetMode() == OPENGL_MODE_32)
    {
        SDL_AtomicSet(&clearThermal, 0);
        grSetRenderTarget(depth, aShadow[0]);
        grClearRenderTarget(aShadow[0], 0x00000000);
        grSetRenderTarget(depth, aShadow[1]);
        grClearRenderTarget(aShadow[1], 0x00000000);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // width, height
    ////////////////////////////////////////////////////////////////////////////////
    float width = (float)pRender->width;
    float height = (float)pRender->height;
    ////////////////////////////////////////////////////////////////////////////////
    // setup - heating
    ////////////////////////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        grSetRenderTarget(0, aShadow[shadowFrame0]);
        setAnalogViewport(width, height, analogWindowSize);
        ////////////////////////////////////////////////////////////////////////////////
        // render - heating
        ////////////////////////////////////////////////////////////////////////////////
        if(window.horizontal.Mode != SIGNAL_MODE_PAUSE && window.thermal.enabled)
        {
            pCanvas3d->render(threadId, RENDER_MODE_THERMAL);
            pCanvas2d->render(threadId, RENDER_MODE_THERMAL);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // copy
    ////////////////////////////////////////////////////////////////////////////////
    if(window.horizontal.Mode != SIGNAL_MODE_PAUSE)
    {
        grViewport(0, 0, width, height, 0.f, 1.f);
        grCopyRenderTarget(aShadow[shadowFrame1], aShadow[shadowFrame0]);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // cooling
    ////////////////////////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        grSetRenderTarget(depth, aShadow[shadowFrame1]);
        if(window.horizontal.Mode != SIGNAL_MODE_PAUSE && window.thermal.enabled)
        {
            pCanvas2d->render(threadId, RENDER_MODE_COLOR_COOLING);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // setup - color
    ////////////////////////////////////////////////////////////////////////////////
    grSetRenderTarget(depth, color);
    grViewport(0, 0, width, height, 0.f, 1.f);
    ////////////////////////////////////////////////////////////////////////////////
    // blend in heating
    ////////////////////////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        pCanvas2d->render(threadId, RENDER_MODE_COLOR_BLEND_IN_HEATING);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // depth test on / off
    ////////////////////////////////////////////////////////////////////////////////
    if(window.display.depthTest3d)
    {
        grStateDepthStencil(DEPTH_ON);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // setup - analog 3d
    ////////////////////////////////////////////////////////////////////////////////
    setAnalogViewport(width, height, analogWindowSize);
    pCanvas3d->dirLight = Vector4(-cameraOsc.ortho.View.RowZ().x, -cameraOsc.ortho.View.RowZ().y, -cameraOsc.ortho.View.RowZ().z, 0);
    ////////////////////////////////////////////////////////////////////////////////
    // render - analog 3d
    ////////////////////////////////////////////////////////////////////////////////
    grStateRasterizer(RASTERIZER_CULLNONE);
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_ANALOG_3D);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_ANALOG_3D);
    ////////////////////////////////////////////////////////////////////////////////
    // cull off
    ////////////////////////////////////////////////////////////////////////////////
    grStateRasterizer(RASTERIZER_CULLNONE);
    pCanvas3d->dirLight = Vector4(0, 0.f, 1.f, 0.f);
    ////////////////////////////////////////////////////////////////////////////////
    // render - analog 3d
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_ANALOG_2D);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_ANALOG_2D);
    ////////////////////////////////////////////////////////////////////////////////
    // setup - measure
    ////////////////////////////////////////////////////////////////////////////////
    grStateDepthStencil(DEPTH_ON);
    ////////////////////////////////////////////////////////////////////////////////
    // render - measure
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_MEASURE);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_MEASURE);
    ////////////////////////////////////////////////////////////////////////////////
    // setup - digital
    ////////////////////////////////////////////////////////////////////////////////
    grStateDepthStencil(DEPTH_STENCIL_OFF);
    if(window.display.depthTest3d)
    {
        grStateDepthStencil(DEPTH_ON);
    }
    setFFTViewport(width, height, analogWindowSize);
    pCanvas3d->dirLight = Vector4(0, -1.f, 0.f, 0.f);
    ////////////////////////////////////////////////////////////////////////////////
    // render - digital
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_DIGITAL);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_DIGITAL);
    ////////////////////////////////////////////////////////////////////////////////
    // render - FFT3D
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_FFT3D);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_FFT3D);
    ////////////////////////////////////////////////////////////////////////////////
    // setup - FFT2D
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->dirLight = Vector4(0, 0.f, 1.f, 0.f);
    ////////////////////////////////////////////////////////////////////////////////
    // render - FFT2D
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_FFT2D);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_FFT2D);
    ////////////////////////////////////////////////////////////////////////////////
    // setup - slider
    ////////////////////////////////////////////////////////////////////////////////
    grViewport(0, 0, (float)width, (float)height, -1.f, 1.f);
    ////////////////////////////////////////////////////////////////////////////////
    // render - slider
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_SLIDER);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_SLIDER);
    ////////////////////////////////////////////////////////////////////////////////
    // end
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->mainEnd(threadId);
    pCanvas2d->mainEnd(threadId);
    ////////////////////////////////////////////////////////////////////////////////
    // output to screen
    ////////////////////////////////////////////////////////////////////////////////
    grCopyToBackBuffer(color, 0, 0, pRender->width, pRender->height);
    int ret = SDL_GL_SetSwapInterval(0);
    SDL_GL_SwapWindow(sdlWindow);
}

int OsciloscopeManager::stop()
{
    #ifndef PLATFORM_MAC
    // close renderer
    SDL_GL_DeleteContext(pOsciloscope->sdlContext);
    SDL_DestroyWindow(pOsciloscope->sdlWindow);
    #endif
    //////////////////////////////////////////////////
    // cleanup
    //////////////////////////////////////////////////
    pRender->stop();
    pCanvas2d->stop();
    pCanvas3d->stop();
    pFont->stop();
    deallocate();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// uploadFirmware
//
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//
// callbacks
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeManager::onApplicationInit()
{
    pOsciloscope->setThreadPriority(THREAD_ID_MAIN);
    int renderWidth  = pRender->width;
    int renderHeight = pRender->height;
    int ret = SDL_GL_SetSwapInterval(0);
    color = grCreateRenderTarget(RENDER_TARGET_TYPE_RGBA,  renderWidth, renderHeight);
    if(settings.getSettings()->renderDepthBuffer)
    {
        depth = grCreateDepthStencil(DEPTH_STENCIL_TYPE_Z32, renderWidth, renderHeight);
    }
    else
    {
        depth = 0;
    }
    for(int i = 0; i < 2; i++)
    {
        aShadow[i] = grCreateRenderTarget(RENDER_TARGET_TYPE_FLOAT32, renderWidth, renderHeight);
    }
}

int SDLCALL EventFilter(void* userdata, SDL_Event* event)
{
    pInput->onEvent(*event);
    if(event->type == SDL_SYSWMEVENT)
    {
        return 1;
    }
    return 0;
}

bool OsciloscopeManager::onApplicationIdle()
{
    // lock
    SDL_AtomicLock(&renderLock);
    if(SDL_AtomicGet(&signalMode) != SIGNAL_MODE_PAUSE)
    {
        window.horizontal.Frame = renderWindow.horizontal.Frame;
    }
    // sync user interface -> capture
    renderWindow = window;
    renderData.cameraFFT = cameraFFT.ortho;
    renderData.cameraOsc = cameraOsc.ortho;
    renderData.width = pRender->width;
    renderData.height = pRender->height;
    renderData.sliderMode = sliderMode;
    renderData.sliderSize = analogWindowSize;
    renderData.fftScaleX = fftScaleX;
    renderData.fftScaleY = fftScaleY;
    renderData.oscScaleX = oscScaleX;
    renderData.oscScaleY = oscScaleY;
    renderData.zoomFFT = cameraFFT.zoom;
    renderData.zoomOsc = cameraOsc.zoom;
    renderData.signalPosition = signalPosition;
    renderData.signalZoom     = signalZoom;
    renderData.sliderPosition = sliderPosition;
    renderData.maxEts         = settings.getHardware()->fpgaEtsCount;
    renderData.shadowTexture  = aShadow[pTimer->getFrame(0) % 2];
    renderData.shadowLine3dShader = shadowLine3dShader;
    renderData.shadowColorShader = shadowColorShader;
    renderData.shadowCoolingShader = shadowCoolingShader;
    renderData.analogChannelYVoltageStep0 = settings.getHardware()->getAnalogStep(window.horizontal.Capture, 0, window.channel01.Capture);
    renderData.analogChannelYVoltageStep1 = settings.getHardware()->getAnalogStep(window.horizontal.Capture, 1, window.channel02.Capture);
    renderData.analogChannelOffsets0 = settings.getHardware()->getAnalogOffsetDouble(window.horizontal.Capture, 0, window.channel01.Capture);
    renderData.analogChannelOffsets1 = settings.getHardware()->getAnalogOffsetDouble(window.horizontal.Capture, 1, window.channel02.Capture);
    renderData.analogChannelPositin0 = sfGetYPositionA(getHw());
    renderData.analogChannelPositin1 = sfGetYPositionB(getHw());
    renderData.colorBackground = settings.getColors()->renderBackground;
    renderData.colorTime       = settings.getColors()->renderTime;
    renderData.colorChannel0   = settings.getColors()->renderChannel0;
    renderData.colorChannel1   = settings.getColors()->renderChannel1;
    renderData.colorFunction   = settings.getColors()->renderFunction;
    renderData.colorXyGraph    = settings.getColors()->renderXyGraph;
    renderData.colorGrid       = settings.getColors()->renderGrid;
    renderData.colorBorder     = settings.getColors()->renderBorder;
    renderData.colorTrigger    = settings.getColors()->renderTrigger;
    renderData.colorDigital    = settings.getColors()->renderDigital;
    renderData.etsAttr         = 0;
    renderData.flags.bit(rfClearRenderTarget, SDL_AtomicGet(&clearRenderTarget));
    // unlock
    SDL_AtomicUnlock(&renderLock);

    // lock
    uint screenIndex  = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_screenIndex);
    uint frameCount   = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_frameCount);
    uint threadCount  = SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_threadCount);

    // thread id
    uint threadId = screenIndex % threadCount;

    // lock
    if( pOsciloscope->m_captureBuffer.lockScreen(screenIndex) )
    {
        // timer
        pTimer->deltaTime(TIMER_RENDER);

        // render
        renderMain(threadId);

        // measure
        if (window.measure.uiOpen)
        {
            window.measure.data.pick = measureData[threadId].pick;
            // current
            window.measure.data.column[MEASURE_CURRENT] = measureData[threadId].column[MEASURE_CURRENT];
            if (measureData[threadId].column[MEASURE_CURRENT].averageN > 0)
            {
                // min, max
                window.measure.data.column[MEASURE_MINIMUM].Minimum(measureData[threadId].column[MEASURE_CURRENT]);
                window.measure.data.column[MEASURE_MAXIMUM].Maximum(measureData[threadId].column[MEASURE_CURRENT]);
                // save history
                window.measure.data.history[window.measure.data.historyIndex] = measureData[threadId].column[MEASURE_CURRENT];
                // average
                window.measure.data.Average(window.measure.data.column[MEASURE_AVERAGE]);
            }
        }

        // inc
        SDL_AtomicSet(&pOsciloscope->m_captureBuffer.m_screenIndex, screenIndex + 1);

        // unlock
        pOsciloscope->m_captureBuffer.unlockScreen(screenIndex);

    }
    return false;
}

void OsciloscopeManager::onResize(int width, int height, int oldWidth, int oldHeight)
{
    grResizeRenderTarget(color,  width, height);
    if(settings.getSettings()->renderDepthBuffer)
    {
        grResizeDepthStencil(depth, width, height);
    }
    for(int i = 0; i < 2; i++)
    {
        grResizeRenderTarget(aShadow[i], width, height);
    }
    float oldAspect = float(oldWidth) / float(oldHeight);
    float newAspect = float(width)    / float(height);
    analogWindowSize = analogWindowSize * (oldAspect / newAspect);
}

void maximizeOglWindow(int& w, int& h);

int OsciloscopeManager::onEvent(SDL_Event& event)
{
    if(event.window.type == SDL_WINDOWEVENT)
    {
        if(event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
        {
        }
        if(event.window.event == SDL_WINDOWEVENT_RESTORED)
        {
        }
        if(event.window.event == SDL_WINDOWEVENT_ENTER)
        {
            openglFocus = 1;
            sliderMode  = 0;
        }
        if(event.window.event == SDL_WINDOWEVENT_LEAVE)
        {
            openglFocus = 0;
            sliderMode  = 0;
        }
    }
    return 0;
}

void OsciloscopeManager::onCalibrateFrameCaptured(SDisplay& display, int version)
{
    if(calibrate.active)
    {
        uint numSamples = display.samples;
        calibrate.frame++;
        int framesPerCapture = 0;
        switch(calibrate.mode)
        {
            case acOffsetCapture:
            case acGainCapture:
            case acGeneratorCapture:
            case acStepCapture:
                framesPerCapture = settings.getHardware()->referenceFramesPerCapture;
                break;
            default:
                calibrate.frame = 0;
        };
        if(calibrate.frame >= framesPerCapture)
        {
            switch(calibrate.mode)
            {
                case acStartMessageBox:
                    {
                        if(calibrate.messageBox == acmbOk)
                        {
                            calibrate.mode    = acOffsetStart;
                            calibrate.type    = ctNormal;
                            calibrate.channel = 0;
                            calibrate.debug << "acStartMessageBox: ok \n";
                        }
                        if(calibrate.messageBox == acmbCancel)
                        {
                            calibrate.active  = 0;
                            calibrate.reset();
                            calibrate.debug << "acStartMessageBox: cancel \n";
                        }
                    }
                    break;
                case acOffsetStart:
                    {
                        calibrate.voltage = vc2Volt;
                        calibrate.offsetMax = settings.getHardware()->referenceOffsetMaxValue;
                        calibrate.offsetMin = settings.getHardware()->referenceOffsetMinValue;
                        calibrate.iteration = 0;
                        calibrate.mode   = acOffsetSetup;
                        calibrate.debug << "acOffsetStart";
                        calibrate.debug << "\n";
                    }
                    break;
                case acOffsetSetup:
                    {
                        calibrate.offset = calibrate.offsetMin + (calibrate.offsetMax - calibrate.offsetMin) / 2.0;
                        sfSetYRangeScaleA(getHw(), (uint)getAttr(calibrate.voltage), settings.getHardware()->referenceGainValue[calibrate.voltage]);
                        sfSetYRangeScaleB(getHw(), (uint)getAttr(calibrate.voltage), settings.getHardware()->referenceGainValue[calibrate.voltage]);
                        sfSetYPositionA(getHw(), calibrate.offset);
                        sfSetYPositionB(getHw(), calibrate.offset);
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_A_GROUND, 1);
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_B_GROUND, 1);
                        transferData();
                        calibrate.mode = acOffsetCapture;
                        calibrate.debug << "acOffsetSetup:";
                        calibrate.debug << "  setYPositionAB: ";
                        calibrate.debug << calibrate.offset;
                        calibrate.debug << "\n";
                        calibrate.debug << "  offsetMin: ";
                        calibrate.debug << calibrate.offsetMin;
                        calibrate.debug << "\n";
                        calibrate.debug << "  offsetMax: ";
                        calibrate.debug << calibrate.offsetMax;
                        calibrate.debug << "\n";
                    }
                    break;
                case acOffsetCapture:
                    {
                        int ch = calibrate.channel;
                        calibrate.offsetMeasured = 0;
                        for(int sample = 0; sample < numSamples; sample++)
                        {
                            if(ch == 0) { calibrate.offsetMeasured += display.analog0.bytes[sample]; }
                            if(ch == 1) { calibrate.offsetMeasured += display.analog1.bytes[sample]; }
                        }
                        calibrate.offsetMeasured /= double(numSamples);
                        calibrate.mode = acOffsetCalculate;
                        calibrate.debug << "acOffsetCapture:";
                        calibrate.debug << "  offsetMeasured: ";
                        calibrate.debug << calibrate.offsetMeasured;
                        calibrate.debug << "\n";
                    }
                    break;
                case acOffsetCalculate:
                    {
                        double range = fabsf(calibrate.offsetMax - calibrate.offsetMin) / 5.0;
                        calibrate.debug << "acOffsetCalculate:";
                        if((fabs(calibrate.offsetMeasured) < 0.00025) || (calibrate.iteration >= settings.getHardware()->referenceMaxIterations) || (range < 0.001))
                        {
                            calibrate.debug << "  calibratedOffset: ";
                            calibrate.debug << double(calibrate.offset);
                            calibrate.debug << "\n";
                            calibrate.debug << "  calibratedIteration: ";
                            calibrate.debug << calibrate.iteration;
                            calibrate.debug << "\n";
                            settings.getHardware()->calibratedOffsets[calibrate.type][calibrate.channel][calibrate.voltage] = round(calibrate.offset);
                            calibrate.mode = acOffsetVoltageChange;
                        }
                        else
                        {
                            calibrate.debug << "  OffsetStep: ";
                            calibrate.debug << range;
                            calibrate.debug << "\n";
                            if(calibrate.offsetMeasured > 0.f)
                            {
                                calibrate.offsetMin += range;
                            }
                            else
                            {
                                calibrate.offsetMax -= range;
                            }
                            calibrate.iteration++;
                            calibrate.mode = acOffsetSetup;
                        }
                    }
                    break;
                case acOffsetVoltageChange:
                    {
                        calibrate.offsetMax = settings.getHardware()->referenceOffsetMaxValue;
                        calibrate.offsetMin = settings.getHardware()->referenceOffsetMinValue;
                        if(calibrate.voltage == vc10Mili)
                        {
                            if(calibrate.channel == 0)
                            {
                                calibrate.channel = 1;
                                calibrate.mode = acOffsetStart;
                            }
                            else
                            {
                                if(calibrate.type == ctNormal)
                                {
                                    calibrate.channel = 0;
                                    sfSetAnalogSwitchBit(getHw(), CHANNEL_INTERLEAVE, 1);
                                    calibrate.type = ct500Mhz;
                                    calibrate.mode = acOffsetStart;
                                }
                                else // 500 Mhz
                                {
                                    calibrate.channel = 0;
                                    calibrate.mode    = acOffsetEnd;
                                }
                            }
                        }
                        else
                        {
                            calibrate.iteration = 0;
                            calibrate.voltage   = (VoltageCapture)((int)calibrate.voltage + 1);
                            calibrate.offsetMax = settings.getHardware()->referenceOffsetMaxValue;
                            calibrate.offsetMin = settings.getHardware()->referenceOffsetMinValue;
                            calibrate.mode = acOffsetSetup;
                        }
                        calibrate.debug << "acOffsetVoltageChange:";
                        calibrate.debug << "  voltage: ";
                        calibrate.debug << calibrate.voltage;
                        calibrate.debug << "\n";
                    }
                    break;
                case acOffsetEnd:
                    {
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_INTERLEAVE, 0);
                        calibrate.type = ctNormal;
                        calibrate.messageBox = acmbChannel00;
                        calibrate.mode = acGeneratorMessageBox;
                        calibrate.debug << "acOffsetEnd: \n";
                    }
                    break;
                case acGeneratorMessageBox:
                    {
                        if(calibrate.messageBox == acmbOk)
                        {
                            calibrate.mode = acGeneratorStart;
                            calibrate.debug << "acGeneratorMessageBox: ok \n";
                        }
                        if(calibrate.messageBox == acmbCancel)
                        {
                            calibrate.active  = 0;
                            calibrate.reset();
                            calibrate.debug << "acGeneratorMessageBox: cancel \n";
                        }
                    }
                    break;
                case acGeneratorStart:
                    {
                        calibrate.gainMode = 0;
                        calibrate.voltage = vc50Mili;
                        // Generator functionality removed; no hardware API calls
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_A_GROUND, 0);
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_B_GROUND, 0);
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_A_ACDC, 1);
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_B_ACDC, 1);
                        sfSetYPositionA(getHw(), window.channel01.YPosition + settings.getHardware()->calibratedOffsets[calibrate.type][0][calibrate.voltage]);
                        sfSetYPositionB(getHw(), window.channel02.YPosition + settings.getHardware()->calibratedOffsets[calibrate.type][1][calibrate.voltage]);
                        sfSetYRangeScaleA(getHw(), (uint)getAttr(calibrate.voltage), settings.getHardware()->referenceGainValue[calibrate.voltage]);
                        sfSetYRangeScaleB(getHw(), (uint)getAttr(calibrate.voltage), settings.getHardware()->referenceGainValue[calibrate.voltage]);
                        transferData();
                        calibrate.iteration    = 0;
                        calibrate.generatorMax = settings.getHardware()->referenceGeneratorMaxValue;
                        calibrate.generatorMin = settings.getHardware()->referenceGeneratorMinValue;
                        calibrate.mode = acGeneratorSetup;
                        calibrate.debug << "acGeneratorStart: \n";
                        calibrate.debug << "  generatorMin: ";
                        calibrate.debug << calibrate.generatorMin;
                        calibrate.debug << "\n";
                        calibrate.debug << "  generatorMax: ";
                        calibrate.debug << calibrate.generatorMax;
                        calibrate.debug << "\n";
                    }
                    break;
                case acGeneratorSetup:
                    {
                        calibrate.generatorOffset = (calibrate.generatorMin + calibrate.generatorMax) / 2.0;
                        sfSetYPositionA(getHw(), settings.getHardware()->calibratedOffsets[calibrate.type][0][calibrate.voltage]);
                        sfSetYPositionB(getHw(), settings.getHardware()->calibratedOffsets[calibrate.type][1][calibrate.voltage]);
                        // Generator functionality removed: offset set skipped
                        (void)calibrate.generatorOffset;
                        transferData();
                        calibrate.mode = acGeneratorCapture;
                        calibrate.debug << "acGeneratorSetup: \n";
                        calibrate.debug << "  generatorOffset: ";
                        calibrate.debug << calibrate.generatorOffset;
                        calibrate.debug << "\n";
                    }
                    break;
                case acGeneratorCapture:
                    {
                        calibrate.offsetMeasured = 0;
                        for(int sample = 0; sample < numSamples; sample++)
                        {
                            calibrate.offsetMeasured += display.analog0.bytes[sample];
                        }
                        calibrate.offsetMeasured /= double(numSamples);
                        calibrate.offsetMeasured *= double(SCOPEFUN_MAX_VOLTAGE);
                        calibrate.mode = acGeneratorCalculate;
                        calibrate.debug << "acGeneratorCapture: \n";
                        calibrate.debug << "  offsetMeasured: ";
                        calibrate.debug << calibrate.offsetMeasured;
                        calibrate.debug << "\n";
                    }
                    break;
                case acGeneratorCalculate:
                    {
                        int range = max((int)(fabsf(calibrate.offsetMeasured) / 6.0), 1);
                        double percentage = 100 * (calibrate.offsetMeasured / SCOPEFUN_MAX_VOLTAGE);
                        if(fabsf(percentage) < settings.getHardware()->referenceExitPercentage || calibrate.iteration >= settings.getHardware()->referenceMaxIterations)
                        {
                            settings.getHardware()->calibratedOffsetsGenerator[ctNormal][calibrate.channel] = (calibrate.generatorMin + calibrate.generatorMax) / 2.0;
                            settings.getHardware()->calibratedOffsetsGenerator[ct500Mhz][calibrate.channel] = (calibrate.generatorMin + calibrate.generatorMax) / 2.0;
                            calibrate.mode = acGeneratorEnd;
                        }
                        else
                        {
                            if(calibrate.offsetMeasured < 0.f)
                            {
                                calibrate.generatorMin += range;
                            }
                            else
                            {
                                calibrate.generatorMax -= range;
                            }
                            calibrate.iteration++;
                            calibrate.mode = acGeneratorSetup;
                        }
                        calibrate.debug << "acGeneratorCalculate: \n";
                        calibrate.debug << "  percentage: ";
                        calibrate.debug << percentage;
                        calibrate.debug << "\n";
                    }
                    break;
                case acGeneratorEnd:
                    {
                        if(calibrate.channel == 0)
                        {
                            calibrate.iteration  = 0;
                            calibrate.channel    = 1;
                            calibrate.voltage    = vc2Volt;
                            calibrate.mode       = acGeneratorMessageBox;
                            calibrate.messageBox = acmbChannel10;
                            calibrate.debug << "acGeneratorEnd: channel 1 \n";
                        }
                        else
                        {
                            calibrate.iteration  = 0;
                            calibrate.voltage    = vc2Volt;
                            calibrate.mode       = acGainMsgBoxCh0;
                            calibrate.messageBox = acmbChannel00;
                            calibrate.debug << "acGeneratorEnd: gain \n";
                        }
                    }
                    break;
                case acGainMsgBoxCh0:
                    {
                        if(calibrate.messageBox == acmbOk)
                        {
                            calibrate.channel = 0;
                            calibrate.mode    = acGainStart;
                            calibrate.debug << "acGainMsgBoxCh0: ok \n";
                        }
                        if(calibrate.messageBox == acmbCancel)
                        {
                            calibrate.active  = 0;
                            calibrate.reset();
                            calibrate.debug << "acGainMsgBoxCh0: cancel \n";
                        }
                    }
                    break;
                case acGainStart:
                    {
                        calibrate.voltage   = vc2Volt;
                        calibrate.iteration = 0;
                        calibrate.mode = acGainSetup;
                        calibrate.gainMax   = settings.getHardware()->referenceGainMax[calibrate.voltage];
                        calibrate.gainMin   = settings.getHardware()->referenceGainMin[calibrate.voltage];
                        calibrate.gainSet        = calibrate.gainMin + (calibrate.gainMax - calibrate.gainMin) / 2;
                        calibrate.gainSetPrevius = calibrate.gainMax;
                        calibrate.debug << "acGainStart: \n";
                        calibrate.debug << "  gainMin: ";
                        calibrate.debug << calibrate.gainMin;
                        calibrate.debug << "\n";
                        calibrate.debug << "  gainMax: ";
                        calibrate.debug << calibrate.gainMax;
                        calibrate.debug << "\n";
                    }
                    break;
                case acGainSetup:
                    {
                        printf("acGainSetup: %i, ", (int)calibrate.type);
                        double gainGenOffset = settings.getHardware()->referenceGainVoltage[calibrate.voltage] / settings.getHardware()->referenceGeneratorVoltagePerStep;
                        if(calibrate.type == ct500Mhz)
                        {
                            sfSetXRange(getHw(), 0);
                            sfSetAnalogSwitchBit(getHw(), CHANNEL_INTERLEAVE, 1);
                        }
                        else
                        {
                            sfSetXRange(getHw(), 5);
                            sfSetAnalogSwitchBit(getHw(), CHANNEL_INTERLEAVE, 0);
                        }
                        sfSetYPositionA(getHw(), settings.getHardware()->calibratedOffsets[calibrate.type][0][calibrate.voltage]);
                        sfSetYPositionB(getHw(), settings.getHardware()->calibratedOffsets[calibrate.type][1][calibrate.voltage]);
                        // Generator functionality removed: offsets skipped
                        (void)gainGenOffset;
                        sfSetYRangeScaleA(getHw(), (uint)getAttr(calibrate.voltage), (ushort)calibrate.gainSet);
                        sfSetYRangeScaleB(getHw(), (uint)getAttr(calibrate.voltage), (ushort)calibrate.gainSet);
                        transferData();
                        printf("gainSet: %i\n", calibrate.gainSet);
                        calibrate.mode = acGainCapture;
                        calibrate.debug << "acGainSetup: ";
                        calibrate.debug << (int)calibrate.type;
                        calibrate.debug << "\n";
                        calibrate.debug << "  gainSet: ";
                        calibrate.debug << calibrate.gainSet;
                        calibrate.debug << "\n";
                    }
                    break;
                case acGainCapture:
                    {
                        int          ch = calibrate.channel;
                        double yGridMax = double(grid.yCount) / 2.0;
                        calibrate.gainVoltageUp = 0;
                        for(int sample = 0; sample < SCOPEFUN_DISPLAY; sample++)
                        {
                            if(ch == 0) { calibrate.gainVoltageUp += display.analog0.bytes[sample]; }
                            if(ch == 1) { calibrate.gainVoltageUp += display.analog1.bytes[sample]; }
                        }
                        calibrate.gainVoltageUp /= double(SCOPEFUN_DISPLAY);
                        calibrate.gainVoltageUp *= yGridMax * double(captureVoltFromEnum(calibrate.voltage));
                        printf("acGainCapture:  %f\n", calibrate.gainVoltageUp);
                        calibrate.mode = acGainCalculate;
                        calibrate.debug << "acGainCapture: ";
                        calibrate.debug <<    calibrate.gainVoltageUp;
                        calibrate.debug << "\n";
                    }
                    break;
                case acGainCalculate:
                    {
                        double  up = calibrate.gainVoltageUp;
                        double ref = settings.getHardware()->referenceGainVoltage[calibrate.voltage];
                        calibrate.percentage = 100.0 * (up / ref);
                        if(fabsf(calibrate.percentage - 100.0) < settings.getHardware()->referenceExitPercentage || calibrate.iteration >= settings.getHardware()->referenceMaxIterations)
                        {
                            settings.getHardware()->calibratedGainValue[calibrate.type][calibrate.channel][calibrate.voltage] = calibrate.gainSet;
                            calibrate.mode = acGainVoltageChange;
                        }
                        else
                        {
                            calibrate.iteration++;
                            int gain = calibrate.gainSetPrevius;
                            calibrate.gainSetPrevius = calibrate.gainSet;
                            int delta = abs(gain - calibrate.gainSet) / 2;
                            delta = max(1, (int)abs(calibrate.percentage - 100));
                            if(calibrate.percentage > 100.f)
                            {
                                calibrate.gainSet -= delta;
                            }
                            else
                            {
                                calibrate.gainSet += delta;
                            }
                            calibrate.mode = acGainSetup;
                        }
                        printf("acGainCalculate percentage: %f, iteration: %i\n", calibrate.percentage, calibrate.iteration);
                        calibrate.debug << "acGainCalculate: ";
                        calibrate.debug << "  percentage: ";
                        calibrate.debug <<     calibrate.percentage - 100.0;
                        calibrate.debug << "\n";
                        calibrate.debug << "  iteration: ";
                        calibrate.debug <<     calibrate.iteration;
                        calibrate.debug << "\n";
                    }
                    break;
                case acGainVoltageChange:
                    {
                        calibrate.iteration = 0;
                        calibrate.gainMax        = settings.getHardware()->referenceGainMax[calibrate.voltage];
                        calibrate.gainMin        = settings.getHardware()->referenceGainMin[calibrate.voltage];
                        calibrate.gainSet        = calibrate.gainMin + (calibrate.gainMax - calibrate.gainMin) / 2;
                        calibrate.gainSetPrevius = calibrate.gainMax;
                        if(calibrate.voltage == vc10Mili)
                        {
                            if(calibrate.gainMode == 0)
                            {
                                calibrate.voltage = vc2Volt;
                                calibrate.type    = ct500Mhz;
                                calibrate.mode    = acGainSetup;
                            }
                            if(calibrate.gainMode == 1)
                            {
                                calibrate.type       = ctNormal;
                                calibrate.voltage    = vc2Volt;
                                calibrate.messageBox = acmbChannel01;
                                calibrate.mode       = acGainMsgBoxCh1;
                                calibrate.channel    = 1;
                            }
                            if(calibrate.gainMode == 2)
                            {
                                calibrate.type       = ct500Mhz;
                                calibrate.voltage    = vc2Volt;
                                calibrate.messageBox = acmbChannel00;
                                calibrate.mode       = acGainMsgBoxCh1;
                                calibrate.channel    = 1;
                            }
                            if(calibrate.gainMode == 3)
                            {
                                calibrate.messageBox = acmbChannel00;
                                calibrate.channel = 0;
                                calibrate.mode = acStepStart;
                                calibrate.type = ctNormal;
                            }
                            calibrate.gainMode++;
                        }
                        else
                        {
                            calibrate.voltage = (VoltageCapture)((int)calibrate.voltage + 1);
                            calibrate.mode = acGainSetup;
                        }
                        printf("acGainVoltageChange %i\n", calibrate.gainMode);
                        printf("      acGainVoltage %i\n", calibrate.voltage);
                        calibrate.debug << "acGainVoltageChange: ";
                        calibrate.debug << "  acGainVoltage: ";
                        calibrate.debug << calibrate.voltage;
                        calibrate.debug << "  gainMin: ";
                        calibrate.debug << calibrate.gainMin;
                        calibrate.debug << "\n";
                        calibrate.debug << "  gainMax: ";
                        calibrate.debug << calibrate.gainMax;
                        calibrate.debug << "\n";
                        calibrate.debug << "  gainSet: ";
                        calibrate.debug << calibrate.gainSet;
                        calibrate.debug << "\n";
                    }
                    break;
                case acGainMsgBoxCh1:
                    {
                        if(calibrate.messageBox == acmbOk)
                        {
                            calibrate.debug << "acGainMsgBoxCh1: ok \n";
                            calibrate.mode    = acGainStart;
                        }
                        if(calibrate.messageBox == acmbCancel)
                        {
                            calibrate.debug << "acGainMsgBoxCh1: cancel \n";
                            calibrate.active  = 0;
                            calibrate.reset();
                        }
                    }
                    break;
                case acStepStart:
                    {
                        if(calibrate.messageBox == acmbOk)
                        {
                            calibrate.channel = 0;
                            calibrate.voltage = vc2Volt;
                            //calibrate.stepMax = settings.getHardware()->referenceStepMax;
                            //calibrate.stepMin = settings.getHardware()->referenceStepMin;
                            calibrate.stepMax = 1000;
                            calibrate.stepMin = 0;
                            calibrate.mode = acStepSetup;
                            calibrate.debug << "acStepStart: ok \n";
                        }
                        if(calibrate.messageBox == acmbCancel)
                        {
                            calibrate.active = 0;
                            calibrate.reset();
                            calibrate.debug << "acStepStart: cancel \n";
                        }
                    }
                    break;
                case acStepSetup:
                    {
                        calibrate.stepReference = int(calibrate.stepMin + (calibrate.stepMax - calibrate.stepMin) / 2.0);
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_A_GROUND, 1);
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_B_GROUND, 1);
                        if(calibrate.type == ct500Mhz)
                        {
                            sfSetXRange(getHw(), 0);
                            sfSetAnalogSwitchBit(getHw(), CHANNEL_INTERLEAVE, 1);
                        }
                        else
                        {
                            sfSetXRange(getHw(), 5);
                            sfSetAnalogSwitchBit(getHw(), CHANNEL_INTERLEAVE, 0);
                        }
                        if(calibrate.channel == 0)
                        {
                            sfSetYPositionA(getHw(), calibrate.stepReference + settings.getHardware()->calibratedOffsets[calibrate.type][0][calibrate.voltage]);
                            sfSetYRangeScaleA(getHw(), getAttr(calibrate.voltage), (uint)settings.getHardware()->calibratedGainValue[calibrate.type][0][calibrate.voltage]);
                            sfSetYPositionB(getHw(), settings.getHardware()->calibratedOffsets[calibrate.type][1][calibrate.voltage]);
                            sfSetYRangeScaleB(getHw(), (uint)getAttr(calibrate.voltage), settings.getHardware()->calibratedGainValue[calibrate.type][1][calibrate.voltage]);
                        }
                        else
                        {
                            sfSetYPositionA(getHw(), settings.getHardware()->calibratedOffsets[calibrate.type][0][calibrate.voltage]);
                            sfSetYRangeScaleA(getHw(), getAttr(calibrate.voltage), (uint)settings.getHardware()->calibratedGainValue[calibrate.type][0][calibrate.voltage]);
                            sfSetYPositionB(getHw(), calibrate.stepReference + settings.getHardware()->calibratedOffsets[calibrate.type][1][calibrate.voltage]);
                            sfSetYRangeScaleB(getHw(), (uint)getAttr(calibrate.voltage), settings.getHardware()->calibratedGainValue[calibrate.type][1][calibrate.voltage]);
                        }
                        transferData();
                        calibrate.mode = acStepCapture;
                        calibrate.debug << "acStepSetup: ";
                        calibrate.debug << "  CH: ";
                        calibrate.debug << calibrate.channel;
                        calibrate.debug << "  stepReference: ";
                        calibrate.debug << calibrate.stepReference;
                        calibrate.debug << "\n";
                        SDL_Delay(50);
                    }
                    break;
                case acStepCapture:
                    {
                        int          ch = calibrate.channel;
                        calibrate.stepMeasuredOffsetVoltage = 0;
                        calibrate.stepMeasuredOffsetValue   = 0;
                        for(int sample = 0; sample < SCOPEFUN_DISPLAY; sample++)
                        {
                            if(ch == 0)
                            {
                                calibrate.stepMeasuredOffsetValue   += display.analog0.bytes[sample];
                                calibrate.stepMeasuredOffsetVoltage += display.analog0.bytes[sample];
                            }
                            if(ch == 1)
                            {
                                calibrate.stepMeasuredOffsetValue   += display.analog1.bytes[sample];
                                calibrate.stepMeasuredOffsetVoltage += display.analog1.bytes[sample];
                            }
                        }
                        calibrate.stepMeasuredOffsetValue   /= double(SCOPEFUN_DISPLAY);
                        calibrate.stepMeasuredOffsetValue   *= double(SCOPEFUN_MAX_VOLTAGE);
                        calibrate.stepMeasuredOffsetVoltage /= double(SCOPEFUN_DISPLAY);
                        calibrate.mode = acStepCalculate;
                        calibrate.debug << "  stepMeasuredOffsetValue: ";
                        calibrate.debug << calibrate.stepMeasuredOffsetValue;
                        calibrate.debug << "\n";
                        calibrate.debug << "  stepMeasuredOffsetVoltage: ";
                        calibrate.debug << calibrate.stepMeasuredOffsetVoltage;
                        calibrate.debug << "\n";
                    }
                    break;
                case acStepCalculate:
                    {
                        if(calibrate.iteration > settings.getHardware()->referenceMaxIterations || abs(calibrate.stepMeasuredOffsetValue) >= settings.getHardware()->referenceStepMin && abs(calibrate.stepMeasuredOffsetValue) <= settings.getHardware()->referenceStepMax)
                        {
                            double yGridMax      = grid.yCount / 2.0;
                            double dCapture      = double(captureVoltFromEnum(calibrate.voltage));
                            double  iStepVoltage = yGridMax * calibrate.stepMeasuredOffsetVoltage * dCapture;
                            settings.getHardware()->calibratedVoltageStep[calibrate.type][calibrate.channel][calibrate.voltage] = double(iStepVoltage) / calibrate.stepReference;
                            calibrate.mode = acStepVoltageChange;
                            calibrate.debug << "  stepiStepVoltage: ";
                            calibrate.debug << double(iStepVoltage);
                            calibrate.debug << "\n";
                            calibrate.debug << "  calibratedVoltageStep: ";
                            calibrate.debug << (double(iStepVoltage) / calibrate.stepReference) * 100.0;
                            calibrate.debug << "\n";
                        }
                        else
                        {
                            double delta = (calibrate.stepMax - calibrate.stepMin) / 4.0;
                            if(abs(calibrate.stepMeasuredOffsetValue) >= settings.getHardware()->referenceStepMax)
                            {
                                calibrate.stepMax -= delta;
                            }
                            else if(abs(calibrate.stepMeasuredOffsetValue) <= settings.getHardware()->referenceStepMin)
                            {
                                calibrate.stepMin += delta;
                            }
                            calibrate.mode = acStepSetup;
                            calibrate.iteration++;
                            calibrate.debug << "acStepCalculate: ";
                            calibrate.debug << "\n";
                            calibrate.debug << "  stepMin: ";
                            calibrate.debug << calibrate.stepMin;
                            calibrate.debug << "\n";
                            calibrate.debug << "  stepMax: ";
                            calibrate.debug << calibrate.stepMax;
                            calibrate.debug << "\n";
                        }
                    }
                    break;
                case acStepVoltageChange:
                    {
                        calibrate.iteration = 0;
                        calibrate.stepMax = 1000;
                        calibrate.stepMin = 0;
                        if(calibrate.voltage == vc10Mili)
                        {
                            if(calibrate.type == ctNormal)
                            {
                                calibrate.type    = ct500Mhz;
                                calibrate.voltage = vc2Volt;
                                calibrate.mode    = acStepSetup;
                            }
                            else
                            {
                                if(calibrate.channel == 0)
                                {
                                    calibrate.type       = ctNormal;
                                    calibrate.channel    = 1;
                                    calibrate.voltage    = vc2Volt;
                                    calibrate.mode       = acStepMsgBoxCh1;
                                    calibrate.messageBox = acmbChannel01;
                                }
                                else
                                {
                                    calibrate.messageBox = acmbEnd;
                                    calibrate.mode       = acEndMessageBox;
                                }
                            }
                        }
                        else
                        {
                            calibrate.voltage   = (VoltageCapture)((int)calibrate.voltage + 1);
                            calibrate.mode      = acStepSetup;
                        }
                        calibrate.debug << "acStepVoltageChange: ";
                        calibrate.debug << "  voltage: ";
                        calibrate.debug << calibrate.voltage;
                        calibrate.debug << "\n";
                        calibrate.debug << "  stepMin: ";
                        calibrate.debug << calibrate.stepMin;
                        calibrate.debug << "\n";
                        calibrate.debug << "  stepMax: ";
                        calibrate.debug << calibrate.stepMax;
                        calibrate.debug << "\n";
                    }
                    break;
                case acStepMsgBoxCh1:
                    {
                        if(calibrate.messageBox == acmbOk)
                        {
                            calibrate.debug << "acStepMsgBoxCh1: ok \n";
                            calibrate.mode    = acStepSetup;
                        }
                        if(calibrate.messageBox == acmbCancel)
                        {
                            calibrate.active = 0;
                            calibrate.reset();
                            calibrate.debug << "acStepMsgBoxCh1: cancel \n";
                        }
                    }
                    break;
                case acEndMessageBox:
                    {
                        if(calibrate.messageBox == acmbOk)
                        {
                            pOsciloscope->settings.getHardware()->saveCalibrationToFile();
                            calibrate.mode = acSaveSettings;
                            calibrate.debug << "acEndMessageBox: ok \n";
                        }
                        if(calibrate.messageBox == acmbCancel)
                        {
                            calibrate.reset();
                            calibrate.active  = 0;
                            calibrate.debug << "acEndMessageBox: cancel \n";
                        }
                        window.channel01.Ground = 0;
                        window.channel02.Ground = 0;
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_A_GROUND, 0);
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_B_GROUND, 0);
                        window.channel01.AcDc = 0;
                        window.channel02.AcDc = 0;
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_A_ACDC, 0);
                        sfSetAnalogSwitchBit(getHw(), CHANNEL_B_ACDC, 0);
                        sfSetXRange(getHw(), 1);
                        sfSetYPositionA(getHw(), settings.getHardware()->calibratedOffsets[calibrate.type][0][calibrate.voltage]);
                        sfSetYRangeScaleA(getHw(), getAttr(calibrate.voltage), (uint)settings.getHardware()->calibratedGainValue[calibrate.type][0][calibrate.voltage]);
                        sfSetYPositionB(getHw(), settings.getHardware()->calibratedOffsets[calibrate.type][1][calibrate.voltage]);
                        sfSetYRangeScaleB(getHw(), (uint)getAttr(calibrate.voltage), settings.getHardware()->calibratedGainValue[calibrate.type][1][calibrate.voltage]);
                        //transferData();
                    }
                    break;
                case acSaveSettings:
                    {
                        calibrate.mode = acStartMessageBox;
                        calibrate.active = 0;
                        calibrate.debug << "acSaveSettings: \n";
                        FORMAT_BUFFER();
                        #if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
                        FORMAT_PATH_WINAPPDATA("data/calibrate/debug.log");
                        #else
                        FORMAT_PATH("data/calibrate/debug.log");
                        #endif
                        fileSaveString((const char*)formatBuffer, calibrate.debug.get());
                        calibrate.reset();
                    }
                    break;
            }
        }
        // progress
        window.progress.uiValue = (calibrate.mode + 1);
        window.progress.uiRange = (acEnd - 1);
        window.progress.uiActive = 1;
    }
}

void OsciloscopeManager::AutoCalibrate()
{
    thread.hardwareControlFunction(&m_hw);
    thread.wait();
    calibrate.clear();
    calibrate.active      = 1;
    calibrate.mode        = acStartMessageBox;
    calibrate.voltage     = vc2Volt;
    calibrate.frame       = 0;
    calibrate.generator   = 0;
    calibrate.messageBox  = acmbStart;
    SDL_AtomicSet(&signalMode, SIGNAL_MODE_CAPTURE);
}

double OsciloscopeManager::getTriggerVoltagePerStep()
{
    float  captureVoltage = 0.f;
    if(window.trigger.Source == 0)
    {
        captureVoltage = window.channel01.Capture;
    }
    else
    {
        captureVoltage = window.channel02.Capture;
    }
    return captureVoltage * grid.yCount / 1024.0;
}

double OsciloscopeManager::getTriggerVoltage()
{
    float  captureVoltage = 0.f;
    if(window.trigger.Source == 0)
    {
        captureVoltage = window.channel01.Capture;
    }
    else
    {
        captureVoltage = window.channel02.Capture;
    }
    return captureVoltage;
}

void OsciloscopeManager::setFrame(int frame)
{
}

void OsciloscopeManager::allocate()
{
    // frames
    m_captureBuffer.m_dataPtr = (byte*)pMemory->allocate(settings.getSettings()->memoryFrame * MEGABYTE);
    m_captureBuffer.m_dataMax = settings.getSettings()->memoryFrame * MEGABYTE;
    SDL_memset(m_captureBuffer.m_dataPtr, 0, m_captureBuffer.m_dataMax);
    // hardware capture buffer
    pOsciloscope->sizeHardwareCapture = settings.getSettings()->memoryFrame * MEGABYTE;
    pOsciloscope->ptrHardwareCapture  = (byte*)pMemory->allocate(pOsciloscope->sizeHardwareCapture);
    SDL_memset(pOsciloscope->ptrHardwareCapture, 0, pOsciloscope->sizeHardwareCapture);
    //// allocate capture buffer
    //byte* display = (byte*)pMemory->allocate(SCOPEFUN_FRAME_MEMORY);
    //byte* rld = 0;
    //if(settings.getSettings()->memoryRld)
    //{
    //    rld = (byte*)pMemory->allocate(settings.getSettings()->memoryRld * MEGABYTE);
    //}
    //captureBuffer = new CaptureBuffer(display, SCOPEFUN_FRAME_MEMORY, rld, settings.getSettings()->memoryRld * MEGABYTE);
    // display
    uint toAllocateDisplay = settings.getSettings()->historyFrameDisplay * sizeof(OsciloscopeFrame);
    pTmpData = (OsciloscopeFrame*)pMemory->allocate(toAllocateDisplay);
    tmpHistory.init((OsciloscopeFrame*)pTmpData, settings.getSettings()->historyFrameDisplay);
    // threads
    /* for(uint i = 0; i < MAX_THREAD; i++)
     {
         uint toAllocate = settings.getSettings()->historyFrameDisplay * sizeof(OsciloscopeFrame);
         OsciloscopeFrame* pThreadHistory = (OsciloscopeFrame*)pMemory->allocate(toAllocate);
         captureData[i].history.init(pThreadHistory, settings.getSettings()->historyFrameDisplay);
     }*/
    // how much memory is still available
    ularge        min = MINIMUM_HISTORY_COUNT * sizeof(CapturePacket);
    ularge       free = pMemory->maximum(min, MAXIMUM_HISTORY_SIZE);
    ularge        max = settings.getSettings()->memoryHistory * MEGABYTE;
    if(max == 0)
    {
        max = free;
    }
    ularge toAllocate = clamp(free, min, max);
    // history
    //byte* pPacketData = (byte*)pMemory->allocate(toAllocate);
    //captureBuffer->historyMemory.init(pPacketData, toAllocate, window.storage.getPacketSize());
    //// memory
    //captureBuffer->setMemory();
    //// settings
    //settings.getSettings()->historyFrameCount = captureBuffer->historyMemory.ringFrame.getSize();
}


void OsciloscopeManager::deallocate()
{
    /* captureBuffer->historySSD.freeInterfaceMemory();
     captureBuffer->historyMemory.freeInterfaceMemory();
     captureBuffer->historyMemory.freePacketMemory();
     pMemory->free(pTmpData);
     for(uint i = 0; i < MAX_THREAD; i++)
     {
         pMemory->free(captureData[i].history.getData());
     }*/
}

void OsciloscopeManager::setThreadPriority(ThreadID id)
{
    switch(id)
    {
        case THREAD_ID_CAPTURE:
            SDL_SetThreadPriority((SDL_ThreadPriority)settings.getSettings()->priorityCapture);
            break;
        case THREAD_ID_UPDATE:
            SDL_SetThreadPriority((SDL_ThreadPriority)settings.getSettings()->priorityUpdate);
            break;
        case THREAD_ID_MAIN:
            SDL_SetThreadPriority((SDL_ThreadPriority)settings.getSettings()->priorityMain);
            break;
        case THREAD_ID_RENDER:
            SDL_SetThreadPriority((SDL_ThreadPriority)settings.getSettings()->priorityRender);
            break;
    };
}

void OsciloscopeManager::clientUploadDisplay(SDisplay& display)
{
}

void OsciloscopeManager::clearEts(int value)
{
}

void OsciloscopeManager::transferData()
{
    thread.hardwareControlFunction(&m_hw);
    if(!calibrate.active)
    {
        if (!SDL_AtomicGet(&m_undoRedoEnabled))
           return;
        UndoRedo undo;
        undo.m_wnd = window;
        undo.m_hw  = m_hw;
        if(m_hardwareUndo.getCount() >= (SCOPEFUN_MAX_UNDO - 1))
        {
            m_hardwareUndo.popFront();
        }
        m_hardwareUndo.pushBack(undo);
        m_hardwareRedo.clear();
    }
}

int OsciloscopeManager::isUndoActive()
{
    return m_hardwareUndo.getCount();
}

int OsciloscopeManager::transferUndo()
{
    if(!m_hardwareUndo.getCount())
      return 1;

    m_hardwareRedo.pushBack(m_hardwareUndo.last());
    m_hardwareUndo.popBack();
    if (!m_hardwareUndo.getCount())
       return 1;

    UndoRedo undo = m_hardwareUndo.last();
    window = undo.m_wnd;
    m_hw   = undo.m_hw;
    thread.hardwareControlFunction(&m_hw);

    if(m_hardwareRedo.getCount() >= (SCOPEFUN_MAX_UNDO - 1))
    { m_hardwareRedo.popFront(); }

    transferUI();
    return 0;
}

int OsciloscopeManager::isRedoActive()
{
    return m_hardwareRedo.getCount();
}
void OsciloscopeManager::UndoRedoOnOff(int enable)
{
   SDL_AtomicSet(&m_undoRedoEnabled, enable);
}

int OsciloscopeManager::transferRedo()
{
    if(!m_hardwareRedo.getCount())
    { return 1; }

    UndoRedo redo = m_hardwareRedo.last();
    m_hw   = redo.m_hw;
    window = redo.m_wnd;
    thread.hardwareControlFunction(&m_hw);
    m_hardwareUndo.pushBack(redo);
    m_hardwareRedo.popBack();

    if(m_hardwareUndo.getCount() >= (SCOPEFUN_MAX_UNDO - 1))
    { m_hardwareUndo.popFront(); }

    transferUI();
    return 0;
}

void OsciloscopeManager::transferUI()
{
    Flag16 flags;
    flags.set(sfGetAnalogSwitch(&m_hw));
    window.channel01.Ground = flags.is(CHANNEL_A_GROUND);
    window.channel02.Ground = flags.is(CHANNEL_B_GROUND);
    window.channel01.AcDc = flags.is(CHANNEL_A_ACDC);
    window.channel02.AcDc = flags.is(CHANNEL_B_ACDC);
    // ETS support removed
    window.horizontal.ETS = 0;
    // channel A, B
    window.horizontal.Control  = sfGetControl(&m_hw);
    window.channel01.Capture   = captureVoltFromEnum(getVolt(0, sfGetYGainA(&m_hw)));
    window.channel01.Scale     = sfGetYScaleA(&m_hw);
    window.channel01.YPosition = sfGetYPositionA(&m_hw) -pOsciloscope->settings.getHardware()->getAnalogOffset(window.horizontal.Capture, 0, window.channel01.Capture);
    window.channel01.YPosition *= pOsciloscope->settings.getHardware()->getAnalogStep(window.horizontal.Capture, 0, window.channel01.Capture);
    window.channel02.Capture   = captureVoltFromEnum(getVolt(1, sfGetYGainB(&m_hw)));
    window.channel02.Scale     = sfGetYScaleB(&m_hw);
    window.channel02.YPosition = sfGetYPositionB(&m_hw) - pOsciloscope->settings.getHardware()->getAnalogOffset(window.horizontal.Capture, 1, window.channel02.Capture);
    window.channel02.YPosition *= pOsciloscope->settings.getHardware()->getAnalogStep(window.horizontal.Capture, 1, window.channel02.Capture);

    // trigger
    window.trigger.Source = sfGetTriggerSource(&m_hw);
    window.trigger.Mode = sfGetTriggerMode(&m_hw);
    window.trigger.Slope = sfGetTriggerSlope(&m_hw);
    window.trigger.Percent = sfGetTriggerPre(&m_hw);
    window.trigger.His = sfGetTriggerHis(&m_hw);
    window.trigger.Level = sfGetTriggerLevel(&m_hw);
    window.trigger.Holdoff = sfGetHoldoff(&m_hw);
    // time
    window.horizontal.Capture   = captureTimeFromEnum(sfGetXRange(&m_hw));
    window.horizontal.FrameSize = sfGetSampleSize(&m_hw);
    // ui
    SetupUI(window);
}

void OsciloscopeManager::setupControl(WndMain window)
{
    SHardware* hw = getHw();

    // Control register (essential: enables FPGA streaming, default to normal)
    int ctrl = window.horizontal.Control;
    if (ctrl == 0) ctrl = 1;
    sfSetControl(hw, ctrl);

    // Channel A
    sfSetYRangeScaleA(hw, getAttr(captureVoltFromValue(window.channel01.Capture)),
                                getGain(0, captureVoltFromValue(window.channel01.Capture)));
    sfSetYPositionA(hw, window.channel01.YPosition /
        pOsciloscope->settings.getHardware()->getAnalogStep(window.horizontal.Capture, 0, window.channel01.Capture)
        + pOsciloscope->settings.getHardware()->getAnalogOffset(window.horizontal.Capture, 0, window.channel01.Capture));

    // Channel B
    sfSetYRangeScaleB(hw, getAttr(captureVoltFromValue(window.channel02.Capture)),
                                getGain(1, captureVoltFromValue(window.channel02.Capture)));
    sfSetYPositionB(hw, window.channel02.YPosition /
        pOsciloscope->settings.getHardware()->getAnalogStep(window.horizontal.Capture, 1, window.channel02.Capture)
        + pOsciloscope->settings.getHardware()->getAnalogOffset(window.horizontal.Capture, 1, window.channel02.Capture));

    // Trigger settings
    sfSetTriggerSource(hw, window.trigger.Source);
    sfSetTriggerMode(hw, window.trigger.Mode);
    sfSetTriggerSlope(hw, window.trigger.Slope);
    sfSetTriggerPre(hw, window.trigger.Percent);
    sfSetTriggerHis(hw, window.trigger.His);
    sfSetTriggerLevel(hw, window.trigger.Level);

    // Timebase
    sfSetXRange(hw, captureTimeFromValue(window.horizontal.Capture));
    sfSetSampleSize(hw, window.horizontal.FrameSize);
    sfSetHoldoff(hw, window.trigger.Holdoff);
}

ushort OsciloscopeManager::getAttr(uint volt)
{
    return settings.getHardware()->referenceGainAttr[volt];
}

ushort OsciloscopeManager::getGain(int channel, uint volt)
{
    channel = clamp(channel, 0, 1);
    ushort flags = sfGetAnalogSwitch(&m_hw);
    if(flags & CHANNEL_INTERLEAVE)
    {
        return settings.getHardware()->calibratedGainValue[ct500Mhz][channel][volt];
    }
    else
    {
        return settings.getHardware()->calibratedGainValue[ctNormal][channel][volt];
    }
}
ushort OsciloscopeManager::getVolt(int channel, ushort gain)
{
    float capture = pOsciloscope->window.horizontal.Capture;
    for(int i = 0; i < vcLast; i++)
    {
        if(pOsciloscope->settings.getHardware()->calibratedGainValue[getCalibrationType(capture)][channel][i] == gain)
        {
            return i;
        }
    }
    return 0;
}


int OsciloscopeManager::Render()
{
    ////////////////////////////////////////////////////////////////////////////////
    // process events
    ////////////////////////////////////////////////////////////////////////////////
    pInput->clear();
    SDL_PumpEvents();
    SDL_Event e = { 0 };

    while (SDL_PollEvent(&e))
    {
    pManager->queueEvent(e);
    }
    pManager->fireEvents();
    ////////////////////////////////////////////////////////////////////////////////
    // update main loop
    ////////////////////////////////////////////////////////////////////////////////
    pTimer->deltaTime(TIMER_MAIN_THREAD);
    pOsciloscope->dtUpdate = pTimer->getDelta(TIMER_MAIN_THREAD);
    pManager->update(pOsciloscope->dtUpdate);

    // watchdog: detect stuck capture thread
    pOsciloscope->watchdogCheck();

    ////////////////////////////////////////////////////////////////////////////////
    // render
    ////////////////////////////////////////////////////////////////////////////////
    pOsciloscope->onApplicationIdle();

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// CaptureThread
//
////////////////////////////////////////////////////////////////////////////////
void SendToRenderer(uint frameIndex,OsciloscopeThreadRenderer& renderer, OsciloscopeFFT& fft, OsciloscopeThreadData& captureData, uint delay)
{
   // clear
   renderer.clearFast();
   fft.clear();

   // render
   pOsciloscope->renderThread(frameIndex % SDL_AtomicGet(&pOsciloscope->m_captureBuffer.m_threadCount), captureData, renderer, fft);
}


int SDLCALL CaptureDataThreadFunction(void* data)
{
    pTimer->init(TIMER_HARDWARE);
    SDL_MemoryBarrierAcquire();
    ScopeFunCaptureBuffer& captureBuffer = pOsciloscope->m_captureBuffer;
    SInt                   receivedBytes = { 0 };
    SInt               receivedFrameSize = { 0 };
    SInt                   totalReceived = { 0 };
    double                          time = 0.0;
    while(SDL_AtomicGet(&pOsciloscope->captureDataThreadActive) > 0 )
    {
        // timer
        pTimer->deltaTime(TIMER_HARDWARE);
        time += pTimer->getDelta(TIMER_HARDWARE);

        // return flag
        int ret = 0;

        // mode
        SignalMode mode = (SignalMode)SDL_AtomicGet(&pOsciloscope->signalMode);

        // simulate removed

        // capture (both single CAPTURE and continuous PLAY)
        if (mode == SIGNAL_MODE_CAPTURE || mode == SIGNAL_MODE_PLAY)
        {
            int isFpga = pOsciloscope->thread.isFpga();
            int isOpen = pOsciloscope->thread.isOpen();

            if (!isOpen || !isFpga)
            {
                static int lastWaitLog = 0;
                int now = (int)SDL_GetTicks();
                if (now - lastWaitLog > 3000)
                {
                    printf("[CaptureData] Waiting: open=%d fpga=%d\n", isOpen, isFpga);
                    lastWaitLog = now;
                }
                pOsciloscope->watchdogPing();
                SDL_Delay(100);
                continue;
            }
            else
            {
                int ret = sfFrameCapture(getCtx(), &receivedBytes, &receivedFrameSize);
                pOsciloscope->watchdogPing();
            }
        }

        // idle — keep watchdog alive when paused
        if (mode == SIGNAL_MODE_PAUSE)
        {
            pOsciloscope->watchdogPing();
        }

        // capture
        uint   captureIndex = SDL_AtomicGet(&captureBuffer.m_captureIndex);

        // lock
        captureBuffer.lockCapture(captureIndex, receivedBytes.value);

        // info
        uint     frameIndex  = SDL_AtomicGet(&captureBuffer.m_frameIndex);
        uint     frameCount  = SDL_AtomicGet(&captureBuffer.m_frameCount);
        uint     frameStart  = SDL_AtomicGet(&captureBuffer.m_frameStart);
        uint     frameEnd    = SDL_AtomicGet(&captureBuffer.m_frameEnd);
        uint     frameSize   = SDL_AtomicGet(&captureBuffer.m_frameSize);
        uint     frameOffset = SDL_AtomicGet(&captureBuffer.m_frameOffset);

        // copy frame (both CAPTURE and PLAY)
        if (mode == SIGNAL_MODE_CAPTURE || mode == SIGNAL_MODE_PLAY)
        {
            // size
            if (receivedFrameSize.value != frameSize && receivedFrameSize.value != 0)
            {
               frameSize = receivedFrameSize.value;
               frameCount = max<uint>(1, captureBuffer.m_dataMax / receivedFrameSize.value);
               SDL_AtomicSet(&captureBuffer.m_frameSize, frameSize);
               SDL_AtomicSet(&captureBuffer.m_frameCount, frameCount);
            }

            // output
            if (receivedBytes.value != 0)
            {
                ularge framePos = (frameIndex % frameCount) * frameSize;
                int ret = sfFrameOutput(getCtx(), (SFrameData*)&captureBuffer.m_dataPtr[framePos], receivedBytes.value);
                frameOffset = receivedBytes.value;
            }

            // if full frame received and no new data received from hardware
            if (frameOffset == frameSize && receivedBytes.value == 0)
            {
               frameIndex = max<int>(0,frameIndex - 1) % frameCount;
            }

            // if full frame received: index++
            if (receivedBytes.value == frameSize)
            {
                receivedBytes.value = 0;
                receivedFrameSize.value = 0;
                SDL_AtomicSet(&captureBuffer.m_frameIndex, frameIndex + 1);

                // frameStart
                if( (frameIndex+1) >= frameCount )
                    SDL_AtomicSet(&captureBuffer.m_frameStart, frameStart + 1);

                // frameEnd
                SDL_AtomicSet(&captureBuffer.m_frameEnd, frameEnd + 1);
            }
        }

        // clear
        if (mode == SIGNAL_MODE_CLEAR)
        {
            if (SDL_AtomicGet(&captureBuffer.m_frameClear) == 1)
            {
               SDL_AtomicSet(&captureBuffer.m_frameClear, 0);
               SDL_memset(&captureBuffer.m_dataPtr[0], 0, captureBuffer.m_dataMax);
            }
        }

        if (mode == SIGNAL_MODE_PAUSE)
        {
            if (receivedBytes.value == 0)
            {
               frameIndex = max<int>(0,frameIndex - 1) % frameCount;
            }
            //SDL_Delay(10);
        }

        // offset
        SDL_AtomicSet(&captureBuffer.m_frameOffset, frameOffset);

        // index
        SDL_AtomicSet(&captureBuffer.m_captureIndex, captureIndex + 1);

        // unlock
        captureBuffer.unlockCapture(captureIndex, frameOffset, frameIndex);
    }
    SDL_MemoryBarrierRelease();
    return 0;
}

int DisplayFrame(uint maxBytesToRead,uint frameIndex, uint frameCount, uint frameSize, ScopeFunCaptureBuffer& captureBuffer, OsciloscopeThreadData& threadData, OsciloscopeThreadRenderer& renderer, OsciloscopeFFT& fft, float delay, double pos, double zoom)
{
    // safety
    uint index = frameIndex % frameCount;

    // framePos
    ularge framePos = index * frameSize;
           framePos = min<ularge>(framePos, captureBuffer.m_dataMax - 2 * SCOPEFUN_FRAME_HEADER);

    // clear
    SDL_memset(&threadData.m_frame, 0, sizeof(SDisplay));

    // display
    sfFrameDisplay(getCtx(), (SFrameData*)&captureBuffer.m_dataPtr[framePos], min(frameSize, maxBytesToRead), &threadData.m_frame, pos, zoom);

    // hardware
    SFrameHeader header = { 0 };
    sfGetHeader(getCtx(), (SFrameData*)&captureBuffer.m_dataPtr[framePos], &header);
    sfGetHeaderHardware((SFrameHeader*)&header, &threadData.m_hw);

    // ETS removed
    int isEts = 0;

    // 3D history
    threadData.m_historyCount = 0;
    if( isEts || threadData.m_window.fftDigital.is(VIEW_SELECT_OSC_3D) || threadData.m_window.fftDigital.is(VIEW_SELECT_FFT_3D))
    { threadData.m_historyCount = SCOPEFUN_MAX_HISTORY; }
    for(int i = 0; i < threadData.m_historyCount; i++)
    {
        uint historyIndex = (index - i) % frameCount;
        ularge historyPos = historyIndex * frameSize;

        // clear
        SDL_memset(&threadData.m_history[i], 0, sizeof(SDisplay));

        // display
        sfFrameDisplay(getCtx(), (SFrameData*)&captureBuffer.m_dataPtr[historyPos], min(frameSize, maxBytesToRead), &threadData.m_history[i], pos, zoom);
    }
    return 0;
}

int SDLCALL GenerateFrameThreadFunction(void* data)
{
    SDL_MemoryBarrierAcquire();
    pOsciloscope->setThreadPriority(THREAD_ID_RENDER);
    pTimer->init(TIMER_RENDER);
    OsciloscopeRenderData     captureRender;
    OsciloscopeThreadRenderer renderer;
    OsciloscopeFFT            fft;
    OsciloscopeThreadData*    pCaptureData   = new OsciloscopeThreadData();
    ScopeFunCaptureBuffer*    pCaptureBuffer = &pOsciloscope->m_captureBuffer;
    fft.init();
    renderer.init(pOsciloscope->settings.getSettings()->historyFrameDisplay);
    renderer.clear();
    //sfSetNumSamples((SHardware*) & ((SFrameHeader*)pCaptureBuffer->m_dataPtr)->hardware, SCOPEFUN_DISPLAY);
    ularge playFrameIdx = 0;
    uint   delayCapture = 1;
    while( SDL_AtomicGet(&pOsciloscope->generateFrameThreadActive) > 0 )
    {
         // render info
         SDL_AtomicLock(&pOsciloscope->renderLock);
            pCaptureData->m_pos    = pOsciloscope->signalPosition;
            pCaptureData->m_zoom   = pOsciloscope->signalZoom;
            pCaptureData->m_render = pOsciloscope->renderData;
            pCaptureData->m_window = pOsciloscope->renderWindow;
         SDL_AtomicUnlock(&pOsciloscope->renderLock);

         // capture index
         uint  updateIndex   = SDL_AtomicGet(&pCaptureBuffer->m_updateIndex);

         // lock
         uint maxBytesToRead = 0;
         uint frameIndex     = 0;
         pCaptureBuffer->lockUpdate(updateIndex, maxBytesToRead, frameIndex);

         uint  frameCount = SDL_AtomicGet(&pCaptureBuffer->m_frameCount);
         uint  frameSize = SDL_AtomicGet(&pCaptureBuffer->m_frameSize);

         // mode
         uint  signalMode = SDL_AtomicGet(&pOsciloscope->signalMode);

         // display
         DisplayFrame(maxBytesToRead, frameIndex, frameCount, frameSize, *pCaptureBuffer, *pCaptureData, renderer, fft, delayCapture, pCaptureData->m_pos, pCaptureData->m_zoom);

         // render
         SendToRenderer(updateIndex, renderer, fft, *pCaptureData, delayCapture);

         // inc
         SDL_AtomicSet(&pCaptureBuffer->m_updateIndex, updateIndex + 1);

         // unlock
         pCaptureBuffer->unlockUpdate(updateIndex);

         // on capture
         pOsciloscope->onCalibrateFrameCaptured(pCaptureData->m_frame, 2);
    }
    SDL_MemoryBarrierRelease();
    return 0;
}

int SDLCALL ControlHardwareThreadFunction(void* data)
{
    SDL_MemoryBarrierAcquire();
    pOsciloscope->setThreadPriority(THREAD_ID_CAPTURE);
    while(SDL_AtomicGet(&pOsciloscope->controlHardwareThreadActive) > 0 )
    {
        pOsciloscope->thread.update();
        SDL_Delay(1);
    }
    SDL_MemoryBarrierRelease();
    return 0;
}

SFContext* getCtx()
{
    return &pOsciloscope->ctx->sf;
}

SHardware* getHw()
{
    return &pOsciloscope->m_hw;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
