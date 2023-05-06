#include "Bindings.h"

#include <string>
#include <Windows.h>
#include "IniReader.h"

int KeyConvert(char key)
{
    return key - 'A' + 0x41;
}

void Bindings::Init()
{
    HMODULE hModule = GetModuleHandle(nullptr);
    char path[MAX_PATH];
    GetModuleFileName(hModule, path, MAX_PATH);
    std::string strPath = path;
    strPath = strPath.substr(0,strPath.length()-9);
 
    strPath = strPath + "umvc3_noclip.ini";
    CIniReader ini=CIniReader((char*)strPath.c_str());
    menuOpenKey = ini.ReadInteger((char*)"Bindings", (char*)"OpenMenu", VK_F2);
    speedup = ini.ReadInteger((char*)"Bindings", (char*)"Speedup", VK_SHIFT);
    forward = ini.ReadInteger((char*)"Bindings", (char*)"Forward", KeyConvert('W'));
    backward = ini.ReadInteger((char*)"Bindings", (char*)"Backward", KeyConvert('S'));
    left = ini.ReadInteger((char*)"Bindings", (char*)"Left", KeyConvert('A'));
    right = ini.ReadInteger((char*)"Bindings", (char*)"Right", KeyConvert('D'));
    up = ini.ReadInteger((char*)"Bindings", (char*)"Up", KeyConvert('Q'));
    down = ini.ReadInteger((char*)"Bindings", (char*)"Down", KeyConvert('E'));
    rotateXNeg = ini.ReadInteger((char*)"Bindings", (char*)"RotX", VK_LEFT);
    rotateXPos = ini.ReadInteger((char*)"Bindings", (char*)"RotX", VK_RIGHT);
    rotateYNeg = ini.ReadInteger((char*)"Bindings", (char*)"RotY", VK_UP);
    rotateYPos = ini.ReadInteger((char*)"Bindings", (char*)"RotY", VK_DOWN);


    // menuOpenKey = /*ini.ReadInteger((char*)"Bindings", (char*)"OpenMenu", */VK_F2;
    // speedup = /*ini.ReadInteger((char*)"Bindings", (char*)"Speedup", */VK_SHIFT;
    // forward = /*ini.ReadInteger((char*)"Bindings", (char*)"Forward",*/ KeyConvert('W');
    // backward = /*ini.ReadInteger((char*)"Bindings", (char*)"Backward",*/ KeyConvert('S');
    // left = /*ini.ReadInteger((char*)"Bindings", (char*)"Left",*/ KeyConvert('A');
    // right = /*ini.ReadInteger((char*)"Bindings", (char*)"Right",*/ KeyConvert('D');
    // up = /*ini.ReadInteger((char*)"Bindings", (char*)"Up",*/ KeyConvert('Q');
    // down = /*ini.ReadInteger((char*)"Bindings", (char*)"Down",*/ KeyConvert('E');
    // rotateXNeg = /*ini.ReadInteger((char*)"Bindings", (char*)"RotX",*/ VK_LEFT;
    // rotateXPos = /*ini.ReadInteger((char*)"Bindings", (char*)"RotX",*/ VK_RIGHT;
    // rotateYNeg = /*ini.ReadInteger((char*)"Bindings", (char*)"RotY",*/ VK_UP;
    // rotateYPos = /*ini.ReadInteger((char*)"Bindings", (char*)"RotY",*/ VK_DOWN;
    
}
