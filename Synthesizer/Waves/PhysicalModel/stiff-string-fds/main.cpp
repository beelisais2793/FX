#include "pal/pal.h"
#include <algorithm>
#include <vector>
#include <chrono> 
#include <iostream>

using namespace pal;

class StiffString
{
    public:
    /// Create a new stiff string model
    /// @param  n           The number of points in the model.
    /// @param  sampleRate  The sample rate to use (default 44100).
    StiffString(int n, float sampleRate = 44100);

    /// Apply a force to the string.
    /// @param  i       Where to apply the force.
    /// @param  force   The magnitude of the force to apply.
    void applyForce(int i, float force);

    /// Compute and apply a bow force from the set bow parameters.
    void computeBowForce();

    /// Draw a UI for controlling the string.
    void draw(bool plot = true);

    /// Excite the string with a simple impulse force.
    void excite();

    /// Extrapolate a force somewhere on the string.
    /// @param  force   The magnitude of the force to apply.
    /// @param  i       Fractional index of where to apply the force.
    void extrapolateForce(float force, float i);

    /// Compute and get the next output sample.
    /// @returns    The computed sample.
    float getNext();

    /// Get the linearly interpolated value of a vector at some fractional
    /// index.
    /// @param  v   The vector whose values to interpolate.
    /// @param  i   The fractional index to read at.
    /// @returns    The interpolated value.
    float interpolate(const std::vector<float> &v, float i) const;

    /// Reset the string state to zero.
    void reset();

    /// Resize the string.
    /// @param  n   The desired number of points in the string.
    void resize(int n);

    /// Resize the string such that the string will be stable for the chosen
    /// parameters.
    void resizeForStability();

    /// Set the wave speed corresponding to a frequency.
    /// @param  freq    The desired frequency.
    void setWavespeedFromFreq(float freq) { gamma0 = powf(2 * freq, 2); };

    /// Set the bow force.
    /// @param  value   The desired bow force.
    void setBowForce(float value) { fb = value; };

    private:
    // We need three vectors to hold the state of our system at the next,
    // current and previous timestep.
    std::vector<float> ua;
    std::vector<float> ub;
    std::vector<float> uc;

    // We use reference to do quick reference swapping, so we avoid copying
    // arrays.
    std::vector<float> &un;  // Next state.
    std::vector<float> &u;   // Current state.
    std::vector<float> &up;  // Previous state.
    std::vector<float> f;    // Forces.

    // The model parameters
    float gamma0 = 10000;     // The wave speed (pitch).
    float kappa0 = 10;      // The stiffness (inharmonicity).
    float sigma0 = 2;     // The independent damping (sustain).
    float sigma1 = 1e-5;    // The dependent damping (brightness).

    float k = 0;            // Sample period.
    float h = 0;            // Grid spacing.

    // Bow parameters
    float vb = 0.2;     // Bow speed.
    float a = 100;      // Friction characteristic.
    float fb = 0.5;     // Bowing force.
    float pb = 0.17;    // Bowing position.
};

StiffString::StiffString(int n, float sampleRate) :
    un(ua), u(ub), up(uc)
{
    un.resize(n, 0);
    u.resize(n, 0);
    up.resize(n, 0);
    f.resize(n, 0);

    k = 1.0f / sampleRate;
    h = 1.0f / n;
}

void StiffString::applyForce(int i, float force)
{
    f[i] = (1 / h) * force;
}

