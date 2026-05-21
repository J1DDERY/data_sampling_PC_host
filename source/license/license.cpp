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
#ifdef PLATFORM_WIN
#include <windows.h>
#include <vector>
#include <string>

#define MAX_FILE_SIZE 4*1024*1024

using namespace std;

void addFiles(vector<wstring>& names, wstring dir)
{
    wstring path = dir;
    path.append(L"*.*");
    WIN32_FIND_DATAW fd = { 0 };
    HANDLE handle = FindFirstFileW(path.c_str(), &fd);
    do
    {
        if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if(wcscmp(L".", fd.cFileName) != 0 && wcscmp(L"..", fd.cFileName) != 0)
            {
                wstring newdir = dir;
                newdir.append(fd.cFileName);
                newdir.append(L"/");
                addFiles(names, newdir);
            }
        }
        else
        {
            wstring fileName = dir;
            fileName.append(fd.cFileName);
            names.push_back(fileName);
        }
    }
    while(::FindNextFileW(handle, &fd));
}

int UpdateLicense()
{
    char*  header = (char*)malloc(MAX_FILE_SIZE);
    char*  input  = (char*)malloc(MAX_FILE_SIZE);
    char*  output = (char*)malloc(MAX_FILE_SIZE);
    ZeroMemory(header, MAX_FILE_SIZE);
    ZeroMemory(input, MAX_FILE_SIZE);
    ZeroMemory(output, MAX_FILE_SIZE);
    printf("begin");
    // find files
    vector<wstring> names;
    vector<wstring> dir;
    addFiles(names, L"../source/");
    addFiles(names, L"./data/shader21/");
    addFiles(names, L"./data/shader32/");
    // read header
    DWORD bytesRead = 0;
    HANDLE handle     = CreateFileW(L"../source/license/license.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD  headerSize = GetFileSize(handle, NULL);
    if(headerSize > MAX_FILE_SIZE)
    {
        printf("max header file size exceeded");
        headerSize = 0;
    }
    else
    {
        BOOL ret = ReadFile(handle, header, headerSize, &bytesRead, NULL);
    }
    CloseHandle(handle);
    WCHAR currentDir[1024] = { 0 };
    int ret = GetCurrentDirectoryW(1024, (WCHAR*) currentDir);
    wstring workingDir = (WCHAR*)currentDir;
    // write output
    wstring c   = L".c";
    wstring h   = L".h";
    wstring cpp = L".cpp";
    wstring hpp = L".hpp";
    wstring vs  = L".ps";
    wstring ps  = L".vs";
    for(int i = 0; i < (int)names.size(); i++)
    {
        wstring file = names[i];
        size_t  last = file.find_last_of('.');
        wstring fend = file.substr(last, file.length() - last);
        if(fend == cpp || fend == hpp || fend == vs || fend == ps || fend == c || fend == h)
        {
            // safety
            ZeroMemory(input, MAX_FILE_SIZE);
            ZeroMemory(output, MAX_FILE_SIZE);
            // input read
            wstring path     = workingDir + L"\\" + file;
            HANDLE handle    = CreateFileW(path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            DWORD  inputSize = GetFileSize(handle, NULL);
            if(inputSize > MAX_FILE_SIZE || inputSize < headerSize)
            {
                printf("invalid file size: %ws", path.c_str());
                continue;
            }
            BOOL ret = ReadFile(handle, input, inputSize, &bytesRead, NULL);
            CloseHandle(handle);
            // find header insert place
            memcpy(&output[0], (void*)header,            headerSize);
            memcpy(&output[headerSize], &input[headerSize], max((int)0, (int)((int)inputSize - (int)headerSize)));
            // output write
            DWORD written = 0;
            handle = CreateFileW(file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            ret = WriteFile(handle, output, inputSize, &written, NULL);
            CloseHandle(handle);
        }
    }
    printf("end");
    getchar();
    return 0;
}
#else
int UpdateLicense()
{
    return -1;
}
#endif
