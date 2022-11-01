struct Walsh
{
    std::vector<double> duty_table;
    int duty_ctr;
    Oscillators::BlitSquare square;

    Walsh(int n) {
        duty_table.resize(n);
        for(size_t i = 0; i < n; i++)
            duty_table[i] = (double)i / (double)n;
    }
};
