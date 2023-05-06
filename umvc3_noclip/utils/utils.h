#pragma once
typedef __int64 int64;


int64 GetEntryPoint();
int64 _addr(__int64 addr);
unsigned long long baseaddr(unsigned long long addr);
unsigned long long _adjust(unsigned long long addr);