namespace Wavetables
{
    struct Wavetable 
    {
        double operator[](double a) { return Value(a);  }
        virtual double Value(double) = 0;
        virtual void Position(double) {};
        virtual void Sync(double) {};
    };

    struct Saw : public Wavetable 
    {
        double Value(double) override;
    };

    struct Sine : public Wavetable
    {
        double Value(double) override;
    };

    struct Square : public Wavetable
    {
        double Value(double) override;
    };

    struct Triangle : public Wavetable
    {
        double Value(double) override;
    };

    struct Basic : public Wavetable
    {
        double Value(double) override;
        void Position(double) override;
        void Sync(double) override;
    private:
        double sync = 1;
        double position = 0;
        Sine sine;
        Triangle triangle;
        Saw saw;
        Square square;
    };
}