void StiffString::computeBowForce()
{
    // Get all the points we need
    float i = pb * u.size();
    float uf = interpolate(u, i+1);
    float uff = interpolate(u, i+2);
    float ub = interpolate(u, i-1);
    float ubb = interpolate(u, i-2);
    float y = interpolate(u, i);

    float upf = interpolate(up, i+1);
    float upb = interpolate(up, i-1);
    float yp = interpolate(up, i);

    // Precompute the linear stuff that is constant throughout Newton-rahpson
    float c1 = 1 / (1 + sigma0 * k);
    float h2 = h * h;
    float h4 = h2 * h2;
    float k2 = k * k;
    float dxx = (1 / h2) * (ub - 2 * y + uf); 
    float dxxxx = (1 / h4) * (ubb - 4 * ub + 6 * y - 4 * uf + uff); 
    float dxxp = (1 / h2) * (upb - 2 * yp + upf); 
    float dtdxx = (1 / k) * (dxx - dxxp);
    float L = c1 * (k2 * gamma0 * dxx - k2 * kappa0 * dxxxx + sigma0 * k * yp + 2 * k2 * sigma1 * dtdxx + 2 * y - yp);

    // Approximate vr using backwards difference
    float vr = vb - (1 / k) * (y - yp);

    // Prepare for Newton-raphson
    float delta = INFINITY;
    float phi = 0;

    for (int i = 0; i < 100 && fabs(delta) > 1e-3; i++)
    {
        float vr2 = vr * vr;
        float c2 = expf(-a * vr2 + 0.5);
        phi = sqrt(2 * a) * vr * c2;
        float phid = sqrt(2 * a) * (c2 - 2 * a * vr2 * c2);

        float num = (1 / (2 * k)) * (L - c1 * k2 * (1 / h) * fb * phi - yp) - vb - vr;
        float denom = -(c1 * k / 2) * (1 / h) * fb * phi - 1;

        delta = num / denom;
        vr -= delta;
    }

    extrapolateForce(-fb * phi, i);
}

void StiffString::draw(bool plot)
{
    ImGui::SliderFloat("Bowing force", &fb, 0, 100);
    ImGui::SliderFloat("Bowing position", &pb, 0, 1);

    if (plot)
    {
        ImGui::PlotLines("String", u.data(), u.size(), 0, "", 0.0001, -0.0001, ImVec2(0, 120));
    }
}

void StiffString::excite()
{
    u[0.3 * u.size()] += 1.0;
    up[0.3 * u.size()] += 1.0;
}

float StiffString::getNext()
{
    // Precompute whatever we can
    float c1 = 1 / (1 + sigma0 * k);
    float h2 = h * h;
    float h4 = h2 * h2;
    float k2 = k * k;
    int N = u.size();

    // Compute leftmost boundary point.
    float dxx = (1 / h2) * (u[1] - 2 * u[0] + 0); 
    float dxxxx = (1 / h4) * (0 - 4 * 0 + 6 * u[0] - 4 * u[1] + u[2]); 
    float dxxp = (1 / h2) * (up[1] - 2 * up[0] + 0); 
    float dtdxx = (1 / k) * (dxx - dxxp);
    un[0] = c1 * (k2 * gamma0 * dxx - k2 * kappa0 * dxxxx + sigma0 * k * up[0] + 2 * k2 * sigma1 * dtdxx + 2 * u[0] - up[0]);

    // Compute second-to-left boundary point.
    dxx = (1 / h2) * (u[0] - 2 * u[1] + u[2]); 
    dxxxx = (1 / h4) * (0 - 4 * u[0] + 6 * u[1] - 4 * u[2] + u[3]); 
    dxxp = (1 / h2) * (up[0] - 2 * up[1] + up[2]); 
    dtdxx = (1 / k) * (dxx - dxxp);
    un[1] = c1 * (k2 * gamma0 * dxx - k2 * kappa0 * dxxxx + sigma0 * k * up[1] + 2 * k2 * sigma1 * dtdxx + 2 * u[1] - up[1]);

    // Compute inner points.
    for (int i = 2; i < N - 2; i++)
    {
        dxx = (1 / h2) * (u[i-1] - 2 * u[i] + u[i+1]); 
        dxxxx = (1 / h4) * (u[i-2] - 4 * u[i-1] + 6 * u[i] - 4 * u[i+1] + u[i+2]); 
        dxxp = (1 / h2) * (up[i-1] - 2 * up[i] + up[i+1]); 
        dtdxx = (1 / k) * (dxx - dxxp);
        un[i] = c1 * (k2 * gamma0 * dxx - k2 * kappa0 * dxxxx + sigma0 * k * up[i] + 2 * k2 * sigma1 * dtdxx + 2 * u[i] - up[i] + k2 * f[i]);
    }

    /// Compute second-to-right boundary points.
    dxx = (1 / h2) * (u[N-3] - 2 * u[N-2] + u[N-1]); 
    dxxxx = (1 / h4) * (u[N-4] - 4 * u[N-3] + 6 * u[N-2] - 4 * u[N-1] + 0); 
    dxxp = (1 / h2) * (up[N-3] - 2 * up[N-2] + up[N-1]); 
    dtdxx = (1 / k) * (dxx - dxxp);
    un[N-2] = c1 * (k2 * gamma0 * dxx - k2 * kappa0 * dxxxx + sigma0 * k * up[N-2] + 2 * sigma1 * k2 * dtdxx + 2 * u[N-2] - up[N-2]);

    // Compute rightmost point.
    dxx = (1 / h2) * (u[N-2] - 2 * u[N-1] + 0); 
    dxxxx = (1 / h4) * (u[N-3] - 4 * u[N-2] + 6 * u[N-1] - 4 * 0 + 0); 
    dxxp = (1 / h2) * (up[N-2] - 2 * up[N-1] + 0); 
    dtdxx = (1 / k) * (dxx - dxxp);
    un[N-1] = c1 * (k2 * gamma0 * dxx - k2 * kappa0 * dxxxx + sigma0 * k * up[N-1] + 2 * sigma1 * k2 * dtdxx + 2 * u[N-1] - up[N-1]);

    // Perform reference swapping.
    auto &ut = up;
    up = u;
    u = un;
    un = ut;

    for (int i = 0; i < f.size(); i++)
        f[i] = 0;

    return u[0.6 * u.size()];
}

