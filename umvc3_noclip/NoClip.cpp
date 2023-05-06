#include "NoClip.h"

#include <chrono>

#include "Bindings.h"
#include "types.h"
#include "imgui/imgui.h"
#include "utils/utils.h"
#include <iostream>
#include <stdlib.h>

#include "utils/LateStaticInit.h"
#include <algorithm>


struct DollyKeyframe
{
    int time;
    Vector3 pos;
    Vector3 rot;
    float fov;
};

std::vector<DollyKeyframe> KeyFrames;
bool cameraStarted = false;
float p1Pos = 125.0f;
float p2Pos = -125.0f;
bool restarted = false;
int restartTimer = 0;
float rotation = 180.0f;
float rotationUp = 0.0f;
Vector3 pos;
float startime = 0;
int currenttime = 0.0f;
float playtime = 0.0f;
bool playing = false;

float lerp(float startVal, float endVal, float lerpVal)
{
    float lerpResult = (1 - std::abs(lerpVal - 1)) * startVal + std::abs(lerpVal - 1) * endVal;
    return lerpResult;
}

bool compareByTime(const DollyKeyframe& a, const DollyKeyframe& b)
{
    return a.time < b.time;
}

void MoveForward(float speed)
{
    auto xforward = (std::sin(rotation));
    auto yforward = (std::cos(rotation));

    pos.x = pos.x + xforward * speed;
    pos.z = pos.z + yforward * speed;
    pos.y = pos.y + rotationUp * speed * 0.001f;
}

void MoveRight(float speed)
{
    auto yforward = (std::sin(rotation));
    auto xforward = (std::cos(rotation));

    pos.x = (pos.x + xforward * speed);
    pos.z = pos.z - yforward * speed;
}


bool KeyPressed(int key)
{
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}


void PlayingInformation(DollyKeyframe& start, DollyKeyframe& end, float& lerpval, int& keyframe)
{
    auto tm = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::duration_cast<std::chrono::duration<double>>(tm.time_since_epoch()).count();
    auto playtime = now - startime;
    start = KeyFrames[0];
    end = KeyFrames[1];
    for (int i = 0; i < KeyFrames.size(); i++)
    {
        if (KeyFrames[i].time / 60.0f > playtime)
        {
            keyframe = i;
            break;
        }
        else
        {
            if (i + 1 == KeyFrames.size())
            {
                end = KeyFrames[i];
            }
            else
            {
                end = KeyFrames[i + 1];
            }


            start = KeyFrames[i];
        }
    }
    if (KeyFrames[KeyFrames.size() - 1].time / 60.0f < playtime)
    {
        playing = false;
        playtime = 0;

        pos = start.pos;
        auto main = *(sMvc3Main**)_addr(0x140E177e8);
        auto cam = (uMvcCamera**)(((char*)main->mpCamera) + (0x58));
        (*cam)->mFov = start.fov;
        rotation = start.rot.x;
        rotationUp = start.rot.y;
        
    }
    keyframe = 0;
    lerpval = 1.0f - (playtime - start.time / 60.0f) / (end.time / 60.0f - start.time / 60.0f);
}

void TickDolly()
{
    DollyKeyframe start, end;
    float lerpval;
    int keyframe;
    PlayingInformation(start, end, lerpval, keyframe);
    pos.x = lerp(start.pos.x, end.pos.x, lerpval);
    pos.y = lerp(start.pos.y, end.pos.y, lerpval);
    pos.z = lerp(start.pos.z, end.pos.z, lerpval);
    rotation = lerp(start.rot.x, end.rot.x, lerpval);
    rotationUp = lerp(start.rot.y, end.rot.y, lerpval);
    auto fov = lerp(start.fov, end.fov, lerpval);
    auto main = *(sMvc3Main**)_addr(0x140E177e8);
    auto cam = (uMvcCamera**)(((char*)main->mpCamera) + (0x58));
    (*cam)->mFov = fov;
}

