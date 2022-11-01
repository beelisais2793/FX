#pragma once

#define C_2    0.5
#define C_3    0.33333333333333333333
#define C_6    0.16666666666666666667


class LagrangeInterpolation
{
public:
    LagrangeInterpolation (float *y, int n, float x, float r);
    ~LagrangeInterpolation();

    void update (float *y);
    float interpolate (float x);


private:
    std::unique_ptr<float[]> data;
    int num;
    float first;
    float ratio;

};

LagrangeInterpolation::LagrangeInterpolation(float *y, int n, float x, float r)
    : num(n + 4),
      first(x / r),
      ratio(r)
{
    data.reset(new float[num]);
    update(y);
}

LagrangeInterpolation::~LagrangeInterpolation()
{
}


////////////////////////////////////////////////////////////////
//
//    public methods
//

void LagrangeInterpolation::update(float *y)
{
    int n = num - 4;

    data[0] = 2.0 * y[0] - y[1];
    data[1] = data[0];
    for (int i = 0; i < n; i++)
        data[i + 2] = y[i];
    data[num - 2] = 2.0 * y[n - 1] - y[n - 2];
    data[num - 1] = data[num - 2];
}


float LagrangeInterpolation::interpolate (float x)
{
    float pos = logf(x / first) / logf(ratio);
    if (pos <= 0.0)
    {
        return data[0];
    }
    if (pos >= num - 3)
    {
        return data[num - 1];
    }
    int n = (int)floor(pos);
    float lambda   = pos - n;
    float lambda_2 = lambda * lambda;
    float lambda_3 = lambda * lambda_2;
    float c_0 = data[n+1];
    float c_1 = data[n+2] - data[n] * C_3 - data[n+1] * C_2 - data[n+3] * C_6;
    float c_2 = (data[n+2] + data[n]) * C_2 - data[n+1];
    float c_3 = (data[n+3] - data[n]) * C_6 + (data[n+1] - data[n+2]) * C_2;

    return c_3 * lambda_3 + c_2 * lambda_2 + c_1 * lambda + c_0;
}