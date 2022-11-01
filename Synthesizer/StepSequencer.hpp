#pragma once

float HalfNote_ToSeconds(float bpm) {
    return 120.0f/ bpm;
}
float QuarterNote_ToSeconds(float bpm) {
    return 60.0f / bpm;
}
float EigthNote_ToSeconds(float bpm) {
    return 30 / bpm;
}
float SixteenthNote_ToSeconds(float bpm) {
    return 15 / bpm;
}
float DottedQuarter_ToSeconds(float bpm) {
    return 90.0f/ bpm;
}
float DottedEigth_ToSeconds(float bpm) {
    return 45.0f / bpm;
}
float DottedSixteenth_ToSeconds(float bpm) {
    return 22.5f / bpm;
}
float TripletQuarter_ToSeconds(float bpm) {
    return 40.0f / bpm;
}
float TripletEigth_ToSeconds(float bpm) {
    return 20.0f/ bpm;
}
float TripletSixteenth_ToSeconds(float bpm) {
    return 10.0f/ bpm;
}


float HalfNote_ToHz(float bpm) {
    return bpm / 120.0f;
}
float QuarterNote_ToHz(float bpm) {
    return bpm / 60.0f;
}
float EigthNote_ToHz(float bpm) {
    return bpm / 30.0f;
}
float SixteenthNote_ToHz(float bpm) {
    return bpm / 15.0f;
}
float DottedQuarter_ToHz(float bpm) {
    return bpm / 90.0f;
}
float DottedEigth_ToHz(float bpm) {
    return bpm / 45.0f;
}
float DottedSixteenth_ToHz(float bpm) {
    return bpm / 22.5f;
}
float TripletQuarter_ToHz(float bpm) {
    return bpm / 40.0f;
}
float TripletEigth_ToHz(float bpm) {
    return bpm / 20.0f;
}
float TripletSixteenth_ToHz(float bpm) {
    return bpm / 10.0f;
}


// simple function generator sequencer
struct StepSequencer
{
    FunctionGenerator clock;
    double bpm;
    double last = 1;
    std::function< void (void) > callback = [](){ };

    enum {
        HALFNOTE,
        QUARTERNOTE,
        EIGTHNOTE,
        SIXTEENTHNOTE,
        DOTTEDQUARTERNOTE,
        DOTTEDEIGTHNOTE,
        DOTTEDSIXTEENTHNOTE,
        TRIPLETQUARTERNOTE,
        TRIPLETEIGTHNOTE,
        TRIPLETSIXTEENTHNOTE,
    };
    int noteDivision;

    StepSequencer(float bpm) : clock(120.0/60.0f,44100.0f)
    {
        this->bpm = bpm;
        noteDivision = QUARTERNOTE;
    }
    void setBPM(float bpm)
    {
        this->bpm = bpm;
        clock.setFrequency(calcHz());
    }
    void setNoteDivision(int type)
    {
        noteDivision = type;
        clock.setFrequency(calcHz());
    }
    double calcHz()
    {
        switch(noteDivision)
        {
        case HALFNOTE: return HalfNote_ToHz(bpm);
        case QUARTERNOTE: return QuarterNote_ToHz(bpm);
        case EIGTHNOTE: return EigthNote_ToHz(bpm);
        case SIXTEENTHNOTE: return SixteenthNote_ToHz(bpm);
        case DOTTEDQUARTERNOTE: return DottedQuarter_ToHz(bpm);
        case DOTTEDEIGTHNOTE: return DottedEigth_ToHz(bpm);
        case DOTTEDSIXTEENTHNOTE: return DottedSixteenth_ToHz(bpm);
        case TRIPLETQUARTERNOTE: return TripletQuarter_ToHz(bpm);
        case TRIPLETEIGTHNOTE: return TripletEigth_ToHz(bpm);
        case TRIPLETSIXTEENTHNOTE: return TripletSixteenth_ToHz(bpm);
        }
        return QuarterNote_ToHz(bpm);
    }
    bool Tick()
    {
        double tick = clock.Tick();
        bool   pulse = false;
        if(tick < last) pulse = true;
        last = tick;
        return pulse;
    }
    void Step()
    {
        if(Tick()) callback();
    }
};

// let's you make a time that is a note interval 
struct StepTimer
{
    FunctionGenerator clock;
    double bpm;
    std::chrono::high_resolution_clock::time_point t1;
    double tick;
    std::function< void (void) > callback = [](){ };
    double timer = 0;

    enum {
        HALFNOTE,
        QUARTERNOTE,
        EIGTHNOTE,
        SIXTEENTHNOTE,
        DOTTEDQUARTERNOTE,
        DOTTEDEIGTHNOTE,
        DOTTEDSIXTEENTHNOTE,
        TRIPLETQUARTERNOTE,
        TRIPLETEIGTHNOTE,
        TRIPLETSIXTEENTHNOTE,
    };
    int noteDivision;

    StepTimer(float bpm) : clock(120.0/60.0f,44100.0f)
    {        
        noteDivision = QUARTERNOTE;
        t1 = std::chrono::high_resolution_clock::now();
        this->bpm = bpm;
        tick= calcSeconds();
    }
    void setBPM(float bpm)
    {
        this->bpm = bpm;
        tick= calcSeconds();
    }
    void setNoteDivision(int type)
    {
        noteDivision = type;
        tick= calcSeconds();
    }
    double calcSeconds()
    {
        switch(noteDivision)
        {
        case HALFNOTE: return HalfNote_ToSeconds(bpm);
        case QUARTERNOTE: return QuarterNote_ToSeconds(bpm);
        case EIGTHNOTE: return EigthNote_ToSeconds(bpm);
        case SIXTEENTHNOTE: return SixteenthNote_ToSeconds(bpm);
        case DOTTEDQUARTERNOTE: return DottedQuarter_ToSeconds(bpm);
        case DOTTEDEIGTHNOTE: return DottedEigth_ToSeconds(bpm);
        case DOTTEDSIXTEENTHNOTE: return DottedSixteenth_ToSeconds(bpm);
        case TRIPLETQUARTERNOTE: return TripletQuarter_ToSeconds(bpm);
        case TRIPLETEIGTHNOTE: return TripletEigth_ToSeconds(bpm);
        case TRIPLETSIXTEENTHNOTE: return TripletSixteenth_ToSeconds(bpm);
        }
        return QuarterNote_ToSeconds(bpm);
    }
    bool Tick()
    {
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time = std::chrono::duration_cast<std::chrono::duration<double>>(t2-t1);
        timer += time.count();
        t1 = t2;        
        if(timer >= tick) {
            timer = 0;
            return true;
            
        }
        return false;        
    }
    void Step()
    {
        if(Tick()) callback();
    }
};