void NoClip::Tick(Bindings* bindings)
{
    if (playing)
    {
        TickDolly();
    }
    
    float speed = 5.0f;
    if (GetAsyncKeyState(VK_SHIFT))
    {
        speed = 20.0f;
    }

    if (KeyPressed(bindings->left))
    {
        MoveRight(speed);
    }
    if (KeyPressed(bindings->right))
    {
        MoveRight(-speed);
    }
    if (KeyPressed(bindings->up))
    {
        pos.y -= speed;
    }
    if (KeyPressed(bindings->down))
    {
        pos.y += speed;
    }
    if (KeyPressed(bindings->forward))
    {
        MoveForward(speed);
    }
    if (KeyPressed(bindings->backward))
    {
        MoveForward(-speed);
    }

    if (KeyPressed(bindings->rotateXNeg))
    {
        rotation += 0.01f;
    }
    if (KeyPressed(bindings->rotateXPos))
    {
        rotation -= 0.01f;
    }
    if (KeyPressed(bindings->rotateYNeg))
    {
        rotationUp += 5.0f;
    }
    if (KeyPressed(bindings->rotateYPos))
    {
        rotationUp -= 5.0f;
    }
    auto main = *(sMvc3Main**)_addr(0x140E177e8);
    auto cam = (uMvcCamera**)(((char*)main->mpCamera) + (0x58));
    (*cam)->mCameraPos = pos;
    auto xforward = (std::sin(rotation));
    auto yforward = (std::cos(rotation));
    (*cam)->mTargetPos.x = pos.x + (xforward * 500);
    (*cam)->mTargetPos.y = pos.y + rotationUp;
    (*cam)->mTargetPos.z = pos.z + (yforward * 500);
}


void NoClip::TickMenu()
{
    auto main = *(sMvc3Main**)_addr(0x140E177e8);
    auto cam = (uMvcCamera**)(((char*)main->mpCamera) + (0x58));
    bool wasActive = Active;
    if (!Active)
    {
        ImGui::Checkbox("Activate", &Active);
        if (!wasActive && Active)
        {
            pos = (*cam)->mCameraPos;
            rotation = std::atan2((*cam)->mTargetPos.x - (*cam)->mCameraPos.x,
                                  (*cam)->mTargetPos.z - (*cam)->mCameraPos.z);
            rotationUp = (*cam)->mTargetPos.y - (*cam)->mCameraPos.y;
        }
    }
    else
    {
        ImGui::Checkbox("Deactivate", &Active);

        ImGui::SliderFloat("Fov ", &(*cam)->mFov, 5, 300);

        uintptr_t block3 = *(uintptr_t*)0x140D510A0;
        uintptr_t P1RecordingData = *(uintptr_t*)(block3 + 0x90);
        int P1CurrentPlaybackFrame = *(int*)(P1RecordingData + 0x40);
        ImGui::Text("Current Playback Frame: %d\n", P1CurrentPlaybackFrame);


        ImGui::SliderInt("time", &currenttime, 0, 600);
        ImGui::Text("Frame Time : %f", currenttime / 60.0f);
        if (KeyFrames.size() > 1 && ImGui::Button("PlayDolly"))
        {
            auto tm = std::chrono::high_resolution_clock::now();
            startime = std::chrono::duration_cast<std::chrono::duration<double>>(tm.time_since_epoch()).count();
            playtime = 0;

            playing = true;
        }

        if (ImGui::Button("Add dolly"))
        {
            int addIndex = -1;
            for (int i = 0; i < KeyFrames.size(); i++)
            {
                if (KeyFrames[i].time == currenttime)
                {
                    addIndex = i;
                    break;
                }
            }
            if (addIndex == -1)
            {
                KeyFrames.push_back({currenttime, pos, {rotation, rotationUp, 0.0f}, (*cam)->mFov});
            }
            else
            {
                KeyFrames[addIndex] = {currenttime, pos, {rotation, rotationUp, 0.0f}, (*cam)->mFov};
            }
            std::sort(KeyFrames.begin(), KeyFrames.end(), compareByTime);
        }
        int index_to_remove = 0;
        for (auto dol : KeyFrames)
        {
            std::string name = "select " + std::to_string(dol.time);
            if (ImGui::Button(name.c_str()))
            {
                currenttime = dol.time;
                pos = dol.pos;
                rotation = dol.rot.x;
                rotationUp = dol.rot.y;
            }
            ImGui::SameLine();
            std::string name2 = "delete " + std::to_string(dol.time);
            if (ImGui::Button(name2.c_str()))
            {
                KeyFrames.erase(KeyFrames.begin() + index_to_remove);
            }
            index_to_remove++;
        }

        if (playing)
        {
            DollyKeyframe start, end;
            float lerpval = 0;
            int keyframe = 0;
            PlayingInformation(start, end, lerpval, keyframe);
            ImGui::Text("PlayIndex: %d - %f", keyframe - 1, KeyFrames[keyframe].time / 60.0f);
            ImGui::Text("PlayTime: %f", playtime);
            if (KeyFrames[KeyFrames.size() - 1].time / 60.0f < playtime)
            {
                playing = false;
                playtime = 0;
            }
            else
            {
                auto lerpval = 1.0f - (playtime - start.time / 60.0f) / (end.time / 60.0f - start.time / 60.0f);
                ImGui::Text("LerpVal: %f", lerpval);
            }
        }
    }
}
