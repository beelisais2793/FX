#pragma

// Matsuoka oscillator
#define AAA 2.5
#define bi 3.52
#define TAU_R 0.05 // time constant
#define TAU_A 0.75 //time constant
#define Si 1 // s_0 affects only the output amplitude.

// Pendulum system
#define GGG 9.81  // gravitational accelaration m*s^(-2)
#define DDD 0.005 // damping constant kg*m^2*s^(-1)
#define LLL 0.2   // pendulum length
#define MMM 0.2   // pendulum weight
#define M0 0.03

#define Oi 1
#define Hz 440.0
#define PI 3.1415926
#define TIME 100


// Pendulum dynamics
// angular velocity
#define _PEQ1(omega, theta, input, length,gain)(- GGG*sin(theta)/length - DDD*omega/(MMM*pow(length,2)) + gain*tanh(7*input)/(MMM*pow(length,2)))
// angular
#define _PEQ2(omega)(omega)

// Matsuoka oscillator
// x
#define _MEQ1(x, y_other, v, input,gain)((- x - AAA * y_other - bi * v + gain*input + Si)/TAU_R )
// v
#define _MEQ2(v, y)((- v + y )/TAU_A)

class Integrator {
public:
  Integrator() = default;
  Integrator(int count, std::vector<std::function<double (double, std::vector<double>)> > functions, std::vector<double> variables);
  
  std::vector<double> rk4(double t, double dt);
  std::vector<double> rk4_matsuoka(double t, double dt);
  ~Integrator();

    void set(int count, std::vector<std::function<double (double, std::vector<double>)> > functions, std::vector<double> variables)
    {
        this->count = count;
        this->functions = functions;
        this->variables = variables;
        
        k = std::vector<double>(count, 0.0);
        l = std::vector<double>(count, 0.0);
        m = std::vector<double>(count, 0.0);
        lvariables = std::vector<double>(count, 0.0);
        mvariables = std::vector<double>(count, 0.0);
        nvariables = std::vector<double>(count, 0.0);        
    }
private:
  std::vector<double> k;
  std::vector<double> l;
  std::vector<double> m;
  std::vector<double> lvariables;
  std::vector<double> mvariables;
  std::vector<double> nvariables;
  std::vector<std::function<double (double, std::vector<double>)> > functions;
  
  std::vector<double> variables;
  
  int count;
};

Integrator::Integrator(int count, std::vector<std::function<double (double, std::vector<double>)> > functions, std::vector<double> variables) {
  this->count = count;
  this->functions = functions;
  this->variables = variables;
  
  k = std::vector<double>(count, 0.0);
  l = std::vector<double>(count, 0.0);
  m = std::vector<double>(count, 0.0);
  lvariables = std::vector<double>(count, 0.0);
  mvariables = std::vector<double>(count, 0.0);
  nvariables = std::vector<double>(count, 0.0);
}

Integrator::~Integrator() {
}

std::vector<double> Integrator::rk4(double t, double dt) {
  for (int i = 0; i < count; i++) {
    k[i] = dt * functions[i](t, variables);
    lvariables[i] = variables[i] + k[i] / 2.0;
  }
  for (int i = 0; i < count; i++) {
    l[i] = dt * functions[i](t, lvariables);
    mvariables[i] = variables[i] + l[i] / 2.0;
  }
  for (int i = 0; i < count; i++) {
    m[i] = dt * functions[i](t, mvariables);
    nvariables[i] = variables[i] + m[i];
  }
  for (int i = 0; i < count; i++) {
    variables[i] = variables[i] + (k[i] + 2.0*l[i] + 2.0*m[i] + dt*functions[i](t, nvariables)) / 6.0;
  }

  return variables;
}

double PF(double Vi) {
    return 1.0 / (1.0 + exp(-Vi));
}

struct Matsuko5
{
    //double y = 0.0;   // y is output of oscillator,
    double t = 0.0;   // time step
    double dt = 1.0/sampleRate;

    double ye = 0.0;  
    double yf = 0.0;  
    double y = 0.0;   // matsuoka output
    double inc = 440.0/sampleRate;
    double length = 0.2;
    double input_gain = 0.5;  //0.2 input gain from oscillator to pendulum
    double pf_gain = 0.5; // 0.03 from pendulum to oscillator feedback gain

    

    //                        p_omega, p_theta, m_xe, m_ve m_xf, m_vf
    std::vector<double> variables{0.1, 0.0, 0.01, 0.008, 0.002, 0.006};
    Integrator runge;

    Matsuko5()
    {
        double &ty = y;
        double &tlen = length;
        double &inputg = input_gain;
        double &tyf = yf;
        double &pfg = pf_gain;
        double &tye = ye;

        auto functions = std::vector<std::function<double (double, std::vector<double>)>>(6, NULL);
        functions[0] = [ty, tlen,inputg](double t, std::vector<double> args)->double {
        return _PEQ1(args[0], args[1], ty, tlen, inputg);
        };
        functions[1] = [](double t, std::vector<double> args)->double {
        return _PEQ2(args[0]);
        };
        functions[2] = [tyf, pfg](double t, std::vector<double> args)->double {
        return _MEQ1(args[2], tyf, args[3], args[1], pfg);
        };
        functions[3] = [tye](double t, std::vector<double> args)->double {
        return _MEQ2(args[3], tye);
        };
        functions[4] = [tye, pfg](double t, std::vector<double> args)->double {
        return _MEQ1(args[4], tye, args[5], -args[1], pfg);
        };
        functions[5] = [tyf](double t, std::vector<double> args)->double {
        return _MEQ2(args[5], tyf);
        };

        runge = Integrator(6, functions, variables);
    }
    void setFrequency(float f)
    {        
        dt = 1/sampleRate;
        inc = f/sampleRate;
    }
    double Tick()
    {        
        variables = runge.rk4(t, dt);
        ye = std::max(variables[2],0.0);
        yf = std::max(variables[4],0.0);
        y = Oi*(ye - yf);        
        t = fmod(t + y + inc, 1);        
        return sin(2*M_PI*(t + ye + yf + y));
    }
};