void StiffString::extrapolateForce(float force, float i)
{
    float il = floor(i);
    float iu = ceil(i);
    float c = i - il;

    f[il] += (1 / h) * (1 - c) * force;
    f[iu] += (1 / h) * c * force;
}

float StiffString::interpolate(const std::vector<float> &v, float i) const
{
    float il = floor(i);
    float iu = ceil(i);
    float c = i - il;

    return (1 - c) * v[il] + c * v[iu];
}

void StiffString::reset()
{
    std::fill(un.begin(), un.end(), 0);
    std::fill(u.begin(), u.end(), 0);
    std::fill(up.begin(), up.end(), 0);
}

void StiffString::resize(int n)
{
    un.resize(n, 0);
    u.resize(n, 0);
    up.resize(n, 0);
    f.resize(n, 0);

    h = 1.0f / n;
}

void StiffString::resizeForStability()
{
    float hmin = sqrt(0.5 * (gamma0 * k * k + 4 * sigma1 * k + sqrt(powf(gamma0 * k * k + 4 * sigma1 * k, 2) + 16 * kappa0 * k * k)));
    int n = floor(1 / hmin);
    printf("Resize string to %i\n", n);
    resize(n);
}

int main(int argc, char **argv)
{
    RealTimeAudio audio;
    StiffString string(100);
    string.setWavespeedFromFreq(110);
    string.resizeForStability();
    string.setBowForce(50);

    auto start = std::chrono::high_resolution_clock::now();
    float sum = 0;

    for (int i = 0; i < 10 * 44100; i++)
    {
        string.computeBowForce();
        sum += string.getNext();
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Efficiency measurement: time to render 10s of audio: " << duration.count() / 1e6 << " seconds, (sum = " << sum << ")"<< std::endl;

    string.reset();

    audio.callback = [&](int numSamples, int numChannels, float *in, float *out)
    {
        for (int sample = 0; sample < numSamples; sample++)
        {
            // Compute your sample here.
            string.computeBowForce();
            float y = 1e4 * (string.getNext());

            for (int channel = 0; channel < numChannels; channel++)
            {
                // Write it to the output array here.
                out[sample * numChannels + channel] = y;
            }
        }
    };

    Gui gui(800, 600, "Stiff String Example");
    int iterations = 50;

    while (gui.draw())
    {
        ImGui::SetNextWindowSize(ImVec2(350, 0), ImGuiCond_FirstUseEver);
        ImGui::Begin("Audio Setup");
            audio.draw();

            if (ImGui::Button("Excite string"))
            {
                string.applyForce(15, 1000);
            }

            ImGui::InputInt("Num. iterations", &iterations, 10, 100);

            if (ImGui::Button("Get next"))
            {
                for (int i = 0; i < iterations; i++)
                    string.getNext();
            }

            string.draw();
        ImGui::End();

        // Uncomment this to see all the available UI widgets.
        // ImGui::ShowDemoWindow();
    }

    return 0;
}