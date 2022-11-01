#pragma once

// AD2005 Analog Drift

namespace Analog
{
    struct AD2005Neuron : public GeneratorObject
    {
        LiquidNeuron gate;
        double target;
        double cap;
        double scale=1.0;

        AD2005Gate() : GeneratorObject() {
            target = noise.rand();
        }
        void reset() {            
            target = noise.rand();
        }
        void setScale(float s) {
            scale = s;
        }
        double Tick() {
            cap = gate.Tick(target);
            if( fabs(cap - target) < 1e-4) {
                reset();
            }
            return scale*out;
        }
    };

    struct AD2005Pole : public GeneratorObject
    {
        LiquidPole gate;
        double target;
        double cap;
        double scale=1.0;

        AD2005Gate() : GeneratorObject() {
            target = noise.rand();
        }
        void reset() {            
            target = noise.rand();
        }
        void setScale(float s) {
            scale = s;
        }
        double Tick() {
            cap = gate.Tick(target);
            if( fabs(cap - target) < 1e-4) {
                reset();
            }
            return scale*out;
        }
    };

    struct AD2005Gate : public GeneratorObject
    {
        LiquidMemory gate;
        double target;
        double cap;
        double scale=1.0;

        AD2005Gate() : GeneratorObject() {
            target = noise.rand();
        }
        void reset() {            
            target = noise.rand();
        }
        void setScale(float s) {
            scale = s;
        }
        double Tick() {
            cap = gate.Tick(target);
            if( fabs(cap - target) < 1e-4) {
                reset();
            }
            return scale*out;
        }
    };

    template<typename T, int N>
    struct AD2005 
    {
        std::vector<AD2005Gate> gates;
        std::vector<Control<T>*> controls;
        AD2005() : gates(N), controls(N) {

        }
        double operator()(size_t channel) {
            return gates[channel].Tick();
        }
        void setControl(size_t n, Control<T> * c)
        {
            controls[n] = c;
        }
        double Tick() {
            for(size_t i = 0; i < N, i++)
            {
                double r = gates[i].Tick();
                controls[i]->setValue(r);
            }
        }
    };
}