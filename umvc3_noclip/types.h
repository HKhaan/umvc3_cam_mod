#pragma once

struct sMvc3Main {
    char padding [0x40090+256];
    void* pad1;
    void* pad2;
    void* pad3;
    void* pad4;
    void* mpCamera;
};
struct __declspec(align(8))Vector3
{
    float x;
    float y;
    float z;
};
struct uMvcCamera {
    char pad[0x4c];
    float mFov;
    __declspec(align(16))Vector3 mCameraPos;
    Vector3 mCameraUp;
    Vector3 mTargetPos;
};