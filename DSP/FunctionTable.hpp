#pragma once

struct FunctionTable : FunctionProcessor
{
    double * table;
    size_t   size;

    FunctionTable(size_t size, std::function<double (double)> func)
    {
        table = new double[this->size = 2*size];
        for(size_t i = -size; i < size; i++)
            table[i+size] = func((double)i/(double)size);
    }
    ~FunctionTable() {
        if(table) delete [] table;
    }
    double lookup(double val)
    {
        if(val < -1) val = -1;
        if(val > 1) val = 1;
        int pos = val*size;
        pos += size;
        pos %= 2*size;
        float x1 = table[pos];
        float x2 = table[(pos+1)%(size*2)];
        float f  = x1 - floor(x1);
        return x1 + f*(x2-x1);
    }
    double Tick(double I, double A=1, double X=0, double Y=0)
    {
        return A*lookup(I);
    }
};
