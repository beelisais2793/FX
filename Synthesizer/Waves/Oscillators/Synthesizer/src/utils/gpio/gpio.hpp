#pragma once

#define PIN_AMOUNT 32
#include <array>

#ifdef WIN32

#include "../gui/guilib.hpp"
class Synth : public Window
{
    Knob k1, k2, k3, k4, k5, k6, k7, k8, k9, k10;
    Slider s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;
    Sensor sensor1;
 
    void Draw() {
        Background({ 0, 0, 0 });
        Fill({ 1, 1, 1, 1 });
        DrawString(L"O S C   O N E", 13, 202, 30);
        Window::Draw(s9, 130, 30);
       
        Fill({ 0.46, 0.46, 0.46, 1 });
        DrawString(L"tune", 4, 212, 70);
        Window::Draw(k1, 235, 148);
        DrawString(L"wave", 4, 310, 70);
        Window::Draw(k2, 335, 148);
        DrawString(L"sync", 4, 212, 210);
        Window::Draw(k3, 235, 288);
        DrawString(L" fm ", 4, 310, 210);
        Window::Draw(k4, 335, 288);

        DrawString(L"fold", 4, 417, 30);
        Window::Draw(k7, 440, 108);
        DrawString(L"vibra", 5, 506, 30);
        Window::Draw(k9, 540, 108);

        Fill({ 1, 1, 1, 1 });
        DrawString(L"O S C   T W O", 13, 402, 170);
        Window::Draw(s10, 600, 30);
        Fill({ 0.46, 0.46, 0.46, 1 });
        DrawString(L"tune", 4, 413, 210);
        Window::Draw(k5, 440, 288);
        DrawString(L"wave", 4, 512, 210);
        Window::Draw(k6, 540, 288);
        DrawString(L"delay", 4, 812, 210);
        Window::Draw(k8, 840, 288);



        int dx = 70, x = 30 - dx, y = height - 330;
        
        Window::Draw(sensor1, x += dx, height - 430);
        Fill({ 1, 1, 1, 1 });
        DrawString(L"E N V E L O P E   1", 19, (x += 30) + dx, y - 38);
        Window::Draw(s1, x += dx, y);
        Window::Draw(s2, x += dx, y);
        Window::Draw(s3, x += dx, y);
        Window::Draw(s4, x += dx, y);

        x += dx;
        DrawString(L"E N V E L O P E   2", 19, x + dx, y - 38);
        Window::Draw(s5, x += dx, y);
        Window::Draw(s6, x += dx, y);
        Window::Draw(s7, x += dx, y);
        Window::Draw(s8, x += dx, y);
    };

public:
    Synth(std::array<double, PIN_AMOUNT> &params) :
        k1(params[0], 0.5), 
        k2(params[1], 0),
        k3(params[2], 0),
        k4(params[3], 0),
        k5(params[4], 0.5),
        k6(params[5], 0),
        k7(params[6], 0),
        k8(params[7], 0),
        k9(params[8], 0),
        k10(params[9], 0),
        s1(params[16], 0.0),
        s2(params[17], 0.6),
        s3(params[18], 1.0),
        s4(params[19], 0.4),
        s5(params[20], 0.0),
        s6(params[21], 0.6),
        s7(params[22], 1.0),
        s8(params[23], 0.4),
        s9(params[24], 0.4),
        s10(params[25], 0.5),
        sensor1(params[31])
    {
    }
};
#endif

class GPIO
{

private:
    std::array<double, PIN_AMOUNT> params;
    
#ifdef WIN32
    Synth gui{ params };
#endif

public:

#ifdef WIN32
    void Start() { gui.Launch(); }
#endif

#ifdef __linux__
    void Start() 
    {
        while (true) {};
    }
#endif
    double Param(int a) const { return params[a]; };
    double operator[](int a) const { return Param(a); };
};