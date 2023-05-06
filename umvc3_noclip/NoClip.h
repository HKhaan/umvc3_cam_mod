#pragma once
class Bindings;

class NoClip
{
public:
    void Tick(Bindings* bindings);
    void TickMenu();
    bool Active=false;
};
