#ifdef RUNGEKUTTA4_H
#define RUNGEKUTTA4_H

#include <QVector2D>

class RungeKutta4
{
public:
    RungeKutta4();
    QVector2D RK4Step(double time);
    void setUp(double time_step, double damping_constant);
private:
    QVector2D step(QVector2D y, double t);
    double dt, damping;
};

#endif // RUNGEKUTTA4_H